
#ifdef _MSC_VER
#   pragma warning(disable:4786)
#endif

#include "ResolvServer.h"

#ifdef ENABLE_RESOLVER
#include "StdoutLog.h"
#include "ListenSocket.h"
#include "ResolvSocket.h"
#include "SocketHandler.h"

namespace dai {

ResolvServer::ResolvServer(port_t port):
    Thread(),
    m_quit(false),
    m_port(port),
    m_ready(false)
{
}

ResolvServer::~ResolvServer()
{
}

void ResolvServer::Run()
{
    //  StdoutLog log;
    SocketHandler h;
    ListenSocket<ResolvSocket> l(h);

    if (l.Bind("127.0.0.1", m_port))
    {
        return;
    }
    h.Add(&l);

    m_ready = true;
    while (!m_quit && IsRunning())
    {
        h.Select(0, 500000);
    }
    SetRunning(false);
}

void ResolvServer::Quit()
{
    m_quit = true;
}

bool ResolvServer::Ready()
{
    return m_ready;
}

}//namespace dai

#endif // ENABLE_RESOLVER

