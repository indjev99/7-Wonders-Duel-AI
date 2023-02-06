#include "stream_writer.h"

StreamWriter::StreamWriter(std::ostream& out)
    : out(out)
{}

void StreamWriter::write(const std::string& s)
{
    out << s << std::endl;
}
