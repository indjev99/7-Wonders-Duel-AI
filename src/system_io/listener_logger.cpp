#include "listener_logger.h"

#include "io/make_log.h"

ListenerLogger::ListenerLogger()
    : writer(log)
    , listenerWriter(writer)
{}

void ListenerLogger::notifyStart()
{
    log = makeLog();
}

std::vector<Listener*> ListenerLogger::getSubListeners()
{
    return {&listenerWriter};
}
