
#ifdef MACOSX
#   include <stdint.h>
#   include <sys/types.h>
#   include <signal.h>
#endif

#ifdef _WIN32
#   include <windows.h>
#else
#   include <sys/select.h>
#   include <sys/time.h>
#endif

#include "EventTime.h"

namespace dai {

EventTime::EventTime() : m_time(Tick())
{
}

EventTime::EventTime(mytime_t sec, long usec) : m_time(Tick())
{
    m_time += sec * 1000000 + usec;
}

EventTime::~EventTime()
{
}

mytime_t EventTime::Tick()
{
    mytime_t t;
#ifdef _WIN32
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    t = ft.dwHighDateTime;
    t = t << 32;
    t += ft.dwLowDateTime;
    t /= 10; // us
#else
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);
    t = tv.tv_sec;
    t *= 1000000;
    t += tv.tv_usec;
#endif
    return t;
}

EventTime EventTime::operator-(const EventTime& x) const
{
    EventTime t;
    t.m_time = m_time - x.m_time;
    return t;
}

bool EventTime::operator<(const EventTime& x) const
{
    return m_time < x.m_time;
}

}//namespace dai

