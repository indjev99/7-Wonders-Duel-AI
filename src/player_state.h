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

    std::array<bool, NUM_OBJECTS> builtObjects;

    bool buildGameToken;
    bool buildDiscardedToken;
    bool buildDiscardedCard;
    bool destroyBrown;
    bool destroyGray;
    bool playAgain;

    bool illegalMove;

    PlayerState();

    void buildObject(const Object& object);
    void payForAndBuildObject(const Object& object);
    void discardCard();

    int militaryLead() const;
    int getScore(bool onlyBlue = false) const;
    int getResult(bool ended = false) const;
};
