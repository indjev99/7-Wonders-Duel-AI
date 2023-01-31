#pragma once

#include "runner/revealer.h"

#include <iostream>

struct RevealerReader final : Revealer
{
    RevealerReader(std::istream& in);

    Action getAction() override;

private:

    std::istream& in;
};
