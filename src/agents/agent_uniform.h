#pragma once

#include "system/agent.h"

struct AgentUniform final : Agent
{
    Action getAction() override;
};
