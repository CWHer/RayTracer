#pragma once

#include "ray.hpp"
#include "aabb.hpp"

class Material;

struct HitRecord
{
    Point3 p;
    Vec3 normal;
    shared_ptr<Material> mat_ptr;
    double t;
    double u, v; // surface coordinates
    bool front_face;

    inline void setFaceNormal(const Ray &r, const Vec3 &outward_normal)
    {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class Hittable
{
public:
    virtual bool hit(
        const Ray &r, double t_min,
        double t_max, HitRecord &rec) const = 0;

    virtual bool boundingBox(
        double t0, double t1, AABB &output_box) const = 0;
};

class FlipFace : public Hittable
{
private:
    shared_ptr<Hittable> ptr;

public:
    FlipFace(shared_ptr<Hittable> p) : ptr(p) {}

    bool hit(const Ray &r, double t_min,
             double t_max, HitRecord &rec) const override
    {
        if (!ptr->hit(r, t_min, t_max, rec))
            return false;
        rec.front_face = !rec.front_face;
        return true;
    }

    bool boundingBox(double t0, double t1,
                     AABB &output_box) const override
    {
        return ptr->boundingBox(t0, t1, output_box);
    }
};

class Translate : public Hittable
{
private:
    shared_ptr<Hittable> ptr;
    Vec3 offset;

public:
    Translate(shared_ptr<Hittable> p,
              const Vec3 &offset)
        : ptr(p), offset(offset) {}

    bool hit(const Ray &r, double t_min,
             double t_max, HitRecord &rec) const override
    {
        Ray moved_r(r.origin() - offset, r.direction(), r.time());
        if (!ptr->hit(moved_r, t_min, t_max, rec))
            return false;

        rec.p += offset;
        rec.setFaceNormal(moved_r, rec.normal);
        return true;
    }

    bool boundingBox(double t0, double t1,
                     AABB &output_box) const override
    {
        if (!ptr->boundingBox(t0, t1, output_box))
            return false;
        output_box = AABB(output_box.min() + offset,
                          output_box.max() + offset);
        return true;
    }
};

class RotateY : public Hittable
{
private:
    shared_ptr<Hittable> ptr;
    double sin_theta;
    double cos_theta;
    bool hasbox;
    AABB bbox;

public:
    RotateY(shared_ptr<Hittable> p, double angle) : ptr(p)
    {
        auto radians = deg2rad(angle);
        sin_theta = sin(radians);
        cos_theta = cos(radians);
        hasbox = ptr->boundingBox(0, 1, bbox);

        Point3 min(INF, INF, INF);
        Point3 max(-INF, -INF, -INF);

        for (int i = 0; i < 2; ++i)
            for (int j = 0; j < 2; ++j)
                for (int k = 0; k < 2; ++k)
                {
                    auto x = i * bbox.max().x() + (1 - i) * bbox.min().x();
                    auto y = j * bbox.max().y() + (1 - j) * bbox.min().y();
                    auto z = k * bbox.max().z() + (1 - k) * bbox.min().z();

                    auto newx = cos_theta * x + sin_theta * z;
                    auto newz = -sin_theta * x + cos_theta * z;

                    Vec3 tester(newx, y, newz);

                    for (int c = 0; c < 3; ++c)
                    {
                        min[c] = fmin(min[c], tester[c]);
                        max[c] = fmax(max[c], tester[c]);
                    }
                }

        bbox = AABB(min, max);
    }

    bool hit(const Ray &r, double t_min,
             double t_max, HitRecord &rec) const override
    {
        auto origin = r.origin();
        auto direction = r.direction();

        origin[0] = cos_theta * r.origin()[0] - sin_theta * r.origin()[2];
        origin[2] = sin_theta * r.origin()[0] + cos_theta * r.origin()[2];

        direction[0] = cos_theta * r.direction()[0] - sin_theta * r.direction()[2];
        direction[2] = sin_theta * r.direction()[0] + cos_theta * r.direction()[2];

        Ray rotated_r(origin, direction, r.time());

        if (!ptr->hit(rotated_r, t_min, t_max, rec))
            return false;

        auto p = rec.p;
        auto normal = rec.normal;

        p[0] = cos_theta * rec.p[0] + sin_theta * rec.p[2];
        p[2] = -sin_theta * rec.p[0] + cos_theta * rec.p[2];

        normal[0] = cos_theta * rec.normal[0] + sin_theta * rec.normal[2];
        normal[2] = -sin_theta * rec.normal[0] + cos_theta * rec.normal[2];

        rec.p = p;
        rec.setFaceNormal(rotated_r, normal);

        return true;
    }

    bool boundingBox(double t0, double t1,
                     AABB &output_box) const override
    {
        output_box = bbox;
        return hasbox;
    }
};
