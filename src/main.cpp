#include "game_exception.h"
#include "game_runner.h"
#include "listener_printer.h"
#include "player_uniform.h"
#include "random.h"

#include <ctime>
#include <iostream>

void benchmark()
{
    PlayerUniform ai0;
    PlayerUniform ai1;

    GameRunner runner({&ai0, &ai1});

    int cnt = 0;
    while (true)
    {
        if (cnt % 2000 == 0) std::cout << cnt << std::endl;
        runner.playGame();
        cnt++;
    }
}

void playRandom()
{
    PlayerUniform ai0;
    PlayerUniform ai1;
    ListenerPrinter printer;

    GameRunner runner({&ai0, &ai1}, {&printer});

    runner.playGame();
}

int main()
{
    setSeed(time(nullptr));

    playRandom();

    return 0;
}
