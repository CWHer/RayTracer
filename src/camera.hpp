// axis-aligned camera from before
#pragma once

#include "raytracer.h"

class Camera
{
private:
    Point3 origin;
    Point3 lower_left_corner;
    Vec3 horizontal;
    Vec3 vertical;
    Vec3 u, v, w;
    double lens_radius;
    double time0, time1; // open / close times

public:
    Camera(
        Point3 lookfrom,
        Point3 lookat,
        Vec3 vup,
        double vfov, // vertical field-of-view in degrees
        double aspect_ratio,
        double aperture,
        double focus_dist,
        double t0 = 0,
        double t1 = 0)
    {
        auto theta = deg2rad(vfov);
        auto h = tan(theta / 2);
        auto viewport_height = 2 * h;
        auto viewport_width = aspect_ratio * viewport_height;

        w = unitVector(lookfrom - lookat);
        u = unitVector(cross(vup, w));
        v = cross(w, u);

        origin = lookfrom;
        horizontal = focus_dist * viewport_width * u;
        vertical = focus_dist * viewport_height * v;
        lower_left_corner = origin - horizontal / 2 - vertical / 2 - focus_dist * w;

        lens_radius = aperture / 2;
        time0 = t0, time1 = t1;
    }

    Ray getRay(double s, double t) const
    {
        // origin -> (u, v)
        Vec3 rd = lens_radius * randomInUnitDisk();
        Vec3 offset = u * rd.x() + v * rd.y();
        return Ray(origin + offset,
                   lower_left_corner + s * horizontal +
                       t * vertical - origin - offset,
                   randomReal(time0, time1));
    }
};
