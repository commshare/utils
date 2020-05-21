
#ifndef _MUTEX_H_INCLUDE
#define _MUTEX_H_INCLUDE

#include "sockets-config.h"

#ifndef _WIN32
#   include <pthread.h>
#else
#   include "socket_include.h"
#   include <windows.h>
#endif

#include "IMutex.h"

namespace dai {

/**
 * Mutex container class, used by Lock.
 * \ingroup threading
 */
class Mutex : public IMutex
{
public:
    Mutex();
    ~Mutex();

    virtual void Lock() const;
    virtual void Unlock() const;

private:
#ifdef _WIN32
    HANDLE                  m_mutex;
#else
    mutable pthread_mutex_t m_mutex;
#endif
};


}//namespace dai

#endif//_MUTEX_H_INCLUDE

