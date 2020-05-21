
#ifdef _WIN32
#   ifdef _MSC_VER
#       pragma warning(disable:4786)
#   endif
#else
#   include <cerrno>
#endif

#include <sys/types.h>
#include <sys/stat.h>

#include "ISocketHandler.h"
#include "File.h"
#include "HttpPutSocket.h"


namespace dai {


HttpPutSocket::HttpPutSocket(ISocketHandler& h) :
    HttpClientSocket(h)
{
}


HttpPutSocket::HttpPutSocket(ISocketHandler& h, const std::string& url_in) :
    HttpClientSocket(h, url_in)
{
}


void HttpPutSocket::SetFile(const std::string& file)
{
    struct stat st;
    if (!stat(file.c_str(), &st))
    {
        m_filename = file;
        m_content_length = st.st_size;
    }
    else
    {
        Handler().LogError(this, "SetFile", Errno, StrError(Errno), LOG_LEVEL_FATAL);
        SetCloseAndDelete();
    }
}


void HttpPutSocket::SetContentType(const std::string& type)
{
    m_content_type = type;
}


void HttpPutSocket::Open()
{
    // why do I have to specify TcpSocket:: to get to the Open() method??
    TcpSocket::Open(GetUrlHost(), GetUrlPort());
}


void HttpPutSocket::OnConnect()
{
    SetMethod("PUT");
    SetHttpVersion("HTTP/1.1");
    AddResponseHeader("Host", GetUrlHost());
    AddResponseHeader("Content-type", m_content_type);
    AddResponseHeader("Content-length", Utility::l2string(m_content_length));
    AddResponseHeader("User-agent", MyUseragent());
    SendRequest();

    std::unique_ptr<IFile> fil = std::unique_ptr<IFile>(new File);
    if (fil->fopen(m_filename, "rb"))
    {
        size_t n;
        char buf[32768];
        while ((n = fil->fread(buf, 1, 32768)) > 0)
        {
            SendBuf(buf, n);
        }
        fil->fclose();
    }
}


}//namespace dai

