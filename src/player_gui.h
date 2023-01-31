#pragma once

#include "player_ai.h"
#include "listener_gui.h"

struct PlayerGUI final : PlayerAI
{
    PlayerGUI(ListenerGUI& gui);

    Action getAction() override;

private:

    ListenerGUI& gui;

    Action action;
    std::vector<Action> possible;

    bool updateActionFromDeck(int deck);
    bool updateAction();
    bool canAdvance();

    friend ListenerGUI;
};
