#include "game_simulator.h"

#include "game/game_exception.h"
#include "game/results.h"
#include "utils/random.h"

GameSimulator::GameSimulator(GameStateFast& game, const MCConfig& config)
    : game(game)
    , config(config)
{}

int GameSimulator::randDeckObject(int deck) const
{
    return game.getDeckElem(deck, uniformInt(0, game.getDeckSize(deck)));
}

Action GameSimulator::fromDeckAction(const Action& expected, int deck) const
{
    Action action = expected;
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

    int currPlayer = game.getCurrActor();

    const PlayerState& state = game.getPlayerState(currPlayer);

    int cnt = 0;

    do
    {
        action.arg1 = randDeckObject(DECK_PYRAMID_PLAYABLE);

        bool canBeWonder = game.getWondersBuilt() < MAX_WONDERS_BUILT && game.getDeckSize(DECK_SELECTED_WONDERS + currPlayer) > 0;
        double roll = uniformReal(0, 1 - (canBeWonder ? 0 : config.simWonderProb));
        if (roll < config.simDiscardProb)
        {
            action.arg2 = ACT_ARG2_DISCARD;
            break;
        }
        else if (roll < config.simDiscardProb + config.simBuildProb)
        {
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
    return resultSign(game.getResult(player));
}
