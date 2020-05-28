#include <cstdio>
#include <vector>
#include <optional>
#include <limits>

#include "ppma_io.hpp"
#include "vector.hpp"
#include "color.hpp"
#include "loader.hpp"
#include "triangle.hpp"

struct Camera {
    Vector3 pos;
};

struct Light {
    Vector3 pos;
    double intensity;
};

struct Ray {
    Vector3 from;
    Vector3 dir;
};


/*
 * Returns the light intensity as a function of:
 * - normal: The normal of the surface at the intersection point.
 * - light_dir: A vector pointing towards the point light.
 * - camera_vector: A vector pointing towards the camera used for specular
 *   lighting.
 * - specular_parameter: Specifies the exponent in the specular equation.
 */
double light_intensity(const Vector3& normal, const Vector3& light_dir,
        const Vector3& camera_vector, double specular_parameter) {

    double cos_angle = dot(normalized(normal), normalized(light_dir));
    
    // Ambient light
    double lighting = 0.2;

    // Diffuse light
    lighting += (cos_angle >= 0.0 ? cos_angle : 0.0);

    // Specular light
    float s = specular_parameter;
    Vector3 R = (normal*2*dot(normal, light_dir)) - light_dir;
    double numerator = dot(R, camera_vector);
    double denominator = (length(R) * length(camera_vector));
    if(numerator >= 0) {
        double tmp = std::pow(numerator / denominator, s); 
        lighting += tmp;
    }
    return lighting;
}


std::optional<std::pair<Vector3, Vector3>> intersects_triangle(const Ray& ray, 
        const Triangle& t) {
    Vector3 normal = normalized(cross((t.v1 - t.v0), (t.v2 - t.v0)));
    double D = dot(normal, t.v0);
    double s = (D - dot(normal, ray.from)) / dot(normal, ray.dir);
    
    Vector3 P = ray.dir*s + ray.from;

    Vector3 e0 = t.v1 - t.v0;
    Vector3 e1 = t.v2 - t.v1;
    Vector3 e2 = t.v0 - t.v2;
    Vector3 C0 = P - t.v0; 
    Vector3 C1 = P - t.v1; 
    Vector3 C2 = P - t.v2; 

    // If inside triangle
    if (dot(normal, cross(e0, C0)) > 0 &&
        dot(normal, cross(e1, C1)) > 0 &&
        dot(normal, cross(e2, C2)) > 0 &&
        s > 0.000001) {

        return std::optional<std::pair<Vector3, Vector3>>({P, normal});
    }
    return std::nullopt;
}

// double light_intensity(const Vector3& normal, const Vector3& light_dir,
//         const Vector3& camera_vector, double specular_parameter) {
    
Color cast_ray(const Ray& ray, const std::vector<Triangle> &triangles, 
        const Light& l, int recursion_depth) {

    std::pair<Color, double> c = {{0, 0, 0}, 
        std::numeric_limits<double>::max()};
    for(auto &t : triangles) {
        auto intersection = intersects_triangle(ray, t);
        if(intersection) {
            auto P = intersection.value().first;
            auto N = intersection.value().second;
            auto new_dir = N*dot(ray.dir, N)*2 - ray.dir;

            // auto reflected_color = cast_ray({P, new_dir}, triangles, l, 
            //         recursion_depth+1);

            auto color = t.m.color * light_intensity(N, (l.pos - P), 
                    ray.from - P, 100);

            auto s = length(P-ray.from);
            if (s < c.second) {
                c = {
                    color,
                    s
                };
            }
        }
    }
    return c.first;
}

double frand(double min, double max) {
    double f = static_cast<double>(rand())/RAND_MAX;
    return min + f * (max - min);
}

int main(int argc, char **argv) {

    const int W = 400;
    const int H = 400;
    const int max_color = 255;

    Light light = {{0, 0, 0.0}, 0.5};

    Material r = {{0.5, 0.0, 0.0}, 1.0, 1.0};
    
    auto triangles = load_obj("../models/pillar.obj");

    int red[W*H];
    int green[W*H];
    int blue[W*H];

    int i = 0;
    for(int x = 0; x < W; x++) {
        for(int y = 0; y < H; y++) {
            double dx = static_cast<double>(x)/W - 0.5;
            double dy = static_cast<double>(H-y)/H - 0.5;
            const Vector3 from_vector = {dx, dy, 0};

            // this defines atan(1/2) fov
            const Ray ray = {from_vector, {dx, dy, -1}};

            // cast the ray from this pixel
            Color c = cast_ray(ray, triangles, light, 0);
            c = color_clamped(c);

            red[x + y*W] = static_cast<int>(c.r*max_color);
            green[x + y*W] = static_cast<int>(c.g*max_color);
            blue[x + y*W] = static_cast<int>(c.b*max_color);
            
            if(y % (H/40) == 0) {
                printf("#");
                fflush(stdout);
            }
        }
        printf("\t%i\n", x);
    }
    red[0] = 255;
    green[0] = 255;
    blue[0] = 255;
    ppma_write("out.ppm", W, H, red, green, blue);
    return 0;
}
