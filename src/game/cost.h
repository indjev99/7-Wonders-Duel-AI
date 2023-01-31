#pragma once

#include "resources.h"

#include <array>
#include <initializer_list>

#define OBJ_NONE -100

struct Cost
{
    int chain;
    int coins;
    std::array<int, NUM_RESOURCES> resources;

    Cost(int coins, const std::initializer_list<int>& resourceList, int chain = OBJ_NONE);
    Cost(const std::initializer_list<int>& resourceList, int chain = OBJ_NONE);
    Cost(int coins, int chain = OBJ_NONE);
    Cost();
};
