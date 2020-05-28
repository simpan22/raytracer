#pragma once
#include "color.hpp"
#include "vector.hpp"

struct Material {
    Color color;
    double specular_parameter;
    double reflective_parameter;
};

struct Triangle {
    Vector3 v0;
    Vector3 v1;
    Vector3 v2;
    Material m;
};
