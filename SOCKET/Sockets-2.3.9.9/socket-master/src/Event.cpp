

#ifdef _WIN32
#else
#   include <ctime>
#endif

#include "Event.h"

namespace dai {

// statics
long Event::m_unique_id = 0;

Event::Event(IEventOwner *from, long sec, long usec, unsigned long data) :
    m_from(from),
    m_data(data),
    m_time(sec, usec),
    m_id(++m_unique_id)
{
}


Event::~Event()
{
}


bool Event::operator<(Event& e)
{
    return m_time < e.m_time;
}


long Event::GetID() const
{
    return m_id;
}


const EventTime& Event::GetTime() const
{
    return m_time;
}


IEventOwner *Event::GetFrom() const
{
    return m_from;
}


unsigned long Event::Data() const
{
    return m_data;
}

}//namespace dai
