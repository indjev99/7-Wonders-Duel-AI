#pragma once

#include <array>

constexpr double INF = 1e9;

constexpr int NUM_SIM_MODES = 3;
constexpr int SIM_MODE_NORMAL = 0;
constexpr int SIM_MODE_SCIENCE = 1;
constexpr int SIM_MODE_MILITARY = 2;

struct MCConfig
{
    double secsPerMove = 1.0;

    double explrFactor = 1.5;

    double simDiscardProb = 0.1;
    double simBuildProb = 0.45;
    double simWonderProb = 0.45;

    std::array<double, NUM_SIM_MODES> simModeProbs = {0.5, 0.5, 0.0};

    int verbosity = 0;
};
