#include "cost.h"

#include <algorithm>

Cost::Cost(int coins, std::initializer_list<int> resourceList, int chain)
    : chain(chain)
    , coins(coins)
{
    std::fill(resources.begin(), resources.end(), 0);
    for (int resource : resourceList)
    {
        resources[resource]++;
    }
}

Cost::Cost(std::initializer_list<int> resourceList, int chain)
    : Cost(0, resourceList, chain)
{}

Cost::Cost(int coins, int chain)
    : Cost(coins, {}, chain)
{}

Cost::Cost()
    : Cost(0, {}, NO_CHAIN)
{}