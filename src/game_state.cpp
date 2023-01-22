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
    objectLocations[id] = ObjectLocation(DECK_BUILT, currPlayer);
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
    // TODO: reveal below
}

void GameState::revealGameToken(int id)
{
    revealMiscObject(id, DECK_TOKENS, DECK_GAME_TOKENS);
}

void GameState::revealBoxToken(int pos, int id)
{
    revealMiscObject(id, DECK_TOKENS, DECK_BOX_TOKENS);
}

void GameState::buildPyramidCard(int id)
{
    startPlayPyramidCard(id);
    playerStates[currPlayer].payForAndBuildObject(objects[id]);
    endPlayPyramidCard(id);

    objectLocations[id] = ObjectLocation(DECK_BUILT, currPlayer);
}

void GameState::discardPyramidCard(int id)
{
    startPlayPyramidCard(id);
    endPlayPyramidCard(id);

    objectLocations[id] = ObjectLocation(DECK_DISCARDED, currPlayer);
}

void GameState::buildWonderWithPyramidCard(int id, int cardId)
{
    if (wondersBuilt >= MAX_WONDERS_BUILT) throw EXC_MAX_WONDERS_BUILT;

    const ObjectLocation& loc = objectLocations[id];
    if (loc.deck != DECK_SELECTED_WONDERS || loc.pos != currPlayer) throw EXC_OBJECT_NOT_AVALIABLE;

    startPlayPyramidCard(cardId);
    playerStates[currPlayer].payForAndBuildObject(objects[id]);
    endPlayPyramidCard(cardId);

    objectLocations[id] = ObjectLocation(DECK_BUILT, currPlayer);
    objectLocations[cardId] = ObjectLocation(DECK_UNDER_WONDER, currPlayer);
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

GameState::GameState()
{
    currPlayer = 0;
    currAge = 0;
    wondersBuilt = 0;

    deckStarts[DECK_AGE_1] = 0;
    deckStarts[DECK_AGE_2] = deckStarts[DECK_AGE_1] + NUM_AGE_1_CARDS;
    deckStarts[DECK_AGE_3] = deckStarts[DECK_AGE_1] + NUM_AGE_2_CARDS;
    deckStarts[DECK_GUILD] = deckStarts[DECK_AGE_1] + NUM_AGE_3_CARDS;
    deckStarts[DECK_TOKENS] = deckStarts[DECK_AGE_1] + NUM_GUILD_CARDS;
    deckStarts[DECK_WONDERS] = deckStarts[DECK_AGE_1] + NUM_TOKENS;

    std::copy(age1Cards.begin(), age1Cards.end(), deckObjects.begin() + deckStarts[DECK_AGE_1]);
    std::copy(age2Cards.begin(), age2Cards.end(), deckObjects.begin() + deckStarts[DECK_AGE_2]);
    std::copy(age3Cards.begin(), age3Cards.end(), deckObjects.begin() + deckStarts[DECK_AGE_3]);
    std::copy(guildCards.begin(), guildCards.end(), deckObjects.begin() + deckStarts[DECK_GUILD]);
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
}
