#ifndef SAMPLER_H
#define SAMPLER_H

#include <random>

#include "ray/namespace.h"
#include "ray/Vec3.h"

RAY_NAMESPACE_OPEN_SCOPE

class Sampler
{
public:
    Sampler(int seed) : seed(seed), generator(std::mt19937(seed)), 
                        dist(0.0, 1.0) {}

    double getDouble() { return dist(generator); }

    double getDouble(double start, double end)
    {
        return (end - start) * getDouble() + start;
    }

    Vec3 inUnitDisk() 
    {
        while (true) 
        {
            Vec3 p = Vec3(getDouble(-1.0, 1.0), getDouble(-1.0, 1.0), 0);
            if (p.length_squared() >= 1) 
                continue;
            return p;
        }
    }
private:
    int seed;
    std::mt19937 generator;
    std::uniform_real_distribution<double> dist;
};

RAY_NAMESPACE_CLOSE_SCOPE

#endif