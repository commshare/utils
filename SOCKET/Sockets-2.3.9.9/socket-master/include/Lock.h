
#ifndef _LOCK_H_INCLUDE
#define _LOCK_H_INCLUDE

#include "sockets-config.h"

namespace dai {

class IMutex;

/**
 * IMutex encapsulation class.
 * \ingroup threading
 */
class Lock
{
public:
    Lock(const IMutex&);
    ~Lock();

private:
    const IMutex& m_mutex;
};

}// namespace dai

#endif//_LOCK_H_INCLUDE

