#pragma once

#include "listener.h"

struct Agent : Listener
{
    void setPlayer(int player);

    virtual Action getAction() = 0;

protected:

    int player;
};
