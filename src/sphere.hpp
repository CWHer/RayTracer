#pragma once

#include "hittable.h"
#include "vec3.hpp"

class Sphere : public Hittable
{
private:
    Point3 center;
    double radius;
    shared_ptr<Material> mat_ptr;

public:
    Sphere() {}
    Sphere(Point3 c, double r,
           shared_ptr<Material> mat_ptr)
        : center(c), radius(r), mat_ptr(mat_ptr) {}

    bool hit(const Ray &r, double t_min,
             double t_max, HitRecord &rec) const override
    {
        Vec3 oc = r.origin() - center;
        auto a = r.direction().lengthSquared();
        auto half_b = dot(oc, r.direction());
        auto c = oc.lengthSquared() - radius * radius;

        auto discriminant = half_b * half_b - a * c;
        if (discriminant < 0)
            return false;
        auto sqrtd = sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        auto root = (-half_b - sqrtd) / a;
        if (root < t_min || t_max < root)
        {
            root = (-half_b + sqrtd) / a;
            if (root < t_min || t_max < root)
                return false;
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        Vec3 outward_normal = (rec.p - center) / radius;
        rec.setFaceNormal(r, outward_normal);
        rec.mat_ptr = mat_ptr;

        return true;
    }
};