#undef UNICODE

#include <iostream>
#include <memory>
#include <string>
#include <windows.h>

struct PipeIO
{
    PipeIO(const std::string& pipeName);

    void write(const std::string& s);

    std::string read();

private:

    static std::unique_ptr<char[]> toChars(const std::string& str);

    static constexpr int BUF_SIZE = 4096;

    HANDLE pipeHandle;
};
