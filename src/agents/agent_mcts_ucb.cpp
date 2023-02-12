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
    expandLimit = 100;
    if (depth == 1) std::cerr << std::endl;

    debugPrintIdent(depth);
    std::cerr << "Depth: " << depth << ", ";

    if (runGame.isTerminal())
    {
        std::cerr << "Terminal: " << resultToString(runGame.getResult(player)) << std::endl;
        return;
    }

    std::cerr << "Actor: " << actorToString(runGame.getCurrActor()) << std::endl;

    std::vector<BanditArm<Action>> armsSorted = nodes[curr].arms;

    std::sort(armsSorted.begin(), armsSorted.end(), [](auto& left, auto& right){ return left.safeAvgReward() > right.safeAvgReward(); });

    for (const BanditArm<Action>& arm : armsSorted)
    {
        debugPrintIdent(depth + 1);
        std::cerr << actionToString(arm.action) << " : " << arm.safeAvgReward() << " with " << arm.numGames << std::endl;
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
    BanditArm<Action>& arm = nodes[curr].arms[chosen];

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

void AgentMctsUcb::notifyActionPost(const Action& action)
{
    lastAction = action;
}

Action AgentMctsUcb::getAction()
{
    const std::vector<Action>& possible = game->getPossibleActions();

    if (possible.size() == 1) return possible[0];

    nodes.clear();
    nodes.push_back(MctsNode(GameStateFast(game)));

    if (config.simModesUcb)
    {
        std::vector<int> modes(NUM_SIM_MODES);
        std::iota(modes.begin(), modes.end(), 0);
        for (int i = 0; i < NUM_PLAYERS; i++)
        {
            modeArms[i] = makeArms(modes);
        }
    }

    int root = 0;

    DO_FOR_SECS(config.secsPerMove)
    {
        if (config.simModesUcb)
        {
            for (int i = 0; i < NUM_PLAYERS; i++)
            {
                int chosen = findBestArm(modeArms[i], nodes[root].numGames, config.explrFactor);
                config.simModes[i] = modeArms[i][chosen].action;
            }
        }

        runGame.clone(game);
        double reward = mctsIteration(root);

        if (config.simModesUcb)
        {
            for (int i = 0; i < NUM_PLAYERS; i++)
            {
                modeArms[i][config.simModes[i]].update(i == player ? reward : -reward);
            }
        }
    }

    int chosen = findBestArm(nodes[root].arms);

    if (config.verbosity > 0)
    {
        std::cerr << "Expected outcome: " << nodes[root].arms[chosen].safeAvgReward() << " with " << nodes[root].numGames << std::endl << std::endl;
    }

    if (config.verbosity > 1)
    {
        if (config.simModesUcb)
        {
            for (int i = 0; i < NUM_PLAYERS; i++)
            {
                for (int j = 0; j < NUM_SIM_MODES; j++)
                {
                    std::cerr << actorToString(i) << " " << j << " : " << modeArms[i][j].numGames << " with " << modeArms[i][j].avgReward() << std::endl;
                }
                std::cerr << std::endl;
            }
        }

        runGame.clone(game);
        debugPrintNode(root);
    }

    Action action = nodes[root].arms[chosen].action;

    return action;
}
