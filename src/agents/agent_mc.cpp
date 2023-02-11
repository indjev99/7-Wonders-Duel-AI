#include "agent_mc.h"

#include "mc.h"

#include "game/results.h"
#include "time/timer.h"
#include "utils/random.h"

#include <iostream>

AgentMc::AgentMc(const MCConfig& config)
    : config(config)
{}

Action AgentMc::getAction()
{
    const std::vector<Action>& possible = game->getPossibleActions();
    if (possible.size() == 1) return possible[0];

    Action bestAction;
    double bestScore = -INF;

    for (int i = 0; i < (int) possible.size(); i++)
    {
        const Action& action = possible[i];

        double score = 0;
        int numGames = 0;

        DO_FOR_SECS(config.secsPerMove / possible.size())
        {
            GameStateFast runGame(game);
            runGame.doAction(action);
            score += simRandGame(runGame, player, config);
            numGames++;
        }

        if (numGames == 0) continue;

        score /= numGames;

        if (score > bestScore)
        {
            bestScore = score;
            bestAction = action;
        }
    }

    if (config.verbosity)
        std::cerr << "Expected outcome: " << bestScore << std::endl << std::endl;

    return bestAction;
}
