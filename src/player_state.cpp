#include "player_state.h"

#include <cstring>

PlayerState::PlayerState()
{
    std::memset(this, 0, sizeof(PlayerState));
}
