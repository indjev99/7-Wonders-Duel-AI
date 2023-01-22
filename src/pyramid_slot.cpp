#include "pyramid_slot.h"

#include "object_location.h"

PyramidSlot::PyramidSlot(int deck, int objectId, int coveredBy)
    : deck(deck)
    , objectId(objectId)
    , coveredBy(coveredBy)
{}

PyramidSlot::PyramidSlot()
    : PyramidSlot(DECK_INVALID, SLOT_EMPTY, 0)
{}
