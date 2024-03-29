#pragma once

#include "system/listener.h"

struct ListenerPrettyPrinter final : Listener
{
    void notifyStart() override;
    void notifyActionPre(const Action& action) override;
    void notifyActionPost(const Action& action) override;
    void notifyEnd() override;

private:

    void printSummary();
};
