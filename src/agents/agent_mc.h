#pragma once

#include "mc_config.h"

#include "system/agent.h"

struct AgentMc final : Agent
{
    AgentMc(const MCConfig& config = MCConfig());

    Action getAction() override;

private:

    MCConfig config;
};
