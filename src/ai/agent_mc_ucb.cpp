#include "agent_mc_ucb.h"

#include "constants.h"

#include "utils/random.h"
#include "game/results.h"

#include <cmath>
#include <numeric>

struct BanditArm
{
    int numGames = 0;
    int totalReward = 0;
};

AgentMcUcb::AgentMcUcb(int avgNumSims, double explrFactor, bool branchRelative)
    : avgNumSims(avgNumSims)
    , explrFactor(explrFactor)
    , branchRelative(branchRelative)
{}

int findBestArm(const std::vector<BanditArm>& arms, int t = 1, double explrFactor = 0)
{
    int bestArm = -1;
    double bestScore;

    double logT = log(t);

    std::vector<int> perm(arms.size());
    std::iota(perm.begin(), perm.end(), 0);
    randShuffle(perm);

    for (int i : perm)
    {
        if (arms[i].numGames == 0)
        {
            bestArm = i;
            break;
        }

        double score = (double) arms[i].totalReward / arms[i].numGames + explrFactor * sqrt(logT / arms[i].numGames);

        if (bestArm == -1 || score > bestScore)
        {
            bestArm = i;
            bestScore = score;
        }
    }

    return bestArm;
}

Action AgentMcUcb::getAction()
{
    const std::vector<Action>& possible = game->getPossibleActions();

    if (possible.size() == 1) return possible[0];

    std::vector<BanditArm> arms(possible.size());
    int numSims = !branchRelative ? avgNumSims : avgNumSims * possible.size() / AVG_BRANCHES;

    for (int t = 0; t < numSims; ++t)
    {
        int best = findBestArm(arms, t + 1, explrFactor);

        GameState runGame = *game;
        runGame.doAction(possible[best]);
        while (!runGame.isTerminal())
        {
            runGame.doAction(uniformElem(runGame.getPossibleActions()));
        }
        arms[best].totalReward += resultSign(runGame.getResult(player));
        arms[best].numGames++;
    }

    return possible[findBestArm(arms)];
}
