#include "agent_mc_ucb.h"

#include "mc.h"

#include <cmath>
#include <numeric>

AgentMcUcb::AgentMcUcb(const MCConfig& config)
    : config(config)
{}

Action AgentMcUcb::getAction()
{
    const std::vector<Action>& possible = game->getPossibleActions();
    if (possible.size() == 1) return possible[0];

    int numSims = config.numSims(possible.size());

    std::vector<BanditArm> arms = makeArms(possible);

    for (int t = 0; t < numSims; t++)
    {
        int chosen = findBestArm(arms, t, config.explrFactor);

        GameStateFast runGame(game);
        runGame.doAction(arms[chosen].action);
        arms[chosen].totalReward += simRandGame(runGame, player, config);
        arms[chosen].numGames++;
    }

    return arms[findBestArm(arms)].action;
}
