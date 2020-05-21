
#ifndef _HTTP_RESPONSE_H_INCLUDE
#define _HTTP_RESPONSE_H_INCLUDE

#include "HttpTransaction.h"

#include <list>

namespace dai {

class IFile;

class HttpResponse : public HttpTransaction
{
public:
    HttpResponse(const std::string& version = "HTTP/1.0");
    HttpResponse(const HttpResponse& src);

    ~HttpResponse() = default;

    HttpResponse& operator=(const HttpResponse& src);

    /** HTTP/1.x */
    void SetHttpVersion(const std::string& value);

    const std::string& HttpVersion() const;

    void SetHttpStatusCode(int value);

    int HttpStatusCode() const;

    void SetHttpStatusMsg(const std::string& value);

    const std::string& HttpStatusMsg() const;

    void SetCookie(const std::string& value);

    const std::string Cookie(const std::string& name) const;

    std::list<std::string> CookieNames() const;

    void Write(const std::string& str);

    void Write(const char *buf, size_t sz);

    void Writef(const char *format, ...);

    const IFile& GetFile() const;

    IFile& GetFile();

    /** Replace memfile with file on disk, opened for read. */
    void SetFile(const std::string& path);

    /** Replace memfile with another IFile */
    void SetFile(IFile *f);

    void Reset();

private:
    std::string                    m_http_version;
    int                            m_http_status_code;
    std::string                    m_http_status_msg;
    Utility::ncmap <std::string>   m_cookie;
    mutable std::unique_ptr<IFile> m_file;

}; // end of class

}//namespace dai

#endif//_HTTP_RESPONSE_H_INCLUDE

