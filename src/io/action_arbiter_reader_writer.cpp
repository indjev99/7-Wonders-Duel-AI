#include "action_arbiter_reader_writer.h"

#include "game/game_exception.h"
#include "game/lang.h"

ActionArbiterReaderWriter::ActionArbiterReaderWriter(StringReader& reader, StringWriter& writer)
    : reader(reader)
    , writer(writer)
    , lastReadAction()
{}

void ActionArbiterReaderWriter::write(const std::string& s)
{
    Action action;
    try
    {
        action = actionFromString(s);
    }
    catch (const GameException& e) {}

    if (action.type == ACT_NONE)
    {
        writer.write(s);
        return;
    }

    if (action == lastReadAction) return;

    writer.write(actionToString(action));
}

std::string ActionArbiterReaderWriter::read()
{
    std::vector<Action> possible = game->getPossibleActions();

    for (auto it = pendingActions.begin(); it != pendingActions.end(); it++)
    {
        Action action = *it;
        if (std::find(possible.begin(), possible.end(), action) != possible.end())
        {
            pendingActions.erase(it);
            lastReadAction = action;
            return actionToString(action);
        }
    }

    while (true)
    {
        std::string s = reader.read();

        Action action;
        try
        {
            action = actionFromString(s);
        }
        catch (const GameException& e) {}

        if (action.type == ACT_NONE)
        {
            return s;
        }

        if (std::find(possible.begin(), possible.end(), action) != possible.end())
        {
            lastReadAction = action;
            return actionToString(action);
        }

        pendingActions.push_back(action);
    }
}

void ActionArbiterReaderWriter::notifyStart()
{
    pendingActions = {};
    lastReadAction = Action();
}
