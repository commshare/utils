
#ifndef _IP_V4_ADDRESS_H_INCLUDE
#define _IP_V4_ADDRESS_H_INCLUDE

#include "sockets-config.h"
#include "SocketAddress.h"

namespace dai {

/**
 * Ipv4 address implementation.
 * \ingroup basic
 */
class Ipv4Address : public SocketAddress
{
public:

    /**
     * Create empty Ipv4 address structure.
     * \param port Port number
     */
    Ipv4Address(port_t port = 0);

    /**
     * Create Ipv4 address structure.
     *  \param a Socket address in network byte order (as returned by Utility::u2ip)
     *  \param port Port number in host byte order
     */
    Ipv4Address(ipaddr_t a, port_t port);

    /**
     * Create Ipv4 address structure.
     *   \param a Socket address in network byte order
     *   \param port Port number in host byte order
     */
    Ipv4Address(struct in_addr& a, port_t port);

    /**
     * Create Ipv4 address structure.
     *   \param host Hostname to be resolved
     *   \param port Port number in host byte order
     */
    Ipv4Address(const std::string& host, port_t port);
    Ipv4Address(struct sockaddr_in&);
    ~Ipv4Address();

    // SocketAddress implementation

    operator struct sockaddr *();
    operator socklen_t();
    bool operator==(SocketAddress&);

    void   SetPort(port_t port);
    port_t GetPort();

    void SetAddress(struct sockaddr *sa);

    int GetFamily();

    bool IsValid();

    std::unique_ptr<SocketAddress> GetCopy();

    // Convert address struct to text.
    std::string Convert(bool include_port = false);
    std::string Reverse();

    // Resolve hostname.
    static bool Resolve(const std::string& hostname, struct in_addr& a);

    /**
     *
     * @param to
     * @return Reverse resolve (IP to hostname).
     */
    static bool Reverse(struct in_addr& a, std::string& name);

    /**
     * Convert address struct to text.
     */
    static std::string Convert(struct in_addr& a);

private:
    // copy constructor
    Ipv4Address(const Ipv4Address&) {}

    // assignment operator
    Ipv4Address& operator=(const Ipv4Address&)
    {
        return *this;
    }

    struct sockaddr_in m_addr {};
    bool               m_valid{true};
};


}//namespace dai

#endif//_IP_V4_ADDRESS_H_INCLUDE

