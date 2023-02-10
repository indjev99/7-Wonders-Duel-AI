#include "listener_writer.h"

#include "game/lang.h"

ListenerWriter::ListenerWriter(StringWriter& writer)
    : writer(writer)
{}

void ListenerWriter::notifyActionPre(const Action& action)
{
    writer.write(actionToString(action));
}
