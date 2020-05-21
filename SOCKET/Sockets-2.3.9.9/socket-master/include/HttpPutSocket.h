
#ifndef _HTTP_PUT_SOCKET_H_INCLUDE
#define _HTTP_PUT_SOCKET_H_INCLUDE

#include "sockets-config.h"
#include "HttpClientSocket.h"

namespace dai {


class ISocketHandler;

/**
 * Put http page.
 * \ingroup http
 */
class HttpPutSocket : public HttpClientSocket
{
public:
    HttpPutSocket(ISocketHandler&);

    /**
     * client constructor,
     * \param url_in = 'http://host:port/resource'
     */
    HttpPutSocket(ISocketHandler&, const std::string& url_in);

    ~HttpPutSocket() = default;

    // these must be specified before connecting / adding to handler
    /** Set filename to send. */
    void SetFile(const std::string&);

    /** Set mimetype of file to send. */
    void SetContentType(const std::string&);

    /** connect to host:port derived from url in constructor */
    void Open();

    /** http put client implemented in OnConnect */
    void OnConnect();

private:
    // copy constructor
    HttpPutSocket(const HttpPutSocket& s) : HttpClientSocket(s) {}

    // assignment operator
    HttpPutSocket& operator=(const HttpPutSocket&)
    {
        return *this;
    }
    //
    std::string m_filename;
    std::string m_content_type;
    long        m_content_length;
};


}//namespace dai

#endif//_HTTP_PUT_SOCKET_H_INCLUDE

