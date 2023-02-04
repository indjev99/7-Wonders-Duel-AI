#include "mc_config.h"

#include "game/game_state.h"

struct GameSimulator
{
    GameSimulator(GameStateFast& game, const MCConfig& config);

    int simGame(int player);

private:

    int randDeckObject(int deck) const;

    Action fromDeckAction(const Action& expected, int deck) const;
    Action playPyramidCardAction() const;
    Action destroyObjectAction(int type) const;
    Action chooseStartPlayerAction() const;
    Action revealGuildAction() const;
    Action revealFirstPlayerAction() const;
    Action action() const;

    GameStateFast& game;
    MCConfig config;
};
