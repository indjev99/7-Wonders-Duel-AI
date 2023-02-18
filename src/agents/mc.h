#pragma once

#include "mc_config.h"

#include "game/game_state.h"
#include "utils/random.h"

#include <vector>

#define CHILD_NONE -100

template <class ActionT>
struct BanditArm
{
    ActionT action;

    int child;

    int numGames;
    float totalReward;

    BanditArm(const ActionT& action);

    float avgReward() const;
    float safeAvgReward() const;

    void update(float reward);
};

template <class ActionT>
std::vector<BanditArm<ActionT>> makeArms(const std::vector<ActionT>& possible);

template <class ActionT>
float armScore(const BanditArm<ActionT>& arm, int numGamesAvailable = 1, float explrFactor = 0);

template <class ActionT>
float armScoreImpl(const BanditArm<ActionT>& arm, float logGamesAvailable = 0, float explrFactor = 0);

template <class ActionT>
int findBestArm(const std::vector<BanditArm<ActionT>>& arms, int totalNumGames = 1, float explrFactor = 0);

template <class ActionT>
int findLeastGamesArm(const std::vector<BanditArm<ActionT>>& arms);


template <class ActionT>
BanditArm<ActionT>::BanditArm(const ActionT& action)
    : action(action)
    , child(CHILD_NONE)
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
float armScore(const BanditArm<ActionT>& arm, int numGamesAvailable, float explrFactor)
{
    if (arm.numGames == 0) return INF;
    return armScoreImpl(arm, logf(numGamesAvailable), explrFactor);
}

template <class ActionT>
float armScoreImpl(const BanditArm<ActionT>& arm, float logGamesAvailable, float explrFactor)
{
    return arm.avgReward() + explrFactor * sqrtf(logGamesAvailable / arm.numGames);
}

template <class ActionT>
int findBestArm(const std::vector<BanditArm<ActionT>>& arms, int totalNumGames, float explrFactor)
{
    int bestArm = -1;
    float bestScore = -INF;

    float logTotalGames = logf(totalNumGames);

    FOR_IN_UNIFORM_PERM(i, arms.size())
    {
        if (arms[i].numGames == 0)
        {
            bestArm = i;
            break;
        }

        float score = armScoreImpl(arms[i], logTotalGames, explrFactor);

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

    FOR_IN_UNIFORM_PERM(i, arms.size())
    {
        if (arms[i].numGames < leastGames)
        {
            leastGamesArm = i;
            leastGames = arms[i].numGames;
        }
    }

    return leastGamesArm;
}
