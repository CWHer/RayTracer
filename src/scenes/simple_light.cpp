#include "../raytracer.h"
#include "../hittable_list.hpp"
#include "../sphere.hpp"
#include "../camera.hpp"
#include "../material.hpp"
#include "../texture.hpp"
#include "../aarect.hpp"

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

HittableList simpleLight()
{
    HittableList objects;

    auto pertext = make_shared<NoiseTexture>(4);
    objects.add(make_shared<Sphere>(Point3(0, -1000, 0), 1000, make_shared<Lambertian>(pertext)));
    objects.add(make_shared<Sphere>(Point3(0, 2, 0), 2, make_shared<Lambertian>(pertext)));

    auto difflight = make_shared<DiffuseLight>(make_shared<SolidColor>(4, 4, 4));
    objects.add(make_shared<Sphere>(Point3(0, 7, 0), 2, difflight));
    objects.add(make_shared<XYRect>(3, 5, 1, 3, -2, difflight));

    return objects;
}

int main()
{
    // Image
    const auto aspect_ratio = 3.0 / 2.0;
    const int image_width = 600;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 100;
    const int max_depth = 50;

    // World
    const Color background(0, 0, 0);
    HittableList world = simpleLight();

    // Camera
    Point3 lookfrom(26, 3, 6);
    Point3 lookat(0, 2, 0);
    Vec3 vup(0, 1, 0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.0;

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