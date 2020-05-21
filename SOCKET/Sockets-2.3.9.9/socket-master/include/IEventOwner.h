
#ifndef _I_EVENT_OWNER_H_INCLUDE
#define _I_EVENT_OWNER_H_INCLUDE

#include "sockets-config.h"
#include "IEventHandler.h"

namespace dai {


/**
 * Any class that wants to use timer events inherits this.
 * \ingroup timer
 */
class IEventOwner
{
public:
    IEventOwner(IEventHandler& h);

    virtual ~IEventOwner();

    /**
     * Schedule event.
     * \param sec Seconds until event
     * \param usec Microseconds until event
     * \return Event ID
     */
    long AddEvent(long sec, long usec);

    /** Clear all events scheduled by this owner. */
    void ClearEvents();

    /**
     * Remove one event scheduled by this owner.
     * \param eid Event ID to remove
     */
    void RemoveEvent(long eid);

    /** Event callback will fire when time is up. */
    virtual void OnEvent(int) = 0;

    IEventHandler& GetEventHandler();

    void SetHandlerInvalid(bool x = true)
    {
        m_handler_invalid = x;
    }

private:
    IEventHandler& m_event_handler;
    bool           m_handler_invalid;
};

}//namespace dai

#endif//_I_EVENT_OWNER_H_INCLUDE

