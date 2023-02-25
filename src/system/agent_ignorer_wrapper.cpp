#include "agent_ignorer_wrapper.h"

#include <algorithm>

AgentIgnorerWrapper::AgentIgnorerWrapper(Agent* agent, std::vector<Agent*> ignored)
    : agent(agent)
    , ignored(ignored)
{}

Action AgentIgnorerWrapper::getAction()
{
    for (Agent* curr : ignored)
    {
        curr->getAction();
    }

    return agent->getAction();
}

std::vector<Listener*> AgentIgnorerWrapper::getSubListeners()
{
    std::vector<Listener*> subListeners(ignored.begin(), ignored.end());
    subListeners.push_back(agent);
    return subListeners;
}
