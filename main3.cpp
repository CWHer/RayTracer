// Rendering surface normals on a sphere

#include "ray.hpp"
#include "color.hpp"

#include <iostream>
#include <cmath>

const double eps = 1e-8;

double hit_sphere(const point3 &center, double radius, const Ray &r)
{ //not a 2D circle, so discriminat is needed
    Vec3 oc = r.origin() - center;
    auto a = dot(r.direction(), r.direction());
    auto half_b = dot(oc, r.direction());
    auto c = dot(oc, oc) - radius * radius;
    auto discriminant = half_b * half_b - a * c;
    // return (discriminant > eps);
    if (discriminant < eps)
        return -1;
    else
        return (-half_b - std::sqrt(discriminant)) / a; //in this case, '-' should satisfy
}

color ray_color(const Ray &r)
{
    double t = hit_sphere(point3(0, 0, -1), 0.5, r);
    if (t > eps)
    {
        Vec3 norm = unit_vector(r.at(t) - Vec3(0, 0, -1));
        return 0.5 * color(norm.x() + 1, norm.y() + 1, norm.z() + 1);
        //x in [-1,1], x+1 in [0,2], 0.5*(x+1) in [0,1]
    }
    Vec3 unit_direction = unit_vector(r.direction());
    t = 0.5 * (unit_direction.y() + 1.0);                               //t can not reach 1, maxinum of y is ~0.44
    return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0); //blend
}

int main()
{
    std::ios::sync_with_stdio(0);
    const double aspect_ratio = 16.0 / 9.0;
    const int image_width = 384;
    const int image_height = static_cast<int>(image_width / aspect_ratio);

    std::cout << "P3\n"
              << image_width << " " << image_height << "\n255\n";

    auto viewport_height = 2.0;
    auto viewport_width = aspect_ratio * viewport_height;
    auto focal_length = 1.0;

    auto origin = point3(0, 0, 0);
    auto horizontal = Vec3(viewport_width, 0, 0);
    auto vertical = Vec3(0, viewport_height, 0);
    auto lower_left_corner = origin - horizontal / 2 - vertical / 2 - Vec3(0, 0, focal_length);

    for (int j = image_height - 1; j >= 0; --j)
    {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i)
        {
            auto u = double(i) / (image_width - 1);
            auto v = double(j) / (image_height - 1);
            Ray r(origin, lower_left_corner + u * horizontal + v * vertical - origin); //the length of r.dir varies, so there is horizontal gradient too.
            color pixel_color = ray_color(r);
            write_color(std::cout, pixel_color);
        }
    }

    std::cerr << "\nDone.\n";
}