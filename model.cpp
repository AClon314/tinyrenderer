#include "model.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

Model::Model(const char *filename) : verts_(), faces_(), uvs_() {
    std::ifstream in;
    in.open(filename, std::ifstream::in);
    if (in.fail()) return;
    std::string line;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;  // 丢弃1个字符
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            Vec3f v;
            for (int i = 0; i < 3; i++) iss >> v[i];
            verts_.push_back(v);
        } else if (!line.compare(0, 2, "f ")) {
            std::vector<fIndex> f;
            int v, vt, vn;
            iss >> trash;
            while (iss >> v >> trash >> vt >> trash >> vn) {
                f.push_back({v - 1, vt - 1, vn - 1});  // in wavefront obj all indices start at 1, not zero
            }
            faces_.push_back(f);
        } else if (!line.compare(0, 3, "vt ")) {
            // vt  0.566 0.146 0.000
            iss >> trash >> trash;
            Vec3f _uv;
            for (int i = 0; i < 3; i++) iss >> _uv[i];
            uvs_.push_back(_uv);
        }
    }
    std::cerr << "# v# " << verts_.size() << " f# " << faces_.size() << " uv#" << uvs_.size() << std::endl;
}

Model::~Model() {
}

int Model::nverts() {
    return (int)verts_.size();
}

int Model::nfaces() {
    return (int)faces_.size();
}

std::vector<fIndex> Model::face(int idx) {
    return faces_[idx];
}

Vec3f Model::vert(int i) {
    return verts_[i];
}

Vec3f Model::uv(int i) {
    return uvs_[i];
}
