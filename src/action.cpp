#include "action.h"

Action::Action(int type, int arg1, int arg2)
    : type(type)
    , arg1(arg1)
    , arg2(arg2)
{}

bool Action::isPlayerMove() const
{
    return type >= 0;
}
