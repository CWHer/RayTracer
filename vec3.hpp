#ifndef __Vec3__
#define __Vec3__

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
    friend Vec3 unit_vector(Vec3 v);

private:
    double e[3];

public:
    Vec3() : e{0, 0, 0} {}
    Vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}

    double x() const { return e[0]; }
    double y() const { return e[1]; }
    double z() const { return e[2]; }

    Vec3 operator-() const { return Vec3(-e[0], -e[1], -e[2]); }
    double operator[](int i) const { return e[i]; }
    double &operator[](int i) { return e[i]; }

    Vec3 &operator+=(const Vec3 &rhs)
    {
        for (int i = 0; i < 3; ++i)
            e[i] += rhs.e[i];
        return *this;
    }

    Vec3 &operator*=(const double t)
    {
        for (int i = 0; i < 3; ++i)
            e[i] *= t;
        return *this;
    }

    Vec3 &operator/=(const double t)
    {
        return *this *= 1 / t;
    }

    double length() const
    {
        return std::sqrt(length_sqr());
    }

    double length_sqr() const
    {
        return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
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

inline Vec3 unit_vector(Vec3 v)
{
    return v / v.length();
}

// rand functions
inline Vec3 random()
{
    return Vec3(random_double(), random_double(), random_double());
}

inline Vec3 random(double min, double max)
{
    return Vec3(random_double(min, max), random_double(min, max), random_double(min, max));
}

inline Vec3 random_in_unit_sphere()
{
    //old version
    Vec3 p;
    do
    {
        p = random(-1, 1);
    } while (p.length() >= 1);
    return p;
}

inline Vec3 random_in_hemisphere(const Vec3 &normal)
{ //a uniform scatter direction for all angles
    Vec3 in_unit_sphere = random_in_unit_sphere();
    if (dot(in_unit_sphere, normal) > 0) // In the same hemisphere as the normal
        return in_unit_sphere;
    else
        return -in_unit_sphere;
}

inline Vec3 random_unit_vector()
{ //on the surface of a unit sphere
    auto a = random_double(0, 2 * pi);
    auto z = random_double(-1, 1);
    auto r = sqrt(1 - z * z);
    return Vec3(r * cos(a), r * sin(a), z);
}

Vec3 reflect(const Vec3 &v, const Vec3 &n)
{
    return v - 2 * dot(v, n) * n;
}

Vec3 refract(const Vec3 &uv, const Vec3 &n, double etai_over_etat)
{ //uv should be unit vector
    auto cos_theta = dot(-uv, n);
    Vec3 r_out_parallel = etai_over_etat * (uv + cos_theta * n);
    Vec3 r_out_perp = -sqrt(1.0 - r_out_parallel.length_sqr()) * n;
    return r_out_parallel + r_out_perp;
}

// Type aliases for Vec3
// using point3 = Vec3;
// using color = Vec3;
typedef Vec3 Point3; // 3D point
typedef Vec3 Color;  // RGB color

#endif