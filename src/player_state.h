#pragma once

#include "objects.h"
#include "object_types.h"
#include "resources.h"
#include "sciences.h"

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
    void payForAndBuildObject(const Object& object);
    void discardCard();
    void destroyObject(const Object& object);

    int getCost(const Object& object) const;
    bool canPayFor(const Object& object) const;

    int militaryLead() const;
    int getScore(int onlyType = OT_NONE) const;
    int getResult(bool terminal) const;
};
