#include <cstdio>
#include <vector>
#include <cmath>
#include <optional>

#include "ppma_io.hpp"

struct Vector2 {
    float x;
    float y;
};

struct Vector3 {
    float x;
    float y;
    float z;
};

struct Color {
    float r;
    float g;
    float b;
};

struct Ball {
    Vector3 pos;
    float radius;
    Color color;
};

/* Always looks in the negative z direction */
struct Camera {
    Vector3 pos;
    Vector2 size;
};

struct Light {
    Vector3 pos;
    Color color;
};

struct Ray {
    Vector3 from;
    Vector3 dir;
};

Color color_clamped(const Color &a) {
    Color c;
    c.r = a.r <= 1.0f ? a.r : 1.0f;
    c.g = a.g <= 1.0f ? a.g : 1.0f;
    c.b = a.b <= 1.0f ? a.b : 1.0f;

    c.r = c.r >= 0.0f ? c.r : 0.0f;
    c.g = c.g >= 0.0f ? c.g : 0.0f;
    c.b = c.b >= 0.0f ? c.b : 0.0f;
    return c;
}

Color operator+(const Color &a, const Color &b) {
    return {a.r + b.r, a.g + b.g, a.b + b.b};
}

Color operator-(const Color &a, const Color &b) {
    return {a.r - b.r, a.g - b.g, a.b - b.b};
}

Color operator/(const Color &a, const float b) {
    return {a.r/b, a.g/b, a.b/b};
}

Color operator*(const Color &a, const float b) {
    return {a.r*b, a.g*b, a.b*b};
}

Vector3 operator+(const Vector3 &a, const Vector3 &b) {
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

Vector3 operator-(const Vector3 &a, const Vector3 &b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

Vector3 operator/(const Vector3 &a, const float b) {
    return {a.x/b, a.y/b, a.z/b};
}

Vector3 operator*(const Vector3 &a, const float b) {
    return {a.x*b, a.y*b, a.z*b};
}

float vector_length(const Vector3 &v) {
    return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
}

float vector_dot(const Vector3 &a, const Vector3 b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

Vector3 vector_normalized(const Vector3 &v) {
    return v/vector_length(v); 
}

void print_vector(const Vector3& v) {
    printf("(%f, %f, %f)\n", v.x, v.y, v.z);
}

std::optional<Vector3> intersects_ball(const Ray& ray, const Ball& ball) {
    Vector3 norm_dir = vector_normalized(ray.dir);
    float a = vector_length(norm_dir);
    a = a * a;
    float b = -2 * vector_dot(norm_dir, ball.pos - ray.from);
    float c = vector_length(ball.pos - ray.from); 
    c = c*c - (ball.radius*ball.radius);

    if(b*b - 4*a*c > 0) {
        float t = (-b - sqrtf(b*b - 4*a*c)) / 2*a;
        Vector3 intersection_point = (norm_dir * t) + ray.from;
        return std::optional<Vector3>(intersection_point);
    }
    return std::nullopt;
}

Color cast_ray(const Ray& ray,
        const std::vector<Ball> &balls,
        const std::vector<Light> &lights) {

    auto ball = balls[0]; // TODO: Handle more than one ball
    auto light = lights[0]; // TODO: Handle more than one light 
    auto intersection = intersects_ball(ray, ball);

    if (intersection) {
        Vector3 intersection_point = intersection.value();
        Vector3 normal_vector = vector_normalized(intersection_point - ball.pos);
        Vector3 light_dir = vector_normalized(light.pos - intersection_point);
        float dot = vector_dot(normal_vector, light_dir);
        if(dot >= 0) { 
            auto blended = Color({
                    0.5f*ball.color.r + 0.5f*light.color.r,
                    0.5f*ball.color.g + 0.5f*light.color.g,
                    0.5f*ball.color.b + 0.5f*light.color.b
                    });
            return ball.color * 0.1f + (blended * dot) * 0.9f;
        } else {
            return ball.color * 0.1f;
        }
    } else {
        return Color({0.2f, 0.2f, 0.2f}); 
    }
}

int main(int argc, char **argv) {
    const int W = 640;
    const int H = 640;

    Light l1 = {{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}};
    Ball b1 = {{0.0f, 0.0f, -1.0f}, 0.3f, {0.0f, 1.0f, 0.0f}};

    std::vector<Ball> balls = {b1};
    std::vector<Light> lights = {l1};


    int red[W*H];
    int green[W*H];
    int blue[W*H];
    for(int x = 0; x < W; x++) {
        for(int y = 0; y < H; y++) {
            const Vector3 from_vector = {
                static_cast<float>(x)/W - 0.5f, 
                static_cast<float>(H-y)/H - 0.5f, 
                0};
            const Ray ray = {from_vector, {0, 0, -1}};

            // cast the ray from this pixel
            Color c = cast_ray(ray, balls, lights);
            // c = color_clamped(c);

            red[x + y*W] = static_cast<int>(c.r*255.0f);
            green[x + y*W] = static_cast<int>(c.g*255.0f);
            blue[x + y*W] = static_cast<int>(c.b*255.0f);
        }
    }
    ppma_write("test.ppm", W, H, red, green, blue);
    return 0;
}
