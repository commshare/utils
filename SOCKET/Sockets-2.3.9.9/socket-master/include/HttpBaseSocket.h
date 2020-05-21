
#ifndef _HTTP_BASE_SOCKET_H_INCLUDE
#define _HTTP_BASE_SOCKET_H_INCLUDE

#include "HTTPSocket.h"
#include "HttpRequest.h"
#include "IHttpServer.h"
#include "HttpResponse.h"

namespace dai {

class HttpResponse;

class HttpBaseSocket : public HTTPSocket, public IHttpServer
{
public:
    HttpBaseSocket(ISocketHandler& h);
    ~HttpBaseSocket() = default;

    void OnFirst();
    void OnHeader(const std::string& key, const std::string& value);
    void OnHeaderComplete();
    void OnData(const char *, size_t);

    // implements IHttpServer::Respond
    void IHttpServer_Respond(const HttpResponse& res);

    void OnTransferLimit();

protected:
    HttpBaseSocket(const HttpBaseSocket& s) : HTTPSocket(s) {} // copy constructor
    //
    HttpRequest  m_req;
    HttpResponse m_res;

    void Reset();

private:
    HttpBaseSocket& operator=(const HttpBaseSocket&)
    {
        return *this;    // assignment operator
    }
    void Execute();

    //
    size_t m_body_size_left{};
    bool   m_b_keepalive;
};


}//namespace dai

#endif//_HTTP_BASE_SOCKET_H_INCLUDE

