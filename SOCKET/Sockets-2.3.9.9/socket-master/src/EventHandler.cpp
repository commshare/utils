
#ifdef _MSC_VER
#   pragma warning(disable:4786)
#endif

#include "EventHandler.h"
#include "IEventOwner.h"
#include "Event.h"
#include "TcpSocket.h"

namespace dai {

EventHandler::EventHandler(StdLog *p) : SocketHandler(p), m_quit(false)
{
    EnableRelease();
}

EventHandler::EventHandler(IMutex& m, StdLog *p) : SocketHandler(m, p), m_quit(false)
{
    EnableRelease();
}

EventHandler::~EventHandler()
{
    while (!m_events.empty())
    {
        auto it = m_events.begin();
        Event *e = *it;
        e -> GetFrom() -> SetHandlerInvalid();
        delete e;
        m_events.erase(it);
    }
}


bool EventHandler::GetTimeUntilNextEvent(struct timeval *tv)
{
    if (m_events.empty())
        return false;

    auto it = m_events.begin();
    if (it != m_events.end())
    {
        EventTime now;
        mytime_t diff = (*it) -> GetTime() - now;
        if (diff < 1)
        {
            diff = 1;
        }
        tv -> tv_sec = static_cast<long>(diff / 1000000);
        tv -> tv_usec = static_cast<long>(diff % 1000000);
        return true;
    }
    return false;
}


void EventHandler::CheckEvents()
{
    EventTime now;
    auto it = m_events.begin();
    while (it != m_events.end() && (*it) -> GetTime() < now)
    {
        Event *e = *it;
        Socket *s = dynamic_cast<Socket *>(e -> GetFrom());
        /*
        s == NULL    This is another object implementing 'IEventOwner' and not a socket.
        s != NULL    This is a Socket implementing IEventOwner, and we can check that the
                 object instance still is valid using SocketHandler::Valid.
        */
        if (!s || (s && Valid( e -> Data() )))
        {
            e -> GetFrom() -> OnEvent(e -> GetID());
        }
        for (it = m_events.begin(); it != m_events.end(); ++it)
            if (*it == e)
            {
                delete e;
                break;
            }
        if (it != m_events.end())
            m_events.erase(it);
        it = m_events.begin();
    }
}


long EventHandler::AddEvent(IEventOwner *from, long sec, long usec)
{
    Socket *s = dynamic_cast<Socket *>(from);
    Event *e = new Event(from, sec, usec, s ? s -> UniqueIdentifier() : 0);
    auto it = m_events.begin();
    while (it != m_events.end() && *(*it) < *e)
    {
        ++it;
    }
    m_events.insert(it, e);
    Release();
    return e -> GetID();
}


void EventHandler::ClearEvents(IEventOwner *from)
{
    bool repeat;
    do
    {
        repeat = false;
        for (auto it = m_events.begin(); it != m_events.end(); ++it)
        {
            Event *e = *it;
            if (e -> GetFrom() == from)
            {
                delete e;
                m_events.erase(it);
                repeat = true;
                break;
            }
        }
    }
    while (repeat);
}


void EventHandler::EventLoop()
{
    while (!m_quit)
    {
        struct timeval tv;
        if (GetTimeUntilNextEvent(&tv))
        {
            Select(&tv);
            CheckEvents();
        }
        else
        {
            Select();
        }
    }
}


void EventHandler::SetQuit(bool x)
{
    m_quit = x;
}


void EventHandler::RemoveEvent(IEventOwner *from, long eid)
{
    for (auto it = m_events.begin(); it != m_events.end(); ++it)
    {
        Event *e = *it;
        if (from == e -> GetFrom() && eid == e -> GetID())
        {
            delete e;
            m_events.erase(it);
            break;
        }
    }
}

void EventHandler::Add(Socket *p)
{
    SocketHandler::Add( p );
}

}//namespace dai
