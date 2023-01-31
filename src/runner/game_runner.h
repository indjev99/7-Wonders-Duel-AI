#pragma once

#include "agent.h"
#include "listener.h"
#include "revealer.h"

#include <array>
#include <vector>

struct GameRunner
{
    GameRunner(Revealer* revealer, const std::array<Agent*, NUM_PLAYERS>& agents, const std::vector<Listener*>& listeners = {});
    GameRunner(const GameRunner&) = delete;
    GameRunner& operator=(const GameRunner&) = delete;

    int playGame();

private:

    Revealer* revealer;
    std::array<Agent*, NUM_PLAYERS> agents;
    std::vector<Listener*> listeners;

    GameState game;
};
