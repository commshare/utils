
#ifndef _EVENT_H_INCLUDE
#define _EVENT_H_INCLUDE

#include "sockets-config.h"

#include "EventTime.h"

#ifdef _WIN32
#else
#   include <sys/select.h>
#endif

namespace dai {


class IEventOwner;

/**
 * Store information about a timer event.
 * \ingroup timer
 */
class Event
{
public:
    Event(IEventOwner *, long sec, long usec, unsigned long data = 0);
    ~Event();

    bool operator<(Event&);

    long GetID() const;

    const EventTime& GetTime() const;

    IEventOwner *GetFrom() const;

    unsigned long Data() const;

private:
    // copy constructor
    Event(const Event&) {}

    // assignment operator
    Event& operator=(const Event&)
    {
        return *this;
    }

    IEventOwner   *m_from;
    unsigned long  m_data;
    EventTime      m_time;
    static long    m_unique_id;
    long           m_id;
};


}//namespace dai

#endif//_SOCKETS_Event_H

