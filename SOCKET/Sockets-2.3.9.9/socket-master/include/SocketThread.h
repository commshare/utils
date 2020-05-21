
#ifndef _SOCKET_THREAD_H_INCLUDE
#define _SOCKET_THREAD_H_INCLUDE

#include "sockets-config.h"

#ifdef ENABLE_DETACH

#include "Thread.h"
#include "SocketHandler.h"

namespace dai {

class Socket;

/**
 * Detached socket run thread.
 * \ingroup internal
 */
class SocketThread : public Thread
{
public:
    SocketThread(Socket *p);
    ~SocketThread();

    void Run();

private:
    SocketThread(const SocketThread& s) : m_socket(s.m_socket) {}

    SocketThread& operator=(const SocketThread&)
    {
        return *this;
    }

    SocketHandler  m_h;
    Socket        *m_socket;
};

}//namespace dai

#endif//ENABLE_DETACH

#endif//_SOCKET_THREAD_H_INCLUDE
