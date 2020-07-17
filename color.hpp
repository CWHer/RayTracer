#ifndef __COLOR__
#define __COLOR__

#include "vec3.hpp"
#include <iostream>
void write_color(std::ostream &out, color pixel_color)
{
    out << static_cast<int>(255.999 * pixel_color.x()) << ' '
        << static_cast<int>(255.999 * pixel_color.y()) << ' '
        << static_cast<int>(255.999 * pixel_color.z()) << '\n';
}

#endif