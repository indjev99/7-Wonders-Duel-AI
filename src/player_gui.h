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

    bool guiCanAdvanceFromDeck(int deck);
    bool guiCanAdvance();

    friend ListenerGUI;
};
