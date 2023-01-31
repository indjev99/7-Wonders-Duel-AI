#pragma once

#include "runner/listener.h"

#include <iostream>

struct ListenerWriter final : Listener
{
    ListenerWriter(std::ostream& out);

    void notifyActionPre(const Action& action) override;

private:

    std::ostream& out;
};
