#pragma once
#include <limits>
#include <memory>
#include <math.h>
#include <cstdlib>
#include <iostream>

// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions

inline double degrees_to_radians(double degrees) {
	return degrees * pi / 180.0;
}

inline double random_double() {
	// FIXME: Leaving this print statement in until all uses
	//  of this method have been knocked out
	std::cerr << "Deprecated: random_double()" << std::endl;
	return rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) {
	return min + (max - min) * random_double();
}

inline int random_int(int min, int max) {
	return static_cast<int>(random_double(min, max + 1));
}

inline double clamp(double x, double min, double max) {
	if (x < min) return min;
	if (x > max) return max;
	return x;
}

// Common Headers

#include "Vec3.h"
#include "Ray.h"