#include "player_state.h"

#include "coin_cost_calculator.h"
#include "constants.h"
#include "results.h"

#include <cassert>
#include <cstring>

PlayerState::PlayerState()
{
    std::memset(this, 0, sizeof(PlayerState));
}

void PlayerState::buildObject(const Object& object)
{
    builtObjects[object.id] = true;
    typeCounts[object.type]++;

    if (object.effectFunc != nullptr) object.effectFunc(*this);

    if (object.type == OT_WONDER && builtObjects[O_TOKEN_THEOLOGY]) playAgain = true;

    if (object.type == OT_RED)
    {
        int mLead = militaryLead();
        if (mLead >= MILITARY_THRESHOLD_2 && !builtObjects[O_LOOTING_LOOTING_1]) buildObject(objects[O_LOOTING_LOOTING_1]);
        if (mLead >= MILITARY_THRESHOLD_3 && !builtObjects[O_LOOTING_LOOTING_2]) buildObject(objects[O_LOOTING_LOOTING_2]);
    }
}

void PlayerState::payForAndBuildObject(const Object& object)
{
    int resourceCoinCost = calculateResourceCoinCost(*this, object);
    int coinCost = object.cost.coins + resourceCoinCost;

    if (coinCost > coins)
    {
        illegalMove = true;
    }

    coins -= coinCost;
    if (otherPlayer->builtObjects[O_TOKEN_ECONOMY]) otherPlayer->coins += std::max(0, resourceCoinCost);

    buildObject(object);
}

void PlayerState::discardCard()
{
    coins += BASE_DISCARD_COINS + typeCounts[OT_YELLOW];
}

int PlayerState::militaryLead() const
{
    return military - otherPlayer->military;
}

int PlayerState::getScore(bool onlyBlue) const
{
    int score = 0;

    for (int id = 0; id < NUM_OBJECTS; ++id)
    {
        if (builtObjects[id])
        {
            const Object& object = objects[id];
            if (onlyBlue && object.type != OT_BLUE) continue;
            score += object.score;
            if (object.scoreFunc != nullptr) score += object.scoreFunc(*this);
        }
    }

    if (onlyBlue) return score;

    int mLead = militaryLead();
    if (mLead >= MILITARY_THRESHOLD_3) score += MILITARY_SCORE_3;
    else if (mLead >= MILITARY_THRESHOLD_2) score += MILITARY_SCORE_2;
    else if (mLead >= MILITARY_THRESHOLD_1) score += MILITARY_SCORE_1;

    score += coins / COIN_PACKET_SIZE;

    return score;
}

int PlayerState::getResult(bool ended) const
{
    if (illegalMove) return - RESULT_WIN_ILLEGAL;
    if (otherPlayer->illegalMove) return RESULT_WIN_ILLEGAL;

    if (distincSciences >= SCIENCE_THRESHOLD_WIN) return RESULT_WIN_SCIENCE;
    if (otherPlayer->distincSciences >= SCIENCE_THRESHOLD_WIN) return - RESULT_WIN_SCIENCE;

    int mLead = militaryLead();
    if (mLead >= MILITARY_THRESHOLD_WIN) return RESULT_WIN_MILITARY;
    if (mLead <= -MILITARY_THRESHOLD_WIN) return - RESULT_WIN_MILITARY;

    if (!ended) return RESULT_DRAW;

    int score = getScore();
    int otherScore = otherPlayer->getScore();

    if (score > otherScore) return RESULT_WIN_CIVILIAN;
    if (score < otherScore) return - RESULT_WIN_CIVILIAN;

    int blueScore = getScore(true);
    int otherBlueScore = getScore(true);

    if (blueScore > otherBlueScore) return RESULT_WIN_TIEBREAK;
    if (blueScore < otherBlueScore) return - RESULT_WIN_TIEBREAK;

    return RESULT_DRAW;
}
