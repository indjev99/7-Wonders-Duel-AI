#include "listener_start_end_notifier.h"

ListenerStartEndNotifier::ListenerStartEndNotifier(StringWriter& writer)
    : writer(writer)
{}

void ListenerStartEndNotifier::notifyStart()
{
    writer.write("Start game");
}

void ListenerStartEndNotifier::notifyEnd()
{
    writer.write("End game");
}
