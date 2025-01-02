#include "tgaimage.h"
#include "geometry.h"
/// gprof O0优化，代码质量与执行时间无明显关系
/// 优化，就是将易理解的短代码，转为难理解的、或更长或更短代码。所以不要太早优化。

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor black = TGAColor(0,   0,   0,   255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green = TGAColor(0,   255, 0,   255);
const TGAColor blue  = TGAColor(0,   0,   255, 255);
const TGAColor rgb[] = {red, green, blue};

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    bool steep = false;
    if (std::abs(x0-x1) < std::abs(y0-y1)){
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int dx = x1-x0;
    int dy = y1-y0;
    // float derror = std::abs(dy)/(float)(dx);     /// x每增加1，y的增量；注意abs(y)，因为swap是保证x是递增的
    int derror2 = std::abs(dy)*2;                   /// 所有浮点数，都可以转为整数
    // float error = 0;
    int error = 0;
    int y = y0;
    for (int x=x0; x<=x1; x++) {
        /// 逐像素绘制

        // float t = (x-x0)/(float)(x1-x0);
        // int y = y0*(1.-t) + y1*t;
        if (steep){
            image.set(y, x, color);
        } else {
            image.set(x, y, color);
        }
        error += derror2;
        if (error > dx) {
            y += (y1>y0?1:-1);
            error -= dx*2;  /// 将所有涉及到的浮点数*dx*2
        }
    }
}

// 枚举一个包围盒中所有的像素，找到在三角形内的像素，计算该像素的重心坐标。如果有分量是负数，那这个像素就不在三角形内。
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

int main(int argc, char** argv) {
    TGAImage image(200, 200, TGAImage::RGB);
    Vec2i pts[3] = {Vec2i(10, 70),   Vec2i(50, 170),  Vec2i(70, 80)};
    triangle(pts, image, red);

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    return 0;
}

