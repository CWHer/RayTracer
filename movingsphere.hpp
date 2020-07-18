#ifndef __MOVINGSPHERE__
#define __MOVINGSPHERE__

#include "raytracer.h"
#include "hittable.h"
#include "material.hpp"

class MovingSphere : public Hittable
{
private:
    Point3 center0, center1;
    double time0, time1;
    double radius;
    shared_ptr<Material> mat_ptr;

public:
    MovingSphere() {}
    MovingSphere(Point3 _cen0, Point3 _cen1, double _t0, double _t1, double _r, shared_ptr<Material> _m)
        : center0(_cen0), center1(_cen1), time0(_t0), time1(_t1), radius(_r), mat_ptr(_m) {}

    Point3 center(double time) const
    {
        return center0 + ((time - time0) / (time1 - time0)) * (center1 - center0);
    }

    bool hit(const Ray &r, double tmin, double tmax, hit_record &rec) const override
    {
        Vec3 oc = r.origin() - center(r.time());
        auto a = r.direction().length_sqr();
        auto half_b = dot(oc, r.direction());
        auto c = oc.length_sqr() - radius * radius;
        auto discriminant = half_b * half_b - a * c;

        if (discriminant > 0)
        {
            auto root = sqrt(discriminant);
            auto temp = (-half_b - root) / a; //negative root
            if (temp < tmax && temp > tmin)
            {
                rec.t = temp;
                rec.p = r.at(rec.t);
                Vec3 outward_norm = (rec.p - center(r.time())) / radius;
                rec.set_face_normal(r, outward_norm);
                rec.mat_ptr = mat_ptr;
                return 1;
            }
            temp = (-half_b + root) / a; //positive root
            if (temp < tmax && temp > tmin)
            {
                rec.t = temp;
                rec.p = r.at(rec.t);
                Vec3 outward_norm = (rec.p - center(r.time())) / radius;
                rec.set_face_normal(r, outward_norm);
                rec.mat_ptr = mat_ptr;
                return 1;
            }
        }
        return 0;
    }
};

#endif