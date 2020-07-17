#ifndef __RAYTRACER__
#define __RAYTRACER__

#include <cmath>
#include <cstdlib>
#include <limits>
#include <memory>
#include <iostream>

// Usings

using std::make_shared;
using std::shared_ptr;
using std::sqrt;

// Constants

const double infinity = std::numeric_limits<double>::infinity();
// const double pi = 3.1415926535897932385;
const double pi = acos(-1);
const double eps = 1e-8;

// Utility Functions

inline double degrees_to_radians(double degrees)
{
    return degrees * pi / 180;
}

// Common Headers

#include "ray.hpp"
#include "vec3.hpp"
#include "color.hpp"

#endif