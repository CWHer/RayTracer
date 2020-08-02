#ifndef __CONSTANTMEDIUM__
#define __CONSTANTMEDIUM__

#include "hittable.h"
#include "raytracer.h"
#include "material.hpp"

class ConstantMedium : public Hittable
{
private:
    shared_ptr<Hittable> boundary;
    shared_ptr<Material> phase_function;
    double neg_inv_density;

public:
    ConstantMedium(shared_ptr<Hittable> b, double d, shared_ptr<Texture> a)
        : boundary(b), neg_inv_density(-1 / d)
    {
        phase_function = make_shared<Isotropic>(a);
    }

    bool hit(const Ray &r, double tmin, double tmax, hit_record &rec) const override
    {
        // Print occasional samples when debugging. To enable, set enableDebug true.
        const bool enableDebug = false;
        const bool debugging = enableDebug && random_double() < 0.00001;

        hit_record rec1, rec2;

        if (!boundary->hit(r, -infinity, infinity, rec1))
            return 0;
        if (!boundary->hit(r, rec1.t + eps, infinity, rec2))
            return 0;

        if (debugging)
            std::cerr << "\nt0=" << rec1.t << ", t1=" << rec2.t << '\n';

        rec1.t = std::max(rec1.t, tmin);
        rec2.t = std::min(rec2.t, tmax);

        if (rec1.t >= rec2.t)
            return 0;
        rec1.t = std::max(0.0, rec1.t);

        const auto ray_length = r.direction().length();
        const auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
        const auto hit_distance = neg_inv_density * log(random_double());

        if (hit_distance > distance_inside_boundary)
            return 0;

        rec.t = rec1.t + hit_distance / ray_length;
        rec.p = r.at(rec.t);

        if (debugging)
        {
            std::cerr << "hit_distance = " << hit_distance << '\n'
                      << "rec.t = " << rec.t << '\n'
                      << "rec.p = " << rec.p << '\n';
        }

        rec.norm = Vec3(1, 0, 0); // arbitrary
        rec.front_face = 1;       // also arbitrary
        rec.mat_ptr = phase_function;

        return 1;
    }

    bool bounding_box(double t0, double t1, AABB &output_box) const override
    {
        return boundary->bounding_box(t0, t1, output_box);
    }
};

#endif