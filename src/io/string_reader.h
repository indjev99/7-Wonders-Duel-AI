#pragma once

#include <string>

struct StringReader
{
    virtual std::string read() = 0;

    virtual ~StringReader() = default;
};
