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

Vector3 operator-(const Vector3 &a) {
    return {0 - a.x, 0 - a.y, 0 - a.z};
}

Vector3 operator/(const Vector3 &a, const double b) {
    return {a.x/b, a.y/b, a.z/b};
}

Vector3 operator*(const Vector3 &a, const double b) {
    return {a.x*b, a.y*b, a.z*b};
}

double length(const Vector3 &v) {
    return std::sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

double dot(const Vector3 &a, const Vector3 b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

Vector3 cross(const Vector3& a, const Vector3& b) {
    return {a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x}; 
}

Vector3 normalized(const Vector3 &v) {
    return v/length(v); 
}

void print_vector(const Vector3& v) {
    printf("(%f, %f, %f)\n", v.x, v.y, v.z);
}
