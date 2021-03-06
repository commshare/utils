
#include "SocketHandlerEp.h"
#include "Exception.h"
#include "IMutex.h"
#include "Utility.h"

#include <cerrno>

namespace dai {

SocketHandlerEp::SocketHandlerEp(StdLog *p):
    SocketHandler(p), m_epoll(-1)
{
#ifdef LINUX
    m_epoll = epoll_create(FD_SETSIZE);
    if (m_epoll == -1)
    {
        throw Exception(StrError(Errno));
    }
#endif
}

SocketHandlerEp::SocketHandlerEp(IMutex& mutex, StdLog *p) :
    SocketHandler(mutex, p),
    m_epoll(-1)
{
#ifdef LINUX
    m_epoll = epoll_create(FD_SETSIZE);
    if (m_epoll == -1)
    {
        throw Exception(StrError(Errno));
    }
#endif
}


SocketHandlerEp::SocketHandlerEp(IMutex& mutex, ISocketHandler& parent, StdLog *p):
    SocketHandler(mutex, parent, p),
    m_epoll(-1)
{
#ifdef LINUX
    m_epoll = epoll_create(FD_SETSIZE);
    if (m_epoll == -1)
    {
        throw Exception(StrError(Errno));
    }
#endif
}


SocketHandlerEp::~SocketHandlerEp()
{
#ifdef LINUX
    if (m_epoll != -1)
    {
        close(m_epoll);
    }
#endif
}


ISocketHandler *SocketHandlerEp::Create(StdLog *log)
{
    return new SocketHandlerEp(log);
}


ISocketHandler *SocketHandlerEp::Create(IMutex& mutex, ISocketHandler& parent, StdLog *log)
{
    return new SocketHandlerEp(mutex, parent, log);
}


#ifdef LINUX
void SocketHandlerEp::ISocketHandler_Add(Socket *p, bool bRead, bool bWrite)
{
    struct epoll_event stat;
    SOCKET s = p->GetSocket();
    stat.data.ptr = p;
    stat.events   = (bRead ? EPOLLIN : 0) | (bWrite ? EPOLLOUT : 0);
    if (epoll_ctl(m_epoll, EPOLL_CTL_ADD, s, &stat) == -1)
    {
        LogError(NULL, "epoll_ctl: EPOLL_CTL_ADD", Errno, StrError(Errno));
    }
}


void SocketHandlerEp::ISocketHandler_Mod(Socket *p, bool r, bool w)
{
    struct epoll_event stat;
    SOCKET s = p -> GetSocket();
    stat.data.ptr = p;
    stat.events = (r ? EPOLLIN : 0) | (w ? EPOLLOUT : 0);
    if (epoll_ctl(m_epoll, EPOLL_CTL_MOD, s, &stat) == -1)
    {
        // LogError(NULL, "epoll_ctl: EPOLL_CTL_MOD", Errno, StrError(Errno));
    }
}


void SocketHandlerEp::ISocketHandler_Del(Socket *p)
{
    struct epoll_event stat;
    stat.events = 0;
    stat.data.ptr = p;
    if (epoll_ctl(m_epoll, EPOLL_CTL_DEL, p -> GetSocket(), &stat) == -1)
    {
        // LogError(NULL, "epoll_ctl: EPOLL_CTL_DEL", Errno, StrError(Errno));
    }
}


int SocketHandlerEp::ISocketHandler_Select(struct timeval *tsel)
{
    int n;
    if (m_b_use_mutex)
    {
        m_mutex.Unlock();
        n = epoll_wait(m_epoll, m_events, MAX_EVENTS_EP_WAIT, tsel ? tsel -> tv_sec * 1000 + tsel -> tv_usec / 1000 : -1);
        m_mutex.Lock();
    }
    else
    {
        n = epoll_wait(m_epoll, m_events, MAX_EVENTS_EP_WAIT, tsel ? tsel -> tv_sec * 1000 + tsel -> tv_usec / 1000 : -1);
    }
    if (n == -1)
    {
        LogError(nullptr, "epoll_wait", Errno, StrError(Errno));
    }
    else if (!n)
    {
    }
    else if (n > 0)
    {
        for (int x = 0; x < n; x++)
        {
            Socket *p = static_cast<Socket *>(m_events[x].data.ptr);
            if ((m_events[x].events & EPOLLIN) || (m_events[x].events & EPOLLHUP))
            {
#ifdef HAVE_OPENSSL
                if (p -> IsSSLNegotiate())
                {
                    p -> SSLNegotiate();
                }
                else
#endif
                {
                    p -> OnRead();
                }
            }
            if (m_events[x].events & EPOLLOUT)
            {
#ifdef HAVE_OPENSSL
                if (p -> IsSSLNegotiate())
                {
                    p -> SSLNegotiate();
                }
                else
#endif
                {
                    p -> OnWrite();
                }
            }
            if (m_events[x].events & EPOLLERR)
            {
                p -> OnException();
            }
        }
    }
    return n;
}
#endif


}//namespace dai

