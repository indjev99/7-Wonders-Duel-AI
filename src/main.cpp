#include "ai/agent_mc.h"
#include "ai/agent_mc_ucb.h"
#include "ai/agent_mcts_ucb.h"
#include "ai/agent_uniform.h"
#include "game/results.h"
#include "gui/agent_gui.h"
#include "gui/listener_gui.h"
#include "io/make_log.h"
#include "io/pipe_reader_writer.h"
#include "io/stream_reader.h"
#include "io/stream_writer.h"
#include "runner/game_runner.h"
#include "runner/revealer_uniform.h"
#include "text/agent_reader.h"
#include "text/listener_pretty_printer.h"
#include "text/listener_writer.h"
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
    StreamReader logReader(log);

    RevealerReader revealer(logReader);
    AgentReader agent1(logReader);
    AgentReader agent2(logReader);

    ListenerGUI gui(true);

    ListenerPrettyPrinter pretty;

    GameRunner runner(&revealer, {&agent1, &agent2}, {&gui});
    runner.playGame();
}

void playGame(Agent* agent1, Agent* agent2, bool advanceButton = false)
{
    std::ofstream log = makeLog();
    StreamWriter logWriter(log);

    RevealerUniform revealer;

    ListenerWriter logger(logWriter);
    ListenerGUI gui(advanceButton);

    AgentGUI pGui1(gui);
    AgentGUI pGui2(gui);

    if (agent1 == nullptr) agent1 = &pGui1;
    if (agent2 == nullptr) agent2 = &pGui2;

    GameRunner runner(&revealer, {agent1, agent2}, {&logger, &gui});
    runner.playGame();
}

int main()
{
    int seed = time(nullptr);
    // int seed = 1675537839;

    std::cerr << "SEED: " << seed << std::endl;

    setSeed(seed);

    AgentUniform u1;
    AgentUniform u2;

    AgentMc mc1;
    AgentMc mc2;

    AgentMcUcb mcUcb1;
    AgentMcUcb mcUcb2;

    AgentMctsUcb mctsUcb1;
    AgentMctsUcb mctsUcb2;

    playGame(nullptr, &mctsUcb1);

    // benchmark(&mctsUcb1, &mctsUcb2);

    // replayGame("logs/1675521232.log");

    return 0;
}
