#pragma once

#include "runner/agent.h"

struct AgentMC final : Agent
{
    AgentMC(int avgNumSims = 40000, bool branchRelative = true);

    Action getAction() override;

private:

    int avgNumSims;
    bool branchRelative;
};
