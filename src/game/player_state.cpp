#include "player_state.h"

#include "constants.h"
#include "game_exception.h"
#include "results.h"

#include <cassert>
#include <cstring>

PlayerState::PlayerState()
{
    std::memset(this, 0, sizeof(PlayerState));

    coins = INITIAL_COINS;
    shouldDestroyType = OBJ_NONE;
    cachedCoinCostObj = OBJ_NONE;
}

void PlayerState::buildObject(const Object& object)
{
    if (objectsBuilt[object.id])
        throw GameException("Object already built.", {{"objectId", object.id}});

    objectsBuilt[object.id] = true;
    typeCounts[object.type]++;

    if (object.effectFunc != nullptr) object.effectFunc(*this);

    if (object.type == OT_RED && objectsBuilt[O_TOKEN_STRATEGY]) military += STRATEGY_EXTRA_MILITARY;
    if (object.type == OT_WONDER && objectsBuilt[O_TOKEN_THEOLOGY]) shouldPlayAgain = true;

    int mLead = militaryLead();
    if (mLead >= MILITARY_THRESHOLD_2 && !objectsBuilt[O_LOOTING_LOOTING_1]) buildObject(objects[O_LOOTING_LOOTING_1]);
    if (mLead >= MILITARY_THRESHOLD_3 && !objectsBuilt[O_LOOTING_LOOTING_2]) buildObject(objects[O_LOOTING_LOOTING_2]);

    cachedCoinCostObj = OBJ_NONE;
    otherPlayer->cachedCoinCostObj = OBJ_NONE;
}

void PlayerState::destroyObject(const Object& object)
{
    if (!objectsBuilt[object.id])
        throw GameException("Object not built but destroyed.", {{"objectId", object.id}});

    if (object.revEffectFunc == nullptr && object.effectFunc != nullptr)
        throw GameException("Object with effect destoryed but has no reverse effect.", {{"objectId", object.id}});

    objectsBuilt[object.id] = false;
    typeCounts[object.type]--;

    if (object.revEffectFunc != nullptr) object.revEffectFunc(*this);

    int mLead = militaryLead();
    if (mLead <= - MILITARY_THRESHOLD_2 && !otherPlayer->objectsBuilt[O_LOOTING_LOOTING_1]) otherPlayer->buildObject(objects[O_LOOTING_LOOTING_1]);
    if (mLead <= - MILITARY_THRESHOLD_3 && !otherPlayer->objectsBuilt[O_LOOTING_LOOTING_2]) otherPlayer->buildObject(objects[O_LOOTING_LOOTING_2]);

    cachedCoinCostObj = OBJ_NONE;
    otherPlayer->cachedCoinCostObj = OBJ_NONE;
}

void PlayerState::payForAndBuildObject(const Object& object)
{
    int coinCost = getCost(object);

    if (coinCost > coins)
        throw GameException("Not enough coins.", {{"objectId", object.id}, {"coinCost", coinCost}, {"coins", coins}});

    coins -= coinCost;
    if (otherPlayer->objectsBuilt[O_TOKEN_ECONOMY]) otherPlayer->coins += std::max(0, coinCost - object.cost.coins);

    buildObject(object);
}

void PlayerState::discardCard()
{
    coins += BASE_DISCARD_COINS + typeCounts[OT_YELLOW];
}

int PlayerState::getScore(int onlyType) const
{
    int score = 0;

    for (int id = 0; id < NUM_OBJECTS; id++)
    {
        if (objectsBuilt[id])
        {
            const Object& object = objects[id];
            if (onlyType != OT_NONE && object.type != onlyType) continue;
            score += object.score;
            if (object.scoreFunc != nullptr) score += object.scoreFunc(*this);
        }
    }

    if (onlyType == OT_NONE || onlyType == OT_MILITARY)
    {
        int mLead = militaryLead();
        if (mLead >= MILITARY_THRESHOLD_3) score += MILITARY_SCORE_3;
        else if (mLead >= MILITARY_THRESHOLD_2) score += MILITARY_SCORE_2;
        else if (mLead >= MILITARY_THRESHOLD_1) score += MILITARY_SCORE_1;        
    }

    if (onlyType == OT_NONE || onlyType == OT_COINS)
        score += coins / COIN_PACKET_SIZE;

    return score;
}

int PlayerState::getResult(bool terminal) const
{
    if (distincSciences >= SCIENCE_THRESHOLD_WIN) return RESULT_WIN_SCIENCE;
    if (otherPlayer->distincSciences >= SCIENCE_THRESHOLD_WIN) return - RESULT_WIN_SCIENCE;

    int mLead = militaryLead();
    if (mLead >= MILITARY_THRESHOLD_WIN) return RESULT_WIN_MILITARY;
    if (mLead <= -MILITARY_THRESHOLD_WIN) return - RESULT_WIN_MILITARY;

    if (!terminal) return RESULT_DRAW;

    int score = getScore();
    int otherScore = otherPlayer->getScore();

    if (score > otherScore) return RESULT_WIN_CIVILIAN;
    if (score < otherScore) return - RESULT_WIN_CIVILIAN;

    int blueScore = getScore(OT_BLUE);
    int otherBlueScore = otherPlayer->getScore(OT_BLUE);

    if (blueScore > otherBlueScore) return RESULT_WIN_TIEBREAK;
    if (blueScore < otherBlueScore) return - RESULT_WIN_TIEBREAK;

    return RESULT_DRAW;
}
