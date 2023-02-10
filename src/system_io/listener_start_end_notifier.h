#pragma once

#include "io/string_writer.h"
#include "system/listener.h"

#include <vector>

struct ListenerStartEndNotifier final : Listener
{
    ListenerStartEndNotifier(StringWriter& writer);

    void notifyStart() override;
    void notifyEnd() override;

private:

    StringWriter& writer;
};
