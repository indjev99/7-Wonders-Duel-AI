#include "mc_config.h"

int MCConfig::numSims(int numBranches)
{
    return !branchRelative ? avgNumSims : avgNumSims * numBranches / AVG_BRANCHES;
}
