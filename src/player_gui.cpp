#include "player_gui.h"

PlayerGUI::PlayerGUI(ListenerGUI& gui)
    : gui(gui)
{}

Action PlayerGUI::getAction()
{
    action = game->getExpectedAction();
    gui.drawState(false, false, this);
    return action;
}

bool PlayerGUI::guiCanAdvanceFromDeck(int deck)
{
    int id = gui.pressedId;
    if (game->getObjectDeck(id) == deck)
    {
        action.arg1 = id;
        return true;
    }
    return false;
}

#include <iostream>

bool PlayerGUI::guiCanAdvance()
{
    int id = gui.pressedId;

    if (id == OBJ_NONE) return false;

    switch (action.type)
    {
    case ACT_MOVE_PLAY_PYRAMID_CARD:
        if (id < NUM_OBJECT_TYPES && game->isPlayableCard(id))
        {
            action.arg1 = id;
            action.arg2 = ACT_ARG2_DISCARD;
            return true;
        }
        return false;

    case ACT_MOVE_BUILD_GAME_TOKEN:
        return guiCanAdvanceFromDeck(DECK_GAME_TOKENS);

    case ACT_MOVE_BUILD_BOX_TOKEN:
        return guiCanAdvanceFromDeck(DECK_BOX_TOKENS);

    case ACT_MOVE_BUILD_DISCARDED:
        return guiCanAdvanceFromDeck(DECK_DISCARDED);

    case ACT_MOVE_DESTROY_OBJECT:
        if (id < NUM_OBJECT_TYPES && objects[id].type == action.arg2 && game->getPlayerState(1 - player).objectsBuilt[id])
        {
            action.arg1 = id;
            return true;
        }
        return false;


    case ACT_MOVE_SELECT_WONDER:
        return guiCanAdvanceFromDeck(DECK_REVEALED_WONDERS);

    case ACT_MOVE_CHOOSE_START_PLAYER:
        // for (int i = 0; i < NUM_PLAYERS; i++)
        // {
        //     if (id == ListenerGUI::)
        //     action.arg1 = i;
        //     return true;
        // }
        // return false;
        action.arg1 = player;
        return true;
    }

    return false;
}
