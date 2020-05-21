
#ifndef _HTTPD_SOCKET_H_INCLUDE
#define _HTTPD_SOCKET_H_INCLUDE

#include "sockets-config.h"
#include "HTTPSocket.h"
#include "ISocketHandler.h"

namespace dai {

class HttpdCookies;
class HttpdForm;
class IFile;

/**
 * \defgroup webserver Webserver framework
 */

/**
 * Web server socket framework.
 * \ingroup webserver
 */
class HttpdSocket : public HTTPSocket
{
public:
    HttpdSocket(ISocketHandler&);
    ~HttpdSocket();

    void OnFirst();
    void OnHeader(const std::string& key, const std::string& value);
    void OnHeaderComplete();
    void OnData(const char *, size_t);

    /**
     * This method needs to be implemented with logic to produce
     * a response to an incoming request.
     */
    virtual void Exec() = 0;

    /**
     * Get current date in http rfc format.
     */
    const std::string& GetHttpDate() const;

    /**
     * Get pointer to cookie class.
     */
    HttpdCookies *GetCookies();

    /**
     * Get pointer to query string/form data class.
     */
    const HttpdForm *GetForm() const;

    size_t ContentLength() const
    {
        return m_content_length;
    }

    const IFile *Body() const
    {
        return m_file;
    }

    int RequestId() const
    {
        return m_request_id;
    }

protected:
    HttpdSocket(const HttpdSocket& s) : HTTPSocket(s) {}

    /**
     * Decode and send a base64-encoded string.
     * \param str64 Base64-encoded string
     * \param type Mime type of content (content-type header)
     */
    void Send64(const std::string& str64, const std::string& type);

    std::string datetime2httpdate(const std::string& dt);

    std::string GetDate();

    void Reset();

    // headers
    std::string m_http_cookie;
    std::string m_content_type;
    std::string m_content_length_str;
    std::string m_if_modified_since;

private:
    HttpdSocket& operator=(const HttpdSocket& s)
    {
        return *this;
    }

    static int         m_request_count;
    static std::string m_start;
    size_t             m_content_length;
    IFile             *m_file;
    size_t             m_received;
    int                m_request_id;
    std::string        m_http_date;
    HttpdCookies      *m_cookies;
    HttpdForm         *m_form;
};

}//namespace dai

#endif//_HTTPD_SOCKET_H_INCLUDE

