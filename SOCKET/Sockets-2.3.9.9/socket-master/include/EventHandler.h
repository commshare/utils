
#ifndef _EVENT_HANDLE_H_INCLUDE
#define _EVENT_HANDLE_H_INCLUDE

#include "sockets-config.h"
#include "SocketHandler.h"
#include "IEventHandler.h"

namespace dai {


class StdLog;
class IEventOwner;
class Event;
class TcpSocket;

/**
 * SocketHandler implementing the IEventHandler interface.
 * \ingroup timer
 */
class EventHandler : public SocketHandler, public IEventHandler
{
public:
    EventHandler(StdLog * = NULL);
    EventHandler(IMutex&, StdLog * = NULL);
    ~EventHandler();

    bool GetTimeUntilNextEvent(struct timeval *tv);

    void CheckEvents();

    long AddEvent(IEventOwner *from, long sec, long usec);

    void ClearEvents(IEventOwner *from);

    void RemoveEvent(IEventOwner *from, long eid);

    /**
     * SocketHandler while() loop implemented with event functionality.
     */
    void EventLoop();

    /**
     * Stop event loop.
     */
    void SetQuit(bool = true);

    void Add(Socket *);

private:
    // copy constructor
    EventHandler(const EventHandler&) {}

    // assignment operator
    EventHandler& operator=(const EventHandler&)
    {
        return *this;
    }

    std::list<Event *> m_events;
    bool               m_quit;
};


}//namespace dai

#endif//_EVENT_HANDLE_H_INCLUDE

