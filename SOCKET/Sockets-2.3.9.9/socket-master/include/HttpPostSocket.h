
#ifndef _HTTP_POST_SOCKET_H_INCLUDE
#define _HTTP_POST_SOCKET_H_INCLUDE

#include "sockets-config.h"
#include "HttpClientSocket.h"
#include "Mutex.h"

namespace dai {

class ISocketHandler;

/**
 * Generate a http post request, get response.
 * \ingroup http
 */
class HttpPostSocket : public HttpClientSocket
{
public:
    HttpPostSocket(ISocketHandler&);

    /*
     * client constructor,
     * \param url_in = 'http://host:port/resource'
     */
    HttpPostSocket(ISocketHandler&, const std::string& url_in);

    ~HttpPostSocket() = default;

    // these must be specified before connecting / adding to handler
    /** Add field to post. */
    void AddField(const std::string& name, const std::string& value);

    /** Add multiline field to post. */
    void AddMultilineField(const std::string& name, std::list<std::string>& values);

    /** Add file to post. */
    void AddFile(const std::string& name, const std::string& filename, const std::string& type);

    /** use this to post with content-type multipart/form-data.
    // when adding a file to the post, this is the default and only content-type */
    void SetMultipart();

    /** connect to host:port derived from url in constructor */
    void Open();

    /** http put client implemented in OnConnect */
    void OnConnect();

protected:
    std::map<std::string, std::list<std::string> > m_fields;

private:
    // copy constructor
    HttpPostSocket(const HttpPostSocket& s) : HttpClientSocket(s) {}

    // assignment operator
    HttpPostSocket& operator=(const HttpPostSocket&)
    {
        return *this;
    }

    void DoMultipartPost();

    //
    std::map<std::string, std::string> m_files;
    std::string                        m_boundary;
    std::map<std::string, long>        m_content_length;
    std::map<std::string, std::string> m_content_type;
    bool                               m_bMultipart;
    static int                         m_boundary_count;
    static Mutex                       m_boundary_mutex;
};

}//namespace dai

#endif // _HTTP_POST_SOCKET_H_INCLUDE

