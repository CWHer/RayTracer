#pragma once

#include "hittable.h"
#include "vec3.hpp"
#include "aabb.hpp"
#include <algorithm>

// y=x^2/b^2-z^2/a^2;
// must contain (0,0,0) or may not run well

// caution! unfinished!
class Paraboloid : public Hittable
{
private:
    double a, b;
    double x0, x1, z0, z1;
    shared_ptr<Material> mat_ptr;

    inline double sqr(double x) const { return x * x; }

public:
    Paraboloid(shared_ptr<Material> _p,
               double _x0, double _x1, double _z0, double _z1,
               double _a = 2, double _b = 2)
        : mat_ptr(_p), x0(_x0), x1(_x1), z0(_z0), z1(_z1), a(_a), b(_b) {}

    bool hit(const Ray &r, double tmin, double tmax, hit_record &rec) const override
    {
        // At^2+Bt+C=0
        double A = sqr(r.direction().x()) / sqr(b) - sqr(r.direction().z()) / sqr(a);
        double B = 2 * r.origin().x() * r.direction().x() / sqr(b) -
                   2 * r.origin().z() * r.direction().z() / sqr(a) -
                   r.direction().y();
        double C = sqr(r.origin().x()) / sqr(b) -
                   sqr(r.origin().z()) / sqr(a) -
                   r.origin().y();
        auto discriminant = sqr(B) - 4 * A * C;
        if (discriminant > 0)
        {
            auto root = sqrt(discriminant);
            auto temp = (-B - root) / (2 * A);
            if (temp > tmin && temp < tmax)
            {
                auto pos = r.at(temp);
                if (pos.x() > x0 && pos.x() < x1 &&
                    pos.z() > z0 && pos.z() < z1)
                {
                    rec.t = temp;
                    rec.p = pos;
                    rec.u = (pos.x() - x0) / (x1 - x0); // from left
                    rec.v = (z1 - pos.z()) / (z1 - z0); // from up
                    rec.mat_ptr = mat_ptr;
                    auto outward_norm = Vec3(-2 * pos.x() / sqr(b), 1, 2 * pos.z() / sqr(a));
                    outward_norm = unit_vector(outward_norm);
                    rec.set_face_normal(r, outward_norm);
                    return 1;
                }
            }
            temp = (-B + root) / (2 * A);
            if (temp > tmin && temp < tmax)
            {
                auto pos = r.at(temp);
                if (pos.x() > x0 && pos.x() < x1 &&
                    pos.z() > z0 && pos.z() < z1)
                {
                    rec.t = temp;
                    rec.p = pos;
                    rec.u = (pos.x() - x0) / (x1 - x0); // from left
                    rec.v = (z1 - pos.z()) / (z1 - z0); // from up
                    rec.mat_ptr = mat_ptr;
                    auto outward_norm = Vec3(-2 * pos.x() / sqr(b), 1, 2 * pos.z() / sqr(a));
                    outward_norm = unit_vector(outward_norm);
                    rec.set_face_normal(r, outward_norm);
                    return 1;
                }
            }
        }

        return 0;
    }

    bool bounding_box(double t0, double t1, AABB &output_box) const override
    {
        double xmin = x0 < 0 && x1 > 0 ? 0 : std::min(fabs(x0), fabs(x1));
        double xmax = std::max(fabs(x0), fabs(x1));
        double zmin = z0 < 0 && z1 > 0 ? 0 : std::min(fabs(z0), fabs(z1));
        double zmax = std::max(fabs(z0), fabs(z1));
        output_box = AABB(
            Point3(x0, z0, sqr(xmin) / sqr(b) - sqr(zmax) / sqr(a)),
            Point3(x1, z1, sqr(xmax) / sqr(b) - sqr(zmin) / sqr(a)));
        return true;
    }
};
