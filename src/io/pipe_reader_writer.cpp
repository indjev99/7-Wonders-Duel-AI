#include "pipe_reader_writer.h"

#include <iostream>

PipeReaderWriter::PipeReaderWriter(const std::string& pipeName)
{
    std::unique_ptr<char[]> pipeNameChars = toChars(pipeName);

    pipeHandle = CreateNamedPipe(
        pipeNameChars.get(),
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        1,
        BUF_SIZE,
        BUF_SIZE,
        0,
        NULL);

    if (pipeHandle == INVALID_HANDLE_VALUE)
    {
        std::cerr << "Failed CreateNamedPipe" << std::endl;
        exit(-1);
    }

    ConnectNamedPipe(pipeHandle, NULL);
}

void PipeReaderWriter::write(const std::string& s)
{
    std::unique_ptr<char[]> sChars = toChars(s);

    std::cerr << "< " << s << std::endl << std::endl;

    DWORD bytesWritten;
    bool succ = WriteFile( 
        pipeHandle,
        sChars.get(),
        s.size(),
        &bytesWritten,
        NULL);

    if (!succ || bytesWritten != s.size())
    {
        std::cerr << "Failed WriteFile" << std::endl;
        exit(-1);
    }
}

std::string PipeReaderWriter::read()
{
    std::unique_ptr<char[]> response = std::make_unique<char[]>(BUF_SIZE);

    DWORD bytesRead;
    bool succ = ReadFile(
        pipeHandle,
        response.get(),
        BUF_SIZE,
        &bytesRead,
        NULL);

    if (!succ)
    {
        std::cerr << "Failed ReadFile" << std::endl;
        exit(-1);
    }

    std::string s;
    s.assign(response.get(), bytesRead);

    std::cerr << "> " << s << std::endl << std::endl;

    return s;
}

std::unique_ptr<char[]> PipeReaderWriter::toChars(const std::string& str)
{
    std::unique_ptr<char[]> chars = std::make_unique<char[]>(str.size() + 1);
    strcpy(chars.get(), str.c_str());
    return chars;
}
