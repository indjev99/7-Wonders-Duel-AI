#include <iostream>

#include "game_state.h"
#include "lang.h"

int main()
{
    GameState state;
    while (!state.queuedActions.empty())
    {
        std::cout << std::endl;
        const Action& scheduled = state.queuedActions.front();
        std::cout << actionToString(scheduled) << std::endl;
        std::string actionStr;
        std::getline(std::cin, actionStr);
        Action action = actionFromString(actionStr);
        state.doAction(action);
    }

    return 0;
}
