
#ifndef _SOCKET_INCLUDE_H_INCLUDE
#define _SOCKET_INCLUDE_H_INCLUDE

#include "sockets-config.h"

#ifdef _MSC_VER
#   pragma warning(disable:4514)
#endif

// common defines affecting library and applications using library
// Define SOCKETS_DYNAMIC_TEMP to use dynamically allocated buffers
// in read operations - helps on ECOS
#define SOCKETS_DYNAMIC_TEMP

// define type to uniquely identify a socket instance.
typedef unsigned long socketuid_t;

// platform specific stuff
#if (defined(__unix__) || defined(unix)) && !defined(USG)
#   include <sys/param.h>
#endif

#include <list>

// int64
#ifdef _WIN32
#   ifndef uint64_t
typedef unsigned __int64 uint64_t;
#   endif
#   ifndef int64_t
typedef __int64 int64_t;
#   endif
#else
#   include <cstdlib>
#   ifdef SOLARIS
#       include <sys/types.h>
#   else
#       include <cstdint>
#   endif
#endif

#ifndef _WIN32
// ----------------------------------------
// common unix includes / defines
#   include <sys/time.h>
#   include <sys/types.h>
#   include <sys/socket.h>
#   include <netinet/in.h>
#   include <arpa/inet.h>
#   include <unistd.h>

// all typedefs in this file will be declared outside the sockets namespace,
// because some os's will already have one or more of the type defined.
typedef int SOCKET;
#   define Errno    errno
#   define StrError strerror

namespace dai {
// WIN32 adapt
#define closesocket close
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

#ifndef INADDR_NONE
#   define INADDR_NONE ((unsigned long) -1)
#endif // INADDR_NONE
}// namespace dai

#endif//!_WIN32


// ----------------------------------------
// Generic
#ifndef SOL_IP
#   define SOL_IP IPPROTO_IP
#endif


// ----------------------------------------
// OS specific adaptions

#ifdef SOLARIS
// ----------------------------------------
// Solaris
typedef unsigned short port_t;

namespace dai {
// no defs
}//namespace dai

#define s6_addr16 _S6_un._S6_u8
#define MSG_NOSIGNAL 0

#elif defined __FreeBSD__
// ----------------------------------------
// FreeBSD
# if __FreeBSD_version >= 400014
#  define s6_addr16 __u6_addr.__u6_addr16
#  if !defined(MSG_NOSIGNAL)
#   define MSG_NOSIGNAL 0
#  endif
#  include <netinet/in.h>
typedef in_addr_t ipaddr_t;
typedef in_port_t port_t;

namespace dai {
// no defs
}// namespace dai

#  define IPV6_ADD_MEMBERSHIP IPV6_JOIN_GROUP
#  define IPV6_DROP_MEMBERSHIP IPV6_LEAVE_GROUP
# else
#  error FreeBSD versions prior to 400014 does not support ipv6
# endif

#elif defined MACOSX
// ----------------------------------------
// Mac OS X
//#include <string.h>
#ifdef __DARWIN_UNIX03
typedef unsigned short port_t;
#else
#include <mach/port.h>
#endif // __DARWIN_UNIX03
typedef unsigned long ipaddr_t;

namespace dai {
// no defs
}//namespace dai


#define s6_addr16            __u6_addr.__u6_addr16
#define MSG_NOSIGNAL         0 // oops - thanks Derek
#define IPV6_ADD_MEMBERSHIP  IPV6_JOIN_GROUP
#define IPV6_DROP_MEMBERSHIP IPV6_LEAVE_GROUP

#elif defined _WIN32
// ----------------------------------------
// Win32
#ifdef _MSC_VER
#pragma comment(lib, "wsock32.lib")
#endif
#define strcasecmp _stricmp
#ifndef __CYGWIN__
#define snprintf sprintf_s
#define vsnprintf vsprintf_s
#endif

typedef unsigned long ipaddr_t;
typedef unsigned short port_t;
typedef int socklen_t;

namespace dai {
// no defs
}//namespace dai

// 1.8.6: define FD_SETSIZE to something bigger than 64 if there are a lot of
// simultaneous connections (must be done before including winsock.h)
//#define FD_SETSIZE 1024

// windows 2000 with ipv6 preview installed:
//    http://msdn.microsoft.com/downloads/sdks/platform/tpipv6.asp
// see the FAQ on how to install
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#if _MSC_VER < 1200
#ifndef __CYGWIN__
#ifdef ENABLE_IPV6
#include <tpipv6.h>  // For IPv6 Tech Preview.
#endif
#endif
#endif // _MSC_VER < 1200


#define MSG_NOSIGNAL 0
//#define SHUT_RDWR 2
#define SHUT_WR 1

#define Errno WSAGetLastError()
const char *StrError(int x);

namespace dai {

// class WSAInitializer is a part of the Socket class (on win32)
// as a static instance - so whenever an application uses a Socket,
// winsock is initialized
class WSAInitializer // Winsock Initializer
{
public:
    WSAInitializer()
    {
        if (WSAStartup(0x101, &m_wsadata))
        {
            exit(-1);
        }
    }
    ~WSAInitializer()
    {
        WSACleanup();
    }
private:
    WSADATA m_wsadata;
};

}//namespace dai


#else
// ----------------------------------------
// LINUX
typedef unsigned long ipaddr_t;
typedef unsigned short port_t;

namespace dai {
// no defs
}//namespace dai

#endif

namespace dai {
/**
 * List type containing file descriptors.
 */
typedef std::list<SOCKET> socket_v;
}//namespace dai



// getaddrinfo / getnameinfo replacements
#ifdef NO_GETADDRINFO
#   ifndef AI_NUMERICHOST
#       define AI_NUMERICHOST 1
#   endif
#   ifndef NI_NUMERICHOST
#       define NI_NUMERICHOST 1
#   endif
#endif

#endif//_SOCKET_INCLUDE_H_INCLUDE
