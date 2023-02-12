#include "listener_possible_player_action_reader.h"

#include "game/game_exception.h"
#include "game/lang.h"

#include <algorithm>

constexpr int ACTOR_UNKNOWN = -10;
const std::string ACTOR_ACTION_SEP = ":";

ListenerPossiblePlayerActionReader::ListenerPossiblePlayerActionReader(StringReader& reader)
    : reader(reader)
{}

void ListenerPossiblePlayerActionReader::notifyStart()
{
    pendingActions.clear();
}

std::string ListenerPossiblePlayerActionReader::read()
{
    for (auto it = pendingActions.begin(); it != pendingActions.end(); it++)
    {
        auto [actor, action] = *it;
        if (isPossible(actor, action))
        {
            pendingActions.erase(it);
            return actionToString(action);
        }
    }

    while (true)
    {
        std::string s = reader.read();
        int actor = ACTOR_UNKNOWN;
        Action action;

        int sepPos = s.find(ACTOR_ACTION_SEP, 0);
        if (sepPos != (int) std::string::npos)
        {
            actor = actorFromString(s.substr(0, sepPos));
            action = actionFromString(s.substr(sepPos));
        }
        else action = actionFromString(s);

        if (isPossible(actor, action))
        {
            return actionToString(action);
        }

        pendingActions.push_back({actor, action});
    }
}

bool ListenerPossiblePlayerActionReader::isPossible(int actor, const Action& action)
{
    std::vector<Action> possible = game->getPossibleActions();
    return action.type == ACT_ABORT_GAME ||
        ((actor == ACTOR_UNKNOWN || actor == game->getCurrActor()) &&
        std::find(possible.begin(), possible.end(), action) != possible.end());
}
