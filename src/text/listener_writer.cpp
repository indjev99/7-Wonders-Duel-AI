#include "listener_writer.h"

#include "game/lang.h"

ListenerWriter::ListenerWriter(std::ostream& out)
    : out(out)
{}

void ListenerWriter::notifyActionPre(const Action& action)
{
    out << actionToString(action) << std::endl;
}
