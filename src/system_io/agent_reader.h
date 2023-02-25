#pragma once

#include "io/string_reader.h"
#include "system/agent.h"

struct AgentReader final : Agent
{
    AgentReader(StringReader& reader);

    Action getAction() override;

private:

    StringReader& reader;
};
