#include "revealer_uniform.h"

#include "utils/random.h"

Action RevealerUniform::getAction()
{
    return uniformElem(game->getPossibleActions());
}
