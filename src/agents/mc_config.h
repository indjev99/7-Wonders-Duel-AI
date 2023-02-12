#pragma once

#include "game/constants.h"

#include <array>

constexpr double INF = 1e9;

constexpr int NUM_SIM_MODES = 3;

constexpr int SIM_MODE_NORMAL = 0;
constexpr int SIM_MODE_SCIENCE = 1;
constexpr int SIM_MODE_MILITARY = 2;

constexpr int SIM_MODE_NONE = -1;

struct MCConfig
{
    double secsPerMove = 1.0;

    double explrFactor = 1.5;

    double simDiscardProb = 0.1;
    double simBuildProb = 0.45;
    double simWonderProb = 0.45;

    bool simModesUcb = true;
    bool simPacifist = true;

    std::array<double, NUM_SIM_MODES> simModeProbs = {0.5, 0.5, 0.0};

    bool testMode = false;

    int verbosity = 0;

    std::array<int, NUM_PLAYERS> simModes = {SIM_MODE_NONE, SIM_MODE_NONE};
};
