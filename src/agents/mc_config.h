#pragma once

constexpr double INF = 1e9;
constexpr double AVG_BRANCHES = 5.75;

struct MCConfig
{
    double secsPerMove = 1.0;

    double explrFactor = 1.5;

    double simDiscardProb = 0.1;
    double simBuildProb = 0.45;
    double simWonderProb = 0.45;

    double simModeNormalProb = 0.5;
    double simModeScienceProb = 0.5;
    double simModeMilitaryProb = 0.0;
};
