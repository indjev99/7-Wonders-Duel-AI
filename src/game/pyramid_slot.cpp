#include "pyramid_slot.h"

#include "object.h"
#include "object_location.h"

PyramidSlot::PyramidSlot(int deck, int objectId, int coveredBy)
    : deck(deck)
    , objectId(objectId)
    , coveredBy(coveredBy)
    , playablePos(POS_NONE)
{}

PyramidSlot::PyramidSlot()
    : PyramidSlot(DECK_NONE, OBJ_NONE, 0)
{}
