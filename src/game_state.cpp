#include "game_state.h"

#include "constants.h"
#include "game_exception.h"
#include "objects.h"
#include "object_decks.h"
#include "object_location.h"
#include "player_state.h"
#include "pyramid_slot.h"

#include <algorithm>

#include <iostream>

void GameState::verifyPos(int pos, int deck)
{
    int start = deck == DECK_CARD_PYRAMID ? 0 : deckStarts[deck];
    int end = deck == DECK_CARD_PYRAMID ? PYRAMID_SIZE : deckEnds[deck];

    if (pos < start || pos >= end)
        throw GameException("Position not in deck.", {{"pos", pos}, {"deck", deck}, {"deckStart", start}, {"deckEnd", end}});
}

void GameState::verifyObj(int id)
{
    if (id < 0 || id >= NUM_OBJECTS)
        throw GameException("Invalid object id.", {{"objectId", id}});
}

void GameState::drawObject(int id, int deck)
{
    verifyObj(id);

    if (objectLocations[id].deck != deck)
        throw GameException("Object not in correct deck.", {{"objectId", id}, {"objectDeck", objectLocations[id].deck}, {"deck", deck}});

    if (deckEnds[deck] <= deckStarts[deck])
        throw GameException("Drawing from empty deck.", {{"deck", deck}, {"deckStart", deckStarts[deck]}, {"deckEnd", deckEnds[deck]}});

    int pos = objectLocations[id].pos;

    verifyPos(pos, deck);

    if (deckObjects[pos] != id)
        throw GameException("Object not in its position.", {{"objectId", id}, {"objectPos", pos}, {"objectInObjectPos", deckObjects[pos]}});

    int otherPos = deckEnds[deck] - 1;
    int otherId = deckObjects[otherPos];

    deckObjects[pos] = otherId;
    objectLocations[otherId].pos = pos;

    deckObjects[otherPos] = OBJ_NONE;
    objectLocations[id] = ObjectLocation();

    deckEnds[deck]--;
}

void GameState::insertObject(int id, int deck)
{
    verifyObj(id);

    if (deckEnds[deck] >= deckStarts[deck + 1])
        throw GameException("Inserting into full deck.", {{"deck", deck}, {"deckEnd", deckEnds[deck]}, {"maxDeckEnd", deckStarts[deck + 1]}});

    if (objectLocations[id].deck != DECK_NONE)
        throw GameException("Object already in a deck.", {{"objectId", id}, {"objectDeck", objectLocations[id].deck}});

    if (deckObjects[deckEnds[deck]] != OBJ_NONE)
        throw GameException("Deck end already has object.", {{"deck", deck}, {"deckEnd", deckEnds[deck]}, {"deckEndObject", deckObjects[deckEnds[deck]]}});

    deckObjects[deckEnds[deck]] = id;
    objectLocations[id] = ObjectLocation(deck, deckEnds[deck]);

    deckEnds[deck]++;
}

void GameState::revealPyramidCard(int pos, int id)
{
    verifyObj(id);
    verifyPos(pos, DECK_CARD_PYRAMID);

    PyramidSlot& slot = cardPyramid[pos];

    if (slot.objectId != SLOT_UNREVEALED)
        throw GameException("Pyramid slot not unrevealed.", {{"pos", pos}, {"posObjectId", slot.objectId}});

    drawObject(id, slot.deck);

    slot.objectId = id;
    objectLocations[id] = ObjectLocation(DECK_CARD_PYRAMID, pos);
}

void GameState::revealGuild(int pos)
{
    verifyPos(pos, DECK_CARD_PYRAMID);

    if (cardPyramid[pos].deck == DECK_GUILDS)
        throw GameException("Pyramid slot already has guild.", {{"pos", pos}});

    cardPyramid[pos].deck = DECK_GUILDS;
}

void GameState::revealGameToken(int id)
{
    drawObject(id, DECK_TOKENS);
    insertObject(id, DECK_GAME_TOKENS);
}

void GameState::revealBoxToken(int id)
{
    drawObject(id, DECK_TOKENS);
    insertObject(id, DECK_BOX_TOKENS);
}

void GameState::revealWonder(int id)
{
    drawObject(id, DECK_WONDERS);
    insertObject(id, DECK_REVEALED_WONDERS);
}

void GameState::buildDeckObject(int id, int deck)
{
    drawObject(id, deck);
    playerStates[currPlayer].payForAndBuildObject(objects[id]);
    objectLocations[id] = ObjectLocation(DECK_USED, POS_NONE);
}

void GameState::playPyramidCard(int id)
{
    verifyObj(id);

    const ObjectLocation& loc = objectLocations[id];

    verifyPos(loc.pos, DECK_CARD_PYRAMID);

    if (loc.deck != DECK_CARD_PYRAMID)
        throw GameException("Object not in pyramid.", {{"objectId", id}, {"objectDeck", loc.deck}});

    if (cardPyramid[loc.pos].coveredBy > 0)
        throw GameException("Pyramid slot is covered.", {{"pos", loc.pos}, {"coveredBy", cardPyramid[loc.pos].coveredBy}});

    cardPyramid[loc.pos].objectId = OBJ_NONE;
    for (int pos : pyramidSchemes[currAge][loc.pos].covering)
    {
        cardPyramid[pos].coveredBy--;
        if (cardPyramid[pos].coveredBy == 0) queueAction(Action(ACT_REVEAL_PYRAMID_CARD, pos));
    }
    objectLocations[id] = ObjectLocation();
}

void GameState::buildPyramidCard(int id)
{
    playPyramidCard(id);
    playerStates[currPlayer].payForAndBuildObject(objects[id]);
    objectLocations[id] = ObjectLocation(DECK_USED, POS_NONE);
}

void GameState::discardPyramidCard(int id)
{
    playPyramidCard(id);
    insertObject(id, DECK_DISCARDED);
}

void GameState::playForWonder(int id, int wonderId)
{
    if (wondersBuilt >= MAX_WONDERS_BUILT)
        throw GameException("Max wonders built already.", {});

    playPyramidCard(id);
    objectLocations[id] = ObjectLocation(DECK_USED, POS_NONE);
    buildDeckObject(wonderId, DECK_SELECTED_WONDERS + currPlayer);

    wondersBuilt++;
}

void GameState::buildGameToken(int id)
{
    buildDeckObject(id, DECK_GAME_TOKENS);
}

void GameState::buildBoxToken(int id)
{
    buildDeckObject(id, DECK_BOX_TOKENS);
}

void GameState::buildDiscarded(int id)
{
    buildDeckObject(id, DECK_DISCARDED);
}

void GameState::selectWonder(int id)
{
    drawObject(id, DECK_REVEALED_WONDERS);
    insertObject(id, DECK_SELECTED_WONDERS + currPlayer);
}

void GameState::doAction(const Action& action)
{
    if (queuedActions.empty())
        throw GameException("Game already ended.", {});

    const Action& queued = queuedActions.front();

    if (action.type != queued.type || (queued.type == ACT_REVEAL_PYRAMID_CARD && action.arg1 != queued.arg1))
    {
        if (queued.type == ACT_REVEAL_PYRAMID_CARD && queued.arg1 != action.arg1)
            throw GameException("Unexpected pyramid reveal position.", {{"actionPos", action.arg1}, {"queuedPos", queued.arg1}});
        else
            throw GameException("Unexpected action.", {{"actionType", action.type}, {"queuedType", queued.type}});
    }

    queuedActions.pop();

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
            playForWonder(action.arg1, action.arg2);
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

    case ACT_MOVE_SELECT_WONDER:
        selectWonder(action.arg1);
        --cardsRemaining;
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

    default:
        throw GameException("Unknown action type.", {{"actionType", action.type}});
    }

    if (action.isPlayerMove() && haveNextActions)
            throw GameException("Actions were queued after player move.", {{"actionType", action.type}});

    if (!action.isPlayerMove() && !haveNextActions)
            throw GameException("No actions were queued after game reveal.", {{"actionType", action.type}});

    if (!action.isPlayerMove()) return;

    if (currAge == AGE_WONDER_SELECTION)
    {
        if (cardsRemaining == 0)
        {
            advanceAge();
            return;
        }

        if (deckEnds[DECK_REVEALED_WONDERS] == deckStarts[DECK_REVEALED_WONDERS])
            queueAction(Action(ACT_REVEAL_WONDER), NUM_WONDERS_REVEALED);
        queueAction(Action(ACT_MOVE_SELECT_WONDER));

        currPlayer = (currPlayer + 1) % NUM_PLAYERS; // TODO

        return;
    }

    PlayerState& state = playerStates[currPlayer];

    if (state.shouldBuildGameToken)
    {
        state.shouldBuildGameToken = false;
        queueAction(Action(ACT_MOVE_BUILD_GAME_TOKEN));
        return;
    }

    if (state.shouldBuildBoxToken)
    {
        state.shouldBuildBoxToken = false;
        queueAction(Action(ACT_REVEAL_BOX_TOKEN), NUM_BOX_TOKENS);
        queueAction(Action(ACT_MOVE_BUILD_BOX_TOKEN));
        return;
    }

    if (state.shouldDestroyBrown)
    {
        state.shouldDestroyBrown = false;
        // TODO
    }

    if (state.shouldDestroyGray)
    {
        state.shouldDestroyGray = false;
        // TODO
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
        advanceAge();
        return;
    }

    queueAction(Action(ACT_MOVE_PLAY_PYRAMID_CARD));
    currPlayer = (currPlayer + 1) % NUM_PLAYERS;
}

void GameState::advanceAge()
{
    currAge++;

    if (currAge == NUM_AGES - 1) queueAction(Action(ACT_REVEAL_GUILD), NUM_LAST_AGE_GUILDS);

    for (int pos = 0; pos < PYRAMID_SIZE; ++pos)
    {
        cardPyramid[pos] = PyramidSlot(currAge, SLOT_UNREVEALED, 0);
        if (pyramidSchemes[currAge][pos].revealed) queueAction(Action(ACT_REVEAL_PYRAMID_CARD, pos));
        for (int other : pyramidSchemes[currAge][pos].covering)
        {
            cardPyramid[other].coveredBy++;
        }
    }

    queueAction(Action(ACT_MOVE_PLAY_PYRAMID_CARD)); // TODO
}

void GameState::setupWonderSelection()
{
    currPlayer = 0;
    currAge = AGE_WONDER_SELECTION;
    cardsRemaining = NUM_PLAYERS * NUM_WONDERS_PER_PLAYER;

    queueAction(Action(ACT_REVEAL_WONDER), NUM_WONDERS_REVEALED);
    queueAction(Action(ACT_MOVE_SELECT_WONDER));
}

void GameState::queueAction(const Action& action, int count)
{
    for (int i = 0; i < count; ++i)
    {
        queuedActions.push(action);
    }
}

GameState::GameState()
{
    playerStates[0].otherPlayer = &playerStates[1];
    playerStates[1].otherPlayer = &playerStates[0];

    wondersBuilt = 0;

    for (int i = 0; i < NUM_DECKS; ++i)
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

    queueAction(Action(ACT_REVEAL_GAME_TOKEN), NUM_GAME_TOKENS);

    setupWonderSelection();
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
    deckStarts[NUM_DECKS] = deckStarts[DECK_DISCARDED] + 3 * PYRAMID_SIZE;

    return deckStarts;
}

const std::array<int, NUM_DECKS + 1> GameState::deckStarts = findDeckStarts();
