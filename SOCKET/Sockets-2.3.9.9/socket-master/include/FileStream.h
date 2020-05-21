
#ifndef _FILE_STREAM_H_INCLUDE
#define _FILE_STREAM_H_INCLUDE

#include "IStream.h"

namespace dai {

class IFile;

class FileStream : public IStream
{
public:
    FileStream(IFile& file);

    size_t IStreamRead(char *buf, size_t max_sz);
    void IStreamWrite(const char *buf, size_t sz);

private:
    IFile& m_file;
};

}//namespace dai

#endif//_FILE_STREAM_H_INCLUDE
