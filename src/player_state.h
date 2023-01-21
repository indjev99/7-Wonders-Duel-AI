#pragma once

#include "resources.h"
#include "sciences.h"
#include "objects.h"
#include "object_types.h"

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
    int disctincSciences;

    std::array<bool, NUM_OBJECTS> objects;

    bool buildGameToken;
    bool buildDiscardedToken;
    bool buildDiscardedCard;
    bool destroyBrown;
    bool destroyGray;
    bool playAgain;

    PlayerState();
};
