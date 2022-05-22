#pragma once

#include "raytracer.h"
#include "hittable.h"
#include "aabb.hpp"

// F(x, y, z) = (x ^ 2 + P * y ^ 2 + z ^ 2 - 1) ^ 3 - x ^ 2 * z ^ 3 - Q * y ^ 2 * z ^ 3
// x, y, z = > z, x, y :
//  (z ^ 2 + P * x ^ 2 + y ^ 2 - 1) ^ 3 - z ^ 2 * y ^ 3 - Q * x ^ 2 * y ^ 3

class Heart : public Hittable
{
private:
    Point3 origin;
    double scale;
    shared_ptr<Material> mat_ptr;

    static constexpr double P = 9.0 / 4;
    static constexpr double Q = 9.0 / 80;
    static constexpr double eps = 0.000001;

    inline double square(double x) const { return x * x; }
    inline double cubic(double x) const { return x * x * x; }
    inline double F(Point3 pos) const
    {
        pos = pos - origin;
        double ratio = 1 / fabs(scale);
        double x = pos.x() * ratio, y = pos.y() * ratio, z = pos.z() * ratio;
        return cubic(square(z) + P * square(x) + square(y) - 1) -
               square(z) * cubic(y) - Q * square(x) * cubic(y);
    }
    inline double Fx(Point3 pos) const
    {
        pos = pos - origin;
        double ratio = 1 / fabs(scale);
        double x = pos.x() * ratio, y = pos.y() * ratio, z = pos.z() * ratio;
        return 6 * square(square(z) + P * square(x) + square(y) - 1) * P * x - 2 * Q * cubic(y) * x;
    }
    inline double Fy(Point3 pos) const
    {
        pos = pos - origin;
        double ratio = 1 / fabs(scale);
        double x = pos.x() * ratio, y = pos.y() * ratio, z = pos.z() * ratio;
        return 6 * square(square(z) + P * square(x) + square(y) - 1) * y -
               3 * square(y) * square(z) - 3 * Q * square(y) * square(x);
    }
    inline double Fz(Point3 pos) const
    {
        pos = pos - origin;
        double ratio = 1 / fabs(scale);
        double x = pos.x() * ratio, y = pos.y() * ratio, z = pos.z() * ratio;
        return 6 * square(square(z) + P * square(x) + square(y) - 1) * z - 2 * cubic(y) * z;
    }

public:
    Heart(Point3 origin,
          shared_ptr<Material> p,
          double scale = 1)
        : origin(origin), scale(scale), mat_ptr(p) {}

    bool hit(const Ray &r, double t_min,
             double t_max, HitRecord &rec) const override
    {
        // use Newton's method to find solution

        // 50 iterations: suppose to get 2 different solutions (not exact)
        const int n_step = 50;
        Point3 p;
        double t1 = std::min(t_min + 50, t_max);
        for (int i = 0; i < n_step; ++i)
        {
            p = r.at(t1);
            double val = F(p);
            if (fabs(val) < eps)
                break;
            double step = val / (Fx(p) * r.direction().x() / fabs(scale) +
                                 Fy(p) * r.direction().y() / fabs(scale) +
                                 Fz(p) * r.direction().z() / fabs(scale));
            t1 -= step;
        }

        double t0 = std::max(
            t_min, t1 - scale * randomReal(0.1, 0.2));
        for (int i = 0; i < n_step; ++i)
        {
            p = r.at(t0);
            double val = F(p);
            if (fabs(val) < eps)
                break;
            double step = val / (Fx(p) * r.direction().x() / fabs(scale) +
                                 Fy(p) * r.direction().y() / fabs(scale) +
                                 Fz(p) * r.direction().z() / fabs(scale));
            t0 -= step;
        }

        if (t0 < t_min || t0 > t_max ||
            (t0 > t1 && (t1 > t_min && t1 < t_max) &&
             fabs(F(r.at(t1))) < eps))
            t0 = t1;
        if (fabs(F(r.at(t0))) > eps)
            return false;

        rec.t = t0;
        rec.p = r.at(t0);
        Vec3 out = rec.p - origin;
        Vec3 outward_norm(Fx(rec.p), Fy(rec.p), Fz(rec.p));
        if (dot(out, outward_norm) < 0)
            outward_norm = -outward_norm;
        if (scale < 0)
            outward_norm = -outward_norm;
        outward_norm = unitVector(outward_norm);
        rec.setFaceNormal(r, outward_norm);
        rec.mat_ptr = mat_ptr;
        return true;
    }

    bool boundingBox(double t0, double t1,
                     AABB &output_box) const override
    {
        output_box = AABB(origin - Vec3(2, 2, 2) * fabs(scale),
                          origin + Vec3(2, 2, 2) * fabs(scale));
        return true;
    }
};
