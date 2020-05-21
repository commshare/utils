
#ifndef _HTTP_GET_SOCKET_H_INCLUDE
#define _HTTP_GET_SOCKET_H_INCLUDE

#include "sockets-config.h"
#include "HttpClientSocket.h"

namespace dai {

/**
 * Get http page.
 * \ingroup http
 */
class HttpGetSocket : public HttpClientSocket
{
public:
    HttpGetSocket(ISocketHandler&);
    HttpGetSocket(ISocketHandler&, const std::string& url,
                  const std::string& to_file = "",
                  bool connect = true);
    HttpGetSocket(ISocketHandler&,
                  const std::string& host, port_t port,
                  const std::string& url,
                  const std::string& to_file = "",
                  bool connect = true);

    ~HttpGetSocket() = default;

    void DoConnect();
    void DoConnect(const std::string& host, unsigned short port);

    void OnConnect();

protected:
    HttpGetSocket& operator=(const HttpGetSocket&)
    {
        return *this;
    }

    HttpGetSocket(const HttpGetSocket& s) : HttpClientSocket(s) {}
};

}//namespace dai

#endif//_HTTP_GET_SOCKET_H_INCLUDE

