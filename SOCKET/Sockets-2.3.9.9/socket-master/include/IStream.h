
#ifndef _I_STREAM_H_INCLUDE
#define _I_STREAM_H_INCLUDE

#include "sockets-config.h"
#include <string>

namespace dai {

class IStream
{
public:
    virtual ~IStream() {}

    /**
     * Try to read 'buf_sz' number of bytes from source.
     * @param buf
     * @param buf_sz
     * @return Number of bytes actually read
     */
    virtual size_t IStreamRead(char *buf, size_t buf_sz) = 0;

    /**
     * Write 'sz' bytes to destination
     * @param buf
     * @param sz
     */
    virtual void IStreamWrite(const char *buf, size_t sz) = 0;
};

}//namespace dai

#endif//_I_STREAM_H_INCLUDE
