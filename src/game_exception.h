#pragma once

#include <exception>
#include <string>
#include <vector>

struct GameException : std::exception
{
    std::string text;

    GameException(const std::string& descr, const std::vector<std::pair<std::string, int>> args);

    const char* what() const noexcept override;
};
