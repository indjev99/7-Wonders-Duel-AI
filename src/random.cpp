#include "random.h"

#include <random>

std::default_random_engine generator;

void setSeed(int seed)
{
    generator.seed(seed);
}

int uniformInt(int from, int to)
{
    std::uniform_int_distribution<int> distribution(from, to - 1);
    return distribution(generator);
}

double uniformReal(double from, double to)
{
    std::uniform_real_distribution<double> distribution(from, to);
    return distribution(generator);
}
