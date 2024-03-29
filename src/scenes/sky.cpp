#include "../raytracer.h"
#include "../hittable_list.hpp"
#include "../sphere.hpp"
#include "../camera.hpp"
#include "../material.hpp"
#include "../texture.hpp"
#include "../bvh.hpp"

Color rayColor(const Ray &r, const Hittable &world, int depth)
{

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth < 0)
        return Color(0, 0, 0);
    HitRecord rec;
    // use 0.001 instead of 0.
    //  This gets rid of the shadow acne problem.
    if (!world.hit(r, 0.001, INF, rec))
    {
        Vec3 unit_direction = unitVector(r.direction());
        auto t = 0.5 * (unit_direction.y() + 1.0);
        return (1.0 - t) * Color(1.0, 1.0, 1.0) + t * Color(0.5, 0.7, 1.0);
    }

    Ray scattered;
    Color attenuation;
    Color color = rec.mat_ptr->emitted(rec.u, rec.v, rec.p); // emitted

    if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
        color += attenuation * rayColor(scattered, world, depth - 1);
    return color;
}

HittableList randomScene()
{
    HittableList objects;

    auto d = 0.6;
    for (int a = -6; a < 6; a++)
        for (int b = -6; b < 6; b++)
            for (int c = -2; c < 2; ++c)
            {
                auto choose_mat = randomReal();
                Point3 center(a * 1.0 / d + 0.4 * randomReal(),
                              b * 1.0 / d + 0.4 * randomReal(),
                              c * 1.0 / d + 0.4 * randomReal());
                shared_ptr<Material> sphere_material;

                if (choose_mat < 0.9)
                {
                    // diffuse
                    auto emit = make_shared<SolidColor>(Vec3::random());
                    sphere_material = make_shared<DiffuseLight>(emit);
                    objects.add(make_shared<Sphere>(center, 0.2, sphere_material));
                }
                else if (choose_mat < 0.95)
                {
                    // metal
                    auto albedo = Vec3::random(0.5, 1);
                    auto fuzz = randomReal(0, 0.5);
                    sphere_material = make_shared<Metal>(albedo, fuzz);
                    objects.add(make_shared<Sphere>(center, 0.3, sphere_material));
                }
                else
                {
                    // glass
                    sphere_material = make_shared<Dielectric>(1.5);
                    objects.add(make_shared<Sphere>(center, 0.25, sphere_material));
                }
            }

    HittableList world;
    world.add(make_shared<BVHNode>(objects, 0, 1));

    return world;
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
    HittableList world = randomScene();

    // Camera
    Point3 lookfrom(13, 2, 3);
    Point3 lookat(0, 0, 0);
    Vec3 vup(0, 1, 0);
    auto dist_to_focus = (lookfrom - lookat).length();
    auto aperture = 0.05;

    Camera cam(lookfrom, lookat, vup, 20,
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
                image[j][i] += rayColor(r, world, max_depth);
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
