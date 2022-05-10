#ifndef __HEART__
#define __HEART__

#include "hittable.h"
#include "vec3.hpp"
#include "aabb.hpp"
#include <algorithm>

// F(x,y,z)=(x^2+P*y^2+z^2-1)^3-x^2*z^3-Q*y^2*z^3
// x,y,z => z,x,y
//      (z^2+P*x^2+y^2-1)^3-z^2*y^3-Q*x^2*y*3
class Heart : public Hittable
{
private:
    Point3 ori;
    double scale;
    shared_ptr<Material> mat_ptr;

    static constexpr double P = 9.0 / 4;
    static constexpr double Q = 9.0 / 80;
    static constexpr double eps = 0.000001;

    inline double sqr(double x) const { return x * x; }
    inline double cub(double x) const { return x * x * x; }
    inline double F(Point3 pos) const
    {
        pos = pos - ori;
        double ratio = 1 / fabs(scale);
        double x = pos.x() * ratio, y = pos.y() * ratio, z = pos.z() * ratio;
        return cub(sqr(z) + P * sqr(x) + sqr(y) - 1) -
               sqr(z) * cub(y) -
               Q * sqr(x) * cub(y);
    }
    inline double Fx(Point3 pos) const
    {
        pos = pos - ori;
        double ratio = 1 / fabs(scale);
        double x = pos.x() * ratio, y = pos.y() * ratio, z = pos.z() * ratio;
        return 6 * sqr(sqr(z) + P * sqr(x) + sqr(y) - 1) * P * x -
               2 * Q * cub(y) * x;
    }
    inline double Fy(Point3 pos) const
    {
        pos = pos - ori;
        double ratio = 1 / fabs(scale);
        double x = pos.x() * ratio, y = pos.y() * ratio, z = pos.z() * ratio;
        return 6 * sqr(sqr(z) + P * sqr(x) + sqr(y) - 1) * y -
               3 * sqr(y) * sqr(z) -
               3 * Q * sqr(y) * sqr(x);
    }
    inline double Fz(Point3 pos) const
    {
        pos = pos - ori;
        double ratio = 1 / fabs(scale);
        double x = pos.x() * ratio, y = pos.y() * ratio, z = pos.z() * ratio;
        return 6 * sqr(sqr(z) + P * sqr(x) + sqr(y) - 1) * z -
               2 * cub(y) * z;
    }

public:
    Heart(Point3 _ori, shared_ptr<Material> _p, double _scale = 1)
        : mat_ptr(_p), ori(_ori), scale(_scale) {}

    bool hit(const Ray &r, double tmin, double tmax, hit_record &rec) const override
    {
        // use Newton's method to find solution

        // 50 iterations
        // suppose to get 2 different solutions
        // not exact
        Point3 temp;
        double t1 = std::min(tmin + 50, tmax);
        for (int i = 0; i < 50; ++i)
        {
            temp = r.at(t1);
            double val = F(temp);
            if (fabs(val) < eps)
                break;
            double step = val / (Fx(temp) * r.direction().x() / fabs(scale) +
                                 Fy(temp) * r.direction().y() / fabs(scale) +
                                 Fz(temp) * r.direction().z() / fabs(scale));
            t1 = t1 - step;
            if (fabs(step) < eps)
                break;
        }
        double t0 = std::max(tmin, t1 - scale * random_double(0.1, 0.2));
        for (int i = 0; i < 50; ++i)
        {
            temp = r.at(t0);
            double val = F(temp);
            if (fabs(val) < eps)
                break;
            double step = val / (Fx(temp) * r.direction().x() / fabs(scale) +
                                 Fy(temp) * r.direction().y() / fabs(scale) +
                                 Fz(temp) * r.direction().z() / fabs(scale));
            t0 = t0 - step;
            if (fabs(step) < eps)
                break;
        }

        // debug
        double val1 = F(r.at(t0)), val2 = F(r.at(t1));

        if (t0 < tmin || t0 > tmax)
            t0 = t1;
        if (t0 > t1 && (t1 > tmin && t1 < tmax) && fabs(F(r.at(t1))) < eps)
            std::swap(t0, t1);
        if (fabs(F(r.at(t0))) > eps)
            return 0;

        rec.t = t0;
        rec.p = r.at(t0);
        Vec3 out = rec.p - ori;
        Vec3 outward_norm(Fx(rec.p), Fy(rec.p), Fz(rec.p));
        if (dot(out, outward_norm) < 0)
            outward_norm = -outward_norm;
        if (scale < 0)
            outward_norm = -outward_norm;
        outward_norm = unit_vector(outward_norm);
        rec.set_face_normal(r, outward_norm);
        rec.mat_ptr = mat_ptr;
        return true;
    }

    bool bounding_box(double t0, double t1, AABB &output_box) const override
    {
        output_box = AABB(
            ori - Vec3(2, 2, 2) * fabs(scale), ori + Vec3(2, 2, 2) * fabs(scale));
        return true;
    }
};
#endif