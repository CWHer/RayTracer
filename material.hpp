// 2 main things
// 1. Produce a scattered ray (or say it absorbed the incident ray).
// 2. If scattered, say how much the ray should be attenuated.

#ifndef __MATERIAL__
#define __MATERIAL__

#include "raytracer.h"
#include "texture.hpp"
#include "onb.hpp"
#include "pdf.hpp"

struct scatter_record
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
        const Ray &r_in, const hit_record &rec, scatter_record &srec) const
    {
        return 0;
    }

    virtual double scattering_pdf(
        const Ray &r_in, const hit_record &rec, const Ray &scattered) const
    {
        return 0;
    }
    virtual Color emitted(
        const Ray &r_in, const hit_record &rec, double u, double v, const Point3 &p) const
    {
        return Color(0, 0, 0);
    }
};

class Lambertian : public Material
{
private:
    shared_ptr<Texture> albedo;

public:
    Lambertian(const Color &_a) : albedo(make_shared<SolidColor>(_a)) {}
    Lambertian(shared_ptr<Texture> _a) : albedo(_a) {}

    // scatter always and attenuate by its reflectance R
    // Note we could just as well only scatter with some probability p and have attenuation be albedo/p.
    bool scatter(
        const Ray &r_in, const hit_record &rec, scatter_record &srec) const override
    {
        srec.is_specular = false;
        srec.attenuation = albedo->value(rec.u, rec.v, rec.p);
        srec.pdf_ptr = make_shared<CosinePDF>(rec.norm);
        return 1;
    }
    double scattering_pdf(
        const Ray &r_in, const hit_record &rec, const Ray &scattered) const override
    {
        auto cosine = dot(rec.norm, unit_vector(scattered.direction()));
        return cosine < 0 ? 0 : cosine / pi;
    }
};

class Metal : public Material
{
private:
    Color albedo;
    double fuzz;

public:
    Metal(const Color &_a, double _f) : albedo(_a), fuzz(_f < 1 ? _f : 1) {}
    bool scatter(
        const Ray &r_in, const hit_record &rec, scatter_record &srec) const override
    {
        Vec3 reflected = reflect(unit_vector(r_in.direction()), rec.norm);
        srec.specular_ray = Ray(rec.p, reflected + fuzz * random_in_unit_sphere());
        srec.attenuation = albedo;
        srec.is_specular = 1, srec.pdf_ptr = nullptr;
        // The catch is that for big spheres or grazing rays, we may scatter below the surface.
        // We can just have the surface absorb those.
        return dot(reflected, rec.norm) > 0;
    }
};

class Dielectric : public Material
{
private:
    double ref_idx;

    //Schlick Approximation
    double schlick(double cosine, double ref_idx) const
    {
        auto r0 = (1 - ref_idx) / (1 + ref_idx);
        r0 = r0 * r0;
        return r0 + (1 - r0) * pow((1 - cosine), 5);
    }

public:
    Dielectric(double _r) : ref_idx(_r) {}

    bool scatter(
        const Ray &r_in, const hit_record &rec, scatter_record &srec) const override
    { //Attenuation is always 1 — the glass surface absorbs nothing
        srec.is_specular = 1, srec.pdf_ptr = nullptr;
        srec.attenuation = Color(1, 1, 1);
        double etai_over_etat = (rec.front_face) ? (1.0 / ref_idx) : ref_idx;

        Vec3 unit_direction = unit_vector(r_in.direction());
        double cos_theta = fmin(dot(-unit_direction, rec.norm), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta * cos_theta);
        //total internal reflection
        // Must Reflect >1.0
        if ((etai_over_etat * sin_theta > 1.0) ||
            (random_double() < schlick(cos_theta, etai_over_etat)))
        {

            Vec3 reflected = reflect(unit_direction, rec.norm);
            srec.specular_ray = Ray(rec.p, reflected, r_in.time());
            return 1;
        }

        Vec3 refracted = refract(unit_direction, rec.norm, etai_over_etat);
        srec.specular_ray = Ray(rec.p, refracted, r_in.time());
        return 1;
    }
};

class DiffuseLight : public Material
{
private:
    shared_ptr<Texture> emit;

public:
    DiffuseLight(shared_ptr<Texture> _emit) : emit(_emit) {}

    virtual Color emitted(const Ray &r_in, const hit_record &rec, double u, double v, const Point3 &p) const
    {
        if (rec.front_face)
            return emit->value(u, v, p);
        else
            return Color(0, 0, 0);
    }
};

class Isotropic : public Material
{
private:
    shared_ptr<Texture> albedo;

public:
    Isotropic(shared_ptr<Texture> _a) : albedo(_a) {}

    // bool scatter(
    //     const Ray &r_in, const hit_record &rec, Color &attenuation, Ray &scattered) const override
    // {
    //     scattered = Ray(rec.p, random_in_unit_sphere(), r_in.time());
    //     attenuation = albedo->value(rec.u, rec.v, rec.p);
    //     return 1;
    // }
};

#endif