#include "agent_mcts.h"

#include "game_simulator.h"

#include "game/lang.h"
#include "game/results.h"
#include "utils/timer.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>

static void debugPrintIdent(int depth)
{
    for (int i = 0 ; i < depth; i++)
    {
        std::cerr << "  ";
    }
}

void AgentMcts::debugPrintNode(int curr, int expandLimit, int depth)
{
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

AgentMcts::TreeNode::TreeNode(const GameStateFast& game)
    : arms(makeArms(!game.isTerminal() ? game.getPossibleActions() : std::vector<Action>()))
    , numGames(0)
{}

AgentMcts::AgentMcts(const MCConfig& config)
    : config(config)
{}

float AgentMcts::mctsIterationRec(int curr)
{
    nodes[curr].numGames++;

    if (runGame.isTerminal()) return resultSign(runGame.getResult(player));
    if (runGame.isAgeStart()) return simRandGame(runGame, player, config);

    int currActor = runGame.getCurrActor();
    int chosen = currActor != ACTOR_GAME ? findBestArm(nodes[curr].arms, nodes[curr].numGames, config.explrFactor) : findLeastGamesArm(nodes[curr].arms);
    BanditArm<Action>& arm = nodes[curr].arms[chosen];

    runGame.doAction(arm.action);

    float reward;

    if (arm.child != CHILD_NONE) reward = mctsIterationRec(arm.child);
    else
    {
        arm.child = nodes.size();
        nodes.push_back(TreeNode(runGame));
        nodes.back().numGames++;
        reward = simRandGame(runGame, player, config);
    }

    arm.update(currActor != ACTOR_GAME ? (currActor == player ? reward : -reward) : 0);
    return reward;
}

void AgentMcts::mctsIteration()
{
    const int root = 0;

    numGames++;

    if (config.simModesSmart)
    {
        for (int i = 0; i < NUM_PLAYERS; i++)
        {
            int chosen = findBestArm(modeArms[i], numGames, config.explrFactor);
            config.simModes[i] = modeArms[i][chosen].action;
        }
    }

    runGame.clone(game);
    float reward = mctsIterationRec(root);

    if (config.simModesSmart)
    {
        for (int i = 0; i < NUM_PLAYERS; i++)
        {
            modeArms[i][config.simModes[i]].update(i == player ? reward : -reward);
        }
    }
}

Action AgentMcts::getAction()
{
    const std::vector<Action>& possible = game->getPossibleActions();

    if (possible.size() == 1) return possible[0];

    numGames = 0;

    nodes.clear();
    nodes.push_back(TreeNode(GameStateFast(game)));

    if (config.simModesSmart)
    {
        std::vector<int> modes(NUM_SIM_MODES);
        std::iota(modes.begin(), modes.end(), 0);
        for (int i = 0; i < NUM_PLAYERS; i++)
        {
            modeArms[i] = makeArms(modes);
        }
    }

    DO_FOR_SECS(config.secsPerMove)
    {
        mctsIteration();
    }

    const int root = 0;

    int chosen = findBestArm(nodes[root].arms);

    if (config.verbosity >= 1)
    {
        std::cerr << "Expected outcome: " << nodes[root].arms[chosen].safeAvgReward() << " with " << numGames << std::endl << std::endl;
    }

    if (config.verbosity >= 2)
    {
        if (config.simModesSmart)
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
    }

    if (config.verbosity >= 3)
    {
        runGame.clone(game);
        debugPrintNode(root);
        std::cerr << std::endl;
    }

    Action action = nodes[root].arms[chosen].action;

    return action;
}
