// 2 main things
// 1. Produce a scattered ray (or say it absorbed the incident ray).
// 2. If scattered, say how much the ray should be attenuated.

#ifndef __MATERIAL__
#define __MATERIAL__

#include "raytracer.h"

class Material
{
public:
    virtual bool scatter(
        const Ray &r_in, const hit_record &rec, Color &attenuation, Ray &scattered) const = 0;
};

class Lambertian : public Material
{
private:
    Color albedo; //similar to reflectance
public:
    Lambertian(const Color &_a) : albedo(_a) {}

    // scatter always and attenuate by its reflectance R
    // Note we could just as well only scatter with some probability p and have attenuation be albedo/p.
    bool scatter(
        const Ray &r_in, const hit_record &rec, Color &attenuation, Ray &scattered) const override
    {
        Vec3 scatter_dir = rec.norm + random_unit_vector();
        scattered = Ray(rec.p, scatter_dir);
        attenuation = albedo;
        return 1;
    }
};

class Metal : public Material
{
private:
    Color albedo;
    double fuzz;

public:
    Metal(const Color &_a, double _f) : albedo(_a), fuzz(_f) {}
    bool scatter(
        const Ray &r_in, const hit_record &rec, Color &attenuation, Ray &scattered) const override
    {
        Vec3 reflected = reflect(unit_vector(r_in.direction()), rec.norm);
        scattered = Ray(rec.p, reflected + fuzz * random_in_unit_sphere());
        attenuation = albedo;
        // The catch is that for big spheres or grazing rays, we may scatter below the surface.
        // We can just have the surface absorb those.
        return dot(reflected, rec.norm) > 0;
    }
};

#endif