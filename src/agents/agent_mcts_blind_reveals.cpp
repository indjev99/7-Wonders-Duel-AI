#include "agent_mcts_blind_reveals.h"

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

void AgentMctsBlindReveals::debugPrintNode(int curr, int expandLimit, int depth)
{
    debugPrintIdent(depth);
    std::cerr << "Depth: " << depth << ", ";

    if (runGame.isTerminal())
    {
        std::cerr << "Terminal: " << resultToString(runGame.getResult(player)) << std::endl;
        return;
    }

    if (runGame.getCurrActor() == ACTOR_GAME)
    {
        GameSimulator sim(runGame, config);
        while (runGame.getCurrActor() == ACTOR_GAME)
        {
            sim.simAction();
        }
    }

    std::cerr << "Actor: " << actorToString(runGame.getCurrActor()) << std::endl;

    std::vector<std::pair<std::pair<Action, int>, BanditArmAvailCnt>> actionPosArmsSorted;

    std::copy(nodes[curr].arms.begin(), nodes[curr].arms.end(), std::back_inserter(actionPosArmsSorted));

    std::sort(actionPosArmsSorted.begin(), actionPosArmsSorted.end(), [](auto& left, auto& right){ return left.second.safeAvgReward() > right.second.safeAvgReward(); });

    for (const std::pair<std::pair<Action, int>, BanditArmAvailCnt>& actionPosArm : actionPosArmsSorted)
    {
        const Action& action = actionPosArm.first.first;
        const int& pos = actionPosArm.first.second;
        const BanditArmAvailCnt& arm = actionPosArm.second;
        debugPrintIdent(depth + 1);
        std::cerr << actionToString(action);
        if (pos != POS_NONE) std::cerr << " @ " << pos;
        std::cerr << " : " << arm.safeAvgReward() << " with " << arm.numGames << " / " << arm.numGamesAvailable << std::endl;
        const std::vector<Action>& possible = runGame.getPossibleActions();
        if (arm.numGames >= expandLimit && arm.child != CHILD_NONE &&
            std::find(possible.begin(), possible.end(), action) != possible.end() && (pos == POS_NONE || runGame.getObjectPos(action.arg1) == pos))
        {
            GameStateFast backupGame(&runGame);
            runGame.doAction(action);
            debugPrintNode(arm.child, expandLimit, depth + 1);
            runGame.clone(&backupGame);
        }
    }
}

AgentMctsBlindReveals::BanditArmAvailCnt::BanditArmAvailCnt()
    : Base(std::monostate())
    , numGamesAvailable(0)
{}

AgentMctsBlindReveals::AgentMctsBlindReveals(const MCConfig& config)
    : config(config)
{}

std::pair<Action, AgentMctsBlindReveals::BanditArmAvailCnt*> AgentMctsBlindReveals::findBestActionArm(int curr, float explrFactor)
{
    Action bestAction;
    BanditArmAvailCnt* bestArm = nullptr;
    float bestScore = -INF;

    const std::vector<Action>& possible = runGame.getPossibleActions();

    FOR_IN_UNIFORM_PERM(i, possible.size())
    {
        const Action& action = possible[i];
        int pos = POS_NONE; // action.type == ACT_MOVE_PLAY_PYRAMID_CARD ? runGame.getObjectPos(action.arg1) : POS_NONE;
        std::pair<Action, int> actionPos(action, pos);
        BanditArmAvailCnt& arm = nodes[curr].arms[actionPos];
        arm.numGamesAvailable++;
        float score = armScore(arm, arm.numGamesAvailable, explrFactor);
        if (score > bestScore)
        {
            bestAction = action;
            bestArm = &arm;
            bestScore = score;
        }
    }

    return {bestAction, bestArm};
}

float AgentMctsBlindReveals::mctsIterationRec(int curr)
{
    if (runGame.isTerminal()) return resultSign(runGame.getResult(player));
    if (runGame.isAgeStart()) return simRandGame(runGame, player, config);

    if (runGame.getCurrActor() == ACTOR_GAME)
    {
        GameSimulator sim(runGame, config);
        while (runGame.getCurrActor() == ACTOR_GAME)
        {
            sim.simAction();
        }
    }

    int currActor = runGame.getCurrActor();

    std::pair<Action, BanditArmAvailCnt*> bestActionArm = findBestActionArm(curr, config.explrFactor);
    BanditArmAvailCnt& arm = *bestActionArm.second;

    runGame.doAction(bestActionArm.first);

    float reward;

    if (arm.child != CHILD_NONE) reward = mctsIterationRec(arm.child);
    else
    {
        arm.child = nodes.size();
        nodes.push_back(TreeNode());
        reward = simRandGame(runGame, player, config);
    }

    arm.update(currActor == player ? reward : -reward);
    return reward;
}

void AgentMctsBlindReveals::mctsIteration()
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

Action AgentMctsBlindReveals::getAction()
{
    const std::vector<Action>& possible = game->getPossibleActions();

    if (possible.size() == 1) return possible[0];

    numGames = 0;

    nodes.clear();
    nodes.push_back(TreeNode());

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

    runGame.clone(game);
    std::pair<Action, BanditArmAvailCnt*> bestActionArm = findBestActionArm(root);
    BanditArmAvailCnt& arm = *bestActionArm.second;
    Action action = bestActionArm.first;

    if (config.verbosity > 0)
    {
        std::cerr << "Expected outcome: " << arm.safeAvgReward() << " with " << numGames << std::endl << std::endl;
    }

    if (config.verbosity > 1)
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

        runGame.clone(game);
        debugPrintNode(root);

        std::cerr << std::endl;
    }

    return action;
}
