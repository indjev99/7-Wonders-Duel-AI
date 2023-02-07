#pragma once

#include "string_reader.h"
#include "string_writer.h"

#include "runner/listener.h"

#include <vector>

struct ActionArbiterReaderWriter final : StringReader, StringWriter, Listener
{
    ActionArbiterReaderWriter(StringReader& reader, StringWriter& writer);

    void write(const std::string& s) override;

    std::string read() override;

    void notifyStart() override;

private:

    StringReader& reader;
    StringWriter& writer;

    std::vector<Action> pendingActions;
    Action lastReadAction;
};
