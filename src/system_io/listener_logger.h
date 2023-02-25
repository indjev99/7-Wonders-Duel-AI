#pragma once

#include "listener_writer.h"

#include "io/stream_writer.h"

#include <fstream>

struct ListenerLogger final : Listener
{
    ListenerLogger();

    void notifyStart() override;

    std::vector<Listener*> getSubListeners() override; 

private:

    std::ofstream log;
    StreamWriter writer;
    ListenerWriter listenerWriter;
};
