
#ifndef _THREAD_H_INCLUDE
#define _THREAD_H_INCLUDE

#include "sockets-config.h"

#ifdef _WIN32
#else
#   include <pthread.h>
#endif

#include "Semaphore.h"

namespace dai {

#ifdef _WIN32
// to be
//typedef DWORD  threadfunc_t;
//typedef LPVOID threadparam_t;
//#define STDPREFIX WINAPI
typedef unsigned threadfunc_t;
typedef void *threadparam_t;
#define STDPREFIX __stdcall
#else
typedef void *threadfunc_t;
typedef void *threadparam_t;
#define STDPREFIX
#endif


/**
 *  \defgroup threading Threading
 *  Thread base class.
 *  The Thread class is used by the resolver (ResolvServer) and running a detached socket (SocketThread).
 *  When you know some processing will take a long time and will freeze up a socket, there is always the
 *  possibility to call Detach() on that socket before starting the processing.
 *  When the OnDetached() callback is later called the processing can continue, now in its own thread.
 *  \ingroup threading
 */
class Thread
{
public:
    explicit Thread(bool release = true);
    virtual ~Thread();

    static threadfunc_t STDPREFIX StartThread(threadparam_t);

    virtual void Run() = 0;

#ifdef _WIN32
    HANDLE GetThread()
    {
        return m_thread;
    }
    unsigned GetThreadId()
    {
        return m_dwThreadId;
    }
#else
    pthread_t GetThread()
    {
        return m_thread;
    }
#endif

    bool IsRunning();
    void SetRunning(bool x);

    bool IsReleased();
    void SetRelease(bool x);

    bool DeleteOnExit();
    void SetDeleteOnExit(bool x = true);

    bool IsDestructor();

    void Start()
    {
        SetRelease(true);
    }

    void Stop()
    {
        Start();
        SetRunning(false);
    }

    void Wait();

protected:
#ifdef _WIN32
    HANDLE   m_thread;
    unsigned m_dwThreadId;
#else
    pthread_t      m_thread;
    pthread_attr_t m_attr;
#endif

private:
    Thread(const Thread&) {}

    Thread& operator=(const Thread&)
    {
        return *this;
    }

    Semaphore m_sem;
    bool      m_running;
    bool      m_release;
    bool      m_b_delete_on_exit;
    bool      m_b_destructor;
};

}//namespace dai

#endif//_THREAD_H_INCLUDE

