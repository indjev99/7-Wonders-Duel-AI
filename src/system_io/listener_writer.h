#pragma once

#include "io/string_writer.h"
#include "system/listener.h"

#include <iostream>

struct ListenerWriter final : Listener
{
    ListenerWriter(StringWriter& writer);

    void notifyActionPre(const Action& action) override;

private:

    StringWriter& writer;
};