#ifndef __RAYTRACER__
#define __RAYTRACER__

#include <cmath>
#include <cstdlib>
#include <limits>
#include <memory>
#include <iostream>
#include <random>

// Usings

using std::make_shared;
using std::shared_ptr;
using std::sqrt;

// Constants

const double infinity = std::numeric_limits<double>::infinity();
// const double pi = 3.1415926535897932385;
const double pi = acos(-1);
const double eps = 1e-10;
// used in hit tmin

// Utility Functions

inline double degrees_to_radians(double degrees)
{
    return degrees * pi / 180;
}

// inline double random_double()
// {
//     // Returns a random real in [0,1).
//     return rand() / (RAND_MAX + 1.0);
// }

inline int random_int(int min, int max)
{
    static std::random_device rd;
    static std::mt19937 generator(rd());
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(generator);
}

inline double random_double()
{
    // Returns a random real in [0,1).
    static std::random_device rd;
    static std::mt19937 generator(rd()); // Mersenne Twister 19937 generator
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    return distribution(generator);
}

inline double random_double(double min, double max)
{
    // Returns a random real in [min,max).
    return min + (max - min) * random_double();
}

inline double clamp(double x, double min, double max)
{
    if (x < min)
        return min;
    return x > max ? max : x;
}

// Common Headers

#include "ray.hpp"
#include "vec3.hpp"
#include "color.hpp"
#include "camera.hpp"
#include "hittable.h"

#endif