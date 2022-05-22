#include "../raytracer.h"
#include "../hittable_list.hpp"
#include "../sphere.hpp"
#include "../camera.hpp"
#include "../material.hpp"
#include "../texture.hpp"
#include "../aarect.hpp"
#include "../pdf.hpp"
#include "../box.hpp"

Color rayColor(const Ray &r, const Color &background,
               const Hittable &world, shared_ptr<Hittable> lights, int depth)
{
    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth < 0)
        return Color(0, 0, 0);

    HitRecord rec;
    // use 0.001 instead of 0.
    //  This gets rid of the shadow acne problem.
    if (!world.hit(r, 0.001, INF, rec))
        return background;

    ScatterRecord srec;
    Color emitted = rec.mat_ptr->emitted(r, rec, rec.u, rec.v, rec.p);

    if (!rec.mat_ptr->scatter(r, rec, srec))
        return emitted;

    if (srec.is_specular)
        return srec.attenuation *
               rayColor(srec.specular_ray, background,
                        world, lights, depth - 1);

    auto light_ptr = make_shared<HittablePDF>(lights, rec.p);
    MixturePDF p(light_ptr, srec.pdf_ptr);
    Ray scattered = Ray(rec.p, p.generate(), r.time());
    auto pdf_val = p.value(scattered.direction());

    return emitted + srec.attenuation / pdf_val *
                         rec.mat_ptr->scatteringPDF(r, rec, scattered) *
                         rayColor(scattered, background, world, lights, depth - 1);
}

HittableList cornellBox()
{
    HittableList objects;

    auto red = make_shared<Lambertian>(Color(.65, .05, .05));
    auto white = make_shared<Lambertian>(Color(.73, .73, .73));
    auto green = make_shared<Lambertian>(Color(.12, .45, .15));
    auto light = make_shared<DiffuseLight>(Color(15, 15, 15));

    objects.add(make_shared<FlipFace>(make_shared<YZRect>(0, 555, 0, 555, 555, green)));
    objects.add(make_shared<YZRect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<FlipFace>(make_shared<XZRect>(213, 343, 227, 332, 554, light)));
    objects.add(make_shared<FlipFace>(make_shared<XZRect>(0, 555, 0, 555, 0, white)));
    objects.add(make_shared<XZRect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<FlipFace>(make_shared<XYRect>(0, 555, 0, 555, 555, white)));

    shared_ptr<Material> aluminum = make_shared<Metal>(Color(0.8, 0.85, 0.88), 0.0);
    shared_ptr<Hittable> box1 = make_shared<Box>(Point3(0, 0, 0), Point3(165, 330, 165), aluminum);
    box1 = make_shared<RotateY>(box1, 15);
    box1 = make_shared<Translate>(box1, Vec3(265, 0, 295));
    objects.add(box1);

    auto glass = make_shared<Dielectric>(1.5);
    objects.add(make_shared<Sphere>(Point3(190, 90, 190), 90, glass));

    return objects;
}

int main()
{
    double t = std::clock();

    // Image
    const auto aspect_ratio = 1.0;
    const int image_width = 400;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 1000;
    const int max_depth = 50;

    // World
    auto lights = make_shared<HittableList>();
    lights->add(make_shared<XZRect>(213, 343, 227, 332, 554, shared_ptr<Material>()));
    lights->add(make_shared<Sphere>(Point3(190, 90, 190), 90, nullptr));

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
                image[j][i] += rayColor(r, background, world, lights, max_depth);
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