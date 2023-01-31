#include "ai/agent_mc.h"
#include "ai/agent_uniform.h"
#include "game/results.h"
#include "gui/agent_gui.h"
#include "gui/listener_gui.h"
#include "runner/game_runner.h"
#include "runner/listener_logger.h"
#include "utils/random.h"

#include <cmath>
#include <ctime>
#include <iostream>

void benchmark(Agent* agent1, Agent* agent2)
{
    GameRunner runner({agent1, agent2});

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

void playGame(Agent* agent1, Agent* agent2)
{
    ListenerGUI gui;
    ListenerLogger logger;

    AgentGUI pGui1(gui);
    AgentGUI pGui2(gui);

    if (agent1 == nullptr) agent1 = &pGui1;
    if (agent2 == nullptr) agent2 = &pGui2;

    GameRunner runner({agent1, agent2}, {&gui, &logger});
    runner.playGame();
}

void run()
{
    setSeed(time(nullptr));

    AgentUniform u1;
    AgentUniform u2;

    AgentMC mc1;
    AgentMC mc2;

    playGame(nullptr, &mc2);
}

int main()
{
    run();

    return 0;
}
