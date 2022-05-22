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
#include "../heart.hpp"

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

HittableList randomScene()
{
    HittableList objects;

    auto checker = make_shared<CheckerTexture>(
        make_shared<SolidColor>(0.2, 0.3, 0.1),
        make_shared<SolidColor>(0.9, 0.9, 0.9));
    auto ground_material = make_shared<Lambertian>(checker);
    objects.add(make_shared<Sphere>(
        Point3(0, -1000, 0), 1000, ground_material));

    auto d = 1.1;
    for (int a = -21; a < 21; a++)
        for (int b = -21; b < 21; b++)
        {
            auto choose_mat = randomReal();
            Point3 center(a * 1.0 / d + 0.4 * randomReal(),
                          0.2, b * 1.0 / d + 0.4 * randomReal());
            if ((center - Point3(0, 0.5, 0)).length() > 0.8)
            {
                shared_ptr<Material> sphere_material;

                if (choose_mat < 0.1)
                {
                    // diffuse
                    auto albedo = Vec3::random() * Vec3::random();
                    sphere_material = make_shared<Lambertian>(albedo);
                    objects.add(make_shared<Sphere>(center, 0.2, sphere_material));
                }
                else if (choose_mat < 0.15)
                {
                    // metal
                    auto albedo = Vec3::random(0.5, 1);
                    auto fuzz = randomReal(0, 0.5);
                    sphere_material = make_shared<Metal>(albedo, fuzz);
                    objects.add(make_shared<Sphere>(center, 0.2, sphere_material));
                }
                else if (choose_mat < 0.45)
                {
                    // glass
                    sphere_material = make_shared<Dielectric>(1.5);
                    objects.add(make_shared<Sphere>(center, 0.2, sphere_material));
                }
                else
                {
                    // emit
                    auto emit = make_shared<SolidColor>(Vec3::random());
                    sphere_material = make_shared<DiffuseLight>(emit);
                    objects.add(make_shared<Sphere>(center, 0.2, sphere_material));
                }
            }
        }

    auto sphere_material = make_shared<Dielectric>(1.5);
    objects.add(make_shared<Sphere>(
        Point3(0, 1.1, 0), 1, sphere_material));

    auto red = make_shared<DiffuseLight>(Color(.65, .05, .05));
    auto heart = make_shared<Heart>(Point3(0, 1, 0), red, 0.5);
    objects.add(heart);

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
    const Color background(0, 0, 0);
    HittableList world = randomScene();

    // Camera
    Point3 lookfrom(18, 4, 5);
    Point3 lookat(0, 0, 0);
    Vec3 vup(0, 1, 0);
    auto dist_to_focus = (lookfrom - lookat).length();
    auto aperture = 0.1;

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
    std::cerr << (std::clock() - t) / CLOCKS_PER_SEC << "s\n";

    return 0;
}
