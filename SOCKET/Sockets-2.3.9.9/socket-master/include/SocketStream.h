
#ifndef _SOCKET_STREAM_H_INCLUDE
#define _SOCKET_STREAM_H_INCLUDE

#include "IStream.h"
#include "socket_include.h"

namespace dai {

class ISocketHandler;
class TcpSocket;

class SocketStream : public IStream
{
public:
    SocketStream(ISocketHandler& h, TcpSocket *sock);

    size_t IStreamRead(char *buf, size_t max_sz);
    void IStreamWrite(const char *buf, size_t sz);

private:
    ISocketHandler& m_handler;
    TcpSocket      *m_socket;
    socketuid_t     m_socket_uid;
};

}//namespace dai

#endif//_SOCKET_STREAM_H_INCLUDE
