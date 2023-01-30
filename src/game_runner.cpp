#include "game_runner.h"

#include "random.h"

#include <algorithm>

GameRunner::GameRunner(const std::array<PlayerAI*, NUM_PLAYERS>& playerAIs, const std::vector<Listener*>& listeners)
    : playerAIs(playerAIs)
    , listeners(listeners)
{
    std::copy(playerAIs.begin(), playerAIs.end(), std::back_inserter(this->listeners));
    for (auto listener : this->listeners)
    {
        listener->setGame(game);
    }
    for (int i = 0; i < NUM_PLAYERS; i++)
    {
        this->playerAIs[i]->setPlayer(i);
    }
}

int GameRunner::playGame()
{
    game = GameState();
    for (auto listener : listeners)
    {
        listener->notifyStart();
    }
    while (!game.isTerminal())
    {
        int actor = game.getCurrActor();
        Action action = actor == ACTOR_GAME ? uniformElem(game.getPossibleActions()) : playerAIs[actor]->getAction();
        for (auto listener : listeners)
        {
            listener->notifyActionPre(action);
        }
        game.doAction(action);
        for (auto listener : listeners)
        {
            listener->notifyActionPost(action);
        }
    }
    for (auto listener : listeners)
    {
        listener->notifyEnd();
    }
    return game.getResult(0);
}
