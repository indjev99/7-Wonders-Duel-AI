# 7-Wonders-Duel-AI

An implementation of the popular board game 7 Wonders Duel.
Also contains a class hierarchy of `Listeners` and `Agents` that enables adding any sort of interface.
One implementation is a pair of GUI `Agent` and `Listener` classes for human interactions.
The main product are the AI `Agent` classes which can play against each other or versus a human.

## GUI

On a player's turn, there will be an arrow pointing to them.
At the end of the game, it will point to the winner.
On a bot's turn, its action will be highlighted and will be executed after a bit of time.
On a human player's turn, the relevant card and/or wonder and/or buttons have to be selected.
They will be highlighted and the action will be executed after a bit of time.
During this time it is possible to unselect an object or to select a different one.
There is also an unused implementation for an "Advance" button, instead of a timer, but that is annoying.

Shortcuts:

- Build -- `1`
- Discard -- `2`
- Wonder -- `3`
- Player 1 -- `1`
- Player 2 -- `2`
- Zoom in -- `+`, `=`
- Zoom out -- `-`
- Advance (unused) -- `Space`, `Enter`

## TODO

- Better AIs
