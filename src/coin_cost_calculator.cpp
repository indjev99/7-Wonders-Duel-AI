#include "coin_cost_calculator.h"

const int BASE_COST = 2;
const int MARKET_COST = 1;

void chooseMostExpensive(std::array<int, NUM_RESOURCES>& remaining, const std::array<int, NUM_RESOURCES>& costs, const std::initializer_list<int>& possible)
{
    int argmax = -1;
    int maxCost = 0;
    for (int res : possible)
    {
        if (remaining[res] > 0 && costs[res] > maxCost)
        {
            maxCost = costs[res];
            argmax = res;
        }
    }

    if (argmax != -1)
    {
        remaining[argmax]--;
    }
}

int calculateResourceCoinCost(const PlayerState& state, const Object& object)
{
    if (object.cost.chain != NO_CHAIN && state.objects[object.cost.chain])
    {
        if (state.objects[O_TOKEN_URBANISM]) return -4;
        else return 0;
    }

    bool nonZero = false;
    std::array<int, NUM_RESOURCES> remaining;

    for (int res : R_ALL_LIST)
    {
        remaining[res] = std::max(0, object.cost.resources[res] - state.resources[res]);
        nonZero = nonZero || remaining[res] > 0;
    }

    if (!nonZero) return 0;

    std::array<int, NUM_RESOURCES> costs;

    for (int res : R_ALL_LIST)
    {
        if (state.markets[res]) costs[res] = MARKET_COST;
        else costs[res] = BASE_COST + state.otherPlayer->resources[res];
    }

    for (int i = 0; i < state.brownWildcards; ++i)
    {
        chooseMostExpensive(remaining, costs, R_BROWNS_LIST);
    }

    for (int i = 0; i < state.grayWildcards; ++i)
    {
        chooseMostExpensive(remaining, costs, R_BROWNS_LIST);
    }

    int allWildcards = 0;
    if (object.type == OT_WONDER && state.objects[O_TOKEN_ARCHITECTURE]) allWildcards += 2;
    if (object.type == OT_BLUE && state.objects[O_TOKEN_MASONRY]) allWildcards += 2;

    for (int i = 0; i < allWildcards; ++i)
    {
        chooseMostExpensive(remaining, costs, R_ALL_LIST);
    }

    int cost = 0;
    for (int res : R_ALL_LIST)
    {
        cost += remaining[res] * costs[res];
    }

    return cost;
}
