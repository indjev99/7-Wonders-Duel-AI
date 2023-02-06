#include "agent_reader.h"

#include "game/lang.h"

AgentReader::AgentReader(StringReader& reader)
    : reader(reader)
{}

Action AgentReader::getAction()
{
    return actionFromString(reader.read());
}
