#pragma once

#include "cost.h"

#include <string>

struct PlayerState;

#define OBJ_NONE -100

struct Object
{
    int id;
    std::string name;

    int type;
    Cost cost;
    int score;
    int (*scoreFunc)(const PlayerState&);
    void (*effectFunc)(PlayerState&);
    void (*revEffectFunc)(PlayerState&);

    Object(int id, std::string name, int type, Cost cost, int score, int (*scoreFunc)(const PlayerState&), void (*effectFunc)(PlayerState&), void (*revEffectFunc)(PlayerState&));
    Object(int id, std::string name, int type, Cost cost);
    Object(int id, std::string name, int type, Cost cost, void (*effectFunc)(PlayerState&));
    Object(int id, std::string name, int type, Cost cost, int score);
    Object(int id, std::string name, int type, Cost cost, int score, void (*effectFunc)(PlayerState&));
    Object(int id, std::string name, int type, Cost cost, int (*scoreFunc)(const PlayerState&));
    Object(int id, std::string name, int type, Cost cost, int (*scoreFunc)(const PlayerState&), void (*effectFunc)(PlayerState&));
    Object(int id, std::string name, int type, Cost cost, void (*effectFunc)(PlayerState&), void (*revEffectFunc)(PlayerState&));
    Object();
};
