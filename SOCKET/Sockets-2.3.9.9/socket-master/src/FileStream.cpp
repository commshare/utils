
#include "FileStream.h"
#include "IFile.h"

namespace dai {

FileStream::FileStream(IFile& file) : m_file(file)
{
}

size_t FileStream::IStreamRead(char *buf, size_t max_sz)
{
    return m_file.fread(buf, 1, max_sz);
}

void FileStream::IStreamWrite(const char *buf, size_t sz)
{
    m_file.fwrite(buf, 1, sz);
}

}//namespace dai
