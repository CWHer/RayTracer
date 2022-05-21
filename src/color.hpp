#pragma once

#include "raytracer.h"

void writeColor(std::ostream &out,
                 Color pixel_color,
                 int samples_per_pixel)
{
    // Divide the color total by the number of samples and gamma-correct for gamma=2.0.
    auto scale = 1.0 / samples_per_pixel;
    auto r = sqrt(pixel_color.x() * scale);
    auto g = sqrt(pixel_color.y() * scale);
    auto b = sqrt(pixel_color.z() * scale);

    // Write the translated [0, 255] value of each color component.
    out << static_cast<int>(256 * clamp(r, 0, 0.999)) << ' '
        << static_cast<int>(256 * clamp(g, 0, 0.999)) << ' '
        << static_cast<int>(256 * clamp(b, 0, 0.999)) << '\n';
}