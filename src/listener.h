#pragma once

#include "game_state.h"

struct Listener
{
    void setGame(const GameState& game);

    virtual void notifyStart();
    virtual void notifyAction(const Action& action);
    virtual void notifyEnd();

    virtual ~Listener() = default;

protected:

    const GameState* game;
};
