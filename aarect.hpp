#ifndef __AARECT__
#define __AARECT__

#include "hittable.h"
#include "aabb.hpp"

class XYRect : public Hittable
{
private:
    double x0, x1, y0, y1, k;
    shared_ptr<Material> mat_ptr;

public:
    XYRect() {}

    XYRect(double _x0, double _x1, double _y0, double _y1, double _k, shared_ptr<Material> mat)
        : x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_k), mat_ptr(mat){};

    bool hit(const Ray &r, double t0, double t1, hit_record &rec) const override
    {
        auto t = (k - r.origin().z()) / r.direction().z();
        if (t < t0 || t > t1)
            return 0;
        auto x = r.origin().x() + t * r.direction().x();
        auto y = r.origin().y() + t * r.direction().y();
        if (x < x0 || x > x1 || y < y0 || y > y1)
            return 0;
        rec.u = (x - x0) / (x1 - x0); //from left
        rec.v = (y1 - y) / (y1 - y0); //from up
        rec.t = t;
        auto outward_normal = Vec3(0, 0, 1);
        rec.set_face_normal(r, outward_normal);
        rec.mat_ptr = mat_ptr;
        rec.p = r.at(t);
        return 1;
    }

    bool bounding_box(double t0, double t1, AABB &output_box) const override
    {
        // The bounding box must have non-zero width in each dimension, so pad the Z
        // dimension a small amount.
        // 0.0001
        // output_box = AABB(Point3(x0, y0, k - 0.0001), Point3(x1, y1, k + 0.0001));
        output_box = AABB(Point3(x0, y0, k - eps), Point3(x1, y1, k + eps));
        return 1;
    }
};

class XZRect : public Hittable
{
private:
    double x0, x1, z0, z1, k;
    shared_ptr<Material> mat_ptr;

public:
    XZRect() {}

    XZRect(double _x0, double _x1, double _z0, double _z1, double _k, shared_ptr<Material> mat)
        : x0(_x0), x1(_x1), z0(_z0), z1(_z1), k(_k), mat_ptr(mat){};

    bool hit(const Ray &r, double t0, double t1, hit_record &rec) const override
    {
        auto t = (k - r.origin().y()) / r.direction().y();
        if (t < t0 || t > t1)
            return 0;
        auto x = r.origin().x() + t * r.direction().x();
        auto z = r.origin().z() + t * r.direction().z();
        if (x < x0 || x > x1 || z < z0 || z > z1)
            return 0;
        rec.u = (x - x0) / (x1 - x0);
        rec.v = (z - z0) / (z1 - z0);
        rec.t = t;
        auto outward_normal = Vec3(0, 1, 0);
        rec.set_face_normal(r, outward_normal);
        rec.mat_ptr = mat_ptr;
        rec.p = r.at(t);
        return 1;
    }

    bool bounding_box(double t0, double t1, AABB &output_box) const override
    {
        // The bounding box must have non-zero width in each dimension, so pad the Y
        // dimension a small amount.
        output_box = AABB(Point3(x0, k - eps, z0), Point3(x1, k + eps, z1));
        return 1;
    }
};

class YZRect : public Hittable
{
private:
    double y0, y1, z0, z1, k;
    shared_ptr<Material> mat_ptr;

public:
    YZRect() {}

    YZRect(double _y0, double _y1, double _z0, double _z1, double _k, shared_ptr<Material> mat)
        : y0(_y0), y1(_y1), z0(_z0), z1(_z1), k(_k), mat_ptr(mat){};

    bool hit(const Ray &r, double t0, double t1, hit_record &rec) const override
    {
        auto t = (k - r.origin().x()) / r.direction().x();
        if (t < t0 || t > t1)
            return 0;
        auto y = r.origin().y() + t * r.direction().y();
        auto z = r.origin().z() + t * r.direction().z();
        if (y < y0 || y > y1 || z < z0 || z > z1)
            return 0;
        rec.u = (y - y0) / (y1 - y0);
        rec.v = (z - z0) / (z1 - z0);
        rec.t = t;
        auto outward_normal = Vec3(1, 0, 0);
        rec.set_face_normal(r, outward_normal);
        rec.mat_ptr = mat_ptr;
        rec.p = r.at(t);
        return 1;
    }

    bool bounding_box(double t0, double t1, AABB &output_box) const override
    {
        // The bounding box must have non-zero width in each dimension, so pad the X
        // dimension a small amount.
        output_box = AABB(Point3(k - eps, y0, z0), Point3(k + eps, y1, z1));
        return 1;
    }
};

#endif