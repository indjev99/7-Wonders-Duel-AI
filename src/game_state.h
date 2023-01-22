#pragma once

#include "constants.h"
#include "objects.h"
#include "object_decks.h"
#include "object_location.h"
#include "player_state.h"
#include "pyramid_slot.h"

#include <algorithm>

#define NUM_DECK_LOCATIONS NUM_AGE_1_CARDS + NUM_AGE_2_CARDS + NUM_AGE_3_CARDS + NUM_GUILD_CARDS + NUM_TOKENS + NUM_WONDERS

#define MAX_PYRAMID_LOCATIONS 20

struct GameState
{
    int currPlayer;

    std::array<PlayerState, NUM_PLAYERS> playerStates;

    int currAge;
    int wondersBuilt;

    std::array<int, NUM_DECKS> deckStarts;
    std::array<int, NUM_DECK_LOCATIONS> deckObjects;
    std::array<PyramidSlot, MAX_PYRAMID_LOCATIONS> cardPyramid;
    std::array<ObjectLocation, NUM_OBJECTS> objectLocations;

    GameState();

    void revealPyramidCard(int pos, int id);
    void revealGameToken(int id);
    void revealBoxToken(int pos, int id);

    void buildPyramidCard(int id);
    void discardPyramidCard(int id);
    void buildWonderWithPyramidCard(int id, int cardId);

    void buildGameToken(int id);
    void buildBoxToken(int id);
    void buildDiscarded(int id);

private:

    void drawObject(int deck, int id);
    void revealMiscObject(int id, int fromDeck, int toDeck);
    void buildMiscObject(int id, int fromDeck);
    void startPlayPyramidCard(int id);
    void endPlayPyramidCard(int id);
};
