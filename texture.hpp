#ifndef __TEXTURE__
#define __TEXTURE__

#include "raytracer.h"
#include "raytracer_stb_image.h"
#include "perlin.hpp"

class Texture
{
public:
    virtual Color value(double u, double v, const Point3 &p) const = 0;
};

class SolidColor : public Texture
{
private:
    Color color_value;

public:
    SolidColor() {}
    SolidColor(Color _c) : color_value(_c) {}

    SolidColor(double _r, double _g, double _b)
        : SolidColor(Color(_r, _g, _b)) {}

    Color value(double u, double v, const Point3 &p) const override
    {
        return color_value;
    }
};

class CheckerTexture : public Texture
{
private:
    shared_ptr<Texture> even;
    shared_ptr<Texture> odd;

public:
    CheckerTexture() {}
    CheckerTexture(shared_ptr<Texture> t0, shared_ptr<Texture> t1) : even(t0), odd(t1) {}

    Color value(double u, double v, const Point3 &p) const override
    {
        auto sines = sin(10 * p.x()) * sin(10 * p.y()) * sin(10 * p.z());
        if (sines < 0)
            return odd->value(u, v, p);
        else
            return even->value(u, v, p);
    }
};

class NoiseTexture : public Texture
{
private:
    Perlin noise;
    double scale; //larger scale means more frequent

public:
    NoiseTexture() : scale(1) {}
    NoiseTexture(double _scale) : scale(_scale) {}

    // Color value(double u, double v, const Point3 &p) const override
    // {
    //     return Color(1, 1, 1) * 0.5 * (1.0 + noise.noise(scale * p));
    // }

    Color value(double u, double v, const Point3 &p) const override
    { //marble-like effect
        return Color(1, 1, 1) * 0.5 * (1 + sin(scale * p.z() + 10 * noise.turb(p)));
    }
};

class IMGTexture : public Texture
{
private:
    static const int bytes_per_pixel = 3;
    unsigned char *data;
    int width, height;
    int bytes_per_scanline;

public:
    IMGTexture()
        : data(nullptr), width(0), height(0), bytes_per_scanline(0) {}

    IMGTexture(const char *filename)
    {
        auto components_per_pixel = bytes_per_pixel;

        data = stbi_load(
            filename, &width, &height, &components_per_pixel, components_per_pixel);

        if (!data)
        {
            std::cerr << "ERROR: Could not load texture image file " << filename << "'.\n";
            width = height = 0;
        }

        bytes_per_scanline = bytes_per_pixel * width;
    }

    ~IMGTexture()
    {
        delete data;
    }

    virtual Color value(double u, double v, const Vec3 &p) const override
    {
        // If we have no texture data, then return solid cyan as a debugging aid.
        if (data == nullptr)
            return Color(0, 1, 1);

        // Clamp input texture coordinates to [0,1] x [1,0]
        u = clamp(u, 0.0, 1.0);
        v = clamp(v, 0.0, 1.0); // Flip V to image coordinates

        auto i = static_cast<int>(u * width);
        auto j = static_cast<int>(v * height);

        // Clamp integer mapping, since actual coordinates should be less than 1.0
        if (i >= width)
            i = width - 1;
        if (j >= height)
            j = height - 1;

        const auto color_scale = 1.0 / 255.0;
        auto pixel = data + j * bytes_per_scanline + i * bytes_per_pixel; //left_upper_corner

        return Color(color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]);
    }
};

#endif