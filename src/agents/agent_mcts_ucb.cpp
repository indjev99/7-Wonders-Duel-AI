#include "agent_mcts_ucb.h"

#include "mc.h"

#include "game/lang.h"
#include "time/timer.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>

struct MctsNode
{
    std::vector<BanditArm> arms;

    int numGames;

    MctsNode(const GameStateFast& game)
        : arms(makeArms(!game.isTerminal() ? game.getPossibleActions() : std::vector<Action>()))
        , numGames(0)
    {}
};

void debugPrintIdent(int depth)
{
    for (int i = 0 ; i < depth; i++)
    {
        std::cerr << "  ";
    }
}

void debugPrintNode(const std::vector<MctsNode>& nodes, int curr, const GameStateFast& game, int player, int expandLimit = 1000, int depth = 1)
{
    if (depth == 1) std::cerr << std::endl;

    debugPrintIdent(depth);
    std::cerr << "Depth: " << depth << ", ";

    if (game.isTerminal())
    {
        std::cerr << "Terminal: " << resultToString(game.getResult(player)) << std::endl;
        return;
    }

    std::cerr << "Actor: " << actorToString(game.getCurrActor()) << std::endl;

    std::vector<BanditArm> armsSorted = nodes[curr].arms;

    std::sort(armsSorted.begin(), armsSorted.end(), [](auto& left, auto& right){ return left.numGames > right.numGames; });

    for (const BanditArm& arm : armsSorted)
    {
        debugPrintIdent(depth + 1);
        std::cerr << actionToString(arm.action) << " : " << arm.numGames << " with " << (arm.numGames > 0 ? arm.avgReward() : 0) << std::endl;
        if (arm.numGames >= expandLimit && arm.child != CHILD_NONE)
        {
            GameStateFast runGame(&game);
            runGame.doAction(arm.action);
            debugPrintNode(nodes, arm.child, runGame, player, expandLimit, depth + 1);
        }
    }
}

AgentMctsUcb::AgentMctsUcb(const MCConfig& config)
    : config(config)
{}

double mctsIteration(std::vector<MctsNode>& nodes, int curr, GameStateFast& game, const MCConfig& config, int player)
{
    if (game.isTerminal())
    {        
        nodes[curr].numGames++;
        return game.getResult(player);
    }

    int currAge = game.getCurrAge();
    int currActor = game.getCurrActor();
    int chosen = findBestArm(nodes[curr].arms, nodes[curr].numGames, config.explrFactor);
    BanditArm& arm = nodes[curr].arms[chosen];

    game.doAction(arm.action);

    int newAge = game.getCurrAge();

    double reward;

    if(arm.child != CHILD_NONE) reward = mctsIteration(nodes, arm.child, game, config, player);
    else if (newAge != currAge) reward = simRandGame(game, player, config);
    else
    {
        arm.child = nodes.size();
        nodes.push_back(MctsNode(game));
        nodes.back().numGames++;
        reward = simRandGame(game, player, config);
    }

    nodes[curr].numGames++;
    arm.update(currActor != ACTOR_GAME ? (currActor == player ? reward : -reward) : 0);
    return reward;
}

Action AgentMctsUcb::getAction()
{
    const std::vector<Action>& possible = game->getPossibleActions();

    if (possible.size() == 1) return possible[0];

    std::vector<MctsNode> nodes;
    nodes.push_back(MctsNode(GameStateFast(game)));

    int root = 0;

    DO_FOR_SECS(config.secsPerMove)
    {
        GameStateFast runGame(game);
        mctsIteration(nodes, root, runGame, config, player);
    }

    int chosen = findBestArm(nodes[0].arms);

    std::cerr << "Expected outcome: " << nodes[root].arms[chosen].avgReward() << std::endl << std::endl;

    // GameStateFast runGame(game);
    // debugPrintNode(nodes, root, runGame, player);

    return nodes[root].arms[chosen].action;
}
