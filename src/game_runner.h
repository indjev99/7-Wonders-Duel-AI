#pragma once

#include "listener.h"
#include "game_state.h"
#include "player_ai.h"

#include <array>
#include <vector>

struct GameRunner
{
    GameRunner(const std::array<PlayerAI*, NUM_PLAYERS>& playerAIs, const std::vector<Listener*>& listeners = {});
    GameRunner(const GameRunner&) = delete;
    GameRunner& operator=(const GameRunner&) = delete;

    int playGame();

private:

    std::array<PlayerAI*, NUM_PLAYERS> playerAIs;
    std::vector<Listener*> listeners;

    GameState game;
};
