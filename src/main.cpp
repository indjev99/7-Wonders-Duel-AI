#include <iostream>

#include "game_state.h"

int main()
{
    try
    {
        GameState state;
        while (!state.scheduledActions.empty())
        {
            std::cout << std::endl;
            const Action& scheduled = state.scheduledActions.front();
            std::cout << scheduled.type << " " << scheduled.arg1 << " " << scheduled.arg2 << std::endl;
            Action action;
            std::cin >> action.type >> action.arg1 >> action.arg2;
            state.doAction(action);
        }  
    }
    catch (int e)
    {
        std::cerr << "Error: " << e << std::endl;
    }

    return 0;
}
