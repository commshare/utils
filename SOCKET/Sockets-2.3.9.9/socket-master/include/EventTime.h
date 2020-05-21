
#ifndef _EVENT_TIME_H_INCLUDE
#define _EVENT_TIME_H_INCLUDE

#include "sockets-config.h"

#if defined( _WIN32) && !defined(__CYGWIN__)
#else
#   include <cctype>
#endif

namespace dai {

#if defined( _WIN32) && !defined(__CYGWIN__)
typedef __int64 mytime_t;
#else
using mytime_t = __int64_t;
#endif


/**
 * \defgroup timer EventTimer event handling
 */

/**
 * EventTime primitive, returns current time as a 64-bit number.
 * \ingroup timer
 */
class EventTime
{
public:
    EventTime();
    EventTime(mytime_t sec, long usec);
    ~EventTime();

    static mytime_t Tick();

    operator mytime_t()
    {
        return m_time;
    }

    EventTime operator-(const EventTime& x) const;
    bool operator<(const EventTime& x) const;

private:
    // copy constructor
    EventTime(const EventTime&) {}

    // assignment operator
    EventTime& operator=(const EventTime&)
    {
        return *this;
    }

    mytime_t m_time;
};

}//namespace dai

#endif // _EVENT_TIME_H_INCLUDE

