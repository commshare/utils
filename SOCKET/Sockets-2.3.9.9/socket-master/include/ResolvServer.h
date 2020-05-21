
#ifndef _RESOLVE_SERVER_H
#define _RESOLVE_SERVER_H

#include "sockets-config.h"

#ifdef ENABLE_RESOLVER

#include "socket_include.h"
#include "Thread.h"

namespace dai {

/**
 * \defgroup async Asynchronous DNS
 */

/**
 * Async DNS resolver thread.
 * \ingroup async
 */
class ResolvServer : public Thread
{
public:
    ResolvServer(port_t);
    ~ResolvServer();

    void Run();
    void Quit();

    bool Ready();

private:
    // copy constructor
    ResolvServer(const ResolvServer& ) {}

    // assignment operator
    ResolvServer& operator=(const ResolvServer& )
    {
        return *this;
    }

    bool   m_quit;
    port_t m_port;
    bool   m_ready;
};

}//namespace dai

#endif//ENABLE_RESOLVER

#endif//_RESOLVE_SERVER_H

