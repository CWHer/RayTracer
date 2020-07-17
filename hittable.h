#ifndef __HITTABLE__
#define __HITTABLE__

#include "ray.hpp"

struct hit_record
{
    Point3 p;
    Vec3 norm;
    double t;
    bool front_face;

    inline void set_face_normal(const Ray &r, const Vec3 &outward_normal)
    {
        front_face = dot(r.direction(), outward_normal) < 0;
        norm = front_face ? outward_normal : -outward_normal;
    }
};

class Hittable
{
public:
    virtual bool hit(const Ray &r, double tmin, double tmax, hit_record &rec) const = 0;
};

#endif