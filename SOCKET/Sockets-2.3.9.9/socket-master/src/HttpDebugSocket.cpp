
#ifdef _MSC_VER
#   pragma warning(disable:4786)
#endif

#include "HttpDebugSocket.h"
#include "ISocketHandler.h"

namespace dai {

HttpDebugSocket::HttpDebugSocket(ISocketHandler& h) :
    HTTPSocket(h),
    m_content_length(0),
    m_read_ptr(0)
{
}



void HttpDebugSocket::Init()
{
    if (GetParent()->GetPort() == 443)
    {
#ifdef HAVE_OPENSSL
        EnableSSL();
#else
        Handler().LogError(this, "url_this", -1, "SSL not available", LOG_LEVEL_WARNING);
#endif
    }
}


void HttpDebugSocket::OnFirst()
{
    Send(
        "HTTP/1.1 200 OK\n"
        "Content-type: text/html\n"
        "Connection: close\n"
        "Server: HttpDebugSocket/1.0\n"
        "\n");
    Send(
        "<html><head><title>Echo Request</title></head>"
        "<body><h3>Request Header</h3>");
    Send("<form method='post' action='/test_post'>"
         "<input type='text' name='text' value='test text'><br>"
         "<input type='submit' name='submit' value=' OK '></form>");

    // enctype 'multipart/form-data'
    Sendf("<form action='/test_post' method='post' enctype='multipart/form-data'>");
    Sendf("<input type=file name=the_file><br>");
    Sendf("<input type=text name=the_name><br>");
    Sendf("<input type=submit name=submit value=' test form-data '>");
    Sendf("</form>");

    Send("<pre style='background: #e0e0e0'>");
    Send(GetMethod() + " " + GetUrl() + " " + GetHttpVersion() + "\n");
}


void HttpDebugSocket::OnHeader(const std::string& key, const std::string& value)
{
    if (!strcasecmp(key.c_str(), "content-length"))
        m_content_length = atoi(value.c_str());

    Send(key + ": " + value + "\n");
}


void HttpDebugSocket::OnHeaderComplete()
{
    if (m_content_length || IsChunked())
    {
        Send("</pre><h3>Request Body</h3><pre style='background: #e0e0e0'>");
    }
    else
    {
        Send("</pre><hr></body></html>");
        SetCloseAndDelete();
    }
}


void HttpDebugSocket::OnData(const char *p, size_t l)
{
    SendBuf(p, l);
    m_read_ptr += (int) l;
    if (m_read_ptr >= m_content_length && m_content_length)
    {
        Send("</pre><hr></body></html>");
        SetCloseAndDelete();
    }
}


void HttpDebugSocket::OnDataComplete()
{
    if (!CloseAndDelete())
    {
        Send("</pre><hr></body></html>");
        SetCloseAndDelete();
    }
}


}//namespace dai

