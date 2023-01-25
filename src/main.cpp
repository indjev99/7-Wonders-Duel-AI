#include <iostream>

#include "game_state.h"
#include "lang.h"

int main()
{
    GameState state;
    while (!state.isTerminal())
    {
        std::cout << std::endl;
        std::cout << actorToString(state.currActor()) << ":" << std::endl;
        std::cout << actionToString(state.expectedAction()) << std::endl;

        std::string actionStr;
        std::getline(std::cin, actionStr);
        state.doAction(actionFromString(actionStr));
    }

    return 0;
}
