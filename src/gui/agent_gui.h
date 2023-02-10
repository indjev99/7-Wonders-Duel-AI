#pragma once

#include "listener_gui.h"

#include "system/agent.h"

struct AgentGUI final : Agent
{
    AgentGUI(ListenerGUI& gui);

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
