#include "tgaimage.h"
/// gprof O0优化，代码质量与执行时间无明显关系
/// 优化，就是将易理解的短代码，转为难理解的、或更长或更短代码。所以不要太早优化。

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);

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

int main(int argc, char** argv) {
    TGAImage image(100, 100, TGAImage::RGB);

    for (int i=1; i<=1000000; i++) {
        line(13, 20, 80, 40, image, white); 
        line(20, 13, 40, 80, image, red); 
        line(80, 40, 13, 20, image, red);
    }

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    return 0;
}

