#ifndef COLOUR_H
#define COLOUR_H

#include <iostream>
#include "ray/namespace.h"
#include "ray/rtutils.h"

RAY_NAMESPACE_OPEN_SCOPE

void write_colour(std::ostream& out, Colour colour);

RAY_NAMESPACE_CLOSE_SCOPE

#endif