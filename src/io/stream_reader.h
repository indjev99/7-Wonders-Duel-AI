#pragma once

#include "string_reader.h"

#include <iostream>

struct StreamReader final : StringReader
{
    StreamReader(std::istream& in);

    std::string read() override;

private:

    std::istream& in;
};
