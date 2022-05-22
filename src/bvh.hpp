#pragma once

#include "raytracer.h"
#include "hittable.h"
#include "hittable_list.hpp"
#include "aabb.hpp"

class BVHNode : public Hittable
{
private:
    shared_ptr<Hittable> left;
    shared_ptr<Hittable> right;
    AABB box;

public:
    BVHNode() {}

    BVHNode(HittableList &list, double time0, double time1)
        : BVHNode(list.objects, 0, list.objects.size(), time0, time1) {}

    BVHNode(
        std::vector<shared_ptr<Hittable>> &objects,
        size_t start, size_t end, double time0, double time1);

    bool hit(const Ray &r, double t_min,
             double t_max, HitRecord &rec) const override
    {
        if (!box.hit(r, t_min, t_max))
            return false;

        bool hit_left = left->hit(r, t_min, t_max, rec);
        bool hit_right = right->hit(r, t_min, hit_left ? rec.t : t_max, rec);

        return hit_left | hit_right;
    }

    bool boundingBox(double t0, double t1,
                     AABB &output_box) const override
    {
        output_box = box;
        return true;
    }
};

// 1. randomly choose an axis
// 2. sort the primitives (using std::sort)
// 3. put half in each subtree
BVHNode::BVHNode(
    std::vector<shared_ptr<Hittable>> &objects,
    size_t start, size_t end, double time0, double time1)
{
    int axis = randomInt(0, 2);
    auto comparator = [=](const shared_ptr<Hittable> &lhs,
                          const shared_ptr<Hittable> &rhs)
    {
        AABB box_left, box_right;
        if (!lhs->boundingBox(0, 0, box_left) ||
            !rhs->boundingBox(0, 0, box_right))
            std::cerr << "[ERROR]: No bounding box in BVHnode constructor.\n";
        return box_left.min()[axis] < box_right.min()[axis];
    };
    size_t object_span = end - start;

    if (object_span == 1)
    {
        left = right = objects[start];
    }
    else if (object_span == 2)
    {
        left = objects[start];
        right = objects[start + 1];
        if (!comparator(left, right))
            std::swap(left, right);
    }
    else
    {
        std::sort(objects.begin() + start,
                  objects.begin() + end,
                  comparator);

        auto mid = start + object_span / 2;
        left = make_shared<BVHNode>(objects, start, mid, time0, time1);
        right = make_shared<BVHNode>(objects, mid, end, time0, time1);
    }

    AABB box_left, box_right;

    // in case you sent in something like an infinite plane
    if (!left->boundingBox(time0, time1, box_left) ||
        !right->boundingBox(time0, time1, box_right))
        std::cerr << "[ERROR]: No bounding box in BVHnode constructor.\n";

    box = surroundingBox(box_left, box_right);
}
