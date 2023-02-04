#include "mc_config.h"

#include "game/game_state.h"

#include <vector>

#define CHILD_NONE -1

struct BanditArm
{
    Action action;

    int child;

    int numGames;
    double totalReward;

    BanditArm(const Action& action, int child = CHILD_NONE);

    double avgReward() const;

    void update(double reward);
};

std::vector<BanditArm> makeArms(const std::vector<Action>& possible);

int findBestArm(const std::vector<BanditArm>& arms, int totalNumGames = 1, double explrFactor = 0);

int simRandGame(GameStateFast& game, int player, const MCConfig& config);
