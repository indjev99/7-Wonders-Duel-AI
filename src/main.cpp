#include <iostream>

#include "game_state.h"

int main()
{
    GameState state;
    while (!state.queuedActions.empty())
    {
        std::cout << std::endl;
        const Action& scheduled = state.queuedActions.front();
        std::cout << scheduled.type << " " << scheduled.arg1 << " " << scheduled.arg2 << std::endl;
        Action action;
        std::cin >> action.type >> action.arg1 >> action.arg2;
        state.doAction(action);
    }

    return 0;
}
