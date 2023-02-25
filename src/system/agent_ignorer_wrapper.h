#pragma once

#include "agent.h"

struct AgentIgnorerWrapper final : Agent
{
    AgentIgnorerWrapper(Agent* agent, std::vector<Agent*> ignored);

    Action getAction() override;

    std::vector<Listener*> getSubListeners() override;

private:

    Agent* agent;
    std::vector<Agent*> ignored;
};
