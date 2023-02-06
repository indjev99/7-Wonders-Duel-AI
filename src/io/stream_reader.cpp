#include "stream_reader.h"

StreamReader::StreamReader(std::istream& in)
    : in(in)
{}

std::string StreamReader::read()
{
    std::string s;
    std::getline(in, s);
    return s;
}
