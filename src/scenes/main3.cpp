#include "../raytracer.h"

#include "../hittable_list.hpp"
#include "../sphere.hpp"
#include "../camera.hpp"
#include "../material.hpp"
#include "../moving_sphere.hpp"
#include "../texture.hpp"
#include "../aarect.hpp"
#include "../box.hpp"
#include "../constant_medium.hpp"
#include "../bvh.hpp"

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

HittableList two_spheres()
{
    HittableList objects;

    auto pertext = make_shared<NoiseTexture>(3);

    objects.add(make_shared<Sphere>(Point3(0, -1000, 0), 1000, make_shared<Lambertian>(pertext)));
    objects.add(make_shared<Sphere>(Point3(0, 2, 0), 2, make_shared<Lambertian>(pertext)));

    HittableList world;
    world.add(make_shared<BVHnode>(objects, 0, 1));

    return world;
}

int main()
{
    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = 576; // 384
    // const int image_width = 384;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 100;
    const int max_depth = 50;

    std::cout << "P3\n"
              << image_width << ' ' << image_height << "\n255\n";

    HittableList world = two_spheres();

    Point3 lookfrom(13, 2, 3);
    Point3 lookat(0, 0, 0);
    Vec3 vup(0, 1, 0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.0;

    Camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus, 0, 1);

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