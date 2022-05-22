#pragma once

#include "hittable.h"
#include "aabb.hpp"

class XYRect : public Hittable
{
private:
    double x0, x1, y0, y1, k;
    shared_ptr<Material> mat_ptr;

public:
    XYRect() {}

    XYRect(double x0, double x1,
           double y0, double y1,
           double k, shared_ptr<Material> mat)
        : x0(x0), x1(x1), y0(y0), y1(y1),
          k(k), mat_ptr(mat){};

    bool hit(const Ray &r, double t0,
             double t1, HitRecord &rec) const override
    {
        auto t = (k - r.origin().z()) / r.direction().z();
        if (t < t0 || t > t1)
            return false;

        auto x = r.origin().x() + t * r.direction().x();
        auto y = r.origin().y() + t * r.direction().y();
        if (x < x0 || x > x1 || y < y0 || y > y1)
            return false;

        rec.u = (x - x0) / (x1 - x0); // from left
        rec.v = (y1 - y) / (y1 - y0); // from up
        rec.t = t;
        auto outward_normal = Vec3(0, 0, 1);
        rec.setFaceNormal(r, outward_normal);
        rec.mat_ptr = mat_ptr;
        rec.p = r.at(t);
        return true;
    }

    bool boundingBox(double t0, double t1,
                     AABB &output_box) const override
    {
        // The bounding box must have non-zero width in each dimension,
        //  so pad the Z dimension a small amount.
        output_box = AABB(Point3(x0, y0, k - 0.0001),
                          Point3(x1, y1, k + 0.0001));
        return true;
    }
};

class XZRect : public Hittable
{
private:
    double x0, x1, z0, z1, k;
    shared_ptr<Material> mat_ptr;

public:
    XZRect() {}

    XZRect(double x0, double x1,
           double z0, double z1,
           double k, shared_ptr<Material> mat)
        : x0(x0), x1(x1), z0(z0), z1(z1),
          k(k), mat_ptr(mat){};

    bool hit(const Ray &r, double t0,
             double t1, HitRecord &rec) const override
    {
        auto t = (k - r.origin().y()) / r.direction().y();
        if (t < t0 || t > t1)
            return false;

        auto x = r.origin().x() + t * r.direction().x();
        auto z = r.origin().z() + t * r.direction().z();
        if (x < x0 || x > x1 || z < z0 || z > z1)
            return false;

        rec.u = (x - x0) / (x1 - x0);
        rec.v = (z - z0) / (z1 - z0);
        rec.t = t;
        auto outward_normal = Vec3(0, 1, 0);
        rec.setFaceNormal(r, outward_normal);
        rec.mat_ptr = mat_ptr;
        rec.p = r.at(t);
        return true;
    }

    bool boundingBox(double t0, double t1,
                     AABB &output_box) const override
    {
        output_box = AABB(Point3(x0, k - 0.0001, z0),
                          Point3(x1, k + 0.0001, z1));
        return true;
    }
};

class YZRect : public Hittable
{
private:
    double y0, y1, z0, z1, k;
    shared_ptr<Material> mat_ptr;

public:
    YZRect() {}

    YZRect(double y0, double y1,
           double z0, double z1,
           double k, shared_ptr<Material> mat)
        : y0(y0), y1(y1), z0(z0), z1(z1),
          k(k), mat_ptr(mat){};

    bool hit(const Ray &r, double t0,
             double t1, HitRecord &rec) const override
    {
        auto t = (k - r.origin().x()) / r.direction().x();
        if (t < t0 || t > t1)
            return false;

        auto y = r.origin().y() + t * r.direction().y();
        auto z = r.origin().z() + t * r.direction().z();
        if (y < y0 || y > y1 || z < z0 || z > z1)
            return false;

        rec.u = (y - y0) / (y1 - y0);
        rec.v = (z - z0) / (z1 - z0);
        rec.t = t;
        auto outward_normal = Vec3(1, 0, 0);
        rec.setFaceNormal(r, outward_normal);
        rec.mat_ptr = mat_ptr;
        rec.p = r.at(t);
        return true;
    }

    bool boundingBox(double t0, double t1,
                     AABB &output_box) const override
    {
        output_box = AABB(Point3(k - 0.0001, y0, z0),
                          Point3(k + 0.0001, y1, z1));
        return true;
    }
};
