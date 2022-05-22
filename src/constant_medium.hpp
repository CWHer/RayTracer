#pragma once

#include "hittable.h"
#include "raytracer.h"
#include "material.hpp"

class ConstantMedium : public Hittable
{
private:
    shared_ptr<Hittable> boundary;
    double neg_inv_density;
    shared_ptr<Material> phase_function;

public:
    ConstantMedium(shared_ptr<Hittable> b,
                   double d, shared_ptr<Texture> a)
        : boundary(b), neg_inv_density(-1 / d),
          phase_function(make_shared<Isotropic>(a)) {}

    ConstantMedium(shared_ptr<Hittable> b, double d, Color c)
        : boundary(b), neg_inv_density(-1 / d),
          phase_function(make_shared<Isotropic>(c)) {}

    bool hit(const Ray &r, double t_min,
             double t_max, HitRecord &rec) const override
    {
        // Print occasional samples when debugging. To enable, set enableDebug true.
        const bool enableDebug = false;
        const bool debugging = enableDebug && randomReal() < 0.00001;

        HitRecord rec1, rec2;

        if (!boundary->hit(r, -INF, INF, rec1))
            return false;
        if (!boundary->hit(r, rec1.t + 0.0001, INF, rec2))
            return false;

        if (debugging)
            std::cerr << "\nt0=" << rec1.t << ", t1=" << rec2.t << '\n';

        rec1.t = std::max(rec1.t, t_min);
        rec2.t = std::min(rec2.t, t_max);

        if (rec1.t >= rec2.t)
            return false;
        rec1.t = std::max(0.0, rec1.t);

        const auto ray_length = r.direction().length();
        const auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
        const auto hit_distance = neg_inv_density * log(randomReal());

        if (hit_distance > distance_inside_boundary)
            return false;

        rec.t = rec1.t + hit_distance / ray_length;
        rec.p = r.at(rec.t);

        if (debugging)
        {
            std::cerr << "hit_distance = " << hit_distance << '\n'
                      << "rec.t = " << rec.t << '\n'
                      << "rec.p = " << rec.p << '\n';
        }

        rec.normal = Vec3(1, 0, 0); // arbitrary
        rec.front_face = true;      // also arbitrary
        rec.mat_ptr = phase_function;

        return true;
    }

    bool boundingBox(double t0, double t1,
                     AABB &output_box) const override
    {
        return boundary->boundingBox(t0, t1, output_box);
    }
};
