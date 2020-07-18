// axis-aligned camera from before
#ifndef __CAMERA__
#define __CAMERA__

#include "raytracer.h"

class Camera
{
private:
    Point3 origin;
    Point3 lower_left_corner;
    Vec3 horizontal;
    Vec3 vertical;

public:
    // Camera()
    // {
    //     const auto aspect_ratio = 16.0 / 9;
    //     auto viewport_height = 2.0;
    //     auto viewport_width = aspect_ratio * viewport_height;
    //     auto focal_length = 1.0;

    //     origin = Point3(0, 0, 0);
    //     horizontal = Vec3(viewport_width, 0, 0);
    //     vertical = Vec3(0, viewport_height, 0);
    //     lower_left_corner = origin - horizontal / 2 - vertical / 2 - Vec3(0, 0, focal_length);
    // }

    Camera(
        Point3 lookfrom,
        Point3 lookat,
        Vec3 vup,
        double vfov, // vertical field-of-view in degrees
        double aspect_ratio)
    {
        auto theta = degrees_to_radians(vfov);
        auto h = tan(theta / 2);
        auto viewport_height = 2 * h;
        auto viewport_width = aspect_ratio * viewport_height;

        auto w = unit_vector(lookfrom - lookat);
        auto u = unit_vector(cross(vup, w));
        auto v = unit_vector(cross(w, u));

        origin = lookfrom;
        horizontal = viewport_width * u;
        vertical = viewport_height * v;
        lower_left_corner = origin - horizontal / 2 - vertical / 2 - w;
    }

    Ray get_ray(double u, double v) const
    { //origin->(u,v)
        return Ray(origin, lower_left_corner + u * horizontal + v * vertical - origin);
    }
};

#endif