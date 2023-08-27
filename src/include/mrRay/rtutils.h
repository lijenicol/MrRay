#ifndef MR_RAY_RTUTILS_H
#define MR_RAY_RTUTILS_H

#include <cstdlib>
#include <iostream>
#include <limits>
#include <math.h>
#include <memory>

#include "mrRay/namespace.h"

MR_RAY_NAMESPACE_OPEN_SCOPE

// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions

inline double
degrees_to_radians(double degrees)
{
    return degrees * pi / 180.0;
}

// FIXME: This function isn't thread-safe
inline double
random_double()
{
    return rand() / (RAND_MAX + 1.0);
}

inline double
random_double(double min, double max)
{
    return min + (max - min) * random_double();
}

inline int
random_int(int min, int max)
{
    return static_cast<int>(random_double(min, max + 1));
}

inline double
clamp(double x, double min, double max)
{
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

MR_RAY_NAMESPACE_CLOSE_SCOPE

// Common Headers

#include "ray.h"
#include "vec3.h"

#endif // MR_RAY_RTUTILS_H
