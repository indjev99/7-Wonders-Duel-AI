#pragma once

#include "agent.h"
#include "listener.h"

#include <array>
#include <vector>

struct GameRunner
{
    GameRunner(const std::array<Agent*, NUM_PLAYERS>& playerAIs, const std::vector<Listener*>& listeners = {});
    GameRunner(const GameRunner&) = delete;
    GameRunner& operator=(const GameRunner&) = delete;

    int playGame();

private:

    std::array<Agent*, NUM_PLAYERS> playerAIs;
    std::vector<Listener*> listeners;

    GameState game;
};
