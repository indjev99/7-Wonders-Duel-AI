#pragma once

#include "runner/agent.h"

struct AgentMc final : Agent
{
    AgentMc(int avgNumSims = 40000, bool branchRelative = true);

    Action getAction() override;

private:

    int avgNumSims;
    bool branchRelative;
};
