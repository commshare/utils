
#include "StreamWriter.h"
#include "IStream.h"
#include "Utility.h"


namespace dai {

StreamWriter::StreamWriter(IStream& stream) : m_stream(stream)
{
}

StreamWriter& StreamWriter::operator<<(const char *buf)
{
    m_stream.IStreamWrite(buf, strlen(buf));
    return *this;
}


StreamWriter& StreamWriter::operator<<(const std::string& str)
{
    m_stream.IStreamWrite(str.c_str(), str.size());
    return *this;
}


StreamWriter& StreamWriter::operator<<(short x)
{
    *this << Utility::l2string(x);
    return *this;
}


StreamWriter& StreamWriter::operator<<(int x)
{
    *this << Utility::l2string(x);
    return *this;
}


StreamWriter& StreamWriter::operator<<(long x)
{
    *this << Utility::l2string(x);
    return *this;
}


StreamWriter& StreamWriter::operator<<(double x)
{
    *this << Utility::ToString(x);
    return *this;
}

}//namespace dai
