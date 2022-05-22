#pragma once

#include "raytracer.h"
#include "hittable.hpp"
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
    void getSphereUV(const Vec3 &p, double &u, double &v) const
    {
        // z = cos(phi)
        // x = sin(phi) * cos(theta)
        // y = sin(phi) * sin(theta)
        // u = phi / (2 * pi)
        // v = theta / pi
        // the coordinates here is different from origin
        auto theta = atan2(p.x(), p.z());
        auto phi = acos(p.y());
        u = (theta + PI) / (2 * PI);
        v = phi / PI;
    }

public:
    Sphere() {}
    Sphere(Point3 c, double r,
           shared_ptr<Material> mat_ptr)
        : center(c), radius(r), mat_ptr(mat_ptr) {}

    bool hit(const Ray &r, double t_min,
             double t_max, HitRecord &rec) const override
    {
        Vec3 oc = r.origin() - center;
        auto a = r.direction().lengthSquared();
        auto half_b = dot(oc, r.direction());
        auto c = oc.lengthSquared() - radius * radius;

        auto discriminant = half_b * half_b - a * c;
        if (discriminant < 0)
            return false;
        auto sqrtd = sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        auto root = (-half_b - sqrtd) / a;
        if (root < t_min || t_max < root)
        {
            root = (-half_b + sqrtd) / a;
            if (root < t_min || t_max < root)
                return false;
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        Vec3 outward_normal = (rec.p - center) / radius;
        rec.setFaceNormal(r, outward_normal);
        getSphereUV(outward_normal, rec.u, rec.v);
        rec.mat_ptr = mat_ptr;

        return true;
    }

    bool boundingBox(double t0, double t1,
                     AABB &output_box) const override
    {
        output_box = AABB(center - Vec3(radius, radius, radius),
                          center + Vec3(radius, radius, radius));
        return true;
    }

    double pdfValue(const Point3 &origin, const Vec3 &v) const override
    {
        HitRecord rec;
        if (!hit(Ray(origin, v), 0.001, INF, rec))
            return 0;

        auto cos_phi_max = std::sqrt(
            1 - radius * radius / (center - origin).lengthSquared());
        auto solid_angle = 2 * PI * (1 - cos_phi_max);

        return 1 / solid_angle;
    }

    Vec3 random(const Point3 &origin) const override
    {
        Vec3 direction = center - origin;
        auto distance_squared = direction.lengthSquared();
        ONB uvw;
        uvw.buildFromW(direction);
        return uvw.local(randomToSphere(radius, distance_squared));
    }
};