#include "agent_mcts_ucb.h"

#include "constants.h"
#include "mc.h"

#include "game/lang.h"

#include <cmath>
#include <iostream>
#include <numeric>

struct MctsNode
{
    std::vector<BanditArm> arms;

    int numGames;

    MctsNode(const GameState& game)
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

void debugPrintNode(const std::vector<MctsNode>& nodes, int curr, const GameState& game, int player, int expandLimit = 20, int depth = 1)
{
    if (game.isTerminal())
    {
        debugPrintIdent(depth);
        std::cerr << "Terminal: " << resultToString(game.getResult(player)) << std::endl;
        return;
    }

    debugPrintIdent(depth);
    std::cerr << "Actor: " << actorToString(game.getCurrActor()) << std::endl;

    debugPrintIdent(depth);
    std::cerr << "Children: " << std::endl;

    for (const BanditArm& arm : nodes[curr].arms)
    {
        debugPrintIdent(depth + 1);
        std::cerr << arm.numGames << " with " << ucbScore(arm) << " : " << actionToString(arm.action) << std::endl;
        if (arm.numGames >= expandLimit && arm.child != CHILD_NONE)
        {
            GameState runGame = game;
            debugPrintNode(nodes, arm.child, runGame, player, expandLimit, depth + 1);
        }
    }
}

AgentMctsUcb::AgentMctsUcb(int avgNumSims, double explrFactor, bool branchRelative)
    : avgNumSims(avgNumSims)
    , explrFactor(explrFactor)
    , branchRelative(branchRelative)
{}

double mctsIteration(std::vector<MctsNode>& nodes, int curr, GameState& game, double explrFactor, int player)
{
    if (game.isTerminal())
    {        
        nodes[curr].numGames++;
        return game.getResult(player);
    }

    int currAge = game.getCurrAge();
    int currActor = game.getCurrActor();
    int chosen = findBestArm(nodes[curr].arms, nodes[curr].numGames, explrFactor);
    BanditArm& arm = nodes[curr].arms[chosen];

    game.doAction(arm.action);

    int newAge = game.getCurrAge();

    double reward;

    if(arm.child != CHILD_NONE) reward = mctsIteration(nodes, arm.child, game, explrFactor, player);
    else if (newAge != currAge) reward = simRandGame(game, player);
    else
    {
        arm.child = nodes.size();
        nodes.push_back(MctsNode(game));
        nodes.back().numGames++;
        reward = simRandGame(game, player);
    }

    nodes[curr].numGames++;
    arm.update(currActor != ACTOR_GAME ? (currActor == player ? reward : -reward) : 0);
    return reward;
}

Action AgentMctsUcb::getAction()
{
    const std::vector<Action>& possible = game->getPossibleActions();
    if (possible.size() == 1) return possible[0];

    int numSims = !branchRelative ? avgNumSims : avgNumSims * possible.size() / AVG_BRANCHES;

    std::vector<MctsNode> nodes;
    nodes.push_back(MctsNode(*game));

    int root = 0;

    for (int t = 0; t < numSims; t++)
    {
        GameState runGame = *game;
        mctsIteration(nodes, root, runGame, explrFactor, player);
    }

    GameState runGame = *game;

    return nodes[root].arms[findBestArm(nodes[0].arms)].action;
}
