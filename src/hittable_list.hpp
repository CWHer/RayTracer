#pragma once

#include "raytracer.h"
#include "hittable.hpp"
#include "aabb.hpp"

class HittableList : public Hittable
{
    friend class BVHNode;

private:
    std::vector<shared_ptr<Hittable>> objects;

public:
    HittableList() {}
    HittableList(shared_ptr<Hittable> object) { add(object); }

    void clear() { objects.clear(); }
    void add(shared_ptr<Hittable> object) { objects.push_back(object); }

    bool hit(const Ray &r, double t_min,
             double t_max, HitRecord &rec) const
    {
        HitRecord temp_rec;
        bool hit_anything = false;
        auto closest_t = t_max;

        for (const auto &object : objects)
        {
            if (object->hit(r, t_min, closest_t, temp_rec))
            {
                hit_anything = true;
                closest_t = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }

    bool boundingBox(double t0, double t1,
                     AABB &output_box) const override
    {
        if (objects.empty())
            return false;

        AABB temp_box;
        bool first_box = true;

        for (const auto &object : objects)
        {
            if (!object->boundingBox(t0, t1, temp_box))
                return false;
            output_box = first_box
                             ? temp_box
                             : surroundingBox(output_box, temp_box);
            first_box = false;
        }
        return true;
    }

    double pdfValue(const Point3 &origin, const Vec3 &v) const override
    {
        auto weight = 1.0 / objects.size();
        double sum = 0;

        for (const auto &object : objects)
            sum += weight * object->pdfValue(origin, v);

        return sum;
    }

    Vec3 random(const Vec3 &origin) const override
    {
        auto int_size = static_cast<int>(objects.size());
        return objects[randomInt(0, int_size - 1)]->random(origin);
    }
};