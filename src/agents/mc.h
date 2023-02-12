#include "mc_config.h"

#include "game/game_state.h"
#include "utils/random.h"

#include <vector>

#define CHILD_NONE -1

extern long long totalNumSims;

template <class ActionT>
struct BanditArm
{
    ActionT action;

    int child;

    int numGames;
    float totalReward;

    BanditArm(const ActionT& action, int child = CHILD_NONE);

    float avgReward() const;
    float safeAvgReward() const;

    void update(float reward);
};

template <class ActionT>
std::vector<BanditArm<ActionT>> makeArms(const std::vector<ActionT>& possible);

template <class ActionT>
int findBestArm(const std::vector<BanditArm<ActionT>>& arms, int totalNumGames = 1, float explrFactor = 0);

template <class ActionT>
int findLeastGamesArm(const std::vector<BanditArm<ActionT>>& arms);

int simRandGame(GameStateFast& game, int player, const MCConfig& config);


template <class ActionT>
BanditArm<ActionT>::BanditArm(const ActionT& action, int child)
    : action(action)
    , child(child)
    , numGames(0)
    , totalReward(0)
{}

template <class ActionT>
float BanditArm<ActionT>::avgReward() const
{
    return totalReward / numGames;
}

template <class ActionT>
float BanditArm<ActionT>::safeAvgReward() const
{
    if (numGames == 0) return 0;
    return avgReward();
}

template <class ActionT>
void BanditArm<ActionT>::update(float reward)
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
int findBestArm(const std::vector<BanditArm<ActionT>>& arms, int totalNumGames, float explrFactor)
{
    int bestArm = -1;
    float bestScore = -INF;

    static std::vector<int> perm;
    perm.resize(arms.size());
    std::iota(perm.begin(), perm.end(), 0);
    randShuffle(perm);

    float logTotalGames = totalNumGames > 0 ? logf(totalNumGames) : 0;

    for (int i : perm)
    {
        if (arms[i].numGames == 0)
        {
            bestArm = i;
            break;
        }

        float score = arms[i].avgReward() + explrFactor * sqrtf(logTotalGames / arms[i].numGames);

        if (score > bestScore)
        {
            bestArm = i;
            bestScore = score;
        }
    }

    return bestArm;
}

template <class ActionT>
int findLeastGamesArm(const std::vector<BanditArm<ActionT>>& arms)
{
    int leastGamesArm = -1;
    int leastGames = INF;

    static std::vector<int> perm;
    perm.resize(arms.size());
    std::iota(perm.begin(), perm.end(), 0);
    randShuffle(perm);

    for (int i : perm)
    {
        if (arms[i].numGames < leastGames)
        {
            leastGamesArm = i;
            leastGames = arms[i].numGames;
        }
    }

    return leastGamesArm;
}
