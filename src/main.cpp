#include "ai/agent_mc.h"
#include "ai/agent_uniform.h"
#include "game/results.h"
#include "gui/agent_gui.h"
#include "gui/listener_gui.h"
#include "runner/game_runner.h"
#include "runner/revealer_uniform.h"
#include "text/agent_reader.h"
#include "text/listener_writer.h"
#include "text/make_log.h"
#include "text/revealer_reader.h"
#include "utils/random.h"

#include <cmath>
#include <ctime>
#include <iostream>

void benchmark(Agent* agent1, Agent* agent2)
{
    RevealerUniform revealer;
    GameRunner runner(&revealer, {agent1, agent2});

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

void replayGame(const std::string& logName)
{
    std::ifstream log(logName.c_str());

    RevealerReader revealer(log);
    AgentReader agent1(log);
    AgentReader agent2(log);

    ListenerGUI gui(true);

    GameRunner runner(&revealer, {&agent1, &agent2}, {&gui});
    runner.playGame();
}

void playGame(Agent* agent1, Agent* agent2)
{
    std::ofstream log = makeLog();

    RevealerUniform revealer;

    ListenerGUI gui;
    ListenerWriter logger(log);

    AgentGUI pGui1(gui);
    AgentGUI pGui2(gui);

    if (agent1 == nullptr) agent1 = &pGui1;
    if (agent2 == nullptr) agent2 = &pGui2;

    GameRunner runner(&revealer, {agent1, agent2}, {&gui, &logger});
    runner.playGame();
}

int main()
{
    setSeed(time(nullptr));

    AgentUniform u1;
    AgentUniform u2;

    playGame(nullptr, &mc2);

    return 0;
}
