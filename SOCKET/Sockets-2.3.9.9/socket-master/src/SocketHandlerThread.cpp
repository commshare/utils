#include "SocketHandlerThread.h"
#include "Mutex.h"
#include "ISocketHandler.h"

namespace dai {

SocketHandlerThread::SocketHandlerThread(ISocketHandler& parent) : Thread(false), m_parent(parent),
    m_handler(nullptr)
{
}

ISocketHandler& SocketHandlerThread::Handler()
{
    return *m_handler;
}

void SocketHandlerThread::Run()
{
    Mutex mutex;
    m_handler = m_parent.Create(mutex, m_parent);
    m_sem.Post();
    ISocketHandler& h = *m_handler;
    h.EnableRelease();
    while (IsRunning())
    {
        h.Select(1, 0);
    }
}


void SocketHandlerThread::Wait()
{
    m_sem.Wait();
}

}//namespace dai
