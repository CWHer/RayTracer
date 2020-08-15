#ifndef __PDF__
#define __PDF__

#include "raytracer.h"
#include "vec3.hpp"
#include "onb.hpp"
#include "hittable.hpp"

class PDF
{
public:
    virtual ~PDF() {}
    virtual double value(const Vec3 &direction) const = 0;
    virtual Vec3 generate() const = 0;
};

inline Vec3 random_cosine_direction()
{
    auto r1 = random_double();
    auto r2 = random_double();
    auto z = sqrt(1 - r2);

    auto theta = 2 * pi * r1;
    auto x = cos(theta) * sqrt(r2);
    auto y = sin(theta) * sqrt(r2);

    return Vec3(x, y, z);
}

inline Vec3 random_to_sphere(double radius, double distance_squared)
{
    auto r1 = random_double();
    auto r2 = random_double();
    auto z = 1 + r2 * (sqrt(1 - radius * radius / distance_squared) - 1);

    auto theta = 2 * pi * r1;
    auto x = cos(theta) * sqrt(1 - z * z);
    auto y = sin(theta) * sqrt(1 - z * z);

    return Vec3(x, y, z);
}

class CosinePDF : public PDF
{
private:
    ONB uvw;

public:
    CosinePDF(const Vec3 &w) { uvw.build_from_w(w); }

    double value(const Vec3 &direction) const override
    {
        auto cosine = dot(unit_vector(direction), uvw.w());
        return cosine <= 0 ? 0 : cosine / pi;
    }

    Vec3 generate() const override
    {
        return uvw.local(random_cosine_direction());
    }
};

class HittablePDF : public PDF
{
private:
    Point3 ori;
    shared_ptr<Hittable> ptr;

public:
    HittablePDF(shared_ptr<Hittable> p, const Point3 &origin) : ori(origin), ptr(p) {}

    double value(const Vec3 &direction) const override
    {
        return ptr->pdf_value(ori, direction);
    }

    Vec3 generate() const override
    {
        return ptr->random(ori);
    }
};

class MixturePDF : public PDF
{
private:
    shared_ptr<PDF> p[2];

public:
    MixturePDF(shared_ptr<PDF> p0, shared_ptr<PDF> p1)
    {
        p[0] = p0, p[1] = p1;
    }

    double value(const Vec3 &direction) const override
    {
        return 0.5 * p[0]->value(direction) + 0.5 * p[1]->value(direction);
    }

    Vec3 generate() const override
    {
        if (random_double() < 0.5)
            return p[0]->generate();
        else
            return p[1]->generate();
    }
};

#endif