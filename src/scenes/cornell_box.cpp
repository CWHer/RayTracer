#include "../raytracer.h"
#include "../hittable_list.hpp"
#include "../sphere.hpp"
#include "../camera.hpp"
#include "../material.hpp"
#include "../texture.hpp"
#include "../aarect.hpp"
#include "../box.hpp"
#include "../bvh.hpp"

Color rayColor(const Ray &r, const Color &background, const Hittable &world, int depth)
{
    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth < 0)
        return Color(0, 0, 0);

    HitRecord rec;
    // use 0.001 instead of 0.
    //  This gets rid of the shadow acne problem.
    if (!world.hit(r, 0.001, INF, rec))
        return background;

    Ray scattered;
    Color attenuation;
    Color color = rec.mat_ptr->emitted(rec.u, rec.v, rec.p); // emitted

    if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
        color += attenuation * rayColor(scattered, background, world, depth - 1);
    return color;
}

HittableList cornellBox()
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

    shared_ptr<Hittable> box1 = make_shared<Box>(Point3(0, 0, 0), Point3(165, 330, 165), white);
    box1 = make_shared<RotateY>(box1, 15);
    box1 = make_shared<Translate>(box1, Vec3(265, 0, 295));
    objects.add(box1);

    shared_ptr<Hittable> box2 = make_shared<Box>(Point3(0, 0, 0), Point3(165, 165, 165), white);
    box2 = make_shared<RotateY>(box2, -18);
    box2 = make_shared<Translate>(box2, Vec3(130, 0, 65));
    objects.add(box2);

    HittableList world;
    world.add(make_shared<BVHNode>(objects, 0, 1));

    return world;
}

int main()
{
    // Image
    const auto aspect_ratio = 1.0;
    const int image_width = 600;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 100;
    const int max_depth = 50;

    // World
    const Color background(0, 0, 0);
    HittableList world = cornellBox();

    // Camera
    Point3 lookfrom(278, 278, -800);
    Point3 lookat(278, 278, 0);
    Vec3 vup(0, 1, 0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.0;
    auto vfov = 40.0;

    Camera cam(lookfrom, lookat, vup, vfov,
               aspect_ratio, aperture, dist_to_focus);

    // Render
    std::vector<std::vector<Color>> image(
        image_height, std::vector<Color>(image_width));

    int finished_cnt = 0;
#pragma omp parallel for num_threads(6) schedule(dynamic)
    for (int j = image_height - 1; j >= 0; --j)
    {
        for (int i = 0; i < image_width; ++i)
            for (int s = 0; s < samples_per_pixel; ++s)
            {
                auto u = (i + randomReal()) / (image_width - 1);
                auto v = (j + randomReal()) / (image_height - 1);
                Ray r = cam.getRay(u, v);
                image[j][i] += rayColor(r, background, world, max_depth);
            }
#pragma omp critical
        std::cerr << "\rFinished lines: " << ++finished_cnt << std::flush;
    }

    std::cout << "P3\n"
              << image_width << ' ' << image_height << "\n255\n";

    for (int j = image_height - 1; j >= 0; --j)
        for (int i = 0; i < image_width; ++i)
            writeColor(std::cout, image[j][i], samples_per_pixel);

    std::cerr << "\nDone.\n";

    return 0;
}