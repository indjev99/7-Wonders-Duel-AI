#include "agent_mc.h"

#include "constants.h"
#include "mc.h"

#include "utils/random.h"
#include "game/results.h"

AgentMc::AgentMc(int avgNumSims, bool branchRelative)
    : avgNumSims(avgNumSims)
    , branchRelative(branchRelative)
{}

Action AgentMc::getAction()
{
    const std::vector<Action>& possible = game->getPossibleActions();
    if (possible.size() == 1) return possible[0];

    int numSims = !branchRelative ? avgNumSims : avgNumSims * possible.size() / AVG_BRANCHES;

    Action bestAction;
    double bestScore = -INF;

    for (int i = 0; i < (int) possible.size(); i++)
    {
        const Action& action = possible[i];
        int currSims = numSims * (i + 1) / possible.size() - numSims * i / possible.size();

        if (currSims == 0) continue;

        double score = 0;
        for (int t = 0; t < currSims; t++)
        {
            GameState runGame = *game;
            runGame.doAction(action);
            score += simRandGame(runGame, player);
        }
        score /= currSims;

        if (score > bestScore)
        {
            bestScore = score;
            bestAction = action;
        }
    }

    return bestAction;
}
