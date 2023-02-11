#pragma once

#include "action.h"

#include <string>

std::string ageToString(int age);
std::string actorToString(int actor);
std::string actionToString(const Action& action);
std::string resultToString(int res);

int objectFromString(const std::string& name);
int actorFromString(const std::string& actorStr);
Action actionFromString(const std::string& actionStr);
