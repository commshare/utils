#ifndef _I_MUTEX_H_INCLUDE
#define _I_MUTEX_H_INCLUDE

#include "sockets-config.h"

namespace dai {

/**
 * IMutex interface.
 *  \ingroup threading
 */
class IMutex
{
public:
    virtual ~IMutex() = default;

    virtual void Lock()   const = 0;
    virtual void Unlock() const = 0;
};


}//namespace dai

#endif//_I_MUTEX_H_INCLUDE

