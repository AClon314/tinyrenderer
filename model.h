#ifndef __MODEL_H__  // 防止model.h被重复引用，类似python
#define __MODEL_H__

#include <vector>

#include "geometry.h"

class Model {
   private:
    std::vector<Vec3f> verts_;
    std::vector<std::vector<int> > faces_;
    std::vector<Vec3f> uvs_;

   public:
    Model(const char *filename);
    ~Model();
    int nverts();
    int nfaces();
    Vec3f vert(int i);
    Vec3f uv(int i);
    std::vector<int> face(int idx);
};

#endif  //__MODEL_H__