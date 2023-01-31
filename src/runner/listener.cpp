#include "listener.h"

void Listener::setGame(const GameState& game)
{
    this->game = &game;
}

void Listener::notifyStart() {}
void Listener::notifyActionPre(const Action& action) {}
void Listener::notifyActionPost(const Action& action) {}
void Listener::notifyEnd() {}
