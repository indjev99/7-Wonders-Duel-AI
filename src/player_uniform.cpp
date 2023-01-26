#include "player_uniform.h"

#include "random.h"

Action PlayerUniform::getAction()
{
    return uniformElem(game->getPossibleActions());
}
