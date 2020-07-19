#ifndef __TEXTURE__
#define __TEXTURE__

#include "raytracer.h"

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

#endif