#pragma once

#include "listener.h"
#include "game_state.h"

struct PlayerAI : Listener
{
    void setPlayer(int player);

    virtual Action getAction() = 0;

protected:

    int player;
};
