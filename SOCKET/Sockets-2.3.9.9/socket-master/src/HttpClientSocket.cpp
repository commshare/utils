
#ifdef _MSC_VER
#   pragma warning(disable:4786)
#endif

#include "HttpClientSocket.h"
#include "StdLog.h"
#include "ISocketHandler.h"
#include "File.h"


namespace dai {

HttpClientSocket::HttpClientSocket(ISocketHandler& h):
    HTTPSocket(h),
    m_data_ptr(nullptr),
    m_data_size(0),
    m_content_length(0),
    m_content_length_is_set(false),
    m_data_ptr_set(false),
    m_fil(nullptr),
    m_content_ptr(0),
    m_b_complete(false),
    m_b_close_when_complete(false)
{
}

HttpClientSocket::HttpClientSocket(ISocketHandler& h, const std::string& url_in):
    HTTPSocket(h),
    m_data_ptr(nullptr),
    m_data_size(0),
    m_content_length(0),
    m_content_length_is_set(false),
    m_data_ptr_set(false),
    m_fil(nullptr),
    m_content_ptr(0),
    m_b_complete(false),
    m_b_close_when_complete(false)
{
    std::string url;
    url_this(url_in, m_protocol, m_host, m_port, url, m_url_filename);
    SetUrl(url);
}


HttpClientSocket::HttpClientSocket(ISocketHandler& h,
                                   const std::string& host,
                                   port_t port,
                                   const std::string& url_in):
    HTTPSocket(h),
    m_data_ptr(nullptr),
    m_data_size(0),
    m_content_length(0),
    m_content_length_is_set(false),
    m_data_ptr_set(false),
    m_fil(nullptr),
    m_content_ptr(0),
    m_b_complete(false),
    m_b_close_when_complete(false)
{
    std::string url;
    std::string tmp = "http://" + host + ":" + Utility::l2string(port) + url_in;
    url_this(tmp, m_protocol, m_host, m_port, url, m_url_filename);
    SetUrl(url);
}


HttpClientSocket::~HttpClientSocket()
{
    if (m_data_ptr && !m_data_ptr_set)
    {
        delete[] m_data_ptr;
    }
    if (m_fil)
    {
        m_fil->fclose();
    }
}


void HttpClientSocket::OnFirst()
{
    if (!IsResponse())
    {
        Handler().LogError(this, "OnFirst", 0, "Response expected but not received - aborting", LOG_LEVEL_FATAL);
        SetCloseAndDelete();
    }
    m_content = GetHttpVersion() + " " + GetStatus() + " " + GetStatusText() + "\r\n";
}


void HttpClientSocket::OnHeader(const std::string& key, const std::string& value)
{
    m_content += key + ": " + value + "\r\n";
    if (!strcasecmp(key.c_str(), "content-length"))
    {
        m_content_length = atoi(value.c_str());
        m_content_length_is_set = true;
    }
    else if (!strcasecmp(key.c_str(), "content-type"))
    {
        m_content_type = value;
    }
}


void HttpClientSocket::OnHeaderComplete()
{
    if (!m_filename.empty())
    {
        m_fil = new File;
        if (!m_fil->fopen(m_filename, "wb"))
        {
            delete m_fil;
            m_fil = nullptr;
        }
    }
    if (!m_data_ptr && m_content_length > 0)
    {
        m_data_ptr = new unsigned char[m_content_length];
        m_data_size = m_content_length;
    }
    // make sure we finish in a good way even when response
    // has content-length: 0
    if (m_content_length_is_set && m_content_length == 0)
    {
        EndConnection();
    }
}


void HttpClientSocket::EndConnection()
{
    if (m_fil)
    {
        m_fil->fclose();
        delete m_fil;
        m_fil = nullptr;
    }
    m_b_complete = true;
    OnContent();
    if (m_b_close_when_complete)
    {
        SetCloseAndDelete();
    }
}


void HttpClientSocket::OnData(const char *buf, size_t len)
{
    if (m_fil)
    {
        m_fil->fwrite(buf, 1, len);
    }
    if (m_data_ptr)
    {
        size_t left = m_data_size - m_content_ptr;
        size_t sz = len < left ? len : left;
        if (sz > 0)
            memcpy(m_data_ptr + m_content_ptr, buf, sz);
        m_content_ptr += sz;
        if (len > left)
        {
            Handler().LogError(this, "OnData", -1, "content buffer overflow", LOG_LEVEL_ERROR);
        }
    }
    if (m_content_ptr == m_content_length && m_content_length)
    {
        EndConnection();
    }
}


void HttpClientSocket::OnDelete()
{
    if (!m_b_complete)
    {
        if (m_fil)
        {
            m_fil->fclose();
            delete m_fil;
            m_fil = nullptr;
        }
        m_b_complete = true;
        OnContent();
    }
}


void HttpClientSocket::SetFilename(const std::string& x)
{
    m_filename = x;
}

void HttpClientSocket::SetDataPtr(unsigned char *buf, size_t len)
{
    m_data_ptr = buf;
    m_data_size = len;
    m_data_ptr_set = true;
}


const std::string& HttpClientSocket::GetContent()
{
    return m_content;
}


size_t HttpClientSocket::GetContentLength()
{
    return m_content_length;
}


size_t HttpClientSocket::GetContentPtr()
{
    return m_content_ptr;
}


size_t HttpClientSocket::GetPos()
{
    return m_content_ptr;
}


bool HttpClientSocket::Complete()
{
    return m_b_complete;
}


const unsigned char *HttpClientSocket::GetDataPtr() const
{
    return m_data_ptr;
}


void HttpClientSocket::OnContent()
{
}


void HttpClientSocket::SetCloseOnComplete(bool x)
{
    m_b_close_when_complete = x;
}


const std::string& HttpClientSocket::GetUrlProtocol()
{
    return m_protocol;
}


const std::string& HttpClientSocket::GetUrlHost()
{
    return m_host;
}


port_t HttpClientSocket::GetUrlPort()
{
    return m_port;
}

const std::string& HttpClientSocket::GetUrlFilename()
{
    return m_url_filename;
}

const std::string& HttpClientSocket::GetContentType()
{
    return m_content_type;
}


void HttpClientSocket::Url(const std::string& url_in, std::string& host, port_t& port)
{
    std::string url;
    url_this(url_in, m_protocol, m_host, m_port, url, m_url_filename);
    SetUrl(url);
    host = GetUrlHost();
    port = GetUrlPort();
}

}//namespace dai
