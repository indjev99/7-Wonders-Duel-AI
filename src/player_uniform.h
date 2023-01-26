#pragma once

#include "player_ai.h"

struct PlayerUniform final : PlayerAI
{
    Action getAction() override;
};
