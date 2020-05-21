
#ifndef _STREAM_WRITER_H_INCLUDE
#define _STREAM_WRITER_H_INCLUDE

#include "sockets-config.h"
#include <string>

namespace dai {

class IStream;

class StreamWriter
{
public:
    StreamWriter(IStream& stream);
    virtual ~StreamWriter() {}

    StreamWriter& operator<<(const char *);
    StreamWriter& operator<<(const std::string&);
    StreamWriter& operator<<(short);
    StreamWriter& operator<<(int);
    StreamWriter& operator<<(long);
    StreamWriter& operator<<(double);

private:
    IStream& m_stream;

};

}//namespace dai

#endif // _STREAM_WRITER_H_INCLUDE
