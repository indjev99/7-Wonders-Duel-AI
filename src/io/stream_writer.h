#pragma once

#include "string_writer.h"

#include <iostream>

struct StreamWriter final : StringWriter
{
    StreamWriter(std::ostream& out);

    void write(const std::string& s) override;

private:

    std::ostream& out;
};
