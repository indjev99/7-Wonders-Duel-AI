#include "game_simulator.h"

#include "game/game_exception.h"
#include "utils/random.h"

#include <algorithm>

GameSimulator::GameSimulator(GameStateFast& game, const MCConfig& config)
    : game(game)
    , config(config)
{
    aggressor = ACTOR_NONE;
    for (int i = 0; i < NUM_PLAYERS; i++)
    {
        simModes[i] = config.simModes[i];
        if (simModes[i] != SIM_MODE_NONE) simModes[i] = sampleIntDistr(config.simModeProbs);
        if (game.getMilitaryLead(i) > 0) aggressor = i;
    }
}

int GameSimulator::randDeckObject(int deck) const
{
    return game.getDeckElem(deck, uniformInt(0, game.getDeckSize(deck)));
}

int GameSimulator::modeToken(int deck) const
{
    int currPlayer = game.getCurrActor();
    int mode = simModes[currPlayer];

    if (mode == SIM_MODE_NORMAL) return OBJ_NONE;

    int token = mode == SIM_MODE_SCIENCE ? O_TOKEN_LAW : O_TOKEN_STRATEGY;

    if (game.getObjectDeck(token) == deck) return token;

    return OBJ_NONE;
}

int GameSimulator::modeCard(int deck, bool needToPay) const
{
    int currPlayer = game.getCurrActor();
    const PlayerState& state = game.getPlayerState(currPlayer);
    int mode = simModes[currPlayer];

    if (mode == SIM_MODE_NORMAL) return OBJ_NONE;

    int type = mode == SIM_MODE_SCIENCE ? OT_GREEN : OT_RED;

    static std::vector<int> perm;
    perm.resize(game.getDeckSize(deck));
    std::iota(perm.begin(), perm.end(), 0);

    for (int i : perm)
    {
        int id = game.getDeckElem(deck, i);
        if (objects[id].type == type && (!needToPay || state.canPayFor(objects[id]))) return id;
    }

    return OBJ_NONE;
}

int GameSimulator::modeWonder() const
{
    int currPlayer = game.getCurrActor();
    const PlayerState& state = game.getPlayerState(currPlayer);
    int mode = simModes[currPlayer];

    if (mode == SIM_MODE_NORMAL) return OBJ_NONE;

    int deck = DECK_SELECTED_WONDERS + currPlayer;

    int discWonder = O_WONDER_THE_MAUSOLEUM;

    if (game.getCurrAge() == NUM_AGES - 1 && game.getObjectDeck(discWonder) == deck &&
        state.canPayFor(objects[discWonder]) && modeCard(DECK_DISCARDED, false) != OT_NONE)
    {
        return discWonder;
    }

    return OBJ_NONE;
}

Action GameSimulator::fromDeckAction(const Action& expected, int deck) const
{
    Action action = expected;

    if (expected.type == ACT_MOVE_BUILD_GAME_TOKEN || expected.type == ACT_MOVE_BUILD_BOX_TOKEN)
    {
        action.arg1 = modeToken(deck);
        if (action.arg1 != OBJ_NONE) return action;
    }
    else if (expected.type == ACT_MOVE_BUILD_DISCARDED)
    {
        action.arg1 = modeCard(deck, false);
        if (action.arg1 != OBJ_NONE) return action;
    }

    action.arg1 = randDeckObject(deck);
    return action;
}

Action GameSimulator::chooseStartPlayerAction() const
{
    return Action(ACT_MOVE_CHOOSE_START_PLAYER, game.getCurrActor());
}

Action GameSimulator::destroyObjectAction(int type) const
{
    const PlayerState& other = game.getPlayerState(game.getOpponent(game.getCurrActor()));
    int id;
    do
    {
        id = uniformInt(objectTypeStarts[type], objectTypeStarts[type + 1]);
    }
    while (!other.objectsBuilt[id]);
    return Action(ACT_MOVE_DESTROY_OBJECT, id, type);
}

Action GameSimulator::playPyramidCardAction() const
{
    Action action(ACT_MOVE_PLAY_PYRAMID_CARD);

    action.arg1 = modeCard(DECK_PYRAMID_PLAYABLE, true);
    if (action.arg1 != OBJ_NONE)
    {
        action.arg2 = ACT_ARG2_BUILD;
        return action;
    }

    int currPlayer = game.getCurrActor();
    const PlayerState& state = game.getPlayerState(currPlayer);

    bool canBeWonder = game.getWondersBuilt() < MAX_WONDERS_BUILT && game.getDeckSize(DECK_SELECTED_WONDERS + currPlayer) > 0;

    if (canBeWonder)
    {
        action.arg2 = modeWonder();
        if (action.arg2 != OBJ_NONE)
        {
            action.arg1 = randDeckObject(DECK_PYRAMID_PLAYABLE);
            return action;
        } 
    }

    do
    {
        action.arg1 = randDeckObject(DECK_PYRAMID_PLAYABLE);

        float roll = uniformReal(0, 1 - (canBeWonder ? 0 : config.simWonderProb));
        if (roll < config.simDiscardProb)
        {
            action.arg2 = ACT_ARG2_DISCARD;
            break;
        }
        else if (roll < config.simDiscardProb + config.simBuildProb)
        {
            if (config.simPacifist && objects[action.arg1].type == OT_RED && currPlayer != aggressor && game.getMilitaryLead(currPlayer) >= 0)
                continue;
            action.arg2 = ACT_ARG2_BUILD;
            if (state.canPayFor(objects[action.arg1])) break;
        }
        else
        {
            action.arg2 = randDeckObject(DECK_SELECTED_WONDERS + currPlayer);
            if (state.canPayFor(objects[action.arg2])) break;
        }
    }
    while (true);

    return action;
}

Action GameSimulator::revealGuildAction() const
{
    int pos;
    do
    {
        pos = uniformInt(0, PYRAMID_SIZE);
    }
    while (game.getPyramidSlot(pos).deck == DECK_GUILDS);
    return Action(ACT_REVEAL_GUILD, pos);
}

Action GameSimulator::revealFirstPlayerAction() const
{
    return Action(ACT_REVEAL_FIRST_PLAYER, uniformInt(0, NUM_PLAYERS));
}

Action GameSimulator::action() const
{
    const Action& expected = game.getExpectedAction();

    switch (expected.type)
    {
    case ACT_MOVE_PLAY_PYRAMID_CARD:
        return playPyramidCardAction();

    case ACT_MOVE_BUILD_GAME_TOKEN:
        return fromDeckAction(expected, DECK_GAME_TOKENS);

    case ACT_MOVE_BUILD_BOX_TOKEN:
        return fromDeckAction(expected, DECK_BOX_TOKENS);

    case ACT_MOVE_BUILD_DISCARDED:
        return fromDeckAction(expected, DECK_DISCARDED);

    case ACT_MOVE_DESTROY_OBJECT:
        return destroyObjectAction(expected.arg2);

    case ACT_MOVE_SELECT_WONDER:
        return fromDeckAction(expected, DECK_REVEALED_WONDERS);

    case ACT_MOVE_CHOOSE_START_PLAYER:
        return chooseStartPlayerAction();

    case ACT_REVEAL_GUILD:
        return revealGuildAction();

    case ACT_REVEAL_PYRAMID_CARD:
        return fromDeckAction(expected, game.getPyramidSlot(expected.arg2).deck);

    case ACT_REVEAL_GAME_TOKEN:
        return fromDeckAction(expected, DECK_TOKENS);

    case ACT_REVEAL_BOX_TOKEN:
        return fromDeckAction(expected, DECK_TOKENS);

    case ACT_REVEAL_WONDER:
        return fromDeckAction(expected, DECK_WONDERS);

    case ACT_REVEAL_FIRST_PLAYER:
        return revealFirstPlayerAction();

    default:
        return Action();
    }
}

int GameSimulator::simGame(int player)
{
    while (!game.isTerminal())
    {
        game.doAction(action());
    }
    return game.getResult(player);
}
