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

void replayGame(const std::string& logName, bool advanceButton = true)
{
    ListenerGUI gui(advanceButton);

    std::ifstream log(logName.c_str());
    StreamReader logReader(log);

    RevealerReader revealer(logReader);
    AgentReader agent1(logReader);
    AgentReader agent2(logReader);

    GameRunner runner(&revealer, {&agent1, &agent2}, {&gui});
    runner.playGame();
}

void playGame(Agent* agent1, Agent* agent2, bool advanceButton = false)
{
    ListenerGUI gui(advanceButton);

    AgentGUI aGui1(gui);
    AgentGUI aGui2(gui);

    if (agent1 == nullptr) agent1 = &aGui1;
    if (agent2 == nullptr) agent2 = &aGui2;

    std::ofstream log = makeLog();
    StreamWriter logWriter(log);
    ListenerWriter logger(logWriter);

    RevealerUniform revealer;

    GameRunner runner(&revealer, {agent1, agent2}, {&logger, &gui});
    runner.playGame();
}

void playExternalGame(Agent* agent1, const std::string& pipeName = "//./pipe/7wdai", bool advanceButton = false)
{
    ListenerPrettyPrinter pretty;

    std::ofstream log = makeLog();
    StreamWriter logWriter(log);
    ListenerWriter logger(logWriter);

    PipeReaderWriter pipe(pipeName);

    RevealerReader revealer(pipe);
    AgentReader aPipe1(pipe);
    AgentReader agent2(pipe);
    ListenerWriter sender(pipe);

    if (agent1 == nullptr) agent1 = &aPipe1;

    GameRunner runner(&revealer, {agent1, &agent2}, {&logger, &pretty, &sender});
    runner.playGame();
}

int main()
{
    int seed = time(nullptr);

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

    // playGame(nullptr, &mctsUcb1);

    playExternalGame(nullptr);

    // benchmark(&mctsUcb1, &mctsUcb2);

    // replayGame("logs/1675521232.log");

    return 0;
}
