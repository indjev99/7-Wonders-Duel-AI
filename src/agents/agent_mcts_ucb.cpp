#include "agent_mcts_ucb.h"

#include "game/lang.h"
#include "game/results.h"
#include "time/timer.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>

void debugPrintIdent(int depth)
{
    for (int i = 0 ; i < depth; i++)
    {
        std::cerr << "  ";
    }
}

void AgentMctsUcb::debugPrintNode(int curr, int expandLimit, int depth)
{
    if (depth == 1) std::cerr << std::endl;

    debugPrintIdent(depth);
    std::cerr << "Depth: " << depth << ", ";

    if (runGame.isTerminal())
    {
        std::cerr << "Terminal: " << resultToString(runGame.getResult(player)) << std::endl;
        return;
    }

    std::cerr << "Actor: " << actorToString(runGame.getCurrActor()) << std::endl;

    std::vector<BanditArm> armsSorted = nodes[curr].arms;

    std::sort(armsSorted.begin(), armsSorted.end(), [](auto& left, auto& right){ return left.numGames > right.numGames; });

    for (const BanditArm& arm : armsSorted)
    {
        debugPrintIdent(depth + 1);
        std::cerr << actionToString(arm.action) << " : " << arm.numGames << " with " << (arm.numGames > 0 ? arm.avgReward() : 0) << std::endl;
        if (arm.numGames >= expandLimit && arm.child != CHILD_NONE)
        {
            GameStateFast backupGame(&runGame);
            runGame.doAction(arm.action);
            debugPrintNode(arm.child, expandLimit, depth + 1);
            runGame.clone(&backupGame);
        }
    }
}

AgentMctsUcb::AgentMctsUcb(const MCConfig& config)
    : config(config)
{}

double AgentMctsUcb::mctsIteration(int curr)
{
    if (runGame.isTerminal())
    {        
        nodes[curr].numGames++;
        return resultSign(runGame.getResult(player));
    }

    if (runGame.isAgeStart())
    {
        return simRandGame(runGame, player, config);
    }

    int currActor = runGame.getCurrActor();
    int chosen = findBestArm(nodes[curr].arms, nodes[curr].numGames, config.explrFactor);
    BanditArm& arm = nodes[curr].arms[chosen];

    runGame.doAction(arm.action);

    double reward;

    if (arm.child != CHILD_NONE) reward = mctsIteration(arm.child);
    else
    {
        arm.child = nodes.size();
        nodes.push_back(MctsNode(runGame));
        nodes.back().numGames++;
        reward = simRandGame(runGame, player, config);
    }

    nodes[curr].numGames++;
    arm.update(currActor != ACTOR_GAME ? (currActor == player ? reward : -reward) : 0);
    return reward;
}

Action AgentMctsUcb::getAction()
{
    const std::vector<Action>& possible = game->getPossibleActions();

    if (possible.size() == 1) return possible[0];

    nodes.clear();
    nodes.push_back(MctsNode(GameStateFast(game)));

    int root = 0;

    DO_FOR_SECS(config.secsPerMove)
    {
        runGame.clone(game);
        mctsIteration(root);
    }

    int chosen = findBestArm(nodes[0].arms);

    if (config.verbosity > 0)
        std::cerr << "Expected outcome: " << nodes[root].arms[chosen].avgReward() << std::endl << std::endl;

    std::cerr << nodes[root].numGames << std::endl;

    if (config.verbosity > 1)
    {
        runGame.clone(game);
        debugPrintNode(root);
    }

    return nodes[root].arms[chosen].action;
}
