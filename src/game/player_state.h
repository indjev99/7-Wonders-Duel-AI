#pragma once

#include "objects.h"
#include "object_types.h"
#include "resources.h"
#include "sciences.h"

#include "utils/defines.h"

#include <array>

struct PlayerState
{
    PlayerState* otherPlayer;

    int coins;

    std::array<int, NUM_OBJECT_TYPES> typeCounts;

    std::array<int, NUM_RESOURCES> resources;
    std::array<bool, NUM_RESOURCES> markets;
    int brownWildcards;
    int grayWildcards;

    int military;

    std::array<bool, NUM_SCIENCES> sciences;
    int distincSciences;

    std::array<bool, NUM_OBJECTS> objectsBuilt;

    bool shouldBuildGameToken;
    bool shouldBuildBoxToken;
    bool shouldBuildDiscarded;
    int shouldDestroyType;
    bool shouldPlayAgain;

    PlayerState();

    void buildObject(const Object& object);
    void destroyObject(const Object& object);
    void payForAndBuildObject(const Object& object);
    void discardCard();

    FORCE_INLINE int getCost(const Object& object) const;
    FORCE_INLINE bool canPayFor(const Object& object) const;

    FORCE_INLINE int militaryLead() const;
    int getScore(int onlyType = OT_NONE) const;
    int getResult(bool terminal) const;

private:

    mutable int cachedCoinCostObj;
    mutable int cachedCoinCost;
};

int PlayerState::getCost(const Object& object) const
{
    int calculateCoinCost(const PlayerState& state, const Object& object);

    if (object.id != cachedCoinCostObj)
    {
        cachedCoinCostObj = object.id;
        cachedCoinCost = calculateCoinCost(*this, object);
    }
    return cachedCoinCost;
}

bool PlayerState::canPayFor(const Object& object) const
{
    return getCost(object) <= coins;
}

int PlayerState::militaryLead() const
{
    return military - otherPlayer->military;
}
