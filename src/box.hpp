#ifndef __BOX__
#define __BOX__

#include "raytracer.h"
#include "hittable.hpp"
#include "hittable_list.hpp"
#include "aarect.hpp"

class Box : public Hittable
{
private:
    Point3 box_min;
    Point3 box_max;
    HittableList sides;

public:
    Box() {}

    Box(const Point3 &p0, const Point3 &p1, shared_ptr<Material> ptr)
    {
        box_min = p0;
        box_max = p1;

        sides.add(make_shared<XYRect>(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), ptr));
        sides.add(make_shared<FlipFace>(
            make_shared<XYRect>(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), ptr)));

        sides.add(make_shared<XZRect>(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), ptr));
        sides.add(make_shared<FlipFace>(
            make_shared<XZRect>(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), ptr)));

        sides.add(make_shared<YZRect>(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), ptr));
        sides.add(make_shared<FlipFace>(
            make_shared<YZRect>(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), ptr)));

        // sides.build();
    }

    bool hit(const Ray &r, double t0, double t1, hit_record &rec) const override
    {
        return sides.hit(r, t0, t1, rec);
    }

    bool bounding_box(double t0, double t1, AABB &output_box) const override
    {
        output_box = AABB(box_min, box_max);
        return 1;
    }
};

#endif