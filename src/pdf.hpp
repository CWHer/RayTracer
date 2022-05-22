#pragma once

#include "raytracer.h"
#include "hittable.hpp"
#include "onb.hpp"

inline Vec3 randomCosineDirection()
{
    auto r1 = randomReal();
    auto r2 = randomReal();
    auto z = std::sqrt(1 - r2);

    auto theta = 2 * PI * r1;
    auto x = cos(theta) * std::sqrt(r2);
    auto y = sin(theta) * std::sqrt(r2);

    return Vec3(x, y, z);
}

inline Vec3 randomToSphere(
    double radius, double distance_squared)
{
    auto r1 = randomReal();
    auto r2 = randomReal();
    auto z = 1 + r2 * (sqrt(1 - radius * radius / distance_squared) - 1);

    auto theta = 2 * PI * r1;
    auto x = cos(theta) * std::sqrt(1 - z * z);
    auto y = sin(theta) * std::sqrt(1 - z * z);

    return Vec3(x, y, z);
}

class PDF
{
public:
    virtual ~PDF() {}
    virtual double value(const Vec3 &direction) const = 0;
    virtual Vec3 generate() const = 0;
};

class CosinePDF : public PDF
{
private:
    ONB uvw;

public:
    CosinePDF(const Vec3 &w) { uvw.buildFromW(w); }

    double value(const Vec3 &direction) const override
    {
        auto cosine = dot(unitVector(direction), uvw.w());
        return cosine <= 0 ? 0 : cosine / PI;
    }

    Vec3 generate() const override
    {
        return uvw.local(randomCosineDirection());
    }
};

class HittablePDF : public PDF
{
private:
    Point3 origin;
    shared_ptr<Hittable> ptr;

public:
    HittablePDF(
        shared_ptr<Hittable> p,
        const Point3 &origin)
        : origin(origin), ptr(p) {}

    double value(const Vec3 &direction) const override
    {
        return ptr->pdfValue(origin, direction);
    }

    Vec3 generate() const override
    {
        return ptr->random(origin);
    }
};

class MixturePDF : public PDF
{
private:
    shared_ptr<PDF> p0, p1;

public:
    MixturePDF(
        shared_ptr<PDF> p0,
        shared_ptr<PDF> p1)
        : p0(p0), p1(p1) {}

    double value(const Vec3 &direction) const override
    {
        return 0.5 * p0->value(direction) +
               0.5 * p1->value(direction);
    }

    Vec3 generate() const override
    {
        return randomReal() < 0.5
                   ? p0->generate()
                   : p1->generate();
    }
};