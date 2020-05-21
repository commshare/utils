
#include <cstdio>
#ifdef _WIN32
#   include <process.h>
#   include "socket_include.h"
#else
#   include <unistd.h>
#endif

#include "Thread.h"
#include "Utility.h"

namespace dai {

Thread::Thread(bool release):
    m_thread(0)
    , m_running(true)
    , m_release(false)
    , m_b_delete_on_exit(false)
    , m_b_destructor(false)
{
#ifdef _WIN32
    //  m_thread = ::CreateThread(NULL, 0, StartThread, this, 0, &m_dwThreadId);
    m_thread = (HANDLE)_beginthreadex(NULL, 0, &StartThread, this, 0, &m_dwThreadId);
#else
    pthread_attr_init(&m_attr);
    pthread_attr_setdetachstate(&m_attr, PTHREAD_CREATE_DETACHED);
    if (pthread_create(&m_thread, &m_attr, StartThread, this) == -1)
    {
        perror("Thread: create failed");
        SetRunning(false);
    }
#endif
    m_release = release;
    if (release)
        m_sem.Post();
}

Thread::~Thread()
{
    m_b_destructor = true;
    if (m_running)
    {
        SetRelease(true);
        SetRunning(false);

        // Sleep one second to give thread class Run method enough time to
        // release from run loop
        Utility::Sleep(1000);
    }
#ifdef _WIN32
    if (m_thread)
        ::CloseHandle(m_thread);
#else
    pthread_attr_destroy(&m_attr);
#endif
}


threadfunc_t STDPREFIX Thread::StartThread(threadparam_t zz)
{
    // Sleep here to wait for derived thread class constructor to setup
    // vtable... hurts just looking at it
    Utility::Sleep(5);

    Thread *p = (Thread *)zz;
    p-> Wait();
    if (p-> m_running)
    {
        p-> Run();
    }
    p-> SetRunning(false); // if return
    if (p-> DeleteOnExit() && !p-> IsDestructor())
    {
        delete p;
    }
#ifdef _WIN32
    _endthreadex(0);
#endif
    return (threadfunc_t)nullptr;
}


bool Thread::IsRunning()
{
    return m_running;
}


void Thread::SetRunning(bool x)
{
    m_running = x;
}


bool Thread::IsReleased()
{
    return m_release;
}


void Thread::SetRelease(bool x)
{
    m_release = x;
    if (x)
    {
        m_sem.Post();
    }
}


bool Thread::DeleteOnExit()
{
    return m_b_delete_on_exit;
}


void Thread::SetDeleteOnExit(bool x)
{
    m_b_delete_on_exit = x;
}


bool Thread::IsDestructor()
{
    return m_b_destructor;
}


void Thread::Wait()
{
    m_sem.Wait();
}

}//namespace dai
