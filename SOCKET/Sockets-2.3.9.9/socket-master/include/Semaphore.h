
#ifndef _SEMAPHORE_H_INCLUDE
#define _SEMAPHORE_H_INCLUDE

#include "sockets-config.h"

#ifdef _WIN32
#   include "socket_include.h"
#   include <windows.h>
#else
#   include <pthread.h>
#   ifdef MACOSX
#       include <sys/semaphore.h>
#   else
#       include <semaphore.h>
#   endif
#endif


namespace dai {

#ifdef _WIN32
typedef LONG value_t;
#else
typedef unsigned int value_t;
#endif

/**
 * pthread semaphore wrapper.
 * \ingroup threading
 */
class Semaphore
{
public:
    explicit Semaphore(value_t start_val = 0);
    ~Semaphore();

    /**
     * \return 0 if successful
     */
    int Post();

    /**
     * Wait for Post
     * \return 0 if successful
     */
    int Wait();

    /**
     * Not implemented for win32
     */
    int TryWait();

    /**
     * Not implemented for win32
     */
    int GetValue(int&);

private:
    // copy constructor
    Semaphore(const Semaphore&) {}

    // assignment operator
    Semaphore& operator=(const Semaphore&)
    {
        return *this;
    }

#ifdef _WIN32
    HANDLE m_handle;
#else
    sem_t m_sem{};
#endif
};

}// namespace dai

#endif//_SEMAPHORE_H_INCLUDE

