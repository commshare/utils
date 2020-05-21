
#ifndef _HTTP_DEBUG_SOCKET_H_INCLUDE
#define _HTTP_DEBUG_SOCKET_H_INCLUDE

#include "sockets-config.h"
#include "HTTPSocket.h"

namespace dai {

class ISocketHandler;

/**
 * HTTP request "echo" class. This class echoes a http request/body
 * with a html formatted page.
 * \ingroup http
 */
class HttpDebugSocket : public HTTPSocket
{
public:
    HttpDebugSocket(ISocketHandler&);
    ~HttpDebugSocket() = default;

    void Init();

    void OnFirst();
    void OnHeader(const std::string& key, const std::string& value);
    void OnHeaderComplete();
    void OnData(const char *, size_t);
    void OnDataComplete();

private:
    // copy constructor
    HttpDebugSocket(const HttpDebugSocket& s) : HTTPSocket(s) {}

    // assignment operator
    HttpDebugSocket& operator=(const HttpDebugSocket&)
    {
        return *this;
    }

    int m_content_length;
    int m_read_ptr;
};

}//namespace dai

#endif//_HTTP_DEBUG_SOCKET_H_INCLUDE

