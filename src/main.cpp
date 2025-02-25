#include "agents/agent_mc.h"
#include "agents/agent_mcts.h"
#include "agents/agent_mcts_blind_reveals.h"
#include "agents/agent_uniform.h"
#include "agents/game_simulator.h"
#include "game/game_state.h"
#include "game/lang.h"
#include "game/results.h"
#include "gui/agent_gui.h"
#include "gui/listener_gui.h"
#include "io/pipe_reader_writer.h"
#include "io/stream_reader.h"
#include "io/stream_writer.h"
#include "system/agent_ignorer_wrapper.h"
#include "system/game_runner.h"
#include "system/revealer_uniform.h"
#include "system_io/agent_writer_wrapper.h"
#include "system_io/listener_logger.h"
#include "system_io/listener_possible_player_action_reader.h"
#include "system_io/listener_start_end_notifier.h"
#include "system_io/agent_reader.h"
#include "system_io/listener_pretty_printer.h"
#include "system_io/listener_writer.h"
#include "system_io/revealer_reader.h"
#include "utils/random.h"
#include "utils/timer.h"

#include <cmath>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <map>

void benchmark(Agent* agent1, Agent* agent2)
{
    RevealerUniform revealer;
    GameRunner runner(&revealer, {agent1, agent2});

    int cnt = 0;
    int sum = 0;
    int sumSq = 0;

    std::map<int, int> resCnts;

    while (true)
    {
        int res = runner.playGame();
        int resSign = resultSign(res);

        cnt++;
        sum += resSign;
        sumSq += resSign * resSign;
        resCnts[res]++;

        double mean = (double) sum / cnt;
        double meanStd = sqrt(((double) sumSq / cnt - mean * mean) / std::max(1, cnt - 1));

        std::cout << std::setprecision(2) << std::fixed;

        std::cout << cnt << ": " << mean << " +- " << meanStd << "  (";
        for (int res = RESULT_WIN_CIVILIAN; res <= RESULT_WIN_TIEBREAK; res++)
        {
            std::cout << " " << (double) resCnts[res] / cnt;
        }
        std::cout << " |";
        for (int res = RESULT_WIN_CIVILIAN; res <= RESULT_WIN_TIEBREAK; res++)
        {
            std::cout << " " << (double) resCnts[-res] / cnt;
        }
        std::cout << " )  --  " << totalNumSims / cnt << std::endl;
    }
}

void replayGame(const std::string& logName, Agent* agent1 = nullptr, Agent* agent2 = nullptr, bool advanceButton = true)
{
    ListenerGUI gui(advanceButton);

    std::ifstream log(logName.c_str());
    StreamReader logReader(log);

    RevealerReader revealer(logReader);
    AgentReader agent1Base(logReader);
    AgentReader agent2Base(logReader);

    std::vector<Agent*> ignored1;
    std::vector<Agent*> ignored2;

    if (agent1 != nullptr) ignored1.push_back(agent1);
    if (agent2 != nullptr) ignored2.push_back(agent2);

    AgentIgnorerWrapper agent1Ignorer(&agent1Base, ignored1);
    AgentIgnorerWrapper agent2Ignorer(&agent2Base, ignored2);

    GameRunner runner(&revealer, {&agent1Ignorer, &agent2Ignorer}, {&gui});
    runner.playGame();
}

void playGame(Agent* agent1, Agent* agent2, bool advanceButton = false)
{
    ListenerGUI gui(advanceButton);

    AgentGUI defAgent1(gui);
    AgentGUI defAgent2(gui);

    if (agent1 == nullptr) agent1 = &defAgent1;
    if (agent2 == nullptr) agent2 = &defAgent2;

    ListenerLogger logger;

    RevealerUniform revealer;

    GameRunner runner(&revealer, {agent1, agent2}, {&logger, &gui});
    runner.playGame();
}

void playExternalGame(Agent* agent1, const std::string& pipeName = "//./pipe/7wdai", bool advanceButton = false)
{
    ListenerPrettyPrinter pretty;

    ListenerLogger logger;

    PipeReaderWriter pipe(pipeName);

    ListenerStartEndNotifier startEndNotifier(pipe);
    ListenerPossiblePlayerActionReader possibleReader(pipe);

    RevealerReader revealer(possibleReader);
    AgentReader agent1Base(possibleReader);
    AgentReader agent2(possibleReader);

    AgentWriterWrapper agent1Writer(agent1, pipe);

    std::vector<Agent*> ignored;
    if (agent1 != nullptr) ignored.push_back(&agent1Writer);

    AgentIgnorerWrapper agent1Ignorer(&agent1Base, ignored);

    GameRunner runner(&revealer, {&agent1Ignorer, &agent2}, {&logger, &pretty, &startEndNotifier, &possibleReader});

    while (true)
    {
        runner.playGame();
    }
}

void simulateAfter(const std::string& logName)
{
    GameState game;
    game.reset();

    // ListenerGUI gui(true);
    // gui.setGame(game);

    std::ifstream log(logName.c_str());

    while (log.good())
    {
        std::string actionStr;
        std::getline(log, actionStr);
        if (actionStr == "") break;

        Action action = actionFromString(actionStr);

        game.doAction(action);
    }

    GameState backup(&game);

    MCConfig config;
    config.simLookAheadWonders = false;
    config.simModes = {SIM_MODE_NORMAL, SIM_MODE_SCIENCE};

    int totalRes = 0;
    int numGames = 0;

    DO_FOR_SECS(1)
    {
        game.clone(&backup);
        GameStateFast runGame(&backup);
        GameSimulator sim(runGame, config);

        // gui.notifyStart();

        while (!runGame.isTerminal())
        {
            Action action = sim.getAction();

            // gui.notifyActionPre(action);
            game.doAction(action);
            runGame.doAction(action);
            // gui.notifyActionPost(action);
        }

        int res = game.getResult(0);
        totalRes += resultSign(res);
        numGames += 1;

        // std::cout << numGames << " : " << (float) totalRes / numGames << " with " << res << std::endl;

        // gui.notifyEnd();
    }

    std::cout << numGames << " : " << (float) totalRes / numGames << std::endl;
}

int main()
{
    int seed = time(nullptr);

    std::cerr << "SEED: " << seed << std::endl;

    setSeed(seed);

    MCConfig config1;
    config1.secsPerMove = 9;
    config1.verbosity = 1;
    config1.testMode = true;

    MCConfig config2;
    config2.secsPerMove = 3;
    config2.verbosity = 1;

    AgentMcts mcts1(config1);
    AgentMcts mcts2(config2);

    AgentMctsBlindReveals mctsBR1(config1);
    AgentMctsBlindReveals mctsBR2(config2);

    playGame(nullptr, &mctsBR2);

    // playExternalGame(&mctsBR1);

    // benchmark(&mctsBR1, &mctsBR2);

    // replayGame("science_loss.log");

    // simulateAfter("science_loss.log");

    return 0;
}
