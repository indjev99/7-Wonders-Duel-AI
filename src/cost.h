#pragma once

#include "resources.h"

#include <array>
#include <initializer_list>

#define NO_CHAIN -1

struct Cost
{
    int chain;
    int coins;
    std::array<int, NUM_RESOURCES> resources;

    Cost(int coins, std::initializer_list<int> resourceList, int chain = NO_CHAIN);
    Cost(std::initializer_list<int> resourceList, int chain = NO_CHAIN);
    Cost(int coins, int chain = NO_CHAIN);
    Cost();
};
