#pragma once

#include "vec3.hpp"

class Ray
{
private:
    Point3 orig;
    Vec3 dir;

public:
    Ray() {}
    Ray(const Point3 &_orig, const Vec3 &_dir)
        : orig(_orig), dir(_dir) {}

    Point3 origin() const { return orig; }
    Vec3 direction() const { return dir; }

    Point3 at(double t) const
    {
        return orig + t * dir;
    }
};