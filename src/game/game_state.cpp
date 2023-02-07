#include "game_state.h"

#include "constants.h"
#include "game_exception.h"
#include "objects.h"
#include "object_decks.h"
#include "object_location.h"
#include "player_state.h"
#include "pyramid_slot.h"
#include "results.h"

#include <algorithm>

#define NO_PLAYER -100

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
int GameStateT<CheckValid>::otherPlayer() const
{
    return getOpponent(currPlayer);
}

template <bool CheckValid>
void GameStateT<CheckValid>::advancePlayer()
{
    currPlayer = otherPlayer();
}

template <bool CheckValid>
void GameStateT<CheckValid>::preQueueAction(const Action& action, int count)
{
    for (int i = 0; i < count; i++)
    {
        queuedActions.push_front(action);
    }
}

template <bool CheckValid>
void GameStateT<CheckValid>::queueAction(const Action& action, int count)
{
    for (int i = 0; i < count; i++)
    {
        queuedActions.push_back(action);
    }
}

template <bool CheckValid>
void GameStateT<CheckValid>::drawObject(int id, int deck)
{
    if constexpr (CheckValid)
    {
        verifyObject(id);

        if (objectLocations[id].deck != deck)
            throw GameException("Object not in correct deck.", {{"objectId", id}, {"objectDeck", objectLocations[id].deck}, {"deck", deck}});

        if (isDeckEmpty(deck))
            throw GameException("Drawing from empty deck.", {{"deck", deck}, {"deckStart", deckStarts[deck]}, {"deckEnd", deckEnds[deck]}});        
    }

    int pos = objectLocations[id].pos;

    if constexpr (CheckValid)
    {
        verifyPos(pos, deck);

        if (deckObjects[pos] != id)
            throw GameException("Object not in its position.", {{"objectId", id}, {"objectPos", pos}, {"objectInObjectPos", deckObjects[pos]}});
    }

    int otherPos = deckEnds[deck] - 1;
    int otherId = deckObjects[otherPos];

    deckObjects[pos] = otherId;
    objectLocations[otherId].pos = pos;

    deckObjects[otherPos] = OBJ_NONE;
    objectLocations[id] = ObjectLocation();

    deckEnds[deck]--;
}

template <bool CheckValid>
void GameStateT<CheckValid>::insertObject(int id, int deck)
{
    if constexpr (CheckValid)
    {
        verifyObject(id);

        if (deckEnds[deck] >= deckStarts[deck + 1])
            throw GameException("Inserting into full deck.", {{"deck", deck}, {"deckEnd", deckEnds[deck]}, {"maxDeckEnd", deckStarts[deck + 1]}});

        if (objectLocations[id].deck != DECK_NONE)
            throw GameException("Object already in a deck.", {{"objectId", id}, {"objectDeck", objectLocations[id].deck}});

        if (deckObjects[deckEnds[deck]] != OBJ_NONE)
            throw GameException("Deck end already has object.", {{"deck", deck}, {"deckEnd", deckEnds[deck]}, {"deckEndObject", deckObjects[deckEnds[deck]]}});
    }

    deckObjects[deckEnds[deck]] = id;
    objectLocations[id] = ObjectLocation(deck, deckEnds[deck]);

    deckEnds[deck]++;
}

template <bool CheckValid>
void GameStateT<CheckValid>::revealGuild(int pos)
{
    if constexpr (CheckValid)
    {
        verifyPos(pos, DECK_PYRAMID);

        if (cardPyramid[pos].deck == DECK_GUILDS)
            throw GameException("Pyramid slot already has guild.", {{"pos", pos}});
    }

    cardPyramid[pos].deck = DECK_GUILDS;
}

template <bool CheckValid>
void GameStateT<CheckValid>::revealPyramidCard(int id, int pos)
{
    if constexpr (CheckValid)
    {
        verifyObject(id);
        verifyPos(pos, DECK_PYRAMID);
    }

    PyramidSlot& slot = cardPyramid[pos];

    if constexpr (CheckValid)
    {
        if (slot.objectId != SLOT_UNREVEALED)
            throw GameException("Pyramid slot not unrevealed.", {{"pos", pos}, {"posObjectId", slot.objectId}});
    }

    drawObject(id, slot.deck);

    slot.objectId = id;
    objectLocations[id] = ObjectLocation(DECK_PYRAMID, pos);

    if (slot.coveredBy == 0) addPlayablePyramidCard(pos);
}

template <bool CheckValid>
void GameStateT<CheckValid>::revealGameToken(int id)
{
    drawObject(id, DECK_TOKENS);
    insertObject(id, DECK_GAME_TOKENS);
}

template <bool CheckValid>
void GameStateT<CheckValid>::revealBoxToken(int id)
{
    drawObject(id, DECK_TOKENS);
    insertObject(id, DECK_BOX_TOKENS);
}

template <bool CheckValid>
void GameStateT<CheckValid>::revealWonder(int id)
{
    drawObject(id, DECK_WONDERS);
    insertObject(id, DECK_REVEALED_WONDERS);
}

template <bool CheckValid>
void GameStateT<CheckValid>::revealFirstPlayer(int player)
{
    if constexpr (CheckValid) verifyPlayer(player);

    firstPlayer = player;
}

template <bool CheckValid>
void GameStateT<CheckValid>::buildDeckObject(int id, int deck)
{
    drawObject(id, deck);
    playerStates[currPlayer].payForAndBuildObject(objects[id]);
    objectLocations[id] = ObjectLocation(DECK_USED, POS_NONE);
}

template <bool CheckValid>
void GameStateT<CheckValid>::addPlayablePyramidCard(int pos)
{
    if constexpr (CheckValid) verifyPos(pos, DECK_PYRAMID);

    int playDeck = DECK_PYRAMID_PLAYABLE;

    int id = cardPyramid[pos].objectId;

    if constexpr (CheckValid)
    {
        verifyObject(id);

        if (deckEnds[playDeck] >= deckStarts[playDeck + 1])
            throw GameException("Inserting into full deck.", {{"deck", playDeck}, {"deckEnd", deckEnds[playDeck]}, {"maxDeckEnd", deckStarts[playDeck + 1]}});

        if (deckObjects[deckEnds[playDeck]] != OBJ_NONE)
            throw GameException("Deck end already has object.", {{"deck", playDeck}, {"deckEnd", deckEnds[playDeck]}, {"deckEndObject", deckObjects[deckEnds[playDeck]]}});
    }

    deckObjects[deckEnds[playDeck]] = id;
    cardPyramid[pos].playablePos = deckEnds[playDeck];

    deckEnds[playDeck]++;
}

template <bool CheckValid>
void GameStateT<CheckValid>::remPlayablePyramidCard(int pos)
{
    if constexpr (CheckValid) verifyPos(pos, DECK_PYRAMID);

    int playPos = cardPyramid[pos].playablePos;
    int playDeck = DECK_PYRAMID_PLAYABLE;

    if constexpr (CheckValid)
    {
        verifyPos(playPos, playDeck);

        int id = cardPyramid[pos].objectId;

        verifyObject(id);

        if (deckObjects[playPos] != id)
            throw GameException("Object not in its position.", {{"objectId", id}, {"objectPos", playPos}, {"objectInObjectPos", deckObjects[playPos]}});
    }

    int otherPlayPos = deckEnds[playDeck] - 1;
    int otherId = deckObjects[otherPlayPos];

    deckObjects[playPos] = otherId;
    cardPyramid[objectLocations[otherId].pos].playablePos = playPos;

    deckObjects[otherPlayPos] = OBJ_NONE;
    cardPyramid[pos].playablePos = POS_NONE;

    deckEnds[playDeck]--;
}

template <bool CheckValid>
void GameStateT<CheckValid>::playPyramidCard(int id)
{
    if constexpr (CheckValid) verifyObject(id);

    const ObjectLocation& loc = objectLocations[id];

    if constexpr (CheckValid)
    {
        verifyPos(loc.pos, DECK_PYRAMID);

        if (loc.deck != DECK_PYRAMID)
            throw GameException("Object not in pyramid.", {{"objectId", id}, {"objectDeck", loc.deck}});

        if (cardPyramid[loc.pos].coveredBy > 0)
            throw GameException("Pyramid slot is covered.", {{"pos", loc.pos}, {"coveredBy", cardPyramid[loc.pos].coveredBy}});
    }

    remPlayablePyramidCard(loc.pos);

    for (int pos : pyramidSchemes[currAge][loc.pos].covering)
    {
        PyramidSlot& other = cardPyramid[pos];
        other.coveredBy--;
        if (other.coveredBy == 0)
        {
            if (other.objectId == SLOT_UNREVEALED) queueAction(Action(ACT_REVEAL_PYRAMID_CARD, ACT_ARG_NONE, pos));
            else addPlayablePyramidCard(pos);
        }
    }

    cardPyramid[loc.pos].objectId = OBJ_NONE;
    objectLocations[id] = ObjectLocation();
}

template <bool CheckValid>
void GameStateT<CheckValid>::buildPyramidCard(int id)
{
    playPyramidCard(id);
    playerStates[currPlayer].payForAndBuildObject(objects[id]);
    objectLocations[id] = ObjectLocation(DECK_USED, POS_NONE);
}

template <bool CheckValid>
void GameStateT<CheckValid>::discardPyramidCard(int id)
{
    playPyramidCard(id);
    playerStates[currPlayer].discardCard();
    insertObject(id, DECK_DISCARDED);
}

template <bool CheckValid>
void GameStateT<CheckValid>::usePyramidCardForWonder(int id, int wonderId)
{
    if constexpr (CheckValid)
    {
        if (wondersBuilt >= MAX_WONDERS_BUILT)
            throw GameException("Max wonders built already.", {});
    }

    playPyramidCard(id);
    objectLocations[id] = ObjectLocation(DECK_USED, POS_NONE);
    buildDeckObject(wonderId, DECK_SELECTED_WONDERS + currPlayer);

    wondersBuilt++;
}

template <bool CheckValid>
void GameStateT<CheckValid>::buildGameToken(int id)
{
    buildDeckObject(id, DECK_GAME_TOKENS);
}

template <bool CheckValid>
void GameStateT<CheckValid>::buildBoxToken(int id)
{
    buildDeckObject(id, DECK_BOX_TOKENS);

    while (!isDeckEmpty(DECK_BOX_TOKENS))
    {
        int id2 = deckObjects[deckStarts[DECK_BOX_TOKENS]];
        drawObject(id2, DECK_BOX_TOKENS);
        insertObject(id2, DECK_TOKENS);
    }
}

template <bool CheckValid>
void GameStateT<CheckValid>::buildDiscarded(int id)
{
    drawObject(id, DECK_DISCARDED);
    playerStates[currPlayer].buildObject(objects[id]);
    objectLocations[id] = ObjectLocation(DECK_USED, POS_NONE);
}

template <bool CheckValid>
void GameStateT<CheckValid>::destroyObject(int id, int type)
{
    if constexpr (CheckValid)
    {
        verifyObject(id);

        if (objects[id].type != type)
            throw GameException("Object not of correct type.", {{"objectId", id}, {"objectType", type}, {"type", type}});
    }

    playerStates[otherPlayer()].destroyObject(objects[id]);

    objectLocations[id].deck = DECK_NONE;
    insertObject(id, DECK_DISCARDED);
}

template <bool CheckValid>
void GameStateT<CheckValid>::selectWonder(int id)
{
    drawObject(id, DECK_REVEALED_WONDERS);
    insertObject(id, DECK_SELECTED_WONDERS + currPlayer);
}

template <bool CheckValid>
void GameStateT<CheckValid>::chooseStartPlayer(int player)
{
    if constexpr (CheckValid) verifyPlayer(player);

    currPlayer = player;
}

template <bool CheckValid>
void GameStateT<CheckValid>::doAction(const Action& action)
{
    if constexpr (CheckValid)
    {
        if (isTerminal())
            throw GameException("Game already ended.", {});

        const Action& expected = getExpectedAction();

        if ((action.type != expected.type ||
            (action.type == ACT_REVEAL_PYRAMID_CARD && action.arg2 != expected.arg2) ||
            (action.type == ACT_MOVE_DESTROY_OBJECT && action.arg2 != expected.arg2)))
        {
            if (action.type == ACT_REVEAL_PYRAMID_CARD && action.arg2 != expected.arg2)
                throw GameException("Unexpected pyramid reveal position.", {{"actionPos", action.arg2}, {"expectedPos", expected.arg2}});
            if (action.type == ACT_MOVE_DESTROY_OBJECT && action.arg2 != expected.arg2)
                throw GameException("Unexpected object type.", {{"actionObjectType", action.arg2}, {"expectedObjectType", expected.arg2}});
            else
                throw GameException("Unexpected action.", {{"actionType", action.type}, {"expectedType", expected.type}});
        }
    }

    correctPossibleActions = false;

    queuedActions.pop_front();

    bool haveNextActions = !queuedActions.empty();

    switch (action.type)
    {
    case ACT_MOVE_PLAY_PYRAMID_CARD:
        switch (action.arg2)
        {
        case ACT_ARG2_BUILD:
            buildPyramidCard(action.arg1);
            break;

        case ACT_ARG2_DISCARD:
            discardPyramidCard(action.arg1);
            break;

        default:
            usePyramidCardForWonder(action.arg1, action.arg2);
            break;
        }
        --cardsRemaining;
        break;

    case ACT_MOVE_BUILD_GAME_TOKEN:
        buildGameToken(action.arg1);
        break;

    case ACT_MOVE_BUILD_BOX_TOKEN:
        buildBoxToken(action.arg1);
        break;

    case ACT_MOVE_BUILD_DISCARDED:
        buildDiscarded(action.arg1);
        break;

    case ACT_MOVE_DESTROY_OBJECT:
        destroyObject(action.arg1, action.arg2);
        break;

    case ACT_MOVE_SELECT_WONDER:
        selectWonder(action.arg1);
        --cardsRemaining;
        break;

    case ACT_MOVE_CHOOSE_START_PLAYER:
        chooseStartPlayer(action.arg1);
        break;

    case ACT_REVEAL_GUILD:
        revealGuild(action.arg1);
        break;

    case ACT_REVEAL_PYRAMID_CARD:
        revealPyramidCard(action.arg1, action.arg2);
        break;

    case ACT_REVEAL_GAME_TOKEN:
        revealGameToken(action.arg1);
        break;

    case ACT_REVEAL_BOX_TOKEN:
        revealBoxToken(action.arg1);
        break;

    case ACT_REVEAL_WONDER:
        revealWonder(action.arg1);
        break;

    case ACT_REVEAL_FIRST_PLAYER:
        revealFirstPlayer(action.arg1);
        break;

    default:
        if constexpr (CheckValid)
            throw GameException("Unknown action type.", {{"actionType", action.type}});
    }

    if (currAge == AGE_SETUP && queuedActions.empty())
    {
        startWonderSelection();
        return;
    }

    if constexpr (CheckValid)
    {
        if (!action.isPlayerMove() && !haveNextActions)
                throw GameException("No actions were queued after game reveal.", {{"actionType", action.type}});
    }

    if (!action.isPlayerMove()) return;

    if (currAge == AGE_WONDER_SELECTION)
    {
        if (cardsRemaining == 0)
        {
            advanceAge();
            return;
        }

        if (isDeckEmpty(DECK_REVEALED_WONDERS))
            queueAction(Action(ACT_REVEAL_WONDER), NUM_WONDERS_REVEALED);
    
        if (getDeckSize(DECK_REVEALED_WONDERS) != NUM_WONDERS_REVEALED / 2) advancePlayer();
        queueAction(Action(ACT_MOVE_SELECT_WONDER));

        return;
    }

    PlayerState& state = playerStates[currPlayer];

    if (state.getResult(false) != RESULT_DRAW)
    {
        queuedActions = std::deque<Action>();
        return;
    }

    if (state.shouldBuildGameToken)
    {
        state.shouldBuildGameToken = false;
        if (!isDeckEmpty(DECK_GAME_TOKENS))
        {
            preQueueAction(Action(ACT_MOVE_BUILD_GAME_TOKEN));
            return;
        }
    }

    if (state.shouldBuildBoxToken)
    {
        state.shouldBuildBoxToken = false;
        if (!isDeckEmpty(DECK_TOKENS))
        {
            preQueueAction(Action(ACT_MOVE_BUILD_BOX_TOKEN));
            preQueueAction(Action(ACT_REVEAL_BOX_TOKEN), std::min(NUM_BOX_TOKENS, getDeckSize(DECK_TOKENS)));
            return;
        }
    }

    if (state.shouldBuildDiscarded)
    {
        state.shouldBuildDiscarded = false;
        if (!isDeckEmpty(DECK_DISCARDED))
        {
            preQueueAction(Action(ACT_MOVE_BUILD_DISCARDED));
            return;
        }
    }

    if (state.shouldDestroyType != OT_NONE)
    {
        int type = state.shouldDestroyType;
        state.shouldDestroyType = OT_NONE;
        if (playerStates[otherPlayer()].typeCounts[type] > 0)
        {
            preQueueAction(Action(ACT_MOVE_DESTROY_OBJECT, OBJ_NONE, type));
            return;
        }
    }

    if (state.shouldPlayAgain)
    {
        state.shouldPlayAgain = false;
        if (cardsRemaining > 0)
        {
            queueAction(Action(ACT_MOVE_PLAY_PYRAMID_CARD));
            return;
        }
    }

    if (cardsRemaining == 0)
    {
        if constexpr (CheckValid)
        {
            if (!isTerminal())
                throw GameException("No cards remaining but actions queued.", {{"queuedType", getExpectedAction().type}});
        }

        if (currAge < NUM_AGES - 1) advanceAge();
        return;
    }

    if (action.type != ACT_MOVE_CHOOSE_START_PLAYER) advancePlayer();
    queueAction(Action(ACT_MOVE_PLAY_PYRAMID_CARD));
}

template <bool CheckValid>
void GameStateT<CheckValid>::startWonderSelection()
{
    currPlayer = firstPlayer;
    currAge = AGE_WONDER_SELECTION;
    cardsRemaining = NUM_PLAYERS * NUM_WONDERS_PER_PLAYER;

    queueAction(Action(ACT_REVEAL_WONDER), NUM_WONDERS_REVEALED);
    queueAction(Action(ACT_MOVE_SELECT_WONDER));
}

template <bool CheckValid>
void GameStateT<CheckValid>::advanceAge()
{
    currAge++;
    cardsRemaining = PYRAMID_SIZE;

    if (currAge == NUM_AGES - 1) queueAction(Action(ACT_REVEAL_GUILD), NUM_LAST_AGE_GUILDS);

    for (int pos = 0; pos < PYRAMID_SIZE; pos++)
    {
        cardPyramid[pos] = PyramidSlot(currAge, SLOT_UNREVEALED, 0);
        if (pyramidSchemes[currAge][pos].revealed) queueAction(Action(ACT_REVEAL_PYRAMID_CARD, ACT_ARG_NONE, pos));
        for (int other : pyramidSchemes[currAge][pos].covering)
        {
            cardPyramid[other].coveredBy++;
        }
    }

    if (currAge == 0)
    {
        currPlayer = firstPlayer;
        queueAction(Action(ACT_MOVE_PLAY_PYRAMID_CARD));
    }
    else
    {
        if (playerStates[currPlayer].militaryLead() > 0) advancePlayer();
        queueAction(Action(ACT_MOVE_CHOOSE_START_PLAYER));
    }
}

template <bool CheckValid>
GameStateT<CheckValid>::GameStateT()
{
    linkPlayers();

    firstPlayer = NO_PLAYER;
    currAge = AGE_SETUP;
    currPlayer = NO_PLAYER;
    wondersBuilt = 0;
    correctPossibleActions = false;

    for (int i = 0; i < NUM_DECKS; i++)
    {
        deckEnds[i] = deckStarts[i];
    }

    std::fill(deckObjects.begin(), deckObjects.end(), OBJ_NONE);

    for (int id : age1Cards) insertObject(id, DECK_AGE_1);
    for (int id : age2Cards) insertObject(id, DECK_AGE_2);
    for (int id : age3Cards) insertObject(id, DECK_AGE_3);
    for (int id : guildCards) insertObject(id, DECK_GUILDS);
    for (int id : tokens) insertObject(id, DECK_TOKENS);
    for (int id : wonders) insertObject(id, DECK_WONDERS);

    queueAction(Action(ACT_REVEAL_FIRST_PLAYER));
    queueAction(Action(ACT_REVEAL_GAME_TOKEN), NUM_GAME_TOKENS);
}

template <bool CheckValid>
template <bool OtherCheckValid>
GameStateT<CheckValid>::GameStateT(const GameStateT<OtherCheckValid>* other)
{
    firstPlayer = other->firstPlayer;
    currPlayer = other->currPlayer;
    currAge = other->currAge;
    wondersBuilt = other->wondersBuilt;
    cardsRemaining = other->cardsRemaining;
    playerStates = other->playerStates;
    deckEnds = other->deckEnds;
    deckObjects = other->deckObjects;
    cardPyramid = other->cardPyramid;
    objectLocations = other->objectLocations;
    queuedActions = other->queuedActions;
    correctPossibleActions = false;

    linkPlayers();
}

template <bool CheckValid>
void GameStateT<CheckValid>::reset()
{
    new (this) GameStateT<CheckValid>();
}

template <bool CheckValid>
template <bool OtherCheckValid>
void GameStateT<CheckValid>::clone(const GameStateT<OtherCheckValid>* other)
{
    new (this) GameStateT<CheckValid>(other);
}

template <bool CheckValid>
void GameStateT<CheckValid>::linkPlayers()
{ 
    playerStates[0].otherPlayer = &playerStates[1];
    playerStates[1].otherPlayer = &playerStates[0];
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
        if (firstPlayer == NO_PLAYER)
            throw GameException("First player has not been set yet.", {});
    }

    return firstPlayer;
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
    GameStateT<CheckValid>& thisMut = const_cast<GameStateT<CheckValid>&>(*this);

    if (!thisMut.correctPossibleActions)
    {
        getPossibleActions(thisMut.possibleActions);
        thisMut.correctPossibleActions = true;
    }

    return thisMut.possibleActions;
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
bool GameStateT<CheckValid>::isPlayableCard(int id) const
{
    if constexpr (CheckValid) verifyObject(id);

    const ObjectLocation& loc = objectLocations[id];

    if (loc.deck != DECK_PYRAMID) return false;
    if (cardPyramid[loc.pos].coveredBy > 0) return false;
    return true;
}

template <bool CheckValid>
void GameStateT<CheckValid>::possibleFromDeckActions(std::vector<Action>& possible, const Action& expected, int deck) const
{
    possible.reserve(getDeckSize(deck));

    Action action = expected;

    for (int pos = deckStarts[deck]; pos < deckEnds[deck]; pos++)
    {
        action.arg1 = deckObjects[pos];
        possible.push_back(action);
    }
}

template <bool CheckValid>
void GameStateT<CheckValid>::possibleChooseStartPlayerActions(std::vector<Action>& possible) const
{
    possible.reserve(NUM_PLAYERS);

    for (int player = 0; player < NUM_PLAYERS; player++)
    {
        possible.push_back(Action(ACT_MOVE_CHOOSE_START_PLAYER, player));
    }
}

template <bool CheckValid>
void GameStateT<CheckValid>::possibleDestroyObjectActions(std::vector<Action>& possible, int type) const
{
    const PlayerState& other = playerStates[otherPlayer()];

    possible.reserve(other.typeCounts[type]);

    for (int id = objectTypeStarts[type]; id < objectTypeStarts[type + 1]; id++)
    {
        if (!other.objectsBuilt[id]) continue;
        possible.push_back(Action(ACT_MOVE_DESTROY_OBJECT, id, type));
    }
}

template <bool CheckValid>
void GameStateT<CheckValid>::possiblePlayPyramidCardActions(std::vector<Action>& possible) const
{
    std::vector<int>& possibleWonders = const_cast<std::vector<int>&>(this->possibleWonders);

    const PlayerState& state = playerStates[currPlayer];

    possibleWonders.clear();

    if (wondersBuilt < MAX_WONDERS_BUILT)
    {
        possibleWonders.reserve(getDeckSize(DECK_SELECTED_WONDERS + currPlayer));

        for (int pos = deckStarts[DECK_SELECTED_WONDERS + currPlayer]; pos < deckEnds[DECK_SELECTED_WONDERS + currPlayer]; pos++)
        {
            int id = deckObjects[pos];
            if (state.canPayFor(objects[id])) possibleWonders.push_back(id);
        }
    }

    possible.reserve((2 + possibleWonders.size()) *  cardsRemaining);

    for (int pos = deckStarts[DECK_PYRAMID_PLAYABLE]; pos < deckEnds[DECK_PYRAMID_PLAYABLE]; pos++)
    {
        int id = deckObjects[pos];

        possible.push_back(Action(ACT_MOVE_PLAY_PYRAMID_CARD, id, ACT_ARG2_DISCARD));

        if (state.canPayFor(objects[id])) possible.push_back(Action(ACT_MOVE_PLAY_PYRAMID_CARD, id, ACT_ARG2_BUILD));

        for (int wonderId : possibleWonders)
        {
            possible.push_back(Action(ACT_MOVE_PLAY_PYRAMID_CARD, id, wonderId));
        }
    }
}

template <bool CheckValid>
void GameStateT<CheckValid>::possibleRevealGuildActions(std::vector<Action>& possible) const
{
    possible.reserve(PYRAMID_SIZE);

    for (int pos = 0; pos < PYRAMID_SIZE; pos++)
    {
        if (cardPyramid[pos].deck != DECK_GUILDS) possible.push_back(Action(ACT_REVEAL_GUILD, pos));
    }
}

template <bool CheckValid>
void GameStateT<CheckValid>::possibleRevealFirstPlayerActions(std::vector<Action>& possible) const
{
    possible.reserve(NUM_PLAYERS);

    for (int player = 0; player < NUM_PLAYERS; player++)
    {
        possible.push_back(Action(ACT_REVEAL_FIRST_PLAYER, player));
    }
}

template <bool CheckValid>
void GameStateT<CheckValid>::possibleActionsUnchecked(std::vector<Action>& possible) const
{
    possible.clear();

    const Action& expected = getExpectedAction();

    switch (expected.type)
    {
    case ACT_MOVE_PLAY_PYRAMID_CARD:
        possiblePlayPyramidCardActions(possible);
        break;

    case ACT_MOVE_BUILD_GAME_TOKEN:
        possibleFromDeckActions(possible, expected, DECK_GAME_TOKENS);
        break;

    case ACT_MOVE_BUILD_BOX_TOKEN:
        possibleFromDeckActions(possible, expected, DECK_BOX_TOKENS);
        break;

    case ACT_MOVE_BUILD_DISCARDED:
        possibleFromDeckActions(possible, expected, DECK_DISCARDED);
        break;

    case ACT_MOVE_DESTROY_OBJECT:
        possibleDestroyObjectActions(possible, expected.arg2);
        break;

    case ACT_MOVE_SELECT_WONDER:
        possibleFromDeckActions(possible, expected, DECK_REVEALED_WONDERS);
        break;

    case ACT_MOVE_CHOOSE_START_PLAYER:
        possibleChooseStartPlayerActions(possible);
        break;

    case ACT_REVEAL_GUILD:
        possibleRevealGuildActions(possible);
        break;

    case ACT_REVEAL_PYRAMID_CARD:
        if constexpr (CheckValid) verifyPos(expected.arg2, DECK_PYRAMID);
        possibleFromDeckActions(possible, expected, cardPyramid[expected.arg2].deck);
        break;

    case ACT_REVEAL_GAME_TOKEN:
        possibleFromDeckActions(possible, expected, DECK_TOKENS);
        break;

    case ACT_REVEAL_BOX_TOKEN:
        possibleFromDeckActions(possible, expected, DECK_TOKENS);
        break;

    case ACT_REVEAL_WONDER:
        possibleFromDeckActions(possible, expected, DECK_WONDERS);
        break;

    case ACT_REVEAL_FIRST_PLAYER:
        possibleRevealFirstPlayerActions(possible);
        break;

    default:
        if constexpr (CheckValid)
            throw GameException("Unknown action type.", {{"actionType", expected.type}});
    }
}

std::array<int, NUM_DECKS + 1> findDeckStarts()
{
    std::array<int, NUM_DECKS + 1> deckStarts;

    deckStarts[DECK_AGE_1] = 0;
    deckStarts[DECK_AGE_2] = deckStarts[DECK_AGE_1] + NUM_AGE_1_CARDS;
    deckStarts[DECK_AGE_3] = deckStarts[DECK_AGE_2] + NUM_AGE_2_CARDS;
    deckStarts[DECK_GUILDS] = deckStarts[DECK_AGE_3] + NUM_AGE_3_CARDS;
    deckStarts[DECK_TOKENS] = deckStarts[DECK_GUILDS] + NUM_GUILD_CARDS;
    deckStarts[DECK_WONDERS] = deckStarts[DECK_TOKENS] + NUM_TOKENS;
    deckStarts[DECK_GAME_TOKENS] = deckStarts[DECK_WONDERS] + NUM_WONDERS;
    deckStarts[DECK_BOX_TOKENS] = deckStarts[DECK_GAME_TOKENS] + NUM_GAME_TOKENS;
    deckStarts[DECK_REVEALED_WONDERS] = deckStarts[DECK_BOX_TOKENS] + NUM_BOX_TOKENS;
    deckStarts[DECK_SELECTED_WONDERS + 0] = deckStarts[DECK_REVEALED_WONDERS] + NUM_WONDERS_REVEALED;
    deckStarts[DECK_SELECTED_WONDERS + 1] = deckStarts[DECK_SELECTED_WONDERS + 0] + NUM_WONDERS_PER_PLAYER;
    deckStarts[DECK_DISCARDED] = deckStarts[DECK_SELECTED_WONDERS + 1] + NUM_WONDERS_PER_PLAYER;
    deckStarts[DECK_PYRAMID_PLAYABLE] = deckStarts[DECK_DISCARDED] + MAX_DISCARDED;
    deckStarts[NUM_DECKS] = deckStarts[DECK_PYRAMID_PLAYABLE] + PYRAMID_SIZE;

    return deckStarts;
}

template <bool CheckValid>
const std::array<int, NUM_DECKS + 1> GameStateT<CheckValid>::deckStarts = findDeckStarts();

template struct GameStateT<true>;
template struct GameStateT<false>;

template GameStateT<true>::GameStateT(const GameStateT<true>*);
template GameStateT<true>::GameStateT(const GameStateT<false>*);
template GameStateT<false>::GameStateT(const GameStateT<true>*);
template GameStateT<false>::GameStateT(const GameStateT<false>*);

template void GameStateT<true>::clone(const GameStateT<true>*);
template void GameStateT<true>::clone(const GameStateT<false>*);
template void GameStateT<false>::clone(const GameStateT<true>*);
template void GameStateT<false>::clone(const GameStateT<false>*);
