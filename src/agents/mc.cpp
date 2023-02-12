#include "mc.h"

#include "game_simulator.h"

#include "game/results.h"

long long totalNumSims = 0;

int simRandGame(GameStateFast& game, int player, const MCConfig& config)
{
    totalNumSims++;
    GameSimulator simulator(game, config);
    return resultSign(simulator.simGame(player));
}
