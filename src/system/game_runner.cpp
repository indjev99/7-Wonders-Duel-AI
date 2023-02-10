#include "game_runner.h"

#include <algorithm>

GameRunner::GameRunner(Revealer* revealer_, const std::array<Agent*, NUM_PLAYERS>& agents_, const std::vector<Listener*>& listeners_)
    : revealer(revealer_)
    , agents(agents_)
{
    addListener(revealer, ACTOR_GAME);
    addListeners(listeners_, ACTOR_GAME);
    for (int i = 0; i < NUM_PLAYERS; i++)
    {
        addListener(agents[i], i);
    }
}

void GameRunner::addListener(Listener* listener, int actor)
{
    if (std::find(listeners.begin(), listeners.end(), listener) != listeners.end()) return;

    listener->setGame(game);

    Agent* agent = dynamic_cast<Agent*>(listener);
    if (agent != nullptr) agent->setPlayer(actor);

    listeners.push_back(listener);
    addListeners(listener->getSubListeners(), actor);
}

void GameRunner::addListeners(std::vector<Listener*> newListeners, int actor)
{
    for (Listener* listener : newListeners)
    {
        addListener(listener, actor);
    }
}

int GameRunner::playGame()
{
    game.reset();

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
