#pragma once

#include "io/string_reader.h"
#include "system/listener.h"

#include <vector>

struct ListenerPossiblePlayerActionReader final : Listener, StringReader
{
    ListenerPossiblePlayerActionReader(StringReader& reader);

    std::string read() override;

    void notifyStart() override;

private:

    bool isPossible(int actor, const Action& action);

    StringReader& reader;

    std::vector<std::pair<int, Action>> pendingActions;
};
