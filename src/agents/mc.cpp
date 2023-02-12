#include "mc.h"

#include "game_simulator.h"

#include "game/results.h"

int simRandGame(GameStateFast& game, int player, const MCConfig& config)
{
    GameSimulator simulator(game, config);
    return resultSign(simulator.simGame(player));
}
