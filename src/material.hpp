// 1. Produce a scattered ray (or say it absorbed the incident ray).
// 2. If scattered, say how much the ray should be attenuated.

#pragma once

#include "raytracer.h"

class Material
{
public:
    virtual bool scatter(
        const Ray &r_in, const HitRecord &rec,
        Color &attenuation, Ray &scattered) const = 0;
};

class Lambertian : public Material
{
private:
    Color albedo; // similar to reflectance

public:
    Lambertian(const Color &a) : albedo(a) {}

    // scatter always and attenuate by its reflectance R
    // Note we could just as well only scatter with some probability p
    //  and have attenuation be albedo/p.
    bool scatter(const Ray &r_in, const HitRecord &rec,
                 Color &attenuation, Ray &scattered) const override
    {
        Vec3 scatter_direction = rec.normal + randomUnitVector();

        // Catch degenerate scatter direction
        if (scatter_direction.nearZero())
            scatter_direction = rec.normal;

        scattered = Ray(rec.p, scatter_direction);
        attenuation = albedo;
        return true;
    }
};

class Metal : public Material
{
private:
    Color albedo;
    double fuzz;

public:
    Metal(const Color &a, double f) : albedo(a), fuzz(f) {}

    bool scatter(const Ray &r_in, const HitRecord &rec,
                 Color &attenuation, Ray &scattered) const override
    {
        Vec3 reflected = reflect(unitVector(r_in.direction()), rec.normal);
        scattered = Ray(rec.p, reflected + fuzz * randomInUnitSphere());
        attenuation = albedo;
        // The catch is that for big spheres or grazing rays, we may scatter below the surface.
        // We can just have the surface absorb those.
        return dot(scattered.direction(), rec.normal) > 0;
    }
};

class Dielectric : public Material
{
private:
    double ref_idx;

    // Schlick Approximation
    double reflectance(double cosine, double ref_idx) const
    {
        auto r0 = pow((1 - ref_idx) / (1 + ref_idx), 2);
        return r0 + (1 - r0) * pow((1 - cosine), 5);
    }

public:
    Dielectric(double r) : ref_idx(r) {}

    bool scatter(const Ray &r_in, const HitRecord &rec,
                 Color &attenuation, Ray &scattered) const override
    {
        // Attenuation is always 1 — the glass surface absorbs nothing
        attenuation = Color(1, 1, 1);
        double refraction_ratio = rec.front_face ? (1.0 / ref_idx) : ref_idx;

        Vec3 unit_direction = unitVector(r_in.direction());
        double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

        // total internal reflection
        bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        Vec3 direction = cannot_refract || reflectance(cos_theta, refraction_ratio) > randomReal()
                             ? reflect(unit_direction, rec.normal)
                             : refract(unit_direction, rec.normal, refraction_ratio);

        scattered = Ray(rec.p, direction);
        return true;
    }
};