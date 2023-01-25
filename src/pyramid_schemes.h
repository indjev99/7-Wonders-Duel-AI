#pragma once

#include <array>
#include <vector>

#define NUM_AGES 3
#define PYRAMID_SIZE 20
#define NUM_LAST_AGE_GUILDS 3

struct PyramidSlotScheme
{
    bool revealed;
    std::vector<int> covering;
};

using PyramidScheme = std::array<PyramidSlotScheme, PYRAMID_SIZE>;

extern const std::array<PyramidScheme, NUM_AGES> pyramidSchemes;
