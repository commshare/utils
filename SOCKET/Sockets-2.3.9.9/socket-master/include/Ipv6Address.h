
#ifndef _IP_V6_ADDRESS_H_INCLUDE
#define _IP_V6_ADDRESS_H_INCLUDE

#include "sockets-config.h"

#ifdef ENABLE_IPV6

#include "SocketAddress.h"

#ifdef IPPROTO_IPV6
#if defined( _WIN32) && !defined(__CYGWIN__)
using unsigned __int32 uint32_t;
#endif

namespace dai {

/**
 * Ipv6 address implementation.
 * \ingroup basic
 */
class Ipv6Address : public SocketAddress
{
public:
    /**
     * Create empty Ipv6 address structure.
     * @param port Port number
     */
    Ipv6Address(port_t port = 0);

    /**
     * Create Ipv6 address structure.
     * @param a    Socket address in network byte order
     * @param port Port number in host byte order
     */
    Ipv6Address(struct in6_addr& a, port_t port);

    /**
     * Create Ipv6 address structure.
     * @param host Hostname to be resolved
     * @param port Port number in host byte order
     */
    Ipv6Address(const std::string& host, port_t port);
    Ipv6Address(struct sockaddr_in6&);
    ~Ipv6Address();

    // SocketAddress implementation
    operator struct sockaddr *();
    operator socklen_t();
    bool operator==(SocketAddress&);

    void SetPort(port_t port);

    port_t GetPort();

    void SetAddress(struct sockaddr *sa);

    int GetFamily();

    bool IsValid();

    std::unique_ptr<SocketAddress> GetCopy();

    /** Convert address struct to text. */
    std::string Convert(bool include_port = false);

    std::string Reverse();

    /** Resolve hostname. */
    static bool Resolve(const std::string& hostname, struct in6_addr& a);

    /** Reverse resolve (IP to hostname). */
    static bool Reverse(struct in6_addr& a, std::string& name);

    /** Convert address struct to text. */
    static std::string Convert(struct in6_addr& a, bool mixed = false);

    void SetFlowinfo(uint32_t);

    uint32_t GetFlowinfo();

#ifndef _WIN32
    void SetScopeId(uint32_t);
    uint32_t GetScopeId();
#endif

private:
    // copy constructor
    Ipv6Address(const Ipv6Address&) {}

    // assignment operator
    Ipv6Address& operator=(const Ipv6Address&)
    {
        return *this;
    }

    struct sockaddr_in6 m_addr {};
    bool                m_valid{true};
};


}//namespace dai

#endif//IPPROTO_IPV6
#endif//ENABLE_IPV6

#endif//_IP_V6_ADDRESS_H_INCLUDE

