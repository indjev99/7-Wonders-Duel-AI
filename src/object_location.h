#pragma once

#define NUM_DECKS 6

#define DECK_AGE_1 0
#define DECK_AGE_2 1
#define DECK_AGE_3 2
#define DECK_GUILD 3
#define DECK_TOKENS 4
#define DECK_WONDERS 5

#define DECK_CARD_PYRAMID -1
#define DECK_GAME_TOKENS -2
#define DECK_BOX_TOKENS -3
#define DECK_DISCARDED -4
#define DECK_SELECTED_WONDERS -5
#define DECK_BUILT -6
#define DECK_UNDER_WONDER -7
#define DECK_INVALID -100

#define POS_NONE -1

struct ObjectLocation
{
    int deck;
    int pos;

    ObjectLocation(int deck, int pos);
    ObjectLocation();
};
