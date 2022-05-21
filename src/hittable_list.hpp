#pragma once

#include "raytracer.h"
#include "hittable.h"

class HittableList : public Hittable
{
private:
    std::vector<shared_ptr<Hittable>> objects;

public:
    HittableList() {}
    HittableList(shared_ptr<Hittable> object) { add(object); }

    void clear() { objects.clear(); }
    void add(shared_ptr<Hittable> object) { objects.push_back(object); }

    virtual bool hit(
        const Ray &r, double t_min,
        double t_max, HitRecord &rec) const override
    {
        HitRecord temp_rec;
        bool hit_anything = false;
        auto closest_t = t_max;

        for (const auto &object : objects)
        {
            // only hit the closest one
            if (object->hit(r, t_min, closest_t, temp_rec))
            {
                hit_anything = true;
                closest_t = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }
};