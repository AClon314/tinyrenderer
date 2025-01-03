#include "tgaimage.h"
#include "geometry.h"
#include "model.h"
/// gprof O0优化，代码质量与执行时间无明显关系
/// 优化，就是将易理解的短代码，转为难理解的、或更长或更短代码。所以不要太早优化。

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor black = TGAColor(0,   0,   0,   255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green = TGAColor(0,   255, 0,   255);
const TGAColor blue  = TGAColor(0,   0,   255, 255);
const TGAColor rgb[] = {red, green, blue};
Model *model = NULL;
const int width  = 800;
const int height = 800;

// 判断缓/陡，画直线，lesson 1
void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    bool steep = false;
    if (std::abs(x0-x1)<std::abs(y0-y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0>x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    for (int x=x0; x<=x1; x++) {
        float t = (x-x0)/(float)(x1-x0);
        int y = y0*(1.-t) + y1*t;
        if (steep) {
            image.set(y, x, color);
        } else {
            image.set(x, y, color);
        }
    }
}

// 枚举一个包围盒中所有的像素，找到在三角形内的像素，计算该像素的重心坐标。如果有分量是负数，那这个像素就不在三角形内。lesson 2
Vec3f centroid(Vec2i *pts, Vec2i P) {
    // (AB,AC,PA)的x与y分量做叉积，得到垂直于面的向量
    Vec3f u = Vec3f(pts[2][0]-pts[0][0], pts[1][0]-pts[0][0], pts[0][0]-P[0]) ^ Vec3f(pts[2][1]-pts[0][1], pts[1][1]-pts[0][1], pts[0][1]-P[1]);
    /* `pts` and `P` has integer value as coordinates
       so `abs(u[2])` < 1 means `u[2]` is 0, that means
       triangle is degenerate, in this case return something with negative coordinates
       u[2]是行列式，即AB,AC围起来的平行四边形的面积
        */
    if (std::abs(u[2]) < 1) return Vec3f(-1, 1, 1);     // 三角形太小，返回一个负数
    return Vec3f(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z);  // 重心坐标：三角形内任意一点，能表示成三个顶点的线性组合
}

// 画三角形，lesson 2
void triangle(Vec2i t[], TGAImage &image, TGAColor color) {
    Vec2i bboxmin(image.get_width()-1,  image.get_height()-1);  // 左下角；min初始值为max，max初始值为min
    Vec2i bboxmax(0, 0);    // 右上角
    Vec2i clamp=bboxmin;
    for (int i=0; i<3; i++) {
        // 减少屏幕外绘制
        bboxmin.x = std::max(0, std::min(bboxmin.x, t[i].x));
        bboxmin.y = std::max(0, std::min(bboxmin.y, t[i].y));
        bboxmax.x = std::min(clamp.x, std::max(bboxmax.x, t[i].x));
        bboxmax.y = std::min(clamp.y, std::max(bboxmax.y, t[i].y));
    }
    Vec2i P;
    for (P.x=bboxmin.x; P.x<=bboxmax.x; P.x++) {
        for (P.y=bboxmin.y; P.y<=bboxmax.y; P.y++) {
            // 逐点绘制
            Vec3f bc_screen = centroid(t, P);
            if (bc_screen.x<0 || bc_screen.y<0 || bc_screen.z<0) continue;
            image.set(P.x, P.y, color);
        }
    }
}

// 光栅化，lesson 3
void rasterize(Vec2i p0, Vec2i p1, TGAImage &image, TGAColor color, int ybuffer[]) {
    if (p0.x>p1.x) std::swap(p0, p1);   // keep p0.x <= p1.x
    for (int x=p0.x; x<=p1.x; x++) {
        float t = (x-p0.x)/(float)(p1.x-p0.x);  // t∈[0,1]
        int y = p0.y*(1.-t) + p1.y*t;
        if (ybuffer[x]<y) {
            ybuffer[x] = y;
            for (int j=0; j<16; j++)
                image.set(x, j, color);
        }
    }
}

int main(int argc, char** argv) {
    // if (2==argc) {
    //     model = new Model(argv[1]);
    // } else {
    //     model = new Model("../head.obj");
    // }

    int ybuffer[width];
    for (int i=0; i<width; i++) {
        ybuffer[i] = std::numeric_limits<int>::min();
    }

    // TGAImage image(width, height, TGAImage::RGB);
    // line(20, 34, 744, 400, image, red);
    // line(120, 434, 444, 400, image, green);
    // line(330, 463, 594, 200, image, blue);

    TGAImage image(width, 16, TGAImage::RGB);
    rasterize(Vec2i(20, 34),   Vec2i(744, 400), image, red,   ybuffer);
    rasterize(Vec2i(120, 434), Vec2i(444, 400), image, green, ybuffer);
    rasterize(Vec2i(330, 463), Vec2i(594, 200), image, blue,  ybuffer);

    image.flip_vertically();
    image.write_tga_file("output.tga");
    delete model;
    return 0;
}

