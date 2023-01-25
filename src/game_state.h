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

#define AGE_WONDER_SELECTION -1

#define ACTOR_GAME -1

struct GameState
{
    int currPlayer;

    std::array<PlayerState, NUM_PLAYERS> playerStates;

    int currAge;
    int wondersBuilt;
    int cardsRemaining;

    static const std::array<int, NUM_DECKS + 1> deckStarts;

    std::array<int, NUM_DECKS> deckEnds;
    std::array<int, NUM_DECK_LOCATIONS> deckObjects;
    std::array<PyramidSlot, PYRAMID_SIZE> cardPyramid;
    std::array<ObjectLocation, NUM_OBJECTS> objectLocations;

    std::queue<Action> queuedActions;

    GameState();
    GameState(const GameState& other);
    GameState& operator=(const GameState& other);

    void doAction(const Action& action);

    bool isTerminal() const;
    int getResult(int player) const;
    int currActor() const;
    Action expectedAction() const;
    std::vector<Action> possibleActions() const;

    int getCoins(int player) const;
    int getScore(int player) const;
    int getDistinctSciences(int player) const;
    int getMilitary(int player) const;
    int getMilitaryLead(int player) const;

private:

    void linkPlayers();

    void verifyPlayer(int player) const;
    void verifyPos(int pos, int deck) const;
    void verifyObject(int id) const;

    void queueAction(const Action& action, int count = 1);

    bool isDeckEmpty(int deck) const;

    void drawObject(int id, int deck);
    void insertObject(int id, int deck);

    void revealGuild(int pos);
    void revealPyramidCard(int id, int pos);
    void revealGameToken(int id);
    void revealBoxToken(int id);
    void revealWonder(int id);

    void buildDeckObject(int id, int deck);
    void playPyramidCard(int id);

    void buildPyramidCard(int id);
    void discardPyramidCard(int id);
    void usePyramidCardForWonder(int id, int wonderId);
    void buildGameToken(int id);
    void buildBoxToken(int id);
    void buildDiscarded(int id);
    void selectWonder(int id);

    void setupWonderSelection();
    void advanceAge();

    std::vector<Action> possibleFromDeck(const Action& expected, int deck) const;
    std::vector<Action> possiblePlayPyramidCardActions() const;
    std::vector<Action> possibleRevealGuildActions() const;

    std::vector<Action> possibleActionsUnchecked() const;
};
