#pragma once

#include "revealer.h"

struct RevealerUniform final : Revealer
{
    Action getAction() override;
};
