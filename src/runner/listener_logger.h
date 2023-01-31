#pragma once

#include "listener.h"

#include <iostream>
#include <fstream>

struct ListenerLogger final : Listener
{
    ListenerLogger();
    ListenerLogger(std::ostream& out);

    void notifyActionPre(const Action& action) override;

private:

    std::ofstream defaultOutFile;
    std::ostream& out;
};
