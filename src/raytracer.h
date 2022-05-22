#pragma once

#include <ctime>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <memory>
#include <iostream>
#include <random>
#include <vector>
#include <algorithm>

// Usings

using std::make_shared;
using std::shared_ptr;

// Constants

const double INF = std::numeric_limits<double>::infinity();
const double PI = acos(-1);

// Utility Functions

inline double deg2rad(double degrees)
{
    return degrees * PI / 180;
}

inline int randomInt(int min, int max)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(gen);
}

inline double randomReal()
{
    // Returns a random real in [0, 1).
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(gen);
}

inline double randomReal(double min, double max)
{
    // Returns a random real in [min, max).
    return min + (max - min) * randomReal();
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