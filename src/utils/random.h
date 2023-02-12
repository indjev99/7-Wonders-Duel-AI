#pragma once

#include <algorithm>
#include <random>

extern std::mt19937 generator;

void setSeed(int seed);
int uniformInt(int from, int to);
double uniformReal(double from, double to);

template <class T>
double sampleIntDistr(const T& probs)
{
    double roll = uniformReal(0, 1);
    for (int i = 0; i < (int) probs.size(); i++)
    {
        roll -= probs[i];
        if (roll < 0) return i;
    }
    return 0;
}

template <class T>
const auto& uniformElem(const T& seq)
{
    return seq[uniformInt(0, seq.size())];
}

template <class T>
void randShuffle(T& seq)
{
    return std::shuffle(seq.begin(), seq.end(), generator);
}
