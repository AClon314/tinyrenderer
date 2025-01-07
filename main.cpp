#include "geometry.h"
#include "model.h"
#include "tgaimage.h"
/// gprof O0优化，代码质量与执行时间无明显关系
/// 优化，就是将易理解的短代码，转为难理解的、或更长或更短代码。所以不要太早优化。

const float FLOAT_MAX = std::numeric_limits<float>::max();

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor black = TGAColor(0, 0, 0, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);
const TGAColor rgb[] = {red, green, blue};
Model *model = NULL;
const int width = 800;
const int height = 800;

// 判断缓/陡，画直线，lesson 1
void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    bool steep = false;
    if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    for (int x = x0; x <= x1; x++) {
        float t = (x - x0) / (float)(x1 - x0);
        int y = y0 * (1. - t) + y1 * t;
        if (steep) {
            image.set(y, x, color);
        } else {
            image.set(x, y, color);
        }
    }
}

// 枚举一个包围盒中所有的像素，找到在三角形内的像素，计算该像素的重心坐标。如果有分量是负数，那这个像素就不在三角形内。lesson 2
Vec3f centroid(Vec3f *pts, Vec3f P) {
    // (AB,AC,PA)的x与y分量做叉积，得到垂直于面的向量
    Vec3f u = Vec3f(pts[2][0] - pts[0][0], pts[1][0] - pts[0][0], pts[0][0] - P[0]) ^ Vec3f(pts[2][1] - pts[0][1], pts[1][1] - pts[0][1], pts[0][1] - P[1]);
    /* `pts` and `P` has integer value as coordinates
        so `abs(u[2])` < 1 means `u[2]` is 0, that means
        triangle is degenerate, in this case return something with negative coordinates
        u是三角形ABC的法向量
        u.x=AC,PA面积
        u.y=AB,PA面积
        u[2]=u.z是行列式，即AB,AC围起来的平行四边形的面积
    */
    if (std::abs(u[2]) < 1) return Vec3f(-1, 1, 1);               // 三角形太小，返回一个负数
    return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);  // (a,b,c可互换，与顺序无关) 返回重心坐标的权重：三角形内任意一点，能表示成三个顶点的线性组合
}

// 画三角形+zbuffer，lesson 2~3
void triangle(Vec3f *t, float *zbuffer, TGAImage &image, TGAColor color) {
    Vec2f bboxmin(FLOAT_MAX, FLOAT_MAX);    // 左下角；min初始值为max，max初始值为min
    Vec2f bboxmax(-FLOAT_MAX, -FLOAT_MAX);  // 右上角
    Vec2f clamp(image.get_width() - 1, image.get_height() - 1);
    for (int i = 0; i < 3; i++) {
        // 减少屏幕外绘制
        for (int j = 0; j < 2; j++) {
            bboxmin[j] = std::max(0.f, std::min(bboxmin[j], t[i][j]));
            bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], t[i][j]));
        }
    }
    Vec3f P;
    for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
        for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
            // 逐点绘制
            Vec3f bc_screen = centroid(t, P);  // barycentric coordinate
            if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;
            P.z = 0;
            for (int i = 0; i < 3; i++) P.z += t[i][2] * bc_screen[i];  // 点z * 对应点的重心坐标的权重
            if (zbuffer[int(P.x + P.y * width)] < P.z) {
                // z越小越靠近camera
                zbuffer[int(P.x + P.y * width)] = P.z;
                image.set(P.x, P.y, color);
            }
        }
    }
}

int main(int argc, char **argv) {
    if (2 == argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("../head.obj");
    }

    float *zbuffer = new float[width * height];
    for (int i = width * height; i--; zbuffer[i] = -FLOAT_MAX);

    TGAImage image(width, height, TGAImage::RGB);
    Vec3f light_dir(0, 0, -1);
    for (int i = 0; i < model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        Vec2i screen_coords[3];
        Vec3f world_coords[3];  // face的三个顶点
        for (int j = 0; j < 3; j++) {
            Vec3f v = model->vert(face[j]);
            screen_coords[j] = Vec2i((v.x + 1.) * width / 2., (v.y + 1.) * height / 2.);  // -1~1 mapping to 0~width/height
            world_coords[j] = v;
        }
        Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);  // 法向量
        n.normalize();
        float intensity = n * light_dir;  // 光照强度
        if (intensity > 0) {
            triangle(world_coords, zbuffer, image, TGAColor(intensity * 255, intensity * 255, intensity * 255, 255));
        }
        // ​ 注意，口腔的内部被绘制到了嘴唇的顶部。只是因为我们对不可见三角形的拙劣裁剪，只能在凸多边形上才能正常工作。我们会在写出 z-buffer 后处理掉这个阴影。
    }

    image.flip_vertically();
    image.write_tga_file("output.tga");
    delete model;
    return 0;
}
