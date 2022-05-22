// 1. Produce a scattered ray (or say it absorbed the incident ray).
// 2. If scattered, say how much the ray should be attenuated.

#pragma once

#include "raytracer.h"
#include "texture.hpp"
#include "onb.hpp"
#include "pdf.hpp"

struct ScatterRecord
{
    Ray specular_ray;
    bool is_specular;
    Color attenuation;
    shared_ptr<PDF> pdf_ptr;
};

class Material
{
public:
    virtual bool scatter(
        const Ray &r_in, const HitRecord &rec,
        ScatterRecord &srec) const { return false; }

    virtual double scatteringPDF(
        const Ray &r_in, const HitRecord &rec,
        const Ray &scattered) const { return 0; }

    virtual Color emitted(
        const Ray &r_in, const HitRecord &rec,
        double u, double v, const Point3 &p) const
    {
        return Color(0, 0, 0);
    }
};

class Lambertian : public Material
{
private:
    shared_ptr<Texture> albedo;

public:
    Lambertian(shared_ptr<Texture> a) : albedo(a) {}
    Lambertian(Color c) : albedo(make_shared<SolidColor>(c)) {}

    bool scatter(const Ray &r_in, const HitRecord &rec,
                 ScatterRecord &srec) const override
    {
        srec.is_specular = false;
        srec.attenuation = albedo->value(rec.u, rec.v, rec.p);
        srec.pdf_ptr = make_shared<CosinePDF>(rec.normal);
        return true;
    }

    double scatteringPDF(
        const Ray &r_in, const HitRecord &rec,
        const Ray &scattered) const override
    {
        auto cosine = dot(
            rec.normal, unitVector(scattered.direction()));
        return cosine < 0 ? 0 : cosine / PI;
    }
};

class Metal : public Material
{
private:
    Color albedo;
    double fuzz;

public:
    Metal(const Color &a, double f)
        : albedo(a), fuzz(f < 1 ? f : 1) {}

    bool scatter(const Ray &r_in, const HitRecord &rec,
                 ScatterRecord &srec) const override
    {
        Vec3 reflected = reflect(
            unitVector(r_in.direction()), rec.normal);
        auto direction = reflected + fuzz * randomInUnitSphere();
        srec.specular_ray = Ray(rec.p, direction, r_in.time());
        srec.attenuation = albedo;
        srec.is_specular = true;
        srec.pdf_ptr = nullptr;

        // The catch is that for big spheres or grazing rays, we may scatter below the surface.
        // We can just have the surface absorb those.
        return dot(direction, rec.normal) > 0;
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
                 ScatterRecord &srec) const override
    {
        srec.is_specular = true;
        srec.pdf_ptr = nullptr;
        srec.attenuation = Color(1, 1, 1);
        double refraction_ratio = rec.front_face ? (1.0 / ref_idx) : ref_idx;

        Vec3 unit_direction = unitVector(r_in.direction());
        double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

        bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        Vec3 direction = cannot_refract || reflectance(cos_theta, refraction_ratio) > randomReal()
                             ? reflect(unit_direction, rec.normal)
                             : refract(unit_direction, rec.normal, refraction_ratio);

        srec.specular_ray = Ray(rec.p, direction, r_in.time());
        return true;
    }
};

class DiffuseLight : public Material
{
private:
    shared_ptr<Texture> emit;

public:
    DiffuseLight(shared_ptr<Texture> emit) : emit(emit) {}
    DiffuseLight(Color c) : emit(make_shared<SolidColor>(c)) {}

    virtual Color emitted(const Ray &r_in, const HitRecord &rec,
                          double u, double v, const Point3 &p) const
    {
        return rec.front_face
                   ? emit->value(u, v, p)
                   : Color(0, 0, 0);
    }
};