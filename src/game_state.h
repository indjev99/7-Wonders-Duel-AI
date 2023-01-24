#pragma once

#include "action.h"
#include "constants.h"
#include "objects.h"
#include "object_decks.h"
#include "object_location.h"
#include "player_state.h"
#include "pyramid_schemes.h"
#include "pyramid_slot.h"

#include <algorithm>
#include <queue>

#define MAX_DISCARDED NUM_AGES * PYRAMID_SIZE

#define NUM_DECK_LOCATIONS NUM_AGE_1_CARDS + NUM_AGE_2_CARDS + NUM_AGE_3_CARDS + \
        NUM_GUILD_CARDS + NUM_TOKENS + NUM_WONDERS + NUM_GAME_TOKENS + NUM_BOX_TOKENS + \
        NUM_WONDERS_REVEALED + 2 * NUM_WONDERS_PER_PLAYER + MAX_DISCARDED

#define WONDER_SELECTION_AGE -1

struct GameState
{
    int currPlayer;

    std::array<PlayerState, NUM_PLAYERS> playerStates;

    int currAge;
    int wondersBuilt;

    static const std::array<int, NUM_DECKS + 1> deckStarts;

    std::array<int, NUM_DECKS> deckEnds;
    std::array<int, NUM_DECK_LOCATIONS> deckObjects;
    std::array<PyramidSlot, PYRAMID_SIZE> cardPyramid;
    std::array<ObjectLocation, NUM_OBJECTS> objectLocations;

    std::queue<Action> expectedActions;

    GameState();

    void doAction(const Action& action);

private:

    void advanceAge();

    void revealGuild(int pos);
    void revealPyramidCard(int pos, int id);
    void revealGameToken(int id);
    void revealBoxToken(int id);
    void revealWonder(int id);

    void buildPyramidCard(int id);
    void discardPyramidCard(int id);
    void buildWonderWithPyramidCard(int id, int cardId);
    void buildGameToken(int id);
    void buildBoxToken(int id);
    void buildDiscarded(int id);
    void selectWonder(int id);

    void drawObject(int id, int deck);
    void insertObject(int id, int deck);

    void buildDeckObject(int id, int deck);
    void playPyramidCard(int id);
};
