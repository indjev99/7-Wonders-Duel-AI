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
#include <deque>

#define MAX_DISCARDED NUM_AGES * PYRAMID_SIZE

#define NUM_DECK_LOCATIONS NUM_AGE_1_CARDS + NUM_AGE_2_CARDS + NUM_AGE_3_CARDS + \
        NUM_GUILD_CARDS + NUM_TOKENS + NUM_WONDERS + NUM_GAME_TOKENS + NUM_BOX_TOKENS + \
        NUM_WONDERS_REVEALED + 2 * NUM_WONDERS_PER_PLAYER + MAX_DISCARDED + PYRAMID_SIZE

#define AGE_SETUP -2
#define AGE_WONDER_SELECTION -1

#define ACTOR_GAME -1

template <bool CheckValid>
struct GameStateT
{
    GameStateT();

    template <bool OtherCheckValid>
    GameStateT(const GameStateT<OtherCheckValid>* other);

    void reset();

    template <bool OtherCheckValid>
    void clone(const GameStateT<OtherCheckValid>* other);

    template <bool OtherCheckValid>
    GameStateT(const GameStateT<OtherCheckValid>& other) = delete;

    template <bool OtherCheckValid>
    GameStateT& operator=(const GameStateT<OtherCheckValid>& other) = delete;

    template <bool OtherCheckValid>
    GameStateT(GameStateT<OtherCheckValid>&& other) = delete;

    template <bool OtherCheckValid>
    GameStateT& operator=(GameStateT<OtherCheckValid>&& other) = delete;

    void doAction(const Action& action);

    bool isTerminal() const;
    int getResult(int player) const;

    int getFirstPlayer() const;
    int getCurrAge() const;
    int getCurrActor() const;
    Action getExpectedAction() const;
    const std::vector<Action>& getPossibleActions() const;
    void getPossibleActions(std::vector<Action>& possible) const;

    int getOpponent(int player) const;
    int getCoins(int player) const;
    int getScore(int player, int onlyType = OT_NONE) const;
    int getDistinctSciences(int player) const;
    int getMilitary(int player) const;
    int getMilitaryLead(int player) const;
    int getWondersBuilt() const;

    int getDeckSize(int deck) const;
    bool isDeckEmpty(int deck) const;
    int getDeckElem(int deck, int pos) const;
    int getObjectDeck(int id) const;
    bool isPlayableCard(int id) const;

    const PyramidSlot& getPyramidSlot(int pos) const;
    const PlayerState& getPlayerState(int player) const;

private:

    int firstPlayer;

    int currPlayer;
    int currAge;
    int wondersBuilt;
    int cardsRemaining;

    std::array<PlayerState, NUM_PLAYERS> playerStates;

    static const std::array<int, NUM_DECKS + 1> deckStarts;

    std::array<int, NUM_DECKS> deckEnds;
    std::array<int, NUM_DECK_LOCATIONS> deckObjects;
    std::array<PyramidSlot, PYRAMID_SIZE> cardPyramid;
    std::array<ObjectLocation, NUM_OBJECTS> objectLocations;

    std::deque<Action> queuedActions;

    void linkPlayers();

    void verifyPlayer(int player) const;
    void verifyPos(int pos, int deck) const;
    void verifyObject(int id) const;

    int otherPlayer() const;
    void advancePlayer();

    void preQueueAction(const Action& action, int count = 1);
    void queueAction(const Action& action, int count = 1);

    void drawObject(int id, int deck);
    void insertObject(int id, int deck);

    void revealGuild(int pos);
    void revealPyramidCard(int id, int pos);
    void revealGameToken(int id);
    void revealBoxToken(int id);
    void revealWonder(int id);
    void revealFirstPlayer(int player);

    void buildDeckObject(int id, int deck);
    void playPyramidCard(int id);
    void addPlayablePyramidCard(int pos);
    void remPlayablePyramidCard(int pos);

    void buildPyramidCard(int id);
    void discardPyramidCard(int id);
    void usePyramidCardForWonder(int id, int wonderId);
    void buildGameToken(int id);
    void buildBoxToken(int id);
    void buildDiscarded(int id);
    void destroyObject(int id, int type);
    void selectWonder(int id);
    void chooseStartPlayer(int player);

    void startWonderSelection();
    void advanceAge();

    void possibleFromDeckActions(std::vector<Action>& possible, const Action& expected, int deck) const;
    void possiblePlayPyramidCardActions(std::vector<Action>& possible) const;
    void possibleDestroyObjectActions(std::vector<Action>& possible, int type) const;
    void possibleChooseStartPlayerActions(std::vector<Action>& possible) const;
    void possibleRevealGuildActions(std::vector<Action>& possible) const;
    void possibleRevealFirstPlayerActions(std::vector<Action>& possible) const;

    void possibleActionsUnchecked(std::vector<Action>& possible) const;

    bool correctPossibleActions;
    std::vector<Action> possibleActions;
    std::vector<int> possibleWonders;

    template <bool OtherCheckValid>
    friend class GameStateT;
};

using GameState = GameStateT<true>;
using GameStateFast = GameStateT<false>;
