#include "mc.h"

#include "game_simulator.h"

#include "game/action.h"
#include "game/results.h"
#include "utils/random.h"

#include <cmath>
#include <numeric>
#include <vector>

BanditArm::BanditArm(const Action& action, int child)
    : action(action)
    , child(child)
    , numGames(0)
    , totalReward(0)
{}

void BanditArm::update(double reward)
{
    numGames++;
    totalReward += reward;
}

std::vector<BanditArm> makeArms(const std::vector<Action>& possible)
{
    std::vector<BanditArm> arms;
    arms.reserve(possible.size());

    for (const Action& action : possible)
    {
        arms.push_back(BanditArm(action));
    }

    return arms;
}

double ucbScore(const BanditArm& arm, double totalNumGames, double explrFactor)
{
    return arm.totalReward / arm.numGames + explrFactor * sqrt(log(totalNumGames) / arm.numGames);
}

int findBestArm(const std::vector<BanditArm>& arms, int totalNumGames, double explrFactor)
{
    int bestArm = -1;
    double bestScore;

    std::vector<int> perm(arms.size());
    std::iota(perm.begin(), perm.end(), 0);
    randShuffle(perm);

    for (int i : perm)
    {
        if (arms[i].numGames == 0)
        {
            bestArm = i;
            break;
        }

        double score = ucbScore(arms[i], totalNumGames, explrFactor);

        if (bestArm == -1 || score > bestScore)
        {
            bestArm = i;
            bestScore = score;
        }
    }

    return bestArm;
}

int simRandGame(GameStateFast& game, int player)
{
    GameSimulator simulator(game);
    return simulator.simGame(player);
}
