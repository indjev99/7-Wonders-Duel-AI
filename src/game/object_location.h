#pragma once

#define NUM_DECKS (10 + NUM_PLAYERS)

#define DECK_AGE_1 0
#define DECK_AGE_2 1
#define DECK_AGE_3 2
#define DECK_GUILDS 3
#define DECK_TOKENS 4
#define DECK_WONDERS 5

#define DECK_GAME_TOKENS 6
#define DECK_BOX_TOKENS 7
#define DECK_REVEALED_WONDERS 8
#define DECK_SELECTED_WONDERS 9
#define DECK_DISCARDED (9 + NUM_PLAYERS)

#define DECK_CARD_PYRAMID -1
#define DECK_USED -2

#define DECK_NONE -100
#define POS_NONE -100

struct ObjectLocation
{
    int deck;
    int pos;

    ObjectLocation(int deck, int pos);
    ObjectLocation();
};
