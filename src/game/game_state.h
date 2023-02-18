#pragma once

#include "action.h"
#include "constants.h"
#include "game_exception.h"
#include "objects.h"
#include "object_decks.h"
#include "object_location.h"
#include "player_state.h"
#include "pyramid_schemes.h"
#include "pyramid_slot.h"

#include "utils/defines.h"

#include <deque>

#define MAX_DISCARDED NUM_AGES * PYRAMID_SIZE

#define NUM_DECK_LOCATIONS NUM_AGE_1_CARDS + NUM_AGE_2_CARDS + NUM_AGE_3_CARDS + \
        NUM_GUILD_CARDS + NUM_TOKENS + NUM_WONDERS + NUM_GAME_TOKENS + NUM_BOX_TOKENS + \
        NUM_WONDERS_REVEALED + 2 * NUM_WONDERS_PER_PLAYER + MAX_DISCARDED + PYRAMID_SIZE

#define AGE_SETUP -2
#define AGE_WONDER_SELECTION -1

#define ACTOR_GAME -1
#define ACTOR_NONE -100

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

    FORCE_INLINE bool isTerminal() const;
    FORCE_INLINE int getResult(int player) const;

    FORCE_INLINE int getFirstPlayer() const;
    FORCE_INLINE bool isAgeStart() const;
    FORCE_INLINE int getCurrAge() const;
    FORCE_INLINE int getCurrActor() const;
    FORCE_INLINE Action getExpectedAction() const;
    const std::vector<Action>& getPossibleActions() const;
    void getPossibleActions(std::vector<Action>& possible) const;

    FORCE_INLINE int getOpponent(int player) const;
    FORCE_INLINE int getCoins(int player) const;
    FORCE_INLINE int getScore(int player, int onlyType = OT_NONE) const;
    FORCE_INLINE int getDistinctSciences(int player) const;
    FORCE_INLINE int getMilitary(int player) const;
    FORCE_INLINE int getMilitaryLead(int player) const;
    FORCE_INLINE int getWondersBuilt() const;

    FORCE_INLINE int getDeckSize(int deck) const;
    FORCE_INLINE bool isDeckEmpty(int deck) const;
    FORCE_INLINE int getDeckElem(int deck, int pos) const;
    FORCE_INLINE int getObjectDeck(int id) const;
    FORCE_INLINE int getObjectPos(int id) const;
    FORCE_INLINE bool isPlayableCard(int id) const;

    FORCE_INLINE const PyramidSlot& getPyramidSlot(int pos) const;
    FORCE_INLINE const PlayerState& getPlayerState(int player) const;

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

    FORCE_INLINE void verifyPlayer(int player) const;
    FORCE_INLINE void verifyPos(int pos, int deck) const;
    FORCE_INLINE void verifyObject(int id) const;

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

    mutable bool correctPossibleActions;
    mutable std::vector<Action> possibleActions;
    mutable std::vector<int> possibleWonders;

    template <bool OtherCheckValid>
    friend class GameStateT;
};

using GameState = GameStateT<true>;
using GameStateFast = GameStateT<false>;



template <bool CheckValid>
void GameStateT<CheckValid>::verifyPlayer(int player) const
{
    if constexpr (!CheckValid) return;

    if (player < 0 || player >= NUM_PLAYERS)
        throw GameException("Invalid player.", {{"player", player}});
}

template <bool CheckValid>
void GameStateT<CheckValid>::verifyPos(int pos, int deck) const
{
    if constexpr (!CheckValid) return;

    int start = deck == DECK_PYRAMID ? 0 : deckStarts[deck];
    int end = deck == DECK_PYRAMID ? PYRAMID_SIZE : deckEnds[deck];

    if (pos < start || pos >= end)
        throw GameException("Position not in deck.", {{"pos", pos}, {"deck", deck}, {"deckStart", start}, {"deckEnd", end}});
}

template <bool CheckValid>
void GameStateT<CheckValid>::verifyObject(int id) const
{
    if constexpr (!CheckValid) return;

    if (id < 0 || id >= NUM_OBJECTS)
        throw GameException("Invalid object id.", {{"objectId", id}});
}

template <bool CheckValid>
int GameStateT<CheckValid>::getOpponent(int player) const
{
    if constexpr (CheckValid) verifyPlayer(player);

    return 1 - player;
}

template <bool CheckValid>
bool GameStateT<CheckValid>::isTerminal() const
{
    return queuedActions.empty();
}

template <bool CheckValid>
int GameStateT<CheckValid>::getResult(int player) const
{
    if constexpr (CheckValid)
    {
        verifyPlayer(player);

        if (!isTerminal())
            throw GameException("Game has not ended yet.", {});
    }

    return playerStates[player].getResult(true);
}

template <bool CheckValid>
int GameStateT<CheckValid>::getFirstPlayer() const
{
    if constexpr (CheckValid)
    {
        if (firstPlayer == ACTOR_NONE)
            throw GameException("First player has not been set yet.", {});
    }

    return firstPlayer;
}

template <bool CheckValid>
bool GameStateT<CheckValid>::isAgeStart() const
{
    return !isTerminal() && getExpectedAction() == Action(ACT_REVEAL_PYRAMID_CARD, ACT_ARG_NONE, 0);
}

template <bool CheckValid>
int GameStateT<CheckValid>::getCurrAge() const
{
    return currAge;
}

template <bool CheckValid>
int GameStateT<CheckValid>::getCurrActor() const
{
    if (getExpectedAction().isPlayerMove()) return currPlayer;
    else return ACTOR_GAME;
}

template <bool CheckValid>
Action GameStateT<CheckValid>::getExpectedAction() const
{
    if constexpr (CheckValid)
    {
        if (isTerminal())
            throw GameException("Game has already ended.", {});
    }

    return queuedActions.front();
}

template <bool CheckValid>
const std::vector<Action>& GameStateT<CheckValid>::getPossibleActions() const
{
    if (!correctPossibleActions)
    {
        getPossibleActions(possibleActions);
        correctPossibleActions = true;
    }

    return possibleActions;
}

template <bool CheckValid>
void GameStateT<CheckValid>::getPossibleActions(std::vector<Action>& possible) const
{
    possibleActionsUnchecked(possible);

    if constexpr (CheckValid)
    {
        if (possible.empty())
            throw GameException("No possible actions.", {{"expectedType", getExpectedAction().type}});
    }
}

template <bool CheckValid>
int GameStateT<CheckValid>::getCoins(int player) const
{
    return playerStates[player].coins;
}

template <bool CheckValid>
int GameStateT<CheckValid>::getScore(int player, int onlyType) const
{
    if constexpr (CheckValid) verifyPlayer(player);

    return playerStates[player].getScore(onlyType);
}

template <bool CheckValid>
int GameStateT<CheckValid>::getDistinctSciences(int player) const
{
    if constexpr (CheckValid) verifyPlayer(player);

    return playerStates[player].distincSciences;
}

template <bool CheckValid>
int GameStateT<CheckValid>::getMilitary(int player) const
{
    if constexpr (CheckValid) verifyPlayer(player);

    return playerStates[player].military;
}

template <bool CheckValid>
int GameStateT<CheckValid>::getMilitaryLead(int player) const
{
    if constexpr (CheckValid) verifyPlayer(player);

    return playerStates[player].militaryLead();
}

template <bool CheckValid>
int GameStateT<CheckValid>::getWondersBuilt() const
{
    return wondersBuilt;
}

template <bool CheckValid>
int GameStateT<CheckValid>::getDeckSize(int deck) const
{
    return deckEnds[deck] - deckStarts[deck];
}

template <bool CheckValid>
bool GameStateT<CheckValid>::isDeckEmpty(int deck) const
{
    return getDeckSize(deck) <= 0;
}

template <bool CheckValid>
const PyramidSlot& GameStateT<CheckValid>::getPyramidSlot(int pos) const
{
    if constexpr (CheckValid) verifyPos(pos, DECK_PYRAMID);

    return cardPyramid[pos];
}

template <bool CheckValid>
const PlayerState& GameStateT<CheckValid>::getPlayerState(int player) const
{
    if constexpr (CheckValid) verifyPlayer(player);

    return playerStates[player];
}

template <bool CheckValid>
int GameStateT<CheckValid>::getDeckElem(int deck, int pos) const
{
    pos += deckStarts[deck];

    if constexpr (CheckValid) verifyPos(pos, deck);

    return deckObjects[pos];
}

template <bool CheckValid>
int GameStateT<CheckValid>::getObjectDeck(int id) const
{
    if constexpr (CheckValid) verifyObject(id);

    return objectLocations[id].deck;
}

template <bool CheckValid>
int GameStateT<CheckValid>::getObjectPos(int id) const
{
    if constexpr (CheckValid) verifyObject(id);

    return objectLocations[id].pos;
}

template <bool CheckValid>
bool GameStateT<CheckValid>::isPlayableCard(int id) const
{
    if constexpr (CheckValid) verifyObject(id);

    const ObjectLocation& loc = objectLocations[id];

    if (loc.deck != DECK_PYRAMID) return false;
    if (cardPyramid[loc.pos].coveredBy > 0) return false;
    return true;
}
