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

HittableList finalScene()
{
    HittableList boxes1;
    auto ground = make_shared<Lambertian>(Color(0.48, 0.83, 0.53));

    const int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++)
    {
        for (int j = 0; j < boxes_per_side; j++)
        {
            auto w = 100.0;
            auto x0 = -1000.0 + i * w;
            auto z0 = -1000.0 + j * w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = randomReal(1, 101);
            auto z1 = z0 + w;

            boxes1.add(make_shared<Box>(
                Point3(x0, y0, z0), Point3(x1, y1, z1), ground));
        }
    }

    HittableList objects;

    objects.add(make_shared<BVHNode>(boxes1, 0, 1));

    auto light = make_shared<DiffuseLight>(Color(7, 7, 7));
    objects.add(make_shared<XZRect>(123, 423, 147, 412, 554, light));

    auto center1 = Point3(400, 400, 200);
    auto center2 = center1 + Vec3(30, 0, 0);
    auto MovingSphere_material = make_shared<Lambertian>(Color(0.7, 0.3, 0.1));
    objects.add(make_shared<MovingSphere>(center1, center2, 0, 1, 50, MovingSphere_material));

    objects.add(make_shared<Sphere>(
        Point3(260, 150, 45), 50, make_shared<Dielectric>(1.5)));
    objects.add(make_shared<Sphere>(
        Point3(0, 150, 145), 50, make_shared<Metal>(Color(0.8, 0.8, 0.9), 1.0)));

    auto boundary = make_shared<Sphere>(
        Point3(360, 150, 145), 70, make_shared<Dielectric>(1.5));
    objects.add(boundary);
    objects.add(make_shared<ConstantMedium>(
        boundary, 0.2, make_shared<SolidColor>(0.2, 0.4, 0.9)));
    boundary = make_shared<Sphere>(
        Point3(0, 0, 0), 5000, make_shared<Dielectric>(1.5));
    objects.add(make_shared<ConstantMedium>(
        boundary, .0001, make_shared<SolidColor>(1, 1, 1)));

    auto emat = make_shared<Lambertian>(make_shared<IMGTexture>("earthmap.jpg"));
    objects.add(make_shared<Sphere>(Point3(400, 200, 400), 100, emat));
    auto pertext = make_shared<NoiseTexture>(0.1);
    objects.add(make_shared<Sphere>(Point3(220, 280, 300), 80, make_shared<Lambertian>(pertext)));

    HittableList boxes2;
    auto white = make_shared<Lambertian>(make_shared<SolidColor>(.73, .73, .73));
    int ns = 1000;
    for (int j = 0; j < ns; j++)
        boxes2.add(make_shared<Sphere>(
            Point3::random(0, 165), 10, white));

    objects.add(make_shared<Translate>(
        make_shared<RotateY>(
            make_shared<BVHNode>(boxes2, 0.0, 1.0), 15),
        Vec3(-100, 270, 395)));

    return objects;
}

int main()
{
    double t = std::clock();

    // Image
    const auto aspect_ratio = 1.0;
    const int image_width = 600;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 100;
    const int max_depth = 50;

    // World
    const Color background(0, 0, 0);
    HittableList world = finalScene();

    // Camera
    Point3 lookfrom(478, 278, -600);
    Point3 lookat(278, 278, 0);
    Vec3 vup(0, 1, 0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.0;
    auto vfov = 40.0;

    Camera cam(lookfrom, lookat, vup, vfov,
               aspect_ratio, aperture, dist_to_focus, 0, 1);

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
    std::cerr << (std::clock() - t) / CLOCKS_PER_SEC << "s\n";

    return 0;
}