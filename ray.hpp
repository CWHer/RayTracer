#ifndef __RAY__
#define __RAY__

#include "vec3.hpp"

class Ray
{
private:
    point3 orig;
    Vec3 dir;

public:
    Ray() {}
    Ray(const point3 &_origin, const Vec3 &_dir)
        : orig(_origin), dir(_dir) {}

    point3 origin() const { return orig; }
    Vec3 direction() const { return dir; }

    point3 at(double t) const
    {
        return orig + t * dir;
    }
};

#endif