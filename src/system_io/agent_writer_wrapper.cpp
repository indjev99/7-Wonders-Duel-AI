#include "agent_writer_wrapper.h"

#include "game/lang.h"

AgentWriterWrapper::AgentWriterWrapper(Agent* agent, StringWriter& writer)
    : agent(agent)
    , writer(writer)
{}

Action AgentWriterWrapper::getAction()
{
    Action action = agent->getAction();
    writer.write(actionToString(action));
    return action;
}

std::vector<Listener*> AgentWriterWrapper::getSubListeners()
{
    return {agent};
}
