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

void GameState::verifyPlayer(int player) const
{
    if (player < 0 || player >= NUM_PLAYERS)
        throw GameException("Invalid player.", {{"player", player}});
}

void GameState::verifyPos(int pos, int deck) const
{
    int start = deck == DECK_CARD_PYRAMID ? 0 : deckStarts[deck];
    int end = deck == DECK_CARD_PYRAMID ? PYRAMID_SIZE : deckEnds[deck];

    if (pos < start || pos >= end)
        throw GameException("Position not in deck.", {{"pos", pos}, {"deck", deck}, {"deckStart", start}, {"deckEnd", end}});
}

void GameState::verifyObject(int id) const
{
    if (id < 0 || id >= NUM_OBJECTS)
        throw GameException("Invalid object id.", {{"objectId", id}});
}

int GameState::otherPlayer() const
{
    return 1 - currPlayer;
}

void GameState::advancePlayer()
{
    currPlayer = otherPlayer();
}

void GameState::queueAction(const Action& action, int count)
{
    for (int i = 0; i < count; i++)
    {
        queuedActions.push(action);
    }
}

void GameState::drawObject(int id, int deck)
{
    verifyObject(id);

    if (objectLocations[id].deck != deck)
        throw GameException("Object not in correct deck.", {{"objectId", id}, {"objectDeck", objectLocations[id].deck}, {"deck", deck}});

    if (isDeckEmpty(deck))
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
    verifyObject(id);

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

void GameState::revealGuild(int pos)
{
    verifyPos(pos, DECK_CARD_PYRAMID);

    if (cardPyramid[pos].deck == DECK_GUILDS)
        throw GameException("Pyramid slot already has guild.", {{"pos", pos}});

    cardPyramid[pos].deck = DECK_GUILDS;
}

void GameState::revealPyramidCard(int id, int pos)
{
    verifyObject(id);
    verifyPos(pos, DECK_CARD_PYRAMID);

    PyramidSlot& slot = cardPyramid[pos];

    if (slot.objectId != SLOT_UNREVEALED)
        throw GameException("Pyramid slot not unrevealed.", {{"pos", pos}, {"posObjectId", slot.objectId}});

    drawObject(id, slot.deck);

    slot.objectId = id;
    objectLocations[id] = ObjectLocation(DECK_CARD_PYRAMID, pos);
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

void GameState::revealFirstPlayer(int player)
{
    verifyPlayer(player);
    firstPlayer = player;
}

void GameState::buildDeckObject(int id, int deck)
{
    drawObject(id, deck);
    playerStates[currPlayer].payForAndBuildObject(objects[id]);
    objectLocations[id] = ObjectLocation(DECK_USED, POS_NONE);
}

void GameState::playPyramidCard(int id)
{
    verifyObject(id);

    const ObjectLocation& loc = objectLocations[id];

    verifyPos(loc.pos, DECK_CARD_PYRAMID);

    if (loc.deck != DECK_CARD_PYRAMID)
        throw GameException("Object not in pyramid.", {{"objectId", id}, {"objectDeck", loc.deck}});

    if (cardPyramid[loc.pos].coveredBy > 0)
        throw GameException("Pyramid slot is covered.", {{"pos", loc.pos}, {"coveredBy", cardPyramid[loc.pos].coveredBy}});

    cardPyramid[loc.pos].objectId = OBJ_NONE;
    for (int pos : pyramidSchemes[currAge][loc.pos].covering)
    {
        PyramidSlot& other = cardPyramid[pos];
        other.coveredBy--;
        if (other.objectId == SLOT_UNREVEALED && other.coveredBy == 0) queueAction(Action(ACT_REVEAL_PYRAMID_CARD, ACT_ARG_EMPTY, pos));
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
    playerStates[currPlayer].discardCard();
    insertObject(id, DECK_DISCARDED);
}

void GameState::usePyramidCardForWonder(int id, int wonderId)
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

    while (!isDeckEmpty(DECK_BOX_TOKENS))
    {
        int id2 = deckObjects[deckStarts[DECK_BOX_TOKENS]];
        drawObject(id2, DECK_BOX_TOKENS);
        insertObject(id2, DECK_TOKENS);
    }
}

void GameState::buildDiscarded(int id)
{
    buildDeckObject(id, DECK_DISCARDED);
}

void GameState::destroyObject(int id, int type)
{
    verifyObject(id);

    if (objects[id].type != type)
        throw GameException("Object not of correct type.", {{"objectId", id}, {"objectType", type}, {"type", type}});

    playerStates[otherPlayer()].destroyObject(objects[id]);
}

void GameState::selectWonder(int id)
{
    drawObject(id, DECK_REVEALED_WONDERS);
    insertObject(id, DECK_SELECTED_WONDERS + currPlayer);
}

void GameState::chooseStartPlayer(int player)
{
    verifyPlayer(player);
    currPlayer = player;
}

void GameState::doAction(const Action& action)
{
    if (isTerminal())
        throw GameException("Game already ended.", {});

    correctPossibleActions = false;

    const Action& expected = getExpectedAction();

    if (action.type != expected.type ||
        (action.type == ACT_REVEAL_PYRAMID_CARD && action.arg2 != expected.arg2) ||
        (action.type == ACT_MOVE_DESTROY_OBJECT && action.arg2 != expected.arg2))
    {
        if (action.type == ACT_REVEAL_PYRAMID_CARD && action.arg2 != expected.arg2)
            throw GameException("Unexpected pyramid reveal position.", {{"actionPos", action.arg2}, {"expectedPos", expected.arg2}});
        if (action.type == ACT_MOVE_DESTROY_OBJECT && action.arg2 != expected.arg2)
            throw GameException("Unexpected object type.", {{"actionObjectType", action.arg2}, {"expectedObjectType", expected.arg2}});
        else
            throw GameException("Unexpected action.", {{"actionType", action.type}, {"expectedType", expected.type}});
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
        throw GameException("Unknown action type.", {{"actionType", action.type}});
    }

    if (currAge == AGE_SETUP && queuedActions.empty())
    {
        startWonderSelection();
        return;
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

        if (isDeckEmpty(DECK_REVEALED_WONDERS))
            queueAction(Action(ACT_REVEAL_WONDER), NUM_WONDERS_REVEALED);
    
        if (getDeckSize(DECK_REVEALED_WONDERS) != NUM_WONDERS_REVEALED / 2) advancePlayer();
        queueAction(Action(ACT_MOVE_SELECT_WONDER));

        return;
    }

    PlayerState& state = playerStates[currPlayer];

    if (state.getResult(false) != RESULT_DRAW)
    {
        queuedActions = std::queue<Action>();
        return;
    }

    if (state.shouldBuildGameToken)
    {
        state.shouldBuildGameToken = false;
        if (!isDeckEmpty(DECK_GAME_TOKENS))
        {
            queueAction(Action(ACT_MOVE_BUILD_GAME_TOKEN));
            return;
        }
    }

    if (state.shouldBuildBoxToken)
    {
        state.shouldBuildBoxToken = false;
        if (!isDeckEmpty(DECK_TOKENS))
        {
            queueAction(Action(ACT_REVEAL_BOX_TOKEN), std::min(NUM_BOX_TOKENS, getDeckSize(DECK_TOKENS)));
            queueAction(Action(ACT_MOVE_BUILD_BOX_TOKEN));
            return;
        }
    }

    if (state.shouldDestroyType != OT_NONE)
    {
        int type = state.shouldDestroyType;
        state.shouldDestroyType = OT_NONE;
        if (playerStates[otherPlayer()].typeCounts[type] > 0)
        {
            queueAction(Action(ACT_MOVE_DESTROY_OBJECT, OBJ_NONE, type));
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
        if (!isTerminal())
            throw GameException("No cards remaining but actions queued.", {{"queuedType", getExpectedAction().type}});

        if (currAge < NUM_AGES - 1) advanceAge();
        return;
    }

    advancePlayer();
    queueAction(Action(ACT_MOVE_PLAY_PYRAMID_CARD));
}

void GameState::startWonderSelection()
{
    currPlayer = firstPlayer;
    currAge = AGE_WONDER_SELECTION;
    cardsRemaining = NUM_PLAYERS * NUM_WONDERS_PER_PLAYER;

    queueAction(Action(ACT_REVEAL_WONDER), NUM_WONDERS_REVEALED);
    queueAction(Action(ACT_MOVE_SELECT_WONDER));
}

void GameState::advanceAge()
{
    currAge++;
    cardsRemaining = PYRAMID_SIZE;

    if (currAge == NUM_AGES - 1) queueAction(Action(ACT_REVEAL_GUILD), NUM_LAST_AGE_GUILDS);

    for (int pos = 0; pos < PYRAMID_SIZE; pos++)
    {
        cardPyramid[pos] = PyramidSlot(currAge, SLOT_UNREVEALED, 0);
        if (pyramidSchemes[currAge][pos].revealed) queueAction(Action(ACT_REVEAL_PYRAMID_CARD, ACT_ARG_EMPTY, pos));
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

GameState::GameState()
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

GameState::GameState(const GameState& other)
{
    *this = other;
}

GameState& GameState::operator=(const GameState& other)
{
    firstPlayer = other.firstPlayer;
    currPlayer = other.currPlayer;
    currAge = other.currAge;
    wondersBuilt = other.wondersBuilt;
    cardsRemaining = other.cardsRemaining;
    playerStates = other.playerStates;
    deckEnds = other.deckEnds;
    deckObjects = other.deckObjects;
    cardPyramid = other.cardPyramid;
    objectLocations = other.objectLocations;
    queuedActions = other.queuedActions;
    correctPossibleActions = false;

    linkPlayers();

    return *this;
}

void GameState::reset()
{
    *this = GameState();
}

void GameState::linkPlayers()
{ 
    playerStates[0].otherPlayer = &playerStates[1];
    playerStates[1].otherPlayer = &playerStates[0];
}

bool GameState::isTerminal() const
{
    return queuedActions.empty();
}

int GameState::getResult(int player) const
{
    verifyPlayer(player);

    if (!isTerminal())
        throw GameException("Game has not ended yet.", {});

    return playerStates[player].getResult(true);
}

int GameState::getFirstPlayer() const
{
    if (firstPlayer == NO_PLAYER)
        throw GameException("First player has not been set yet.", {});

    return firstPlayer;
}

int GameState::getCurrAge() const
{
    return currAge;
}

int GameState::getCurrActor() const
{
    if (getExpectedAction().isPlayerMove()) return currPlayer;
    else return ACTOR_GAME;
}

Action GameState::getExpectedAction() const
{
    if (isTerminal())
        throw GameException("Game has already ended.", {});

    return queuedActions.front();
}

const std::vector<Action>& GameState::getPossibleActions() const
{
    GameState& thisMut = const_cast<GameState&>(*this);

    if (!thisMut.correctPossibleActions)
    {
        getPossibleActions(thisMut.possibleActions);
        thisMut.correctPossibleActions = true;
    }

    return thisMut.possibleActions;
}

void GameState::getPossibleActions(std::vector<Action>& possible) const
{
    possibleActionsUnchecked(possible);

    if (possible.empty())
        throw GameException("No possible actions.", {{"expectedType", getExpectedAction().type}});
}

int GameState::getCoins(int player) const
{
    return playerStates[player].coins;
}

int GameState::getScore(int player, int onlyType) const
{
    verifyPlayer(player);

    return playerStates[player].getScore(onlyType);
}

int GameState::getDistinctSciences(int player) const
{
    verifyPlayer(player);

    return playerStates[player].distincSciences;
}

int GameState::getMilitary(int player) const
{
    verifyPlayer(player);

    return playerStates[player].military;
}

int GameState::getMilitaryLead(int player) const
{
    verifyPlayer(player);

    return playerStates[player].militaryLead();
}

int GameState::getDeckSize(int deck) const
{
    return deckEnds[deck] - deckStarts[deck];
}

bool GameState::isDeckEmpty(int deck) const
{
    return getDeckSize(deck) <= 0;
}

const PyramidSlot& GameState::getPyramidSlot(int pos) const
{
    verifyPos(pos, DECK_CARD_PYRAMID);

    return cardPyramid[pos];
}

const PlayerState& GameState::getPlayerState(int player) const
{
    verifyPlayer(player);

    return playerStates[player];
}

int GameState::getDeckElem(int deck, int pos) const
{
    pos += deckStarts[deck];

    verifyPos(pos, deck);

    return deckObjects[pos];
}

int GameState::getObjectDeck(int id) const
{
    verifyObject(id);

    return objectLocations[id].deck;
}

void GameState::possibleFromDeckActions(std::vector<Action>& possible, const Action& expected, int deck) const
{
    possible.reserve(getDeckSize(deck));

    Action action = expected;

    for (int pos = deckStarts[deck]; pos < deckEnds[deck]; pos++)
    {
        action.arg1 = deckObjects[pos];
        possible.push_back(action);
    }
}

void GameState::possibleChooseStartPlayerActions(std::vector<Action>& possible) const
{
    possible.reserve(NUM_PLAYERS);

    for (int player = 0; player < NUM_PLAYERS; player++)
    {
        possible.push_back(Action(ACT_MOVE_CHOOSE_START_PLAYER, player));
    }
}

void GameState::possibleDestroyObjectActions(std::vector<Action>& possible, int type) const
{
    const PlayerState& other = playerStates[otherPlayer()];

    possible.reserve(other.typeCounts[type]);

    for (int id = objectTypeStarts[type]; id < objectTypeStarts[type + 1]; id++)
    {
        if (!other.objectsBuilt[id]) continue;
        possible.push_back(Action(ACT_MOVE_DESTROY_OBJECT, id, type));
    }
}

void GameState::possiblePlayPyramidCardActions(std::vector<Action>& possible) const
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

    for (int pos = 0; pos < PYRAMID_SIZE; ++pos)
    {
        if (cardPyramid[pos].coveredBy > 0 || cardPyramid[pos].objectId < 0) continue;

        int id = cardPyramid[pos].objectId;

        verifyObject(id);

        if (state.canPayFor(objects[id])) possible.push_back(Action(ACT_MOVE_PLAY_PYRAMID_CARD, id, ACT_ARG2_BUILD));
        possible.push_back(Action(ACT_MOVE_PLAY_PYRAMID_CARD, id, ACT_ARG2_DISCARD));

        for (int wonderId : possibleWonders)
        {
            possible.push_back(Action(ACT_MOVE_PLAY_PYRAMID_CARD, id, wonderId));
        }
    }
}

void GameState::possibleRevealGuildActions(std::vector<Action>& possible) const
{
    possible.reserve(PYRAMID_SIZE);

    for (int pos = 0; pos < PYRAMID_SIZE; pos++)
    {
        if (cardPyramid[pos].deck != DECK_GUILDS) possible.push_back(Action(ACT_REVEAL_GUILD, pos));
    }
}

void GameState::possibleRevealFirstPlayerActions(std::vector<Action>& possible) const
{
    possible.reserve(NUM_PLAYERS);

    for (int player = 0; player < NUM_PLAYERS; player++)
    {
        possible.push_back(Action(ACT_REVEAL_FIRST_PLAYER, player));
    }
}

void GameState::possibleActionsUnchecked(std::vector<Action>& possible) const
{
    possible.clear();

    const Action& expected = getExpectedAction();

    switch (expected.type)
    {
    case ACT_MOVE_PLAY_PYRAMID_CARD:
        return possiblePlayPyramidCardActions(possible);

    case ACT_MOVE_BUILD_GAME_TOKEN:
        return possibleFromDeckActions(possible, expected, DECK_GAME_TOKENS);

    case ACT_MOVE_BUILD_BOX_TOKEN:
        return possibleFromDeckActions(possible, expected, DECK_BOX_TOKENS);

    case ACT_MOVE_BUILD_DISCARDED:
        return possibleFromDeckActions(possible, expected, DECK_DISCARDED);

    case ACT_MOVE_DESTROY_OBJECT:
        return possibleDestroyObjectActions(possible, expected.arg2);

    case ACT_MOVE_SELECT_WONDER:
        return possibleFromDeckActions(possible, expected, DECK_REVEALED_WONDERS);

    case ACT_MOVE_CHOOSE_START_PLAYER:
        return possibleChooseStartPlayerActions(possible);

    case ACT_REVEAL_GUILD:
        return possibleRevealGuildActions(possible);

    case ACT_REVEAL_PYRAMID_CARD:
        verifyPos(expected.arg2, DECK_CARD_PYRAMID);
        return possibleFromDeckActions(possible, expected, cardPyramid[expected.arg2].deck);

    case ACT_REVEAL_GAME_TOKEN:
        return possibleFromDeckActions(possible, expected, DECK_TOKENS);

    case ACT_REVEAL_BOX_TOKEN:
        return possibleFromDeckActions(possible, expected, DECK_TOKENS);

    case ACT_REVEAL_WONDER:
        return possibleFromDeckActions(possible, expected, DECK_WONDERS);

    case ACT_REVEAL_FIRST_PLAYER:
        return possibleRevealFirstPlayerActions(possible);

    default:
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
    deckStarts[NUM_DECKS] = deckStarts[DECK_DISCARDED] + MAX_DISCARDED;

    return deckStarts;
}

const std::array<int, NUM_DECKS + 1> GameState::deckStarts = findDeckStarts();
