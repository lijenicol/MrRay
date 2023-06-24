#include "ray/Colour.h"
#include "ray/rtutils.h"

RAY_NAMESPACE_OPEN_SCOPE

void write_colour(std::ostream& out, Colour colour) {
	// Take the colours into gamma space
	double r = sqrt(colour.x());
	double g = sqrt(colour.y());
	double b = sqrt(colour.z());

	// Write the ouput
	out << (int)(255 * clamp(r, 0, 1)) << ' '
		<< (int)(255 * clamp(g, 0, 1)) << ' '
		<< (int)(255 * clamp(b, 0, 1)) << '\n';
}

RAY_NAMESPACE_CLOSE_SCOPE
