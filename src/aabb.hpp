// not what you think....
// Axis-Aligned Bounding Boxes

#pragma once

#include "raytracer.h"

class AABB
{
private:
    Point3 mn;
    Point3 mx;

public:
    AABB() {}
    AABB(const Point3 &_mn, const Point3 &_mx) : mn(_mn), mx(_mx) {}

    Point3 min() const { return mn; }
    Point3 max() const { return mx; }

    bool hit(const Ray &r, double tmin, double tmax) const
    {
        for (int i = 0; i < 3; ++i)
        {
            auto invD = 1.0 / r.direction()[i];
            auto t0 = (mn[i] - r.origin()[i]) * invD;
            auto t1 = (mx[i] - r.origin()[i]) * invD;
            if (t0 > t1)
                std::swap(t0, t1);
            tmin = fmax(t0, tmin);
            tmax = fmin(t1, tmax);
            if (tmax <= tmin)
                return 0;
        }
        return 1;
    }
};

AABB surrounding_box(AABB box0, AABB box1)
{
    Point3 small(fmin(box0.min().x(), box1.min().x()),
                 fmin(box0.min().y(), box1.min().y()),
                 fmin(box0.min().z(), box1.min().z()));
    Point3 big(fmax(box0.max().x(), box1.max().x()),
               fmax(box0.max().y(), box1.max().y()),
               fmax(box0.max().z(), box1.max().z()));
    return AABB(small, big);
}
