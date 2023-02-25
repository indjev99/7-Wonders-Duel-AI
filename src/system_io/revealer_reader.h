#pragma once

#include "io/string_reader.h"
#include "system/revealer.h"

struct RevealerReader final : Revealer
{
    RevealerReader(StringReader& reader);

    Action getAction() override;

private:

    StringReader& reader;
};
