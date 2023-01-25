#include "game_exception.h"
#include "game_state.h"
#include "lang.h"

#include <ctime>
#include <iostream>
#include <random>
#include <stack>

std::default_random_engine generator;

int uniformInt(int from, int to)
{
    std::uniform_int_distribution<int> distribution(from, to - 1);
    return distribution(generator);
}

void printSummary(const GameState& state, int player)
{
    std::cout << std::endl;
    std::cout << "Coins: " << state.getCoins(player) << " " << state.getCoins(1 - player) << std::endl;
    std::cout << "Scores: " << state.getScore(player) << " " << state.getScore(1 - player) << std::endl;
    std::cout << "Sciences: " << state.getDistinctSciences(player) << " " << state.getDistinctSciences(1 - player) << std::endl;
    std::cout << "Militaries: " << state.getMilitary(player) << " " << state.getMilitary(1 - player) << std::endl;
}

int benchmarkPlayRandom()
{
    GameState state;
    std::vector<Action> possible;
    while (!state.isTerminal())
    {
        state.possibleActions(possible);
        Action action = possible[uniformInt(0, possible.size())];
        state.doAction(action);
    }
    return state.getResult(0);
}

void benchmark()
{
    int cnt = 0;
    while (true)
    {
        if (cnt % 2000 == 0) std::cerr << cnt << std::endl;
        benchmarkPlayRandom();
        cnt++;
    }
}

int playRandom(int povPlayer = 0)
{
    std::cout << "PoV: " << actorToString(povPlayer) << std::endl;

    GameState state;
    std::vector<Action> possible;

    printSummary(state, povPlayer);

    while (!state.isTerminal())
    {
        std::cout << std::endl;
        std::cout << "Actor: " << actorToString(state.currActor()) << std::endl;
        state.possibleActions(possible);
        Action action = possible[uniformInt(0, possible.size())];
        std::cout << "Action: " << actionToString(action) << std::endl;
        state.doAction(action);

        if (action.isPlayerMove()) printSummary(state, povPlayer);
    }

    int result = state.getResult(povPlayer);

    std::cout << std::endl;
    std::cout << "Result: " << resultToString(result) << std::endl;

    return result;
}

int playInteractive(int povPlayer = 0)
{
    std::cout << "PoV: " << actorToString(povPlayer) << std::endl;

    GameState state;
    std::string actionStr;
    std::stack<GameState> history;

    history.push(state);

    printSummary(state, povPlayer);

    while (!state.isTerminal())
    {
        try
        {
            std::cout << std::endl;
            std::cout << "Actor: " << actorToString(state.currActor()) << std::endl;
            std::cout << "Expected: " << actionToString(state.expectedAction()) << std::endl;
            std::cout << "Possible: " << std::endl;
            for (const Action& action : state.possibleActions())
            {
                std::cout << "    " << actionToString(action) << std::endl;
            }

            std::cout << "Action: ";
            std::getline(std::cin, actionStr);
            Action action = actionFromString(actionStr);

            if (action.type == ACT_UNDO)
            {
                if (history.size() == 1)
                    throw GameException("No actions to undo.", {});

                history.pop();
                state = history.top();

                printSummary(state, povPlayer);
    
                continue;
            }

            state.doAction(action);
            history.push(state);

            if (action.isPlayerMove()) printSummary(state, povPlayer);
        }
        catch (const GameException& e)
        {
            std::cout << "Error:" << e.what() << std::endl;
            std::cout << "Try again." << std::endl;
            state = history.top();
        }
    }

    int result = state.getResult(povPlayer);

    std::cout << std::endl;
    std::cout << "Result: " << resultToString(result) << std::endl;

    return result;
}

int main()
{
    generator.seed(time(nullptr));

    benchmark();

    return 0;
}
