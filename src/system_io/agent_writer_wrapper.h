#pragma once

#include "io/string_writer.h"
#include "system/agent.h"

struct AgentWriterWrapper final : Agent
{
    AgentWriterWrapper(Agent* agent, StringWriter& writer);

    Action getAction() override;

    std::vector<Listener*> getSubListeners() override;

private:

    Agent* agent;
    StringWriter& writer;
};
