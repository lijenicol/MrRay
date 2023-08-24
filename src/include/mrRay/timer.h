#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <string>

#include "mrRay/namespace.h"

MR_RAY_NAMESPACE_OPEN_SCOPE

/// \class Timer
///
/// RAII style class which prints out elapsed time between object
/// creation and object deletion.
class Timer
{
public:
    Timer(const std::string &name);
    ~Timer();

private:
    std::string name;
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
};

MR_RAY_NAMESPACE_CLOSE_SCOPE

#endif
