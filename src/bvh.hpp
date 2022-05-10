#ifndef __BVH__
#define __BVH__

#include "raytracer.h"
#include "hittable.h"
#include "hittable_list.hpp"
#include "aabb.hpp"
#include "ray.hpp"

#include <algorithm>

class BVHnode : public Hittable
{
private:
    shared_ptr<Hittable> left;
    shared_ptr<Hittable> right;
    AABB box;

public:
    BVHnode() {}

    BVHnode(HittableList &list, double time0, double time1)
        : BVHnode(list.objects, 0, list.objects.size(), time0, time1) {}

    BVHnode(
        std::vector<shared_ptr<Hittable>> &objects,
        size_t start, size_t end, double time0, double time1);

    bool hit(const Ray &r, double tmin, double tmax, hit_record &rec) const override
    {
        if (!box.hit(r, tmin, tmax))
            return 0;

        bool hit_left = left->hit(r, tmin, tmax, rec);
        bool hit_right = right->hit(r, tmin, hit_left ? rec.t : tmax, rec);
        // hit the closet obj

        return hit_left | hit_right;
    }
    bool bounding_box(double t0, double t1, AABB &output_box) const override
    {
        output_box = box;
        return 1;
    }
};

inline bool box_compare(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b, int axis)
{
    AABB box_a, box_b;

    if (!a->bounding_box(0, 0, box_a) || !b->bounding_box(0, 0, box_b))
        std::cerr << "No bounding box in bvh_node constructor.\n";

    return box_a.min()[axis] < box_b.min()[axis];
}

bool box_x_cmp(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b)
{
    return box_compare(a, b, 0);
}

bool box_y_cmp(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b)
{
    return box_compare(a, b, 1);
}

bool box_z_cmp(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b)
{
    return box_compare(a, b, 2);
}

// 1. randomly choose an axis
// 2. sort the primitives (using std::sort)
// 3. put half in each subtree
BVHnode::BVHnode(std::vector<shared_ptr<Hittable>> &objects,
                 size_t start, size_t end, double time0, double time1)
{
    int axis = random_int(0, 2);
    auto cmp = axis == 0   ? box_x_cmp
               : axis == 1 ? box_y_cmp
                           : box_z_cmp;
    size_t object_span = end - start;

    if (object_span == 1)
    {
        left = right = objects[start];
    }
    else if (object_span == 2)
    {
        left = objects[start];
        right = objects[start + 1];
        if (!cmp(left, right))
            std::swap(left, right);
    }
    else
    {
        // not include end
        std::sort(objects.begin() + start, objects.begin() + end, cmp);

        auto mid = start + object_span / 2;
        left = make_shared<BVHnode>(objects, start, mid, time0, time1);
        right = make_shared<BVHnode>(objects, mid, end, time0, time1);
    }

    AABB box_left, box_right;

    // in case you sent in something like an infinite plane
    if (!left->bounding_box(time0, time1, box_left) ||
        !right->bounding_box(time0, time1, box_right))
        std::cerr << "No bounding box in BVHnode constructor.\n";

    box = surrounding_box(box_left, box_right);
}

#endif