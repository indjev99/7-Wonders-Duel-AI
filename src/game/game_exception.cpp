#include "game_exception.h"

GameException::GameException(const std::string& descr, const std::vector<std::pair<std::string, int>> args)
{
    text += "\n" + descr;
    for (const auto& [argName, argVal] : args)
    {
        text += "\n" + argName + " = " + std::to_string(argVal);
    }
}

const char* GameException::what() const noexcept
{
    return text.data();
}
