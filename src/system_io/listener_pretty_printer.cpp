#include "listener_pretty_printer.h"

#include "game/lang.h"

#include <iostream>

void ListenerPrettyPrinter::printSummary()
{
    std::cout << "Coins: " << game->getCoins(0) << " " << game->getCoins(1) << std::endl;
    std::cout << "Scores: " << game->getScore(0) << " " << game->getScore(1) << std::endl;
    std::cout << "Sciences: " << game->getDistinctSciences(0) << " " << game->getDistinctSciences(1) << std::endl;
    std::cout << "Militaries: " << game->getMilitary(0) << " " << game->getMilitary(1) << std::endl;
}

void ListenerPrettyPrinter::notifyStart()
{
    std::cout << std::endl;
    std::cout << "Starting new game" << std::endl;
    std::cout << std::endl;

    printSummary();
    std::cout << std::endl;
}

void ListenerPrettyPrinter::notifyActionPre(const Action& action)
{
    if (game->isAgeStart())
    {
        std::cout << "Starting: " << ageToString(game->getCurrAge()) << std::endl;
        std::cout << std::endl;
    }

    std::cout << actorToString(game->getCurrActor()) << ": " << actionToString(action) << std::endl;
    std::cout << std::endl;

    if (game->getCurrActor() != ACTOR_GAME && game->getCurrAge() >= 0)
    {
        printSummary();
        std::cout << std::endl;
    }
}

void ListenerPrettyPrinter::notifyActionPost(const Action& action)
{
    if (action.isPlayerMove() && action.type != ACT_MOVE_SELECT_WONDER)
    {
        printSummary();
        std::cout << std::endl;
    }
}

void ListenerPrettyPrinter::notifyEnd()
{
    int res = game->getResult(0);

    if (res > 0) std::cout << actorToString(0) << ": " << resultToString(res) << std::endl;
    else if (res < 0) std::cout << actorToString(1) << ": " << resultToString(-res) << std::endl;
    else std::cout << resultToString(res) << std::endl;
    std::cout << std::endl;
}
