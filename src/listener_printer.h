#pragma once

#include "listener.h"

struct ListenerPrinter final : Listener
{
    void notifyStart() override;
    void notifyActionPre(const Action& action) override;
    void notifyEnd() override;

private:

    int currAge;

    void printSummary();
};
