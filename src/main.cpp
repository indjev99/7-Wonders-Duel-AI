#include "game_exception.h"
#include "game_runner.h"
#include "listener_gui.h"
#include "listener_printer.h"
#include "player_mc.h"
#include "player_uniform.h"
#include "random.h"
#include "results.h"

#include <cmath>
#include <ctime>
#include <iostream>

void playPrint(PlayerAI& ai1, PlayerAI& ai2)
{
    ListenerGUI gui;
    ListenerPrinter printer;
    GameRunner runner({&ai1, &ai2}, {&gui, &printer});
    runner.playGame();
}

void benchmark(PlayerAI& ai1, PlayerAI& ai2)
{
    GameRunner runner({&ai1, &ai2});

    int cnt = 0;
    int sum = 0;
    int sumSq = 0;

    while (true)
    {
        int res = runner.playGame();
        int resSign = resultSign(res);

        cnt++;
        sum += resSign;
        sumSq += resSign * resSign;

        double mean = (double) sum / cnt;
        double meanStd = sqrt(((double) sumSq / cnt - mean * mean) / std::max(1, cnt - 1));

        std::cout << cnt << ": " << mean << " +- " << meanStd << std::endl;
    }
}

void run()
{
    setSeed(time(nullptr));

    PlayerUniform u1;
    PlayerUniform u2;

    PlayerMC mc1(2000);
    PlayerMC mc2(2000);

    playPrint(mc1, mc2);
}

int main()
{
    run();

    return 0;
}
