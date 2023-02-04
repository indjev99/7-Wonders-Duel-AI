#pragma once

constexpr double INF = 1e9;
constexpr double AVG_BRANCHES = 5.75;

struct MCConfig
{
    double avgNumSims = 1e5;
    bool branchRelative = true;

    double explrFactor = 1.5;

    double simDiscardProb = 0.1;
    double simBuildProb = 0.45;
    double simWonderProb = 0.45;

    int numSims(int numBranches);
};
