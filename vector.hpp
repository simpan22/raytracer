#pragma once
#include <cmath>

struct Vector3 {
    double x;
    double y;
    double z;
};

Vector3 operator+(const Vector3 &a, const Vector3 &b) {
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

Vector3 operator-(const Vector3 &a, const Vector3 &b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

Vector3 operator/(const Vector3 &a, const double b) {
    return {a.x/b, a.y/b, a.z/b};
}

Vector3 operator*(const Vector3 &a, const double b) {
    return {a.x*b, a.y*b, a.z*b};
}

double vector_length(const Vector3 &v) {
    return std::sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

double vector_dot(const Vector3 &a, const Vector3 b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

Vector3 vector_normalized(const Vector3 &v) {
    return v/vector_length(v); 
}

void print_vector(const Vector3& v) {
    printf("(%f, %f, %f)\n", v.x, v.y, v.z);
}
