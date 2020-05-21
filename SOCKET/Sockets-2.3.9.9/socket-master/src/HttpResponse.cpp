
#ifdef _MSC_VER
#   pragma warning(disable:4786)
#endif
#include <cstdarg>
#include <cstdio>

#include "HttpResponse.h"
#include "HttpRequest.h"
#include "MemFile.h"
#include "File.h"
#include "Parse.h"

namespace dai {

#ifdef _DEBUG
#   define DEB(x) x; fflush(stderr);
#else
#   define DEB(x)
#endif


// --------------------------------------------------------------------------------------
HttpResponse::HttpResponse(const std::string& version):
    HttpTransaction(),
    m_http_version(version),
    m_http_status_code(0),
    m_file(new MemFile)
{
}


// --------------------------------------------------------------------------------------
HttpResponse::HttpResponse(const HttpResponse& src) :
    HttpTransaction(src),
    m_http_version(src.m_http_version),
    m_http_status_code(src.m_http_status_code),
    m_http_status_msg(src.m_http_status_msg),
    m_cookie(src.m_cookie),
    m_file(std::move(src.m_file))
{
}


// --------------------------------------------------------------------------------------
HttpResponse& HttpResponse::operator=(const HttpResponse& src)
{
    m_http_version = src.m_http_version;
    m_http_status_code = src.m_http_status_code;
    m_http_status_msg = src.m_http_status_msg;
    m_cookie = src.m_cookie;
    m_file   = std::move(src.m_file);

    HttpTransaction::operator=(src);

    return *this;
}


// --------------------------------------------------------------------------------------
void HttpResponse::SetHttpVersion(const std::string& value)
{
    m_http_version = value;
}


// --------------------------------------------------------------------------------------
const std::string& HttpResponse::HttpVersion() const
{
    return m_http_version;
}


// --------------------------------------------------------------------------------------
void HttpResponse::SetHttpStatusCode(int value)
{
    m_http_status_code = value;
}


int HttpResponse::HttpStatusCode() const
{
    return m_http_status_code;
}


// --------------------------------------------------------------------------------------
void HttpResponse::SetHttpStatusMsg(const std::string& value)
{
    m_http_status_msg = value;
}


const std::string& HttpResponse::HttpStatusMsg() const
{
    return m_http_status_msg;
}


// --------------------------------------------------------------------------------------
void HttpResponse::SetCookie(const std::string& value)
{
    Parse pa(value, "=");
    std::string name = pa.getword();
    m_cookie[name] = value;
    DEB(fprintf(stderr, "HttpResponse::Set-Cookie<%s>: %s\n", name.c_str(), value.c_str());)
}


const std::string HttpResponse::Cookie(const std::string& name) const
{
    auto it = m_cookie.find(name);
    DEB(fprintf(stderr, "HttpResponse; get value of Cookie<%s>: ", name.c_str());)
    if (it != m_cookie.end())
    {
        DEB(fprintf(stderr, "%s\n", it->second.c_str());)
        return it->second;
    }
    DEB(fprintf(stderr, "\n");)
    return "";
}


std::list<std::string> HttpResponse::CookieNames() const
{
    std::list<std::string> vec;
    DEB(fprintf(stderr, "HttpResponse::CookieNames; ");)
    for (const auto& it : m_cookie)
    {
        DEB(fprintf(stderr, " %s", it->first.c_str());)
        vec.push_back(it.first);
    }
    DEB(fprintf(stderr, "\n");)
    return vec;
}


// --------------------------------------------------------------------------------------
void HttpResponse::Write(const std::string& str)
{
    Write(str.c_str(), str.size());
}


// --------------------------------------------------------------------------------------
void HttpResponse::Write(const char *buf, size_t sz)
{
    m_file->fwrite(buf, 1, sz);
}


// --------------------------------------------------------------------------------------
void HttpResponse::Writef(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    char tmp[10000];
    vsnprintf(tmp, sizeof(tmp), format, ap);
    va_end(ap);
    m_file->fwrite(tmp, 1, strlen(tmp));
}


// --------------------------------------------------------------------------------------
const IFile& HttpResponse::GetFile() const
{
    return *m_file;
}


// --------------------------------------------------------------------------------------
IFile& HttpResponse::GetFile()
{
    return *m_file;
}


// --------------------------------------------------------------------------------------
void HttpResponse::SetFile(const std::string& path)
{
    m_file = std::unique_ptr<IFile>(new File);
    m_file->fopen(path, "rb");
}


// --------------------------------------------------------------------------------------
void HttpResponse::SetFile(IFile *f)
{
    m_file = std::unique_ptr<IFile>(f);
}


// --------------------------------------------------------------------------------------
void HttpResponse::Reset()
{
    HttpTransaction::Reset();
    m_http_version = "";
    m_http_status_code = 0;
    m_http_status_msg = "";
    while (!m_cookie.empty())
    {
        m_cookie.erase(m_cookie.begin());
    }
    m_file = std::unique_ptr<IFile>(new MemFile);
}


}//namespace dai
