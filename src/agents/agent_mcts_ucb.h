#pragma once

#include "mc.h"
#include "mc_config.h"

#include "system/agent.h"

#include <vector>

struct AgentMctsUcb final : Agent
{
    AgentMctsUcb(const MCConfig& config = MCConfig());

    Action getAction() override;

    void notifyActionPost(const Action& action) override;

private:

    struct MctsNode
    {
        std::vector<BanditArm<Action>> arms;

        int numGames;

        MctsNode(const GameStateFast& game)
            : arms(makeArms(!game.isTerminal() ? game.getPossibleActions() : std::vector<Action>()))
            , numGames(0)
        {}
    };

    double mctsIteration(int curr);

    void debugPrintNode(int curr, int expandLimit = 1000, int depth = 1);

    MCConfig config;

    GameStateFast runGame;
    std::vector<MctsNode> nodes;
    std::array<std::vector<BanditArm<int>>, NUM_PLAYERS> modeArms;

    Action lastAction;
};
