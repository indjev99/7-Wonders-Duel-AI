#pragma once

#include "runner/agent.h"

#include <iostream>

struct AgentReader final : Agent
{
    AgentReader(std::istream& in);

    Action getAction() override;

private:

    std::istream& in;
};
