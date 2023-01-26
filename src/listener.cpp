#include "listener.h"

void Listener::setGame(const GameState& game)
{
    this->game = &game;
}

void Listener::notifyStart() {}
void Listener::notifyAction(const Action& action) {}
void Listener::notifyEnd() {}
