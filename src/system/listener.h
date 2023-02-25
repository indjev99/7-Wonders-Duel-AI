#pragma once

#include "game/game_state.h"

#include <vector>

struct Listener
{
    void setGame(const GameState& game);

    virtual void notifyStart();
    virtual void notifyActionPre(const Action& action);
    virtual void notifyActionPost(const Action& action);
    virtual void notifyEnd();

    virtual std::vector<Listener*> getSubListeners();

    virtual ~Listener() = default;

protected:

    const GameState* game;
};
