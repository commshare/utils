
#ifndef _SOCKET_ADDRESS_H_INCLUDE
#define _SOCKET_ADDRESS_H_INCLUDE

#include "sockets-config.h"
#include "socket_include.h"

#include <string>
#include <memory>

namespace dai {

/**
 * This class and its subclasses is intended to be used as replacement
 * for the internal data type 'ipaddr_t' and various implementations of
 * IPv6 addressing found throughout the library.
 *
 * 'ipaddr_t' is an IPv4 address in network byte order.
 * 'port_t' is the portnumber in host byte order.
 * 'struct in6_addr' is an IPv6 address.
 * 'struct in_addr' is an IPv4 address.
 *
 *  \ingroup basic
 */
class SocketAddress
{
public:
    virtual ~SocketAddress() {}

    // Get a pointer to the address struct.
    virtual operator struct sockaddr *() = 0;

    // Get length of address struct.
    virtual operator socklen_t() = 0;

    // Compare two addresses.
    virtual bool operator==(SocketAddress&) = 0;

    /**
     * Set port number.
     * \param port Port number in host byte order
     */
    virtual void SetPort(port_t port) = 0;

    /**
     * Get port number.
     * \return Port number in host byte order
     */
    virtual port_t GetPort() = 0;

    /**
     * Set socket address.
     *  \param sa Pointer to either 'struct sockaddr_in' or 'struct sockaddr_in6'
     */
    virtual void SetAddress(struct sockaddr *sa) = 0;

    // Convert address to text.
    virtual std::string Convert(bool include_port) = 0;

    // Reverse lookup of address.
    virtual std::string Reverse() = 0;

    // Get address family.
    virtual int GetFamily() = 0;

    // Address structure is valid.
    virtual bool IsValid() = 0;

    // Get a copy of this SocketAddress object.
    virtual std::unique_ptr<SocketAddress> GetCopy() = 0;
};

}//namespace dai

#endif//_SOCKET_ADDRESS_H_INCLUDE

