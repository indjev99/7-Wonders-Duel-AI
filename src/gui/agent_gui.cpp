#include "agent_gui.h"

#include <algorithm>

AgentGUI::AgentGUI(ListenerGUI& gui)
    : gui(gui)
{}

Action AgentGUI::getAction()
{
    action = game->getExpectedAction();
    possible = game->getPossibleActions();
    gui.drawState(false, false, this);
    return action;
}

void toggleArg(int& arg, int val)
{
    if (arg == val) arg = ACT_ARG_NONE;
    else arg = val;
}

bool AgentGUI::updateActionFromDeck(int deck)
{
    int id = gui.pressedId;
    if (id < NUM_OBJECTS && game->getObjectDeck(id) == deck)
    {
        toggleArg(action.arg1, id);
        return true;
    }
    return false;
}

bool AgentGUI::updateAction()
{
    int id = gui.pressedId;

    switch (action.type)
    {
    case ACT_MOVE_PLAY_PYRAMID_CARD:
        if (id < NUM_OBJECTS && game->isPlayableCard(id)) toggleArg(action.arg1, id);
        else if (id < NUM_OBJECTS && game->getObjectDeck(id) == DECK_SELECTED_WONDERS + player) toggleArg(action.arg2, id);
        else if (id == ListenerGUI::O_TEXTURE_COPT_BUTTONS + ListenerGUI::COPT_BUILD) toggleArg(action.arg2, ACT_ARG2_BUILD);
        else if (id == ListenerGUI::O_TEXTURE_COPT_BUTTONS + ListenerGUI::COPT_DISCARD) toggleArg(action.arg2, ACT_ARG2_DISCARD);
        else if (id == ListenerGUI::O_TEXTURE_COPT_BUTTONS + ListenerGUI::COPT_WONDER)
        {
            if (action.arg2 >= 0) action.arg2 = ACT_ARG_NONE;
            else toggleArg(action.arg2, ListenerGUI::ACT_ARG2_WONDER);
        }
        else return false;
        return true;

    case ACT_MOVE_BUILD_GAME_TOKEN:
        return updateActionFromDeck(DECK_GAME_TOKENS);

    case ACT_MOVE_BUILD_BOX_TOKEN:
        return updateActionFromDeck(DECK_BOX_TOKENS);

    case ACT_MOVE_BUILD_DISCARDED:
        return updateActionFromDeck(DECK_DISCARDED);

    case ACT_MOVE_DESTROY_OBJECT:
        if (id < NUM_OBJECTS && objects[id].type == action.arg2 && game->getPlayerState(1 - player).objectsBuilt[id])
        {
            toggleArg(action.arg1, id);
            return true;
        }
        return false;

    case ACT_MOVE_SELECT_WONDER:
        return updateActionFromDeck(DECK_REVEALED_WONDERS);

    case ACT_MOVE_CHOOSE_START_PLAYER:
        for (int i = 0; i < NUM_PLAYERS; i++)
        {
            if (id == ListenerGUI::O_TEXTURE_PLAYER_BUTTONS + i)
            {
                toggleArg(action.arg1, i);
                return true;
            }
        }
        return false;
    }

    return false;
}

bool AgentGUI::canAdvance()
{
    return std::find(possible.begin(), possible.end(), action) != possible.end();
}
