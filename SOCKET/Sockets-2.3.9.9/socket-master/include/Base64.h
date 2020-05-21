#ifndef _BASE64_H_INCLUDE
#define _BASE64_H_INCLUDE

#include "sockets-config.h"
#ifdef _MSC_VER
#   pragma warning(disable:4514)
#endif

#include <cstdio>
#include <string>

namespace dai {

class IFile;

/**
 * Base64 encode/decode.
 * \ingroup util
 */
class Base64
{
public:
    Base64();

    void encode(FILE *, std::string&, bool add_crlf = true);
    void encode(IFile *, std::string&, bool add_crlf = true);
    void encode(const std::string&, std::string&, bool add_crlf = true);
    void encode(const char *, size_t, std::string&, bool add_crlf = true);
    void encode(const unsigned char *, size_t, std::string&, bool add_crlf = true);

    void decode(const std::string&, std::string&);
    void decode(const std::string&, unsigned char *, size_t&);
    size_t decode_length(const std::string&);

private:
    Base64(const Base64&) {}

    Base64& operator=(const Base64&)
    {
        return *this;
    }

    static const char *bstr;
    static const char  rstr[128];
};

};// namespace dai

#endif//_BASE64_H_INCLUDE

