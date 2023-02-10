#include "revealer_reader.h"

#include "game/lang.h"

RevealerReader::RevealerReader(StringReader& reader)
    : reader(reader)
{}

Action RevealerReader::getAction()
{
    return actionFromString(reader.read());
}
