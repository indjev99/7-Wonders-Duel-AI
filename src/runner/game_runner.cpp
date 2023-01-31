#include "game_runner.h"

#include <algorithm>

GameRunner::GameRunner(Revealer* revealer_, const std::array<Agent*, NUM_PLAYERS>& agents_, const std::vector<Listener*>& listeners_)
    : revealer(revealer_)
    , agents(agents_)
    , listeners(listeners_)
{
    listeners.push_back(revealer);
    std::copy(agents.begin(), agents.end(), std::back_inserter(listeners));
    for (auto listener : listeners)
    {
        listener->setGame(game);
    }
    for (int i = 0; i < NUM_PLAYERS; i++)
    {
        agents[i]->setPlayer(i);
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
        Action action = actor == ACTOR_GAME ? revealer->getAction() : agents[actor]->getAction();
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
