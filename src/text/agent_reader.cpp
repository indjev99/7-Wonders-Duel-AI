#include "agent_reader.h"

#include "game/lang.h"

AgentReader::AgentReader(std::istream& in)
    : in(in)
{}

Action AgentReader::getAction()
{
    std::string actionStr;
    std::getline(in, actionStr);
    return actionFromString(actionStr);
}
