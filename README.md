# 7-Wonders-Duel-AI

An implementation of the game 7 Wonders Duel.
Also contains a customizable class hierarchy of `Players` and `Listeners` that enables adding any sort of interface.
One implementation is a pair of GUI `Player` and `Listener` classes for human interactions.
The main product are the AI `Player` classes which can play against each other or versus a human.

## GUI

On a player's turn, there will be an arrow pointing to them.
At the end of the game, it will point to the winner.
There on a bot's turn, the played action will be highlighted and the Advance button has to be pressed to proceed to the next turn.
On a player playing through the GUI's turn, the relevant card and/or wonder and/or buttons have to be pressed (they will be highlighted). Then the move has to be finalized by pressing the Advance button.

Shortcuts:

- Advance -- `Space`, `Enter`
- Build -- `1`
- Discard -- `2`
- Wonder -- `3`
- Player 1 -- `1`
- Player 2 -- `2`
- Zoom in -- `+`, `=`
- Zoom out -- `-`
