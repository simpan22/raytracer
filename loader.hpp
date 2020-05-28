#pragma once
#include <fstream>
#include <iostream>

#include "vector.hpp"
#include "triangle.hpp"

struct TriangleIdx {
    int v0;
    int v1;
    int v2;
};

std::vector<Triangle> load_obj(const std::string &filename) {
    std::vector<Vector3> vertex_list;
    std::vector<TriangleIdx> triangle_idx_list;
    std::vector<Triangle> triangle_list;

    std::ifstream file(filename);
    if (!file) {
        std::cout << "Error opening file" << std::endl;
    }
    std::string line;
    while(file) {
        char type;
        file >> type;
        if(type == '#') {
            std::getline(file, line);
        } else {
            if (type == 'v') {
                Vector3 v;
                file >> v.x >> v.y >> v.z;
                vertex_list.push_back(v);
            } else if (type == 'f') {
                TriangleIdx tix;
                file >> tix.v0 >> tix.v1 >> tix.v2;
                triangle_idx_list.push_back(tix);
            } else if (type == 's') {
                std::getline(file, line);
            }
        }
    }
    
    for(auto &tix : triangle_idx_list) {
        Triangle t;
        t.v0 = vertex_list[tix.v0-1];
        t.v1 = vertex_list[tix.v1-1];
        t.v2 = vertex_list[tix.v2-1];
        t.m = {{0.0, 0.5, 0.2}, 100, 0.5};

        triangle_list.push_back(t);
    }
    
    return triangle_list;
}
