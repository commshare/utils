
#ifndef _SOCKET_HANDLER_THREAD_H_INCLUDE
#define _SOCKET_HANDLER_THREAD_H_INCLUDE

#include "Thread.h"
#include "Semaphore.h"

namespace dai {

class ISocketHandler;

class SocketHandlerThread : public Thread
{
public:
    SocketHandlerThread(ISocketHandler& parent);
    ~SocketHandlerThread() = default;

    virtual void Run();

    ISocketHandler& Handler();

    void Wait();

private:
    ISocketHandler& m_parent;
    ISocketHandler *m_handler;
    Semaphore       m_sem;
};

}//namespace dai

#endif//_SOCKET_HANDLER_THREAD_H_INCLUDE
