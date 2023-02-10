#pragma once

#include "io/string_reader.h"
#include "system/listener.h"

#include <vector>

struct ListenerPossibleActionReader final : Listener, StringReader
{
    ListenerPossibleActionReader(StringReader& reader);

    std::string read() override;

    void notifyStart() override;

private:

    StringReader& reader;

    std::vector<Action> pendingActions;
};
