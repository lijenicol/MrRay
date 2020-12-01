#include "Colour.h"
#include "rtutils.h"

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