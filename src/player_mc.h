#pragma once

#include "player_ai.h"

struct PlayerMC final : PlayerAI
{
    PlayerMC(int avgNumSims = 20000, bool branchRelative = true);

    Action getAction() override;

private:

    int avgNumSims;
    bool branchRelative;
};
