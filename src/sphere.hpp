#ifndef __SPHERE__
#define __SPHERE__

#include "hittable.hpp"
#include "vec3.hpp"
#include "aabb.hpp"
#include "onb.hpp"
#include "pdf.hpp"

class Sphere : public Hittable
{
private:
    Point3 center;
    double radius;
    shared_ptr<Material> mat_ptr;

    // u:phi v:theta
    void get_sphere_uv(const Vec3 &p, double &u, double &v) const
    {
        // z=cos(phi)
        // x=sin(phi)cos(theta)
        // y=sin(phi)sin(theta)
        // u=phi/(2pi) v=theta/pi
        // the coordinates of this image is different from origin
        // orgin->this x->z y->x z->y
        auto theta = atan2(p.x(), p.z());
        auto phi = acos(p.y());
        u = (theta + pi) / (2 * pi); // notice:-90->0
        v = phi / pi;
    }

public:
    Sphere() {}
    Sphere(Point3 _c, double _r, shared_ptr<Material> _mat_ptr)
        : center(_c), radius(_r), mat_ptr(_mat_ptr) {}

    bool hit(const Ray &r, double tmin, double tmax, hit_record &rec) const override
    {
        Vec3 oc = r.origin() - center;
        auto a = r.direction().length_sqr();
        auto half_b = dot(oc, r.direction());
        auto c = oc.length_sqr() - radius * radius;
        auto discriminant = half_b * half_b - a * c;

        if (discriminant > 0)
        {
            auto root = sqrt(discriminant);
            auto temp = (-half_b - root) / a; // negative root
            if (temp < tmax && temp > tmin)
            {
                rec.t = temp;
                rec.p = r.at(rec.t);
                Vec3 outward_norm = (rec.p - center) / radius;
                rec.set_face_normal(r, outward_norm);
                get_sphere_uv((rec.p - center) / radius, rec.u, rec.v);
                rec.mat_ptr = mat_ptr;
                return 1;
            }
            temp = (-half_b + root) / a; // positive root
            if (temp < tmax && temp > tmin)
            {
                rec.t = temp;
                rec.p = r.at(rec.t);
                Vec3 outward_norm = (rec.p - center) / radius;
                rec.set_face_normal(r, outward_norm);
                get_sphere_uv((rec.p - center) / radius, rec.u, rec.v);
                rec.mat_ptr = mat_ptr;
                return 1;
            }
        }
        return 0;
    }

    bool bounding_box(double t0, double t1, AABB &output_box) const override
    {
        output_box = AABB(
            center - Vec3(radius, radius, radius),
            center + Vec3(radius, radius, radius));
        return 1;
    }

    double pdf_value(const Point3 &ori, const Vec3 &v) const override
    {
        hit_record rec;
        if (!hit(Ray(ori, v), eps, infinity, rec))
            return 0;

        auto cos_phi_max = sqrt(1 - radius * radius / (center - ori).length_sqr());
        auto solid_angle = 2 * pi * (1 - cos_phi_max);

        return 1 / solid_angle;
    }

    Vec3 random(const Point3 &ori) const override
    {
        Vec3 direction = center - ori;
        auto distance_squared = direction.length_sqr();
        ONB uvw;
        uvw.build_from_w(direction);
        return uvw.local(random_to_sphere(radius, distance_squared));
    }
};

#endif