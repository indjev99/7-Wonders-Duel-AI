#pragma once

#include "string_reader.h"
#include "string_writer.h"

#include "runner/listener.h"

#include <vector>

struct ActionArbiterReaderWriter final : StringReader, StringWriter, Listener
{
    ActionArbiterReaderWriter(StringReader& reader, StringWriter& writer, bool notifyStartEnd);

    void write(const std::string& s) override;

    std::string read() override;

    void notifyStart() override;
    void notifyEnd() override;

private:

    StringReader& reader;
    StringWriter& writer;
    bool notifyStartEnd;

    std::vector<Action> pendingActions;
    Action lastReadAction;
};
