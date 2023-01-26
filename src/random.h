#pragma once

void setSeed(int seed);
int uniformInt(int from, int to);
double uniformReal(double from, double to);

template <class T>
const auto& uniformElem(const T& seq)
{
    return seq[uniformInt(0, seq.size())];
}
