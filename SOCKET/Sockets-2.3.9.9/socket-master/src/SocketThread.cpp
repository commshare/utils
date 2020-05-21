#include "SocketThread.h"

#ifdef ENABLE_DETACH
#include "Utility.h"

namespace dai {

SocketThread::SocketThread(Socket *p) : Thread(false), m_socket(p)
{
    // Creator will release
}

SocketThread::~SocketThread()
{
    if (IsRunning())
    {
        SetRelease(true);
        SetRunning(false);
        m_h.Release();
        Utility::Sleep(5);
    }
}

void SocketThread::Run()
{
    m_h.SetSlave();
    m_h.Add(m_socket);
    m_socket->SetSlaveHandler(&m_h);
    m_socket->OnDetached();
    m_h.EnableRelease();
    while (m_h.GetCount() > 1 && IsRunning())
    {
        m_h.Select(0, 500000);
    }
    // m_socket now deleted oops
    //  (a socket can only be detached if DeleteByHandler() is true)
    // yeah oops m_socket delete its socket thread, that means this
    // so Socket will no longer delete its socket thread, instead we do this:
    SetDeleteOnExit();
}

}//namespace dai

#endif//ENABLE_DETACH
