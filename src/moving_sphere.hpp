#pragma once

#include "raytracer.h"
#include "material.hpp"
#include "aabb.hpp"

class MovingSphere : public Hittable
{
private:
    Point3 center0, center1;
    double time0, time1;
    double radius;
    shared_ptr<Material> mat_ptr;

public:
    MovingSphere() {}
    MovingSphere(Point3 cen0, Point3 cen1,
                 double t0, double t1, double r,
                 shared_ptr<Material> m)
        : center0(cen0), center1(cen1),
          time0(t0), time1(t1), radius(r), mat_ptr(m) {}

    Point3 center(double time) const
    {
        return center0 + ((time - time0) / (time1 - time0)) * (center1 - center0);
    }

    bool hit(const Ray &r, double t_min,
             double t_max, HitRecord &rec) const override
    {
        Vec3 oc = r.origin() - center(r.time());
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
        auto outward_normal = (rec.p - center(r.time())) / radius;
        rec.setFaceNormal(r, outward_normal);
        rec.mat_ptr = mat_ptr;

        return true;
    }

    bool boundingBox(double t0, double t1,
                     AABB &output_box) const override
    {
        AABB box0(center(t0) - Vec3(radius, radius, radius),
                  center(t0) + Vec3(radius, radius, radius));
        AABB box1(center(t1) - Vec3(radius, radius, radius),
                  center(t1) + Vec3(radius, radius, radius));
        output_box = surroundingBox(box0, box1);
        return true;
    }
};
