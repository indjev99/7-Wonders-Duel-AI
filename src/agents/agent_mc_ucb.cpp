#include "agent_mc_ucb.h"

#include "mc.h"

#include "time/timer.h"

#include <cmath>
#include <numeric>

AgentMcUcb::AgentMcUcb(const MCConfig& config)
    : config(config)
{}

Action AgentMcUcb::getAction()
{
    const std::vector<Action>& possible = game->getPossibleActions();
    if (possible.size() == 1) return possible[0];

    std::vector<BanditArm> arms = makeArms(possible);

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

    return arms[findBestArm(arms)].action;
}
