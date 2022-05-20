#pragma once

#include "ray.hpp"
class Material;

struct hit_record
{
    Point3 p;
    Vec3 norm;
    // mat_ptr will be set to point at the material pointer the sphere was given when it was set up in main().
    shared_ptr<Material> mat_ptr;
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