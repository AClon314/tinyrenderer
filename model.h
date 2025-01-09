#ifndef __MODEL_H__  // 防止model.h被重复引用，类似python
#define __MODEL_H__

#include <vector>

#include "geometry.h"

// obj: f v/vt/vn
struct fIndex {
    int v;   // vertex idx
    int vt;  // uv纹理坐标idx
    int vn;  // normal idx
};

class Model {
   private:
    std::vector<Vec3f> verts_;
    std::vector<std::vector<fIndex> > faces_;
    std::vector<Vec3f> uvs_;

   public:
    Model(const char *filename);
    ~Model();
    int nverts();
    int nfaces();
    Vec3f vert(int i);
    Vec3f uv(int i);
    std::vector<fIndex> face(int idx);
};

#endif  //__MODEL_H__