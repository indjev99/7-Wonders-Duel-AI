#pragma once

#include "defines.h"

#include <algorithm>
#include <numeric>
#include <random>

#define FOR_IN_UNIFORM_PERM(i, n) \
    static std::vector<int> LINE_NUM(perm_); \
    LINE_NUM(perm_).resize(n); \
    std::iota(LINE_NUM(perm_).begin(), LINE_NUM(perm_).end(), 0); \
    randShuffle(LINE_NUM(perm_)); \
    for (int i : LINE_NUM(perm_))

extern std::mt19937 generator;

FORCE_INLINE void setSeed(int seed)
{
    generator.seed(seed);
}

FORCE_INLINE int uniformInt(int from, int to)
{
    std::uniform_int_distribution<int> distribution(from, to - 1);
    return distribution(generator);
}

FORCE_INLINE float uniformReal(float from, float to)
{
    std::uniform_real_distribution<float> distribution(from, to);
    return distribution(generator);
}

template <class T>
FORCE_INLINE float sampleIntDistr(const T& probs)
{
    float roll = uniformReal(0, 1);
    for (int i = 0; i < (int) probs.size(); i++)
    {
        roll -= probs[i];
        if (roll < 0) return i;
    }
    return 0;
}

template <class T>
FORCE_INLINE const auto& uniformElem(const T& seq)
{
    return seq[uniformInt(0, seq.size())];
}

template <class T>
FORCE_INLINE void randShuffle(T& seq)
{
    return std::shuffle(seq.begin(), seq.end(), generator);
}
