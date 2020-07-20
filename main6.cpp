#include "raytracer.h"

#include "hittablelist.hpp"
#include "sphere.hpp"
#include "camera.hpp"
#include "material.hpp"
#include "movingsphere.hpp"
#include "texture.hpp"
#include "aarect.hpp"
#include "bvh.hpp"

Color ray_color(const Ray &r, const Color &background, const Hittable &world, int depth)
{
    hit_record rec;
    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth < 0)
        return Color(0, 0, 0);
    //use eps instead of 0. This gets rid of the shadow acne problem.
    if (!world.hit(r, eps, infinity, rec))
        return background;

    Ray scattered;
    Color attenutaion;
    Color emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

    if (!rec.mat_ptr->scatter(r, rec, attenutaion, scattered))
        return emitted;

    return emitted + attenutaion * ray_color(scattered, background, world, depth - 1);
}

HittableList cornell_box()
{
    HittableList objects;

    auto red = make_shared<Lambertian>(make_shared<SolidColor>(.65, .05, .05));
    auto white = make_shared<Lambertian>(make_shared<SolidColor>(.73, .73, .73));
    auto green = make_shared<Lambertian>(make_shared<SolidColor>(.12, .45, .15));
    auto light = make_shared<DiffuseLight>(make_shared<SolidColor>(15, 15, 15));

    objects.add(make_shared<FlipFace>(make_shared<YZRect>(0, 555, 0, 555, 555, green)));
    objects.add(make_shared<YZRect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<XZRect>(213, 343, 227, 332, 554, light));
    objects.add(make_shared<FlipFace>(make_shared<XZRect>(0, 555, 0, 555, 0, white)));
    objects.add(make_shared<XZRect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<FlipFace>(make_shared<XYRect>(0, 555, 0, 555, 555, white)));

    HittableList world;
    world.add(make_shared<BVHnode>(objects, 0, 1));

    return world;
}

int main()
{
    // const auto aspect_ratio = 16.0 / 9.0;
    const auto aspect_ratio = 1.0;
    const int image_width = 576; //384
    // const int image_width = 384;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 150;
    const int max_depth = 50;

    std::cout << "P3\n"
              << image_width << ' ' << image_height << "\n255\n";

    HittableList world = cornell_box();

    Point3 lookfrom(278, 278, -800);
    Point3 lookat(278, 278, 0);
    Vec3 vup(0, 1, 0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.0;
    auto vfov = 40.0;

    Camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, 0, 1);

    const Color background(0, 0, 0);

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

                pixel_color += ray_color(r, background, world, max_depth);
            }
            write_color(std::cout, pixel_color, samples_per_pixel);
        }
    }

    std::cerr << "\nDone.\n";
}