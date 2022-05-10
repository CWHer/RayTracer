#ifndef __RAY__
#define __RAY__

#include "vec3.hpp"

class Ray
{
private:
    Point3 orig;
    Vec3 dir;
    double tm;

public:
    Ray() {}
    Ray(const Point3 &_orig, const Vec3 &_dir, double _tm = 0)
        : orig(_orig), dir(_dir), tm(_tm) {}

    Point3 origin() const { return orig; }
    Vec3 direction() const { return dir; }
    double time() const { return tm; }

    Point3 at(double t) const
    {
        return orig + t * dir;
    }
};

#endif