
#ifdef _WIN32
#   ifdef _MSC_VER
#       pragma warning(disable:4786)
#   endif
#else
#   include <cerrno>
#endif

#include "ISocketHandler.h"
#include "HttpGetSocket.h"


namespace dai {


HttpGetSocket::HttpGetSocket(ISocketHandler& h) : HttpClientSocket(h)
{
}


HttpGetSocket::HttpGetSocket(ISocketHandler& h, const std::string& url_in, const std::string& to_file, bool connect)
    : HttpClientSocket(h, url_in)
{
    if (to_file.size())
    {
        SetFilename(to_file);
    }
    if (connect)
        DoConnect();
}


void HttpGetSocket::DoConnect()
{
    DoConnect(GetUrlHost(), GetUrlPort());
}


void HttpGetSocket::DoConnect(const std::string& host, unsigned short port)
{
    if (!Open(host, port))
    {
        if (!Connecting())
        {
            Handler().LogError(this, "HttpGetSocket", -1, "connect() failed miserably", LOG_LEVEL_FATAL);
            SetCloseAndDelete();
        }
    }
}


HttpGetSocket::HttpGetSocket(ISocketHandler& h, const std::string& host, port_t port, const std::string& url,
                             const std::string& to_file, bool connect) : HttpClientSocket(h, host, port, url)
{
    SetUrl(url);
    if (to_file.size())
    {
        SetFilename(to_file);
    }
    if (connect)
        DoConnect(host, port);
}


void HttpGetSocket::OnConnect()
{
    SetMethod("GET");
    AddResponseHeader("Accept",
                      "text/xml,application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8,video/x-mng,image/png,image/jpeg,image/gif;q=0.2,*/*;q=0.1");
    AddResponseHeader("Accept-Language", "en-us,en;q=0.5");
    AddResponseHeader("Accept-Encoding", "gzip,deflate");
    AddResponseHeader("Accept-Charset", "ISO-8859-1,utf-8;q=0.7,*;q=0.7");
    AddResponseHeader("User-agent", MyUseragent());

    if (GetUrlPort() != 80 && GetUrlPort() != 443)
        AddResponseHeader("Host", GetUrlHost() + ":" + Utility::l2string(GetUrlPort()));
    else
        AddResponseHeader("Host", GetUrlHost());
    SendRequest();
}


}//namespace dai
