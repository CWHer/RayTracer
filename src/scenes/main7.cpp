#include "../raytracer.h"
#include "../hittable_list.hpp"
#include "../sphere.hpp"
#include "../camera.hpp"
#include "../material.hpp"

Color ray_color(const Ray &r, const Hittable &world, int depth)
{
    hit_record rec;
    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth < 0)
        return Color(0, 0, 0);
    // use eps instead of 0. This gets rid of the shadow acne problem.
    if (world.hit(r, eps, infinity, rec))
    {
        Ray scattered;
        Color attenutaion;
        if (rec.mat_ptr->scatter(r, rec, attenutaion, scattered))
            return attenutaion * ray_color(scattered, world, depth - 1);
        return Color(0, 0, 0);
    }
    Vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * Color(1.0, 1.0, 1.0) + t * Color(0.5, 0.7, 1.0);
}

int main()
{
    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = 384;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 100;
    const int max_depth = 50;

    std::cout << "P3\n"
              << image_width << ' ' << image_height << "\n255\n";

    HittableList world;
    // world.add(make_shared<Sphere>(
    //     Point3(0, 0, -1), 0.5, make_shared<Lambertian>(Color(0.7, 0.3, 0.3))));
    world.add(make_shared<Sphere>(Point3(0, 0, -1), 0.5, make_shared<Dielectric>(1.5)));
    // use a negative radius, the geometry is unaffected, but the surface normal points inward.
    // This can be used as a bubble to make a hollow glass sphere
    world.add(make_shared<Sphere>(Point3(0, 0, -1), -0.35, make_shared<Dielectric>(1.5)));
    // world.add(make_shared<Sphere>(
    //     Point3(0, 0, -1), 0.5, make_shared<Dielectric>(2.3)));

    world.add(make_shared<Sphere>(
        Point3(0, -100.5, -1), 100, make_shared<Lambertian>(Color(0.8, 0.8, 0.0))));

    world.add(make_shared<Sphere>(Point3(1, 0, -1), 0.5, make_shared<Metal>(Color(0.8, 0.6, 0.2), 0.3)));
    // world.add(make_shared<Sphere>(Point3(-1, 0, -1), 0.5, make_shared<Metal>(Color(0.8, 0.8, 0.8))));
    // world.add(make_shared<Sphere>(Point3(-1, -0.25, -1), 0.25, make_shared<Metal>(Color(0.8, 0.8, 0.8), 0.7)));
    world.add(make_shared<Sphere>(
        Point3(-1, 0, -1), 0.5, make_shared<Lambertian>(Color(0.1, 0.2, 0.5))));

    Camera cam(Point3(0, 0, 0), Point3(0, 0, -1), Vec3(0, 1, 0), 90, aspect_ratio, 0, 1);

    for (int j = image_height - 1; j >= 0; --j)
    {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i)
        {
            Color pixel_color(0, 0, 0);
            for (int k = 0; k < samples_per_pixel; ++k)
            {
                auto u = (i + random_double()) / (image_width - 1);
                auto v = (j + random_double()) / (image_height - 1);
                Ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, world, max_depth);
            }
            write_color(std::cout, pixel_color, samples_per_pixel);
        }
    }

    std::cerr << "\nDone.\n";
}