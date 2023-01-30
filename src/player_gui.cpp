#include "player_gui.h"

PlayerGUI::PlayerGUI(ListenerGUI& gui)
    : gui(gui)
{}

Action PlayerGUI::getAction()
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

void PlayerGUI::updateActionFromDeck(int deck)
{
    int id = gui.pressedId;
    if (id < NUM_OBJECTS && game->getObjectDeck(id) == deck) toggleArg(action.arg1, id);
}

void PlayerGUI::updateAction()
{
    int id = gui.pressedId;

    switch (action.type)
    {
    case ACT_MOVE_PLAY_PYRAMID_CARD:
        if (id < NUM_OBJECTS && game->isPlayableCard(id)) toggleArg(action.arg1, id);
        if (id < NUM_OBJECTS && game->getObjectDeck(id) == DECK_SELECTED_WONDERS + player) toggleArg(action.arg2, id);
        if (id == ListenerGUI::O_TEXTURE_COPT_BUTTONS + ListenerGUI::COPT_BUILD) toggleArg(action.arg2, ACT_ARG2_BUILD);
        if (id == ListenerGUI::O_TEXTURE_COPT_BUTTONS + ListenerGUI::COPT_DISCARD) toggleArg(action.arg2, ACT_ARG2_DISCARD);
        if (id == ListenerGUI::O_TEXTURE_COPT_BUTTONS + ListenerGUI::COPT_WONDER)
        {
            if (action.arg2 >= 0) action.arg2 = ACT_ARG_NONE;
            else toggleArg(action.arg2, ListenerGUI::ACT_ARG2_WONDER);
        }
        break;

    case ACT_MOVE_BUILD_GAME_TOKEN:
        updateActionFromDeck(DECK_GAME_TOKENS);
        break;

    case ACT_MOVE_BUILD_BOX_TOKEN:
        updateActionFromDeck(DECK_BOX_TOKENS);
        break;

    case ACT_MOVE_BUILD_DISCARDED:
        updateActionFromDeck(DECK_DISCARDED);
        break;

    case ACT_MOVE_DESTROY_OBJECT:
        if (id < NUM_OBJECTS && objects[id].type == action.arg2 && game->getPlayerState(1 - player).objectsBuilt[id])
            toggleArg(action.arg1, id);

    case ACT_MOVE_SELECT_WONDER:
        updateActionFromDeck(DECK_REVEALED_WONDERS);
        break;

    case ACT_MOVE_CHOOSE_START_PLAYER:
        for (int i = 0; i < NUM_PLAYERS; i++)
        {
            if (id == ListenerGUI::O_TEXTURE_PLAYER_BUTTONS + i) toggleArg(action.arg1, i);
        }
        break;
    }
}

bool PlayerGUI::guiCanAdvance()
{
    updateAction();

    return std::find(possible.begin(), possible.end(), action) != possible.end();

    // TODO: Split this into two methods:
    // One handles updating the action
    // The other handles checking if the current action is valid
}
