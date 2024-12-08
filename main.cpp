#include "tgaimage.h"
#include "geometry.h"
#include "algorithm"
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

void triangle(Vec2i t[], TGAImage &image, TGAColor color) {
    std::sort(t, t+3, [](Vec2i a, Vec2i b) { return a.y<b.y; });
    // int total_height = t[2].y-t[0].y;
    for (int i=0; i<=2; i++){
        int x1 = t[(i+1)%3].x;
        int y1 = t[(i+1)%3].y;
        line(t[i].x, t[i].y, x1, y1, image, color);
    }
}

int main(int argc, char** argv) {
    TGAImage image(200, 200, TGAImage::RGB);

    Vec2i t0[3] = {Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80)};
    Vec2i t1[3] = {Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180)};
    Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)};
    triangle(t0, image, red);
    triangle(t1, image, white);
    triangle(t2, image, green);

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    return 0;
}

