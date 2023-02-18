#pragma once

#include "mc_config.h"

#include "game/game_state.h"
#include "utils/defines.h"

extern long long totalNumSims;

int simRandGame(GameStateFast& game, int player, const MCConfig& config);

struct GameSimulator
{
    GameSimulator(GameStateFast& game, const MCConfig& config);

    int simGame(int player);
    void simAction();

private:

    FORCE_INLINE int randDeckObject(int deck) const;

    FORCE_INLINE int modeCard(int deck, bool needToPay) const;
    FORCE_INLINE int modeToken(int deck) const;

    FORCE_INLINE Action lookAheadWonderAction();

    FORCE_INLINE Action fromDeckAction(const Action& expected, int deck) const;
    Action playPyramidCardAction();
    Action destroyObjectAction(int type) const;
    Action chooseStartPlayerAction() const;
    Action revealGuildAction() const;
    Action revealFirstPlayerAction() const;
    Action action();

    GameStateFast& game;
    MCConfig config;

    std::array<int, NUM_PLAYERS> simModes;
    int aggressor;
};
