#include "agent_uniform.h"

#include "utils/random.h"

Action AgentUniform::getAction()
{
    return uniformElem(game->getPossibleActions());
}
