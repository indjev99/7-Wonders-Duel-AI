#include "mc_config.h"

#include "game/game_state.h"
#include "utils/random.h"

#include <vector>

#define CHILD_NONE -1

template <class ActionT>
struct BanditArm
{
    ActionT action;

    int child;

    int numGames;
    double totalReward;

    BanditArm(const ActionT& action, int child = CHILD_NONE);

    double avgReward() const;
    double safeAvgReward() const;

    void update(double reward);
};

template <class ActionT>
std::vector<BanditArm<ActionT>> makeArms(const std::vector<ActionT>& possible);

template <class ActionT>
int findBestArm(const std::vector<BanditArm<ActionT>>& arms, int totalNumGames = 1, double explrFactor = 0);

int simRandGame(GameStateFast& game, int player, const MCConfig& config);


template <class ActionT>
BanditArm<ActionT>::BanditArm(const ActionT& action, int child)
    : action(action)
    , child(child)
    , numGames(0)
    , totalReward(0)
{}

template <class ActionT>
double BanditArm<ActionT>::avgReward() const
{
    return totalReward / numGames;
}

template <class ActionT>
double BanditArm<ActionT>::safeAvgReward() const
{
    if (numGames == 0) return 0;
    return avgReward();
}

template <class ActionT>
void BanditArm<ActionT>::update(double reward)
{
    numGames++;
    totalReward += reward;
}

template <class ActionT>
std::vector<BanditArm<ActionT>> makeArms(const std::vector<ActionT>& possible)
{
    std::vector<BanditArm<ActionT>> arms;
    arms.reserve(possible.size());

    for (const ActionT& action : possible)
    {
        arms.push_back(BanditArm<ActionT>(action));
    }

    return arms;
}

template <class ActionT>
int findBestArm(const std::vector<BanditArm<ActionT>>& arms, int totalNumGames, double explrFactor)
{
    int bestArm = -1;
    double bestScore;

    static std::vector<int> perm;
    perm.resize(arms.size());
    std::iota(perm.begin(), perm.end(), 0);
    randShuffle(perm);

    double logTotalGames = totalNumGames > 0 ? log(totalNumGames) : 0;

    for (int i : perm)
    {
        if (arms[i].numGames == 0)
        {
            bestArm = i;
            break;
        }

        double score = arms[i].avgReward() + explrFactor * sqrt(logTotalGames / arms[i].numGames);

        if (bestArm == -1 || score > bestScore)
        {
            bestArm = i;
            bestScore = score;
        }
    }

    return bestArm;
}
