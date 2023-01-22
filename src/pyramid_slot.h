#pragma once

#define SLOT_EMPTY -2
#define SLOT_UNREVEALED -1

struct PyramidSlot
{
    int deck;
    int objectId;
    int coveredBy;

    PyramidSlot(int deck, int objectId, int coveredBy);
    PyramidSlot();
};
