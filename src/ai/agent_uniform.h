#pragma once

#include "runner/agent.h"

struct AgentUniform final : Agent
{
    Action getAction() override;
};
