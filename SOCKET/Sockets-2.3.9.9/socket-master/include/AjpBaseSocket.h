
#ifndef _AJP_BASE_SOCKET_H_INCLUDE
#define _AJP_BASE_SOCKET_H_INCLUDE

#include "TcpSocket.h"
#include "Utility.h"

#include <map>


namespace dai {

class AjpBaseSocket : public TcpSocket
{
    class Initializer
    {
    public:
        Initializer();
        virtual ~Initializer() {}

        std::map<int, std::string> Method;
        std::map<int, std::string> Header;
        std::map<int, std::string> Attribute;

        Utility::ncmap<int> ResponseHeader;
    };


public:
    AjpBaseSocket(ISocketHandler& h);

    void OnRawData(const char *buf, size_t sz);

    virtual void OnHeader(short id, short len) = 0;
    virtual void OnPacket(const char *buf, size_t sz) = 0;

protected:
    unsigned char get_byte(const char *buf, int& ptr);

    bool get_boolean(const char *buf, int& ptr);

    short get_integer(const char *buf, int& ptr);

    std::string get_string(const char *buf, int& ptr);

    void put_byte(char *buf, int& ptr, unsigned char zz);

    void put_boolean(char *buf, int& ptr, bool zz);

    void put_integer(char *buf, int& ptr, short zz);

    void put_string(char *buf, int& ptr, const std::string& zz);

    /** Reset to original state */
    void reset();

    static Initializer Init;

private:
    int  m_state;
    int  m_length;
    int  m_ptr;
    char m_message[8192];
};

}//namespace dai

#endif//_AJP_BASE_SOCKET_H_INCLUDE

