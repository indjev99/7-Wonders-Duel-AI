#pragma once

#include "mc_config.h"

#include "runner/agent.h"

struct AgentMctsUcb final : Agent
{
    AgentMctsUcb(const MCConfig& config = MCConfig());

    Action getAction() override;

private:

    MCConfig config;
};
