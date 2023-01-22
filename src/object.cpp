#include "object.h"

#include "object_types.h"

Object::Object(int id, std::string name, int type, Cost cost, int score, int (*scoreFunc)(const PlayerState&), void (*effectFunc)(PlayerState&), void (*revEffectFunc)(PlayerState&))
    : id(id)
    , name(name)
    , cost(cost)
    , score(score)
    , scoreFunc(scoreFunc)
    , effectFunc(effectFunc)
    , revEffectFunc(revEffectFunc)
{}

Object::Object(int id, std::string name, int type, Cost cost)
    : Object(id, name, type, cost, 0, nullptr, nullptr, nullptr)
{}

Object::Object(int id, std::string name, int type, Cost cost, void (*effectFunc)(PlayerState&))
    : Object(id, name, type, cost, 0, nullptr, effectFunc, nullptr)
{}

Object::Object(int id, std::string name, int type, Cost cost, int score)
    : Object(id, name, type, cost, score, nullptr, nullptr, nullptr)
{}

Object::Object(int id, std::string name, int type, Cost cost, int score, void (*effectFunc)(PlayerState&))
    : Object(id, name, type, cost, score, nullptr, effectFunc, nullptr)
{}

Object::Object(int id, std::string name, int type, Cost cost, int (*scoreFunc)(const PlayerState&))
    : Object(id, name, type, cost, 0, scoreFunc, nullptr, nullptr)
{}

Object::Object(int id, std::string name, int type, Cost cost, int (*scoreFunc)(const PlayerState&), void (*effectFunc)(PlayerState&))
    : Object(id, name, type, cost, 0, scoreFunc, effectFunc, nullptr)
{}

Object::Object(int id, std::string name, int type, Cost cost, void (*effectFunc)(PlayerState&), void (*revEffectFunc)(PlayerState&))
    : Object(id, name, type, cost, 0, nullptr, effectFunc, revEffectFunc)
{}

Object::Object()
    : Object(-1, "INVALID", -1, Cost(0), 0)
{}
