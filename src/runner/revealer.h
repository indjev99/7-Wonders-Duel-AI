#pragma once

#include "listener.h"

struct Revealer : Listener
{
    virtual Action getAction() = 0;
};
