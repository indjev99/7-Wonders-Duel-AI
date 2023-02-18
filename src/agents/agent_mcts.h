#pragma once

#include "mc.h"
#include "mc_config.h"

#include "system/agent.h"

#include <vector>

struct AgentMcts final : Agent
{
    AgentMcts(const MCConfig& config = MCConfig());

    Action getAction() override;

    void notifyActionPost(const Action& action) override;

private:

    struct TreeNode
    {
        std::vector<BanditArm<Action>> arms;

        int numGames;

        TreeNode(const GameStateFast& game);
    };

    void mctsIteration();
    float mctsIterationRec(int curr);

    void debugPrintNode(int curr, int expandLimit = 1000, int depth = 1);

    MCConfig config;

    GameStateFast runGame;
    std::vector<TreeNode> nodes;
    std::array<std::vector<BanditArm<int>>, NUM_PLAYERS> modeArms;

    Action lastAction;
};
