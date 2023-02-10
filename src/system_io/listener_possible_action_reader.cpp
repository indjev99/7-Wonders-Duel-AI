#include "listener_possible_action_reader.h"

#include "game/game_exception.h"
#include "game/lang.h"

ListenerPossibleActionReader::ListenerPossibleActionReader(StringReader& reader)
    : reader(reader)
{}

std::string ListenerPossibleActionReader::read()
{
    std::vector<Action> possible = game->getPossibleActions();

    for (auto it = pendingActions.begin(); it != pendingActions.end(); it++)
    {
        Action action = *it;
        if (std::find(possible.begin(), possible.end(), action) != possible.end())
        {
            pendingActions.erase(it);
            return actionToString(action);
        }
    }

    while (true)
    {
        std::string s = reader.read();
        Action action = actionFromString(s);;

        if (std::find(possible.begin(), possible.end(), action) != possible.end())
        {
            return actionToString(action);
        }

        pendingActions.push_back(action);
    }
}
