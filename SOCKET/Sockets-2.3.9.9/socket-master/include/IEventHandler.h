
#ifndef _I_EVENT_HANDLER_H_INCLUDE
#define _I_EVENT_HANDLER_H_INCLUDE

#include "sockets-config.h"

namespace dai {

class IEventOwner;

/**
 * Timer event handler pure virtual base class.
 * \ingroup timer
 */
class IEventHandler
{
public:
    virtual ~IEventHandler() {}

    /**
     * Return time left until next event.
     * \param tv Time struct that will be filled by method
     * \return true if time is set for next event
     */
    virtual bool GetTimeUntilNextEvent(struct timeval *) = 0;

    /**
     * Will fire events whose time has come.
     */
    virtual void CheckEvents() = 0;

    /**
     * Schedule event for an owner.
     * \param from Event owner
     * \param sec Seconds until event
     * \param usec Microseconds until event
     */
    virtual long AddEvent(IEventOwner *, long sec, long usec) = 0;

    /**
     * Clear all events for a specific owner.
     */
    virtual void ClearEvents(IEventOwner *) = 0;

    /**
     * Remove one specific event for an event owner.
     */
    virtual void RemoveEvent(IEventOwner *, long) = 0;
};


}//namespace dai

#endif//_I_EVENT_HANDLER_H_INCLUDE

