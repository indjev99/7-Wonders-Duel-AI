#include "timer.h"

Timer::Timer(double secs)
    : secs(secs)
    , startTime(std::chrono::steady_clock::now())
{}

bool Timer::haveTime() const
{
    auto currTime = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff = currTime - startTime;
    return diff.count() < secs;
}
