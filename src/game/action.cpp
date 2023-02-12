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

bool operator==(const Action& left, const Action& right)
{
    return left.type == right.type && left.arg1 == right.arg1 && left.arg2 == right.arg2;
}

bool operator!=(const Action& left, const Action& right)
{
    return !(left == right);
}
