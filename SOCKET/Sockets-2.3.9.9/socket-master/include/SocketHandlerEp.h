
#ifndef _SOCKET_HANDLER_EP_H_INCLUDE
#define _SOCKET_HANDLER_EP_H_INCLUDE

#include "SocketHandler.h"

#ifdef LINUX
#include <sys/epoll.h>
#define MAX_EVENTS_EP_WAIT 100
#endif//LINUX

namespace dai {

class SocketHandlerEp : public SocketHandler
{
public:
    /**
     * SocketHandler constructor.
     * \param log Optional log class pointer
     */
    SocketHandlerEp(StdLog *log = NULL);

    /**
     * SocketHandler threadsafe constructor.
     * \param mutex Externally declared mutex variable
     * \param log Optional log class pointer
     */
    SocketHandlerEp(IMutex& mutex, StdLog *log = NULL);
    SocketHandlerEp(IMutex&, ISocketHandler& parent, StdLog * = NULL);
    ~SocketHandlerEp();

    ISocketHandler *Create(StdLog * = NULL);
    ISocketHandler *Create(IMutex&, ISocketHandler&, StdLog * = NULL);

#ifdef LINUX
    /** Set read/write/exception file descriptor sets (fd_set). */
    void ISocketHandler_Add(Socket *, bool bRead, bool bWrite);
    void ISocketHandler_Mod(Socket *, bool bRead, bool bWrite);
    void ISocketHandler_Del(Socket *);

    size_t MaxCount()
    {
        return 10000; // %!
    }
protected:
    /** Actual call to select() */
    int ISocketHandler_Select(struct timeval *);
#endif // LINUX

private:
    int                m_epoll; ///< epoll file descriptor

#ifdef LINUX
    struct epoll_event m_events[MAX_EVENTS_EP_WAIT];
#endif // LINUX

};

}//namespace dai

#endif // _SOCKET_HANDLER_EP_H_INCLUDE
