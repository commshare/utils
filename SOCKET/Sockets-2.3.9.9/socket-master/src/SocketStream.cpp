
#include "SocketStream.h"
#include "ISocketHandler.h"
#include "TcpSocket.h"

namespace dai {

SocketStream::SocketStream(ISocketHandler& h, TcpSocket *sock) :
    m_handler(h),
    m_socket(sock),
    m_socket_uid(sock->UniqueIdentifier())
{
}


size_t SocketStream::IStreamRead(char *buf, size_t max_sz)
{
    if (m_handler.Valid(m_socket_uid))
    {
        return m_socket->ReadInput(buf, max_sz);
    }
    return 0;
}


void SocketStream::IStreamWrite(const char *buf, size_t sz)
{
    if (m_handler.Valid(m_socket_uid))
    {
        m_socket->SendBuf(buf, sz);
    }
}


}//namespace dai
