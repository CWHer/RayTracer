#ifndef __HITTABLE__
#define __HITTABLE__

#include "ray.hpp"
#include "aabb.hpp"
class Material;

struct hit_record
{
    Point3 p;
    Vec3 norm;
    // mat_ptr will be set to point at the material pointer the sphere was given when it was set up in main().
    shared_ptr<Material> mat_ptr;
    double t;    //Ray
    double u, v; //surface coordinates
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
    virtual bool bounding_box(double t0, double t1, AABB &output_box) const = 0;
};

class FlipFace : public Hittable
{
private:
    shared_ptr<Hittable> ptr;

public:
    FlipFace(shared_ptr<Hittable> _p) : ptr(_p) {}

    bool hit(const Ray &r, double tmin, double tmax, hit_record &rec) const override
    {
        if (!ptr->hit(r, tmin, tmax, rec))
            return 0;
        rec.front_face ^= 1;
        return 1;
    }

    bool bounding_box(double t0, double t1, AABB &output_box) const override
    {
        return ptr->bounding_box(t0, t1, output_box);
    }
};

class Translate : public Hittable
{
private:
    shared_ptr<Hittable> ptr;
    Vec3 offset;

public:
    Translate(shared_ptr<Hittable> _p, const Vec3 &_offset)
        : ptr(_p), offset(_offset) {}

    bool hit(const Ray &r, double tmin, double tmax, hit_record &rec) const override
    {
        Ray moved_r(r.origin() - offset, r.direction(), r.time());
        if (!ptr->hit(moved_r, tmin, tmax, rec))
            return 0;

        rec.p += offset;
        // rec.set_face_normal(moved_r, rec.norm); //already done? normal don't change
        return 1;
    }

    bool bounding_box(double t0, double t1, AABB &output_box) const override
    {
        if (!ptr->bounding_box(t0, t1, output_box))
            return 0;
        output_box = AABB(
            output_box.min() + offset,
            output_box.max() + offset);
        return 1;
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
    RotateY(shared_ptr<Hittable> _p, double angle) : ptr(_p)
    {
        auto radians = degrees_to_radians(angle);
        sin_theta = sin(radians);
        cos_theta = cos(radians);
        hasbox = ptr->bounding_box(0, 1, bbox);

        // if (!hasbox)
        // return;

        Point3 min(infinity, infinity, infinity);
        Point3 max(-infinity, -infinity, -infinity);

        for (int i = 0; i < 2; ++i)
        {
            for (int j = 0; j < 2; ++j)
            {
                for (int k = 0; k < 2; ++k)
                { // 8 grids
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
            }
        }

        bbox = AABB(min, max);
    }

    bool hit(const Ray &r, double tmin, double tmax, hit_record &rec) const override
    {
        auto origin = r.origin();
        auto direction = r.direction();

        origin[0] = cos_theta * r.origin()[0] - sin_theta * r.origin()[2];
        origin[2] = sin_theta * r.origin()[0] + cos_theta * r.origin()[2];

        direction[0] = cos_theta * r.direction()[0] - sin_theta * r.direction()[2];
        direction[2] = sin_theta * r.direction()[0] + cos_theta * r.direction()[2];

        Ray rotated_r(origin, direction, r.time());

        if (!ptr->hit(rotated_r, tmin, tmax, rec))
            return 0;

        auto p = rec.p;
        auto normal = rec.norm;

        p[0] = cos_theta * rec.p[0] + sin_theta * rec.p[2];
        p[2] = -sin_theta * rec.p[0] + cos_theta * rec.p[2];

        normal[0] = cos_theta * rec.norm[0] + sin_theta * rec.norm[2];
        normal[2] = -sin_theta * rec.norm[0] + cos_theta * rec.norm[2];

        rec.p = p;
        rec.set_face_normal(rotated_r, normal);

        return 1;
    }

    bool bounding_box(double t0, double t1, AABB &output_box) const override
    {
        output_box = bbox;
        return hasbox;
    }
};

#endif