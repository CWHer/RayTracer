#include "raytracer.h"

#include "hittablelist.hpp"
#include "sphere.hpp"
#include "camera.hpp"
#include "material.hpp"
#include "movingsphere.hpp"
#include "texture.hpp"
#include "bvh.hpp"
#include <ctime>

void write_ori(std::ostream &out, Color pixel_color, int samples_per_pixel)
{
    //multi-sampled color computation
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();
    // Replace NaN components with zero. See explanation in Ray Tracing: The Rest of Your Life.
    if (r != r)
        r = 0.0;
    if (g != g)
        g = 0.0;
    if (b != b)
        b = 0.0;

    // Divide the color by the number of samples and gamma-correct for gamma=2.0.
    auto scale = 1.0 / samples_per_pixel;
    r = scale * r;
    g = scale * g;
    b = scale * b;

    // Write the translated [0,255] value of each color component.
    out << static_cast<int>(256 * clamp(r, 0, 1 - eps)) << ' '
        << static_cast<int>(256 * clamp(g, 0, 1 - eps)) << ' '
        << static_cast<int>(256 * clamp(b, 0, 1 - eps)) << '\n';
}

inline double random_double_seed()
{
    // Returns a random real in [0,1).
    static std::random_device rd;
    static std::mt19937 generator(20000905 << 2); //Mersenne Twister 19937 generator
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    return distribution(generator);
}

inline double random_double_seed(double min, double max)
{
    // Returns a random real in [min,max).
    return min + (max - min) * random_double_seed();
}

inline static Vec3 random_seed_Vec3(double min, double max)
{
    return Vec3(random_double_seed(min, max), random_double_seed(min, max), random_double_seed(min, max));
}

inline static Vec3 random_seed_Vec3()
{
    return Vec3(random_double_seed(), random_double_seed(), random_double_seed());
}

Color ray_color(const Ray &r, const Hittable &world, int depth)
{
    hit_record rec;
    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth < 0)
        return Color(0, 0, 0);
    //use eps instead of 0. This gets rid of the shadow acne problem.
    if (!world.hit(r, eps, infinity, rec))
    {
        Vec3 unit_direction = unit_vector(r.direction());
        auto t = 0.5 * (unit_direction.y() + 1.0);
        return (1.0 - t) * Color(1.0, 1.0, 1.0) + t * Color(0.5, 0.7, 1.0);
    }

    Ray scattered;
    Color attenutaion;
    Color emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

    if (!rec.mat_ptr->scatter(r, rec, attenutaion, scattered))
        return emitted;

    return emitted + attenutaion * ray_color(scattered, world, depth - 1);
}

double random_r(double mn, double mx)
{
    return (double)rand() / RAND_MAX * (mx - mn) + mn;
}
HittableList random_scene()
{
    HittableList objects;
    auto di = 0.6;
    for (int a = -6; a < 6; a++)
    {
        for (int b = -6; b < 6; b++)
        {
            for (int c = -2; c < 2; ++c)
            {
                auto choose_mat = random_double_seed();
                Point3 center(a * 1.0 / di + 0.4 * random_double_seed(),
                              b * 1.0 / di + 0.4 * random_double_seed(),
                              c * 1.0 / di + 0.4 * random_double_seed());
                shared_ptr<Material> sphere_material;

                auto r = random_r(0.5, 2) * 0.2;
                if (choose_mat < 0.9)
                {
                    // diffuse
                    auto emit = make_shared<SolidColor>(random_seed_Vec3());
                    sphere_material = make_shared<DiffuseLight>(emit);
                    objects.add(make_shared<Sphere>(center, 0.2, sphere_material));
                }
                else if (choose_mat < 0.95)
                {
                    // metal
                    auto albedo = random_seed_Vec3(0.5, 1);
                    auto fuzz = random_double_seed(0, 0.5);
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
        }
    }

    HittableList world;
    world.add(make_shared<BVHnode>(objects, 0, 1));

    return world;
}

int main()
{
    double t = std::clock();

    const auto aspect_ratio = 1.0;
    const int image_width = 3000;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 2000;
    const int max_depth = 50;

    std::cout << "P3\n"
              << image_width << ' ' << image_height << "\n255\n";

    HittableList world = random_scene();

    Point3 lookfrom(13, 2, 3);
    Point3 lookat(0, 0, 0);
    Vec3 vup(0, 1, 0);
    auto dist_to_focus = (lookfrom - lookat).length();
    auto aperture = 0.05;

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
            write_ori(std::cout, pixel_color, samples_per_pixel);
        }
    }

    std::cerr << "\nDone.\n";
    std::cerr << (std::clock() - t) / CLOCKS_PER_SEC;
}
