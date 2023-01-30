#include "player_gui.h"

PlayerGUI::PlayerGUI(ListenerGUI& gui)
    : gui(gui)
{}

Action PlayerGUI::getAction()
{
    action = game->getExpectedAction();
    gui.drawState(false, this);
    return action;
}

bool PlayerGUI::guiCanAdvanceFromDeck(int deck)
{
    int id = gui.pressedObjId;
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
    int id = gui.pressedObjId;

    if (id == OBJ_NONE && action.type != ACT_MOVE_CHOOSE_START_PLAYER) return false;

    std::cerr << "Here: " << id << std::endl;

    switch (action.type)
    {
    case ACT_MOVE_PLAY_PYRAMID_CARD:
        if (game->isPlayableCard(id))
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
        if (objects[id].type == action.arg2 && game->getPlayerState(1 - player).objectsBuilt[id])
        {
            action.arg1 = id;
            return true;
        }
        return false;


    case ACT_MOVE_SELECT_WONDER:
        std::cerr << "revealed wonders" << std::endl;
        return guiCanAdvanceFromDeck(DECK_REVEALED_WONDERS);

    case ACT_MOVE_CHOOSE_START_PLAYER:
        action.arg1 = player;
        return true;
    }

    return false;
}
