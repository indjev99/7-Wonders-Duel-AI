#pragma once

#include "io/string_reader.h"
#include "runner/revealer.h"

#include <iostream>

struct RevealerReader final : Revealer
{
    RevealerReader(StringReader& reader);

    Action getAction() override;

private:

    StringReader& reader;
};
