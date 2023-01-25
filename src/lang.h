#pragma once

#include "action.h"

#include <string>

#define ACT_UNDO -10

std::string actorToString(int actor);
std::string actionToString(const Action& action);
std::string resultToString(int res);

int objectFromString(const std::string& name);
Action actionFromString(const std::string& actionStr);
