#pragma once

#include "runner/agent.h"

struct AgentMcUcb final : Agent
{
    AgentMcUcb(int avgNumSims = 40000, double explrFactor = 2.8284, bool branchRelative = true);

    Action getAction() override;

private:

    int avgNumSims;
    double explrFactor;
    bool branchRelative;
};
