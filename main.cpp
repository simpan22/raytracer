#include <cstdio>
#include <vector>
#include <optional>

#include "ppma_io.hpp"
#include "vector.hpp"
#include "color.hpp"

struct Ball {
    Vector3 pos;
    double radius;
    Color ambient;
    Color diffuse;
};

struct Camera {
    Vector3 pos;
};

struct Light {
    Vector3 pos;
    Color color;
};

struct Ray {
    Vector3 from;
    Vector3 dir;
};

std::optional<Vector3> intersects_ball(const Ray& ray, const Ball& ball) {
    Vector3 norm_dir = vector_normalized(ray.dir);
    double a = vector_length(norm_dir);
    a = a * a;
    double b = -2 * vector_dot(norm_dir, ball.pos - ray.from);
    double c = vector_length(ball.pos - ray.from); 
    c = c*c - (ball.radius*ball.radius);

    if(b*b - 4*a*c > 0) {
        double t = (-b - sqrt(b*b - 4*a*c)) / 2*a;
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
        double dot = vector_dot(normal_vector, light_dir);

        Color diffuse = {0.0, 0.0, 0.0};
        if(dot >= 0) { 
            diffuse = ball.diffuse * dot; 
        }
        return color_linear_interpolate(diffuse, ball.ambient, 0.8);
    } else {
        return Color({0.2, 0.2, 0.2}); 
    }
}

int main(int argc, char **argv) {
    const int W = 800;
    const int H = 800;
    
    const int max_color = 255;

    Light l1 = {{1.0, 1.0, 0.0}};
    Ball b1 = {
        {0.0, 0.0, -1.0}, 
        0.3f, 
        {0.1, 0.1, 0.1},
        {0.1, 0.1, 1.0}
    };

    std::vector<Ball> balls = {b1};
    std::vector<Light> lights = {l1};

    int red[W*H];
    int green[W*H];
    int blue[W*H];
    for(int x = 0; x < W; x++) {
        for(int y = 0; y < H; y++) {
            const Vector3 from_vector = {
                static_cast<double>(x)/W - 0.5, 
                static_cast<double>(H-y)/H - 0.5, 
                0};
            const Ray ray = {from_vector, {0, 0, -1}};

            // cast the ray from this pixel
            Color c = cast_ray(ray, balls, lights);
            // c = color_clamped(c);

            red[x + y*W] = static_cast<int>(c.r*max_color);
            green[x + y*W] = static_cast<int>(c.g*max_color);
            blue[x + y*W] = static_cast<int>(c.b*max_color);
        }
    }
    red[0] = 255;
    green[0] = 255;
    blue[0] = 255;
    ppma_write("test.ppm", W, H, red, green, blue);
    return 0;
}
