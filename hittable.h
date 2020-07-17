#ifndef __HITTABLE__
#define __HITTABLE__

#include "ray.hpp"

struct hit_record
{
    Point3 p;
    Vec3 norm;
    double t;
};

class Hittable
{
public:
    virtual bool hit(const Ray &r, double tmin, double tmax, hit_record &rec) const = 0;
};

#endif