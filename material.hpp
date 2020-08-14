// 2 main things
// 1. Produce a scattered ray (or say it absorbed the incident ray).
// 2. If scattered, say how much the ray should be attenuated.

#ifndef __MATERIAL__
#define __MATERIAL__

#include "raytracer.h"
#include "texture.hpp"
#include "onb.hpp"

class Material
{
public:
    virtual bool scatter(
        const Ray &r_in, const hit_record &rec, Color &albedo, Ray &scattered, double &pdf) const
    {
        return 0;
    }

    virtual double scattering_pdf(const Ray &r_in, const hit_record &rec, const Ray &scattered) const
    {
        return 0;
    }
    virtual Color emitted(const Ray &r_in, const hit_record &rec, double u, double v, const Point3 &p) const
    {
        return Color(0, 0, 0);
    }
};

class Lambertian : public Material
{
private:
    shared_ptr<Texture> albedo;

public:
    Lambertian(shared_ptr<Texture> _a) : albedo(_a) {}

    // scatter always and attenuate by its reflectance R
    // Note we could just as well only scatter with some probability p and have attenuation be albedo/p.
    bool scatter(
        const Ray &r_in, const hit_record &rec, Color &alb, Ray &scattered, double &pdf) const override
    {
        ONB uvw;
        uvw.build_from_w(rec.norm);
        auto direction = uvw.local(random_cosine_direction());
        scattered = Ray(rec.p, unit_vector(direction), r_in.time());
        alb = albedo->value(rec.u, rec.v, rec.p);
        pdf = dot(rec.norm, scattered.direction()) / pi;
        return 1;
    }
    double scattering_pdf(
        const Ray &r_in, const hit_record &rec, const Ray &scattered) const override
    {
        auto cosine = dot(rec.norm, unit_vector(scattered.direction()));
        return cosine < 0 ? 0 : cosine / pi;
    }
};

// class Metal : public Material
// {
// private:
//     Color albedo;
//     double fuzz;

// public:
//     Metal(const Color &_a, double _f) : albedo(_a), fuzz(_f) {}
//     bool scatter(
//         const Ray &r_in, const hit_record &rec, Color &attenuation, Ray &scattered) const override
//     {
//         Vec3 reflected = reflect(unit_vector(r_in.direction()), rec.norm);
//         scattered = Ray(rec.p, reflected + fuzz * random_in_unit_sphere());
//         attenuation = albedo;
//         // The catch is that for big spheres or grazing rays, we may scatter below the surface.
//         // We can just have the surface absorb those.
//         return dot(reflected, rec.norm) > 0;
//     }
// };

// class Dielectric : public Material
// {
// private:
//     double ref_idx;

//     //Schlick Approximation
//     double schlick(double cosine, double ref_idx) const
//     {
//         auto r0 = (1 - ref_idx) / (1 + ref_idx);
//         r0 = r0 * r0;
//         return r0 + (1 - r0) * pow((1 - cosine), 5);
//     }

// public:
//     Dielectric(double _r) : ref_idx(_r) {}

//     bool scatter(
//         const Ray &r_in, const hit_record &rec, Color &attenuation, Ray &scattered) const override
//     { //Attenuation is always 1 — the glass surface absorbs nothing
//         attenuation = Color(1, 1, 1);
//         double etai_over_etat;
//         if (rec.front_face)
//             etai_over_etat = 1.0 / ref_idx;
//         else
//             etai_over_etat = ref_idx;

//         Vec3 unit_direction = unit_vector(r_in.direction());
//         double cos_theta = fmin(dot(-unit_direction, rec.norm), 1.0);
//         double sin_theta = sqrt(1.0 - cos_theta * cos_theta);
//         //total internal reflection
//         if (etai_over_etat * sin_theta > 1.0)
//         {
//             // Must Reflect
//             Vec3 reflected = reflect(unit_direction, rec.norm);
//             scattered = Ray(rec.p, reflected);
//             return 1;
//         }
//         // Can Refract
//         double reflect_prob = schlick(cos_theta, etai_over_etat);
//         if (random_double() < reflect_prob)
//         {
//             Vec3 reflected = reflect(unit_direction, rec.norm);
//             scattered = Ray(rec.p, reflected);
//             return 1;
//         }
//         Vec3 refracted = refract(unit_direction, rec.norm, etai_over_etat);
//         scattered = Ray(rec.p, refracted);
//         return 1;
//     }
// };

class DiffuseLight : public Material
{
private:
    shared_ptr<Texture> emit;

public:
    DiffuseLight(shared_ptr<Texture> _emit) : emit(_emit) {}

    bool scatter(
        const Ray &r_in, const hit_record &rec, Color &alb, Ray &scattered, double &pdf) const override
    {
        return 0;
    }

    virtual Color emitted(const Ray &r_in, const hit_record &rec, double u, double v, const Point3 &p) const
    {
        if (rec.front_face)
            return emit->value(u, v, p);
        else
            return Color(0, 0, 0);
    }
};

// class Isotropic : public Material
// {
// private:
//     shared_ptr<Texture> albedo;

// public:
//     Isotropic(shared_ptr<Texture> _a) : albedo(_a) {}

//     bool scatter(
//         const Ray &r_in, const hit_record &rec, Color &attenuation, Ray &scattered) const override
//     {
//         scattered = Ray(rec.p, random_in_unit_sphere(), r_in.time());
//         attenuation = albedo->value(rec.u, rec.v, rec.p);
//         return 1;
//     }
// };

#endif