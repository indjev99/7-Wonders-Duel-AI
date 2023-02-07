#pragma once

#undef UNICODE

#include "string_reader.h"
#include "string_writer.h"

#include <memory>
#include <windows.h>

struct PipeReaderWriter final : StringReader, StringWriter
{
    PipeReaderWriter(const std::string& pipeName);

    void write(const std::string& s) override;

    std::string read() override;

private:

    static std::unique_ptr<char[]> toChars(const std::string& str);

    static constexpr int BUF_SIZE = 4096;

    HANDLE pipeHandle;
};
