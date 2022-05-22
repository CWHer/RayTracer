#pragma once

#include "raytracer.h"

class Vec3
{
    friend std::ostream &operator<<(std::ostream &, const Vec3 &);
    friend Vec3 operator+(const Vec3 &, const Vec3 &);
    friend Vec3 operator-(const Vec3 &, const Vec3 &);
    friend Vec3 operator*(const Vec3 &, const Vec3 &);
    friend Vec3 operator*(double, const Vec3 &);
    friend Vec3 operator*(const Vec3 &, double);
    friend Vec3 operator/(const Vec3 &, double);
    friend double dot(const Vec3 &, const Vec3 &);
    friend Vec3 cross(const Vec3 &, const Vec3 &);
    friend Vec3 unitVector(Vec3 v);

private:
    double e[3];

public:
    Vec3() : e{0, 0, 0} {}
    Vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}

    double x() const { return e[0]; }
    double y() const { return e[1]; }
    double z() const { return e[2]; }

    inline static Vec3 random()
    {
        return Vec3(randomReal(), randomReal(), randomReal());
    }

    inline static Vec3 random(double min, double max)
    {
        return Vec3(randomReal(min, max), randomReal(min, max), randomReal(min, max));
    }

    Vec3 operator-() const { return Vec3(-e[0], -e[1], -e[2]); }
    double operator[](int i) const { return e[i]; }
    double &operator[](int i) { return e[i]; }

    Vec3 &operator+=(const Vec3 &v)
    {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;
    }

    Vec3 &operator*=(const double t)
    {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    Vec3 &operator/=(const double t)
    {
        return *this *= 1 / t;
    }

    double length() const
    {
        return std::sqrt(lengthSquared());
    }

    double lengthSquared() const
    {
        return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
    }

    bool nearZero() const
    {
        return std::fabs(e[0]) < 1e-6 &&
               std::fabs(e[1]) < 1e-6 &&
               std::fabs(e[2]) < 1e-6;
    }
};

// Vec3 Utility Functions

inline std::ostream &operator<<(std::ostream &out, const Vec3 &v)
{
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline Vec3 operator+(const Vec3 &u, const Vec3 &v)
{
    return Vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline Vec3 operator-(const Vec3 &u, const Vec3 &v)
{
    return Vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline Vec3 operator*(const Vec3 &u, const Vec3 &v)
{
    return Vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline Vec3 operator*(double t, const Vec3 &v)
{
    return Vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

inline Vec3 operator*(const Vec3 &v, double t)
{
    return t * v;
}

inline Vec3 operator/(const Vec3 &v, double t)
{
    return (1 / t) * v;
}

inline double dot(const Vec3 &u, const Vec3 &v)
{
    return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
}

inline Vec3 cross(const Vec3 &u, const Vec3 &v)
{
    return Vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
                u.e[2] * v.e[0] - u.e[0] * v.e[2],
                u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline Vec3 unitVector(Vec3 v)
{
    return v / v.length();
}

// rand functions
inline Vec3 randomInUnitSphere()
{
    while (true)
    {
        Vec3 p = Vec3::random(-1, 1);
        if (p.length() < 1)
            return p;
    }
}

inline Vec3 randomInUnitDisk()
{
    while (true)
    {
        Vec3 p = Vec3(randomReal(-1, 1), randomReal(-1, 1), 0);
        if (p.length() < 1)
            return p;
    }
}

inline Vec3 randomInHemisphere(const Vec3 &normal)
{
    // a uniform scatter direction for all angles
    Vec3 in_unit_sphere = randomInUnitSphere();
    return dot(in_unit_sphere, normal) > 0 ? in_unit_sphere : -in_unit_sphere;
}

inline Vec3 randomUnitVector()
{
    // on the surface of a unit sphere
    auto a = randomReal(0, 2 * PI);
    auto z = randomReal(-1, 1);
    auto r = sqrt(1 - z * z);
    return Vec3(r * cos(a), r * sin(a), z);
}

Vec3 reflect(const Vec3 &v, const Vec3 &n)
{
    return v - 2 * dot(v, n) * n;
}

Vec3 refract(const Vec3 &uv, const Vec3 &n, double etai_over_etat)
{
    // uv should be unit vector
    auto cos_theta = dot(-uv, n);
    Vec3 r_out_parallel = etai_over_etat * (uv + cos_theta * n);
    Vec3 r_out_perp = -sqrt(fabs(1.0 - r_out_parallel.lengthSquared())) * n;
    return r_out_parallel + r_out_perp;
}

// Type aliases for Vec3
typedef Vec3 Point3; // 3D point
typedef Vec3 Color;  // RGB color