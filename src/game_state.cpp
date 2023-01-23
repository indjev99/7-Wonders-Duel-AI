#include "game_state.h"

#include "constants.h"
#include "exceptions.h"
#include "objects.h"
#include "object_decks.h"
#include "object_location.h"
#include "player_state.h"
#include "pyramid_slot.h"

#include <algorithm>

#define NUM_DECK_LOCATIONS NUM_AGE_1_CARDS + NUM_AGE_2_CARDS + NUM_AGE_3_CARDS + NUM_GUILD_CARDS + NUM_TOKENS + NUM_WONDERS

#define MAX_PYRAMID_LOCATIONS 20

void GameState::drawObject(int deck, int id)
{
    if (objectLocations[id].deck != deck) throw EXC_INCORRECT_DECK;

    int pos = objectLocations[id].pos;
    int startPos = deckStarts[deck];
    int startId = deckObjects[startPos];

    std::swap(objectLocations[id], objectLocations[startId]);
    std::swap(deckObjects[pos], deckObjects[startPos]);

    deckStarts[deck]++;
}

void GameState::revealMiscObject(int id, int fromDeck, int toDeck)
{
    drawObject(fromDeck, id);
    objectLocations[id] = ObjectLocation(DECK_GAME_TOKENS, POS_NONE);
}

void GameState::revealPyramidCard(int pos, int id)
{
    PyramidSlot& slot = cardPyramid[pos];

    if (slot.objectId != SLOT_UNREVEALED) throw EXC_INVALID_REVEAL;

    drawObject(slot.deck, id);

    slot.objectId = id;
    objectLocations[id] = ObjectLocation(DECK_CARD_PYRAMID, pos);
}

void GameState::buildMiscObject(int id, int fromDeck)
{
    const ObjectLocation& loc = objectLocations[id];
    if (loc.deck != fromDeck) throw EXC_OBJECT_NOT_AVALIABLE;
    playerStates[currPlayer].buildObject(objects[id]);
    objectLocations[id] = ObjectLocation(DECK_USED, POS_NONE);
}

void GameState::startPlayPyramidCard(int id)
{
    const ObjectLocation& loc = objectLocations[id];
    if (loc.deck != DECK_CARD_PYRAMID || cardPyramid[loc.pos].coveredBy > 0) throw EXC_OBJECT_NOT_AVALIABLE;
}

void GameState::endPlayPyramidCard(int id)
{
    const ObjectLocation& loc = objectLocations[id];
    cardPyramid[loc.pos].objectId = SLOT_EMPTY;
    for (int pos : pyramidSchemes[currAge][loc.pos].covering)
    {
        cardPyramid[pos].coveredBy--;
        if (cardPyramid[pos].coveredBy == 0) expectedActions.push(Action(ACT_REVEAL_PYRAMID_CARD, pos));
    }
}

void GameState::revealGuild(int pos)
{
    if (cardPyramid[pos].deck == DECK_GUILDS) throw EXC_ALREADY_A_GUILD;
    cardPyramid[pos].deck = DECK_GUILDS;
}

void GameState::revealGameToken(int id)
{
    revealMiscObject(id, DECK_TOKENS, DECK_GAME_TOKENS);
}

void GameState::revealBoxToken(int id)
{
    revealMiscObject(id, DECK_TOKENS, DECK_BOX_TOKENS);
}

void GameState::revealWonder(int id)
{
    revealMiscObject(id, DECK_WONDERS, DECK_REVEALED_WONDERS);
}

void GameState::buildPyramidCard(int id)
{
    startPlayPyramidCard(id);
    playerStates[currPlayer].payForAndBuildObject(objects[id]);
    endPlayPyramidCard(id);

    objectLocations[id] = ObjectLocation(DECK_USED, POS_NONE);
}

void GameState::discardPyramidCard(int id)
{
    startPlayPyramidCard(id);
    endPlayPyramidCard(id);

    objectLocations[id] = ObjectLocation(DECK_DISCARDED, POS_NONE);
}

void GameState::buildWonderWithPyramidCard(int id, int cardId)
{
    if (wondersBuilt >= MAX_WONDERS_BUILT) throw EXC_MAX_WONDERS_BUILT;

    const ObjectLocation& loc = objectLocations[id];
    if (loc.deck != DECK_SELECTED_WONDERS + currPlayer) throw EXC_OBJECT_NOT_AVALIABLE;

    startPlayPyramidCard(cardId);
    playerStates[currPlayer].payForAndBuildObject(objects[id]);
    endPlayPyramidCard(cardId);

    objectLocations[id] = ObjectLocation(DECK_USED, POS_NONE);
    objectLocations[cardId] = ObjectLocation(DECK_USED, POS_NONE);
    wondersBuilt++;
}

void GameState::buildGameToken(int id)
{
    buildMiscObject(id, DECK_GAME_TOKENS);
}

void GameState::buildBoxToken(int id)
{
    buildMiscObject(id, DECK_BOX_TOKENS);
}

void GameState::buildDiscarded(int id)
{
    buildMiscObject(id, DECK_DISCARDED);
}

void GameState::selectWonder(int id)
{
    const ObjectLocation& loc = objectLocations[id];
    if (loc.deck != DECK_REVEALED_WONDERS) throw EXC_OBJECT_NOT_AVALIABLE;
    objectLocations[id] = ObjectLocation(DECK_SELECTED_WONDERS + currPlayer, POS_NONE);
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

    deckStarts[DECK_AGE_1] = 0;
    deckStarts[DECK_AGE_2] = deckStarts[DECK_AGE_1] + NUM_AGE_1_CARDS;
    deckStarts[DECK_AGE_3] = deckStarts[DECK_AGE_1] + NUM_AGE_2_CARDS;
    deckStarts[DECK_GUILDS] = deckStarts[DECK_AGE_1] + NUM_AGE_3_CARDS;
    deckStarts[DECK_TOKENS] = deckStarts[DECK_AGE_1] + NUM_GUILD_CARDS;
    deckStarts[DECK_WONDERS] = deckStarts[DECK_AGE_1] + NUM_TOKENS;

    std::copy(age1Cards.begin(), age1Cards.end(), deckObjects.begin() + deckStarts[DECK_AGE_1]);
    std::copy(age2Cards.begin(), age2Cards.end(), deckObjects.begin() + deckStarts[DECK_AGE_2]);
    std::copy(age3Cards.begin(), age3Cards.end(), deckObjects.begin() + deckStarts[DECK_AGE_3]);
    std::copy(guildCards.begin(), guildCards.end(), deckObjects.begin() + deckStarts[DECK_GUILDS]);
    std::copy(tokens.begin(), tokens.end(), deckObjects.begin() + deckStarts[DECK_TOKENS]);
    std::copy(wonders.begin(), wonders.end(), deckObjects.begin() + deckStarts[DECK_WONDERS]);

    int deck = 0;
    for (int i = 0; i < NUM_DECK_LOCATIONS; i++)
    {
        while (deck < NUM_DECKS && deck >= deckStarts[deck + 1]) deck++;
        objectLocations[deckObjects[i]] = ObjectLocation(deck, i);
    }

    for (int id : noDeckObjects)
    {
        objectLocations[id] = ObjectLocation();
    }

    for (int i = 0; i < NUM_GAME_TOKENS; ++i)
    {
        expectedActions.push(Action(ACT_REVEAL_GAME_TOKEN));
    }
}
