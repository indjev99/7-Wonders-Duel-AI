#include "revealer_reader.h"

#include "game/lang.h"

RevealerReader::RevealerReader(std::istream& in)
    : in(in)
{}

Action RevealerReader::getAction()
{
    std::string actionStr;
    std::getline(in, actionStr);
    return actionFromString(actionStr);
}
