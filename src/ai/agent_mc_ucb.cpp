#include "agent_mc_ucb.h"

#include "constants.h"
#include "mc.h"

#include <cmath>
#include <numeric>

AgentMcUcb::AgentMcUcb(int avgNumSims, double explrFactor, bool branchRelative)
    : avgNumSims(avgNumSims)
    , explrFactor(explrFactor)
    , branchRelative(branchRelative)
{}

Action AgentMcUcb::getAction()
{
    const std::vector<Action>& possible = game->getPossibleActions();
    if (possible.size() == 1) return possible[0];

    int numSims = !branchRelative ? avgNumSims : avgNumSims * possible.size() / AVG_BRANCHES;

    std::vector<BanditArm> arms = makeArms(possible);

    for (int t = 0; t < numSims; t++)
    {
        int chosen = findBestArm(arms, t, explrFactor);

        GameStateFast runGame(game);
        runGame.doAction(arms[chosen].action);
        arms[chosen].totalReward += simRandGame(runGame, player);
        arms[chosen].numGames++;
    }

    return arms[findBestArm(arms)].action;
}
