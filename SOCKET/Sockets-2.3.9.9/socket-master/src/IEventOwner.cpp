
#include "IEventOwner.h"

namespace dai {

IEventOwner::IEventOwner(IEventHandler& h) :
    m_event_handler(h),
    m_handler_invalid(false)
{
}


IEventOwner::~IEventOwner()
{
    if (!m_handler_invalid)
    {
        m_event_handler.ClearEvents(this);
    }
}


IEventHandler& IEventOwner::GetEventHandler()
{
    return m_event_handler;
}


long IEventOwner::AddEvent(long sec, long usec)
{
    return m_event_handler.AddEvent(this, sec, usec);
}


void IEventOwner::ClearEvents()
{
    m_event_handler.ClearEvents(this);
}


void IEventOwner::RemoveEvent(long eid)
{
    m_event_handler.RemoveEvent(this, eid);
}


}//namespace dai
