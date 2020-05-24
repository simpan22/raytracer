#include <cstdio>
#include <vector>
#include <optional>

#include "ppma_io.hpp"
#include "vector.hpp"
#include "color.hpp"

struct Ball {
    Vector3 pos;
    double radius;
    Color color;
    double specular_parameter;
};

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
 * Standard algorithm for intersection between line and sphere.
 * Returns optional which has a value if they intersected containing the 
 * parameter of the ray t.
 */
std::optional<double> intersects_ball(const Ray& ray, const Ball& ball) {
    Vector3 norm_dir = vector_normalized(ray.dir);
    double a = vector_length(norm_dir);
    a = a * a;
    double b = -2 * vector_dot(norm_dir, ball.pos - ray.from);
    double c = vector_length(ball.pos - ray.from); 
    c = c*c - (ball.radius*ball.radius);

    if(b*b - 4*a*c > 0) {
        double t = (-b - sqrt(b*b - 4*a*c)) / 2*a;
        if(t < 0.00001) // dont intersect one self
            return std::nullopt;
        return std::optional<double>(t);
    }
    return std::nullopt;
}

double light_intensity(const Vector3& normal, const Vector3& light_dir,
        const Vector3& camera_vector, double specular_parameter) {
    double cos_angle = vector_dot(vector_normalized(normal), vector_normalized(light_dir));
    
    // Ambient light
    double lighting = 0.2;

    // Diffuse light
    lighting += (cos_angle >= 0.0 ? cos_angle : 0.0);

    // Specular light
    float s = specular_parameter;
    Vector3 R = (normal*2*vector_dot(normal, light_dir)) - light_dir;
    double numerator = vector_dot(R, camera_vector);
    double denominator = (vector_length(R) * vector_length(camera_vector));
    if(numerator >= 0) {
        double tmp = std::pow(numerator / denominator, s); 
        lighting += tmp;
    }
    return lighting;
}


Color cast_ray(const Ray& ray,
        const std::vector<Ball> &balls,
        const std::vector<Light> &lights, int recursion_depth) {

    auto light = lights[0]; // TODO: Handle more than one light 

    // t is ray parameter of the intersection point.
    double closest_t = 999999;
    Color return_color;
    for (auto &ball : balls) {
        auto intersection = intersects_ball(ray, ball);
        if (intersection) {
            double t = intersection.value();
            // Only redraw pixel if this ball is closer (smaller t)
            if (t < closest_t) {
                Vector3 intersection_point = (vector_normalized(ray.dir) * t) + ray.from;
                Vector3 normal_vector = vector_normalized(intersection_point - ball.pos);
                Vector3 light_dir = light.pos - intersection_point;
                Vector3 camera_vector = vector_normalized(ray.from - intersection_point);
                Color local_color = ball.color * light.intensity * light_intensity(
                        vector_normalized(normal_vector), 
                        light_dir, 
                        camera_vector, 
                        ball.specular_parameter);

                Ray reflected = {
                    intersection_point, 
                    normal_vector * (vector_dot(normal_vector, camera_vector) * 2) - camera_vector,
                };

                Color reflected_color = {0.0, 0.0, 0.0};
                if (recursion_depth < 2) {
                    reflected_color = cast_ray(reflected, balls, lights, recursion_depth+1);
                    return_color = color_linear_interpolate(local_color, reflected_color, 0.8);
                } else {
                    return_color = local_color;
                }


                closest_t = t;
            }
        }    
    }
    // If no objects closer than 10000 units draw background
    if(closest_t >= 10000) {
        return Color({0.2, 0.2, 0.2});
    }
    return return_color;
}

double frand(double min, double max) {
    double f = static_cast<double>(rand())/RAND_MAX;
    return min + f * (max - min);
}

int main(int argc, char **argv) {
    const int W = 800;
    const int H = 800;
    
    const int max_color = 255;

    Light l1 = {{1.0, 1.0, 0.0}, 0.5};

    std::vector<Ball> balls;
    std::vector<Light> lights = {l1};

    for(int i = 0; i < 30; i++) {
        
        Ball b1 = {
            {(frand(-0.5, 0.5)), frand(-0.5, 0.5), frand(-3, -0.2)}, 
            frand(0.01, 0.2), 
            {frand(0, 1), frand(0,1), frand(0,1)},
            10
        };
        balls.push_back(b1);
    }

    int red[W*H];
    int green[W*H];
    int blue[W*H];
    for(int x = 0; x < W; x++) {
        for(int y = 0; y < H; y++) {
            double dx = static_cast<double>(x)/W - 0.5;
            double dy = static_cast<double>(H-y)/H - 0.5;
            const Vector3 from_vector = {dx, dy, 0};

            const Ray ray = {from_vector, 
                {
                    dx, 
                    dy, 
                    -1
                }
            };

            // cast the ray from this pixel
            Color c = cast_ray(ray, balls, lights, 0);
            c = color_clamped(c);

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
