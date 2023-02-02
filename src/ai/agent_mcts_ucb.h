#pragma once

#include "runner/agent.h"

struct AgentMctsUcb final : Agent
{
    AgentMctsUcb(int avgNumSims = 40000, double explrFactor = 2.8284, bool branchRelative = true);

    Action getAction() override;

private:

    int avgNumSims;
    double explrFactor;
    bool branchRelative;
};
