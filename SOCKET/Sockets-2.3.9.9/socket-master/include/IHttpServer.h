
#ifndef _I_HTTP_SERVER_H_INCLUDE
#define _I_HTTP_SERVER_H_INCLUDE

namespace dai {

class HttpRequest;
class HttpResponse;

class IHttpServer
{
public:
    virtual ~IHttpServer() {}

    /**
     * Complete request has been received and parsed. Send response
     * using the Respond() method.
     */
    virtual void IHttpServer_OnExec(const HttpRequest& req) = 0;

    /** Send response. */
    virtual void IHttpServer_Respond(const HttpResponse& res) = 0;

    /** Called when the body part of the response has been sent. */
    virtual void IHttpServer_OnResponseComplete() = 0;
};


}//namespace dai

#endif//_I_HTTP_SERVER_H_INCLUDE

