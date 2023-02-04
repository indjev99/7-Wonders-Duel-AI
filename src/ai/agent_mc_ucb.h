#pragma once

#include "mc_config.h"

#include "runner/agent.h"

struct AgentMcUcb final : Agent
{
    AgentMcUcb(const MCConfig& config = MCConfig());

    Action getAction() override;

private:

    MCConfig config;
};
