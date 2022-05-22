// Axis-Aligned Bounding Boxes

#pragma once

#include "raytracer.h"

class AABB
{
private:
    Point3 min_p;
    Point3 max_p;

public:
    AABB() {}
    AABB(const Point3 &min,
         const Point3 &max)
        : min_p(min), max_p(max) {}

    Point3 min() const { return min_p; }
    Point3 max() const { return max_p; }

    bool hit(const Ray &r, double t_min, double t_max) const
    {
        for (int i = 0; i < 3; ++i)
        {
            auto inv_d = 1.0 / r.direction()[i];
            auto t0 = (min_p[i] - r.origin()[i]) * inv_d;
            auto t1 = (max_p[i] - r.origin()[i]) * inv_d;
            if (t0 > t1)
                std::swap(t0, t1);
            t_min = fmax(t0, t_min);
            t_max = fmin(t1, t_max);
            if (t_max <= t_min)
                return false;
        }
        return true;
    }
};

AABB surroundingBox(AABB box0, AABB box1)
{
    Point3 small(fmin(box0.min().x(), box1.min().x()),
                 fmin(box0.min().y(), box1.min().y()),
                 fmin(box0.min().z(), box1.min().z()));
    Point3 big(fmax(box0.max().x(), box1.max().x()),
               fmax(box0.max().y(), box1.max().y()),
               fmax(box0.max().z(), box1.max().z()));
    return AABB(small, big);
}
