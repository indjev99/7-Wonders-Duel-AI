#include "agent_mc.h"

#include "mc.h"

#include "time/timer.h"

#include <cmath>
#include <iostream>
#include <numeric>

AgentMc::AgentMc(const MCConfig& config)
    : config(config)
{}

Action AgentMc::getAction()
{
    const std::vector<Action>& possible = game->getPossibleActions();
    if (possible.size() == 1) return possible[0];

    std::vector<BanditArm<Action>> arms = makeArms(possible);

    int numGames = 0;

    DO_FOR_SECS(config.secsPerMove)
    {
        int chosen = findBestArm(arms, numGames, config.explrFactor);

        GameStateFast runGame(game);
        runGame.doAction(arms[chosen].action);
        arms[chosen].totalReward += simRandGame(runGame, player, config);
        arms[chosen].numGames++;
        numGames++;
    }

    int chosen = findBestArm(arms);

    if (config.verbosity > 0)
        std::cerr << "Expected outcome: " << arms[chosen].avgReward() << std::endl << std::endl;

    return arms[chosen].action;
}
