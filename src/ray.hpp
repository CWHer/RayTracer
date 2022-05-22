#pragma once

#include "vec3.hpp"

class Ray
{
private:
    Point3 orig;
    Vec3 dir;
    double t;

public:
    Ray() {}
    Ray(const Point3 &origin,
        const Vec3 &direction,
        double time = 0)
        : orig(origin), dir(direction), t(time) {}

    Point3 origin() const { return orig; }
    Vec3 direction() const { return dir; }
    double time() const { return t; }

    Point3 at(double t) const { return orig + t * dir; }
};