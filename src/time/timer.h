#pragma once

#include <chrono>

constexpr int ITERS_PER_TIME_CHECK = 10;

#define DO_FOR_SECS(secs) Timer timer_(secs); while (timer_.haveTime()) for (int t_ = 0; t_ < ITERS_PER_TIME_CHECK; t_++)

struct Timer
{
    Timer(double secs);

    bool haveTime() const;

private:

    double secs;
    std::chrono::time_point<std::chrono::steady_clock> startTime;
};
