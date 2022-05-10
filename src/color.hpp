#ifndef __COLOR__
#define __COLOR__

#include "raytracer.h"

void write_color(std::ostream &out, Color pixel_color, int samples_per_pixel)
{
    // multi-sampled color computation
    // // auto scale = 1.0 / samples_per_pixel;
    // auto r = pixel_color.x() / samples_per_pixel;
    // auto g = pixel_color.y() / samples_per_pixel;
    // auto b = pixel_color.z() / samples_per_pixel;

    // Divide the color total by the number of samples and gamma-correct for gamma=2.0.
    // auto scale = 1.0 / samples_per_pixel;
    auto r = sqrt(pixel_color.x() / samples_per_pixel);
    auto g = sqrt(pixel_color.y() / samples_per_pixel);
    auto b = sqrt(pixel_color.z() / samples_per_pixel);

    // Write the translated [0,255] value of each color component.
    out << static_cast<int>(256 * clamp(r, 0, 1 - eps)) << ' '
        << static_cast<int>(256 * clamp(g, 0, 1 - eps)) << ' '
        << static_cast<int>(256 * clamp(b, 0, 1 - eps)) << '\n';
}

#endif