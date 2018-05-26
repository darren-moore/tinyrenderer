#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

Model::Model(const char *filename) : verts_(), faces_(), face_tex_(), texCoords_() {
    std::ifstream in;
    in.open (filename, std::ifstream::in);
    if (in.fail()) return;
    std::string line;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            Vec3f v;
            for (int i=0;i<3;i++) iss >> v[i];
            verts_.push_back(v);
        } else if (!line.compare(0, 2, "f ")) {
            std::vector<int> f;
		std::vector<int> t;
            int itrash, idx, tex_idx;
            iss >> trash;
            while (iss >> idx >> trash >> tex_idx >> trash >> itrash) {
                idx--; tex_idx--; // in wavefront obj all indices start at 1, not zero
                f.push_back(idx);
		t.push_back(tex_idx);
            }
            faces_.push_back(f);
		face_tex_.push_back(t);
        }
	else if(!line.compare(0,3, "vt ")){
		Vec2f coords;
		iss >> trash >> trash;
		for(int i=0;i<2;i++) iss >> coords[i];
		texCoords_.push_back(coords);
	}
    }
    std::cerr << "# v# " << verts_.size() << " f# "  << faces_.size() << std::endl;
}

Model::~Model() {
}

int Model::nverts() {
    return (int)verts_.size();
}

int Model::nfaces() {
    return (int)faces_.size();
}

std::vector<int> Model::face(int idx) {
    return faces_[idx];
}

std::vector<int> Model::face_tex(int idx) {
	return face_tex_[idx];
}

Vec3f Model::vert(int i) {
    return verts_[i];
}

Vec2f Model::texCoord(int i) {
	return texCoords_[i];
}


