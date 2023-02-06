#pragma once

#include <string>

struct StringWriter
{
    virtual void write(const std::string& s) = 0;

    virtual ~StringWriter() = default;
};
