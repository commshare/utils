
#ifndef _RESOLVE_SOCKET_H
#define _RESOLVE_SOCKET_H

#include <map>

#include "sockets-config.h"

#ifdef ENABLE_RESOLVER
#include "TcpSocket.h"

namespace dai {

class Mutex;

/**
 * Async DNS resolver socket.
 * \ingroup async
 */
class ResolvSocket : public TcpSocket
{
    typedef std::map<std::string, /* type */
            std::map<std::string, std::string> > cache_t; /* host, result */
    typedef std::map<std::string, /* type */
            std::map<std::string, time_t> > timeout_t; /* host, time */

public:
    ResolvSocket(ISocketHandler&);
    ResolvSocket(ISocketHandler&, Socket *parent, const std::string& host, port_t port, bool ipv6 = false);
    ResolvSocket(ISocketHandler&, Socket *parent, ipaddr_t);

#ifdef ENABLE_IPV6
    ResolvSocket(ISocketHandler&, Socket *parent, in6_addr&);
#endif

    ~ResolvSocket();

    void OnAccept()
    {
        m_bServer = true;
    }

    void OnLine(const std::string& line);
    void OnDetached();
    void OnDelete();

    void SetId(int x)
    {
        m_resolv_id = x;
    }

    int GetId()
    {
        return m_resolv_id;
    }

    void OnConnect();

#ifdef ENABLE_IPV6
    void SetResolveIpv6(bool x = true)
    {
        m_resolve_ipv6 = x;
    }
#endif

private:
    // copy constructor
    ResolvSocket(const ResolvSocket& s) : TcpSocket(s) {}

    // assignment operator
    ResolvSocket& operator=(const ResolvSocket&)
    {
        return *this;
    }

    std::string m_query;
    std::string m_data;
    bool        m_bServer;
    Socket     *m_parent;
    socketuid_t m_parent_uid;
    int         m_resolv_id;
    std::string m_resolv_host;
    port_t      m_resolv_port;
    ipaddr_t    m_resolv_address;
#ifdef ENABLE_IPV6
    bool     m_resolve_ipv6;
    in6_addr m_resolv_address6;
#endif
    static cache_t   m_cache;
    static timeout_t m_cache_to;
    static Mutex     m_cache_mutex;
    bool             m_cached;
};


}//namespace dai

#endif//ENABLE_RESOLVER

#endif//_RESOLVE_SOCKET_H

