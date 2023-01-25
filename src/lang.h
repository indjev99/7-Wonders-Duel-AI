#pragma once

#include "action.h"

#include <string>

std::string actorToString(int actor);
int objectFromString(const std::string& name);
std::string actionToString(const Action& action);
Action actionFromString(const std::string& actionStr);
