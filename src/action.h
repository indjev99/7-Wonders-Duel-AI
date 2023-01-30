#pragma once

#define ACT_MOVE_PLAY_PYRAMID_CARD 0 // id, {wonderId | ACT_ARG2_BUILD | ACT_ARG2_DISCARD}
#define ACT_MOVE_BUILD_GAME_TOKEN 1 // id
#define ACT_MOVE_BUILD_BOX_TOKEN 2 // id
#define ACT_MOVE_BUILD_DISCARDED 3 // id
#define ACT_MOVE_DESTROY_OBJECT 4 // id, type (type must match expected type)
#define ACT_MOVE_SELECT_WONDER 5 // id
#define ACT_MOVE_CHOOSE_START_PLAYER 6 // player

#define ACT_REVEAL_GUILD -1 // pos
#define ACT_REVEAL_PYRAMID_CARD -2 // pos, id (pos must match expected pos)
#define ACT_REVEAL_GAME_TOKEN -3 // id
#define ACT_REVEAL_BOX_TOKEN -4 // id
#define ACT_REVEAL_WONDER -5 // id
#define ACT_REVEAL_FIRST_PLAYER -6 // player

#define ACT_ARG2_BUILD -1
#define ACT_ARG2_DISCARD -2

#define ACT_INVALID -100
#define ACT_ARG_NONE -100

struct Action
{
    int type;
    int arg1;
    int arg2;

    Action(int type = ACT_INVALID, int arg1 = ACT_ARG_NONE, int arg2 = ACT_ARG_NONE);

    bool isPlayerMove() const;
};

bool operator==(const Action& left, const Action& right);
