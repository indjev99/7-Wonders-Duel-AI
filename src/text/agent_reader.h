#pragma once

#include "io/string_reader.h"
#include "runner/agent.h"

#include <iostream>

struct AgentReader final : Agent
{
    AgentReader(StringReader& reader);

    Action getAction() override;

private:

    StringReader& reader;
};
