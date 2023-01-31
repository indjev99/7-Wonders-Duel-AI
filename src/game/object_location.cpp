#include "object_location.h"


ObjectLocation::ObjectLocation(int deck, int pos)
    : deck(deck)
    , pos(pos)
{}

ObjectLocation::ObjectLocation()
    : ObjectLocation(DECK_NONE, POS_NONE)
{}
