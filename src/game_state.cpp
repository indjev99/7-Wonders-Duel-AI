#include "game_state.h"

#include "constants.h"
#include "exceptions.h"
#include "objects.h"
#include "object_decks.h"
#include "object_location.h"
#include "player_state.h"
#include "pyramid_slot.h"

#include <algorithm>

void GameState::drawObject(int id, int deck)
{
    if (objectLocations[id].deck != deck) throw EXC_INCORRECT_DECK;
    if (deckEnds[deck] <= deckStarts[deck]) throw EXC_DECK_EMPTY;

    int pos = objectLocations[id].pos;

    if (deckObjects[pos] != id) throw EXC_NOT_IN_DECK_POS;

    int otherPos = deckEnds[deck] - 1;
    int otherId = deckObjects[otherPos];

    deckObjects[pos] = id;
    objectLocations[otherId].pos = pos;

    deckObjects[otherPos] = OBJ_NONE;
    objectLocations[otherId] = ObjectLocation();

    deckEnds[deck]--;
}

void GameState::insertObject(int id, int deck)
{
    if (deckEnds[deck] >= deckStarts[deck + 1]) throw EXC_DECK_FULL;
    if (objectLocations[id].deck != DECK_NONE) throw EXC_OBJ_ALREADY_IN_DECK;
    if (deckObjects[id] != OBJ_NONE) throw EXC_OBJ_ALREADY_IN_DECK;

    deckObjects[deckEnds[deck]] = id;
    objectLocations[id] = ObjectLocation(deck, deckEnds[deck]);

    deckEnds[deck]++;
}

void GameState::revealPyramidCard(int pos, int id)
{
    PyramidSlot& slot = cardPyramid[pos];

    if (slot.objectId != SLOT_UNREVEALED) throw EXC_INVALID_REVEAL;

    drawObject(id, slot.deck);

    slot.objectId = id;
    objectLocations[id] = ObjectLocation(DECK_CARD_PYRAMID, pos);
}

void GameState::revealGuild(int pos)
{
    if (cardPyramid[pos].deck == DECK_GUILDS) throw EXC_ALREADY_A_GUILD;
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
    const ObjectLocation& loc = objectLocations[id];
    if (loc.deck != DECK_CARD_PYRAMID || cardPyramid[loc.pos].coveredBy > 0) throw EXC_OBJECT_NOT_AVALIABLE;
    cardPyramid[loc.pos].objectId = SLOT_EMPTY;
    for (int pos : pyramidSchemes[currAge][loc.pos].covering)
    {
        cardPyramid[pos].coveredBy--;
        if (cardPyramid[pos].coveredBy == 0) expectedActions.push(Action(ACT_REVEAL_PYRAMID_CARD, pos));
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

void GameState::buildWonderWithPyramidCard(int id, int cardId)
{
    if (wondersBuilt >= MAX_WONDERS_BUILT) throw EXC_MAX_WONDERS_BUILT;

    playPyramidCard(cardId);
    objectLocations[cardId] = ObjectLocation(DECK_USED, POS_NONE);

    buildDeckObject(id, DECK_SELECTED_WONDERS + currPlayer);

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
    if (expectedActions.empty()) throw EXC_GAME_ENDED;

    const Action& expected = expectedActions.front();

    bool typeMatch =
        (expected.type != ACT_MOVE_PYRAMID_CARD && expected.type == action.type) ||
        (expected.type == ACT_MOVE_PYRAMID_CARD && (
            action.type == ACT_MOVE_BUILD_PYRAMID_CARD ||
            action.type == ACT_MOVE_DISCARD_PYRAMID_CARD ||
            action.type == ACT_MOVE_BUILD_WONDER_WITH_PYRAMID_CARD));

    if (!typeMatch || (expected.type == ACT_REVEAL_PYRAMID_CARD && expected.arg1 != action.arg1))
    {
        if (expected.type >= 0) throw EXC_UNEXPECTED_MOVE;
        else throw EXC_UNEXPECTED_REVEAL;
    }

    switch (action.type)
    {
    case ACT_MOVE_BUILD_PYRAMID_CARD:
        buildPyramidCard(action.arg1);
        break;

    case ACT_MOVE_DISCARD_PYRAMID_CARD:
        discardPyramidCard(action.arg1);
        break;

    case ACT_MOVE_BUILD_WONDER_WITH_PYRAMID_CARD:
        buildWonderWithPyramidCard(action.arg1, action.arg2);
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
    }
}

void GameState::advanceAge()
{
    currAge++;

    if (currAge == NUM_AGES - 1)
    {
        for (int i = 0; i < NUM_LAST_AGE_GUILDS; ++i)
        {
            expectedActions.push(Action(ACT_REVEAL_GUILD));
        }
    }

    for (int pos = 0; pos < PYRAMID_SIZE; ++pos)
    {
        cardPyramid[pos] = PyramidSlot(currAge, SLOT_UNREVEALED, 0);
        if (pyramidSchemes[currAge][pos].revealed) expectedActions.push(Action(ACT_REVEAL_PYRAMID_CARD, pos));
        for (int other : pyramidSchemes[currAge][pos].covering)
        {
            cardPyramid[other].coveredBy++;
        }
    }
}

GameState::GameState()
{
    currPlayer = 0;
    currAge = WONDER_SELECTION_AGE;
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

    for (int i = 0; i < NUM_GAME_TOKENS; ++i)
    {
        expectedActions.push(Action(ACT_REVEAL_GAME_TOKEN));
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
    deckStarts[DECK_SELECTED_WONDERS_0] = deckStarts[DECK_REVEALED_WONDERS] + NUM_WONDERS_REVEALED;
    deckStarts[DECK_SELECTED_WONDERS_1] = deckStarts[DECK_SELECTED_WONDERS_0] + NUM_WONDERS_PER_PLAYER;
    deckStarts[DECK_DISCARDED] = deckStarts[DECK_SELECTED_WONDERS_1] + NUM_WONDERS_PER_PLAYER;
    deckStarts[NUM_DECKS] = deckStarts[DECK_DISCARDED] + 3 * PYRAMID_SIZE;

    return deckStarts;
}

const std::array<int, NUM_DECKS + 1> GameState::deckStarts = findDeckStarts();
