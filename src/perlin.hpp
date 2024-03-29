#pragma once

#include "raytracer.h"

class Perlin
{
private:
    static const int point_count = 256;
    std::vector<Vec3> ranvec;
    std::vector<int> perm_x;
    std::vector<int> perm_y;
    std::vector<int> perm_z;

    static std::vector<int> generatePerm()
    {
        std::vector<int> p(point_count);
        for (int i = 0; i < point_count; ++i)
            p[i] = i;
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::shuffle(p.begin(), p.end(), gen);
        return p;
    }

    inline double interp(Vec3 c[2][2][2],
                         double u, double v, double w) const
    {
        auto uu = u * u * (3 - 2 * u);
        auto vv = v * v * (3 - 2 * v);
        auto ww = w * w * (3 - 2 * w);
        auto accum = 0.0;

        for (int i = 0; i < 2; i++)
            for (int j = 0; j < 2; j++)
                for (int k = 0; k < 2; k++)
                {
                    Vec3 weight_v(u - i, v - j, w - k);
                    accum += (i * uu + (1 - i) * (1 - uu)) *
                             (j * vv + (1 - j) * (1 - vv)) *
                             (k * ww + (1 - k) * (1 - ww)) *
                             dot(c[i][j][k], weight_v);
                }

        return accum;
    }

public:
    Perlin()
    {
        ranvec.resize(point_count);
        for (int i = 0; i < point_count; ++i)
            ranvec[i] = unitVector(Vec3::random(-1, 1));

        perm_x = generatePerm();
        perm_y = generatePerm();
        perm_z = generatePerm();
    }

    double noise(const Point3 &p) const
    {
        auto u = p.x() - floor(p.x());
        auto v = p.y() - floor(p.y());
        auto w = p.z() - floor(p.z());
        int i = static_cast<int>(floor(p.x()));
        int j = static_cast<int>(floor(p.y()));
        int k = static_cast<int>(floor(p.z()));
        Vec3 c[2][2][2];

        for (int di = 0; di < 2; di++)
            for (int dj = 0; dj < 2; dj++)
                for (int dk = 0; dk < 2; dk++)
                    c[di][dj][dk] = ranvec[perm_x[(i + di) & 255] ^
                                           perm_y[(j + dj) & 255] ^
                                           perm_z[(k + dk) & 255]];

        return interp(c, u, v, w);
    }

    double turb(const Point3 &p, int depth = 7) const
    {
        auto accum = 0.0;
        auto temp_p = p;
        auto weight = 1.0;

        for (int i = 0; i < depth; i++)
        {
            accum += weight * noise(temp_p);
            weight *= 0.5;
            temp_p *= 2;
        }

        return fabs(accum);
    }
};
