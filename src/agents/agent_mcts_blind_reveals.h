#pragma once

#include "mc.h"
#include "mc_config.h"

#include "system/agent.h"

#include <map>
#include <variant>
#include <vector>

struct AgentMctsBlindReveals final : Agent
{
    AgentMctsBlindReveals(const MCConfig& config = MCConfig());

    Action getAction() override;

private:

    struct BanditArmAvailCnt : BanditArm<std::monostate>
    {
        using Base = BanditArm<std::monostate>;

        int numGamesAvailable;

        BanditArmAvailCnt();
    };

    struct TreeNode
    {
        std::map<std::pair<Action, int>, BanditArmAvailCnt> arms;
    };

    std::pair<Action, BanditArmAvailCnt*> findBestActionArm(int curr, float explrFactor = 0);

    void mctsIteration();
    float mctsIterationRec(int curr);

    void debugPrintNode(int curr, int expandLimit = 1000, int depth = 0);

    MCConfig config;

    int numGames;
    GameStateFast runGame;
    std::vector<TreeNode> nodes;
    std::array<std::vector<BanditArm<int>>, NUM_PLAYERS> modeArms;
};
