#ifndef _FILE_H_INCLUDE
#define _FILE_H_INCLUDE

#include "sockets-config.h"
#include "IFile.h"

#include <cstdio>

namespace dai {

/**
 * IFile implementation of a disk file.
 * \ingroup file
 */
class File : public IFile
{
public:
    File();
    File(FILE *);

    /**
     * convenience: calls fopen()
     */
    File(const std::string& path, const std::string& mode);
    ~File();

    bool fopen(const std::string& path, const std::string& mode);
    void fclose() const;

    size_t fread(char *, size_t, size_t) const;
    size_t fwrite(const char *, size_t, size_t);

    char *fgets(char *, int) const;
    void fprintf(const char *format, ...);
    off_t size() const;
    bool eof() const;
    void reset_read() const;
    void reset_write();
    const std::string& Path() const;

private:
    // copy constructor
    File(const File&) {}

    // assignment operator
    File& operator=(const File&)
    {
        return *this;
    }

    std::string m_path;
    std::string m_mode;
    mutable FILE *m_fil;
    bool m_b_close;
    mutable long m_rptr;
    long m_wptr;
};


}//namespace dai

#endif//_FILE_H_INCLUDE

