
#ifndef _HTTPD_COOKIES_H_INCLUDE
#define _HTTPD_COOKIES_H_INCLUDE

#include "sockets-config.h"

#include <list>
#include <string>

namespace dai {

//! Store the cookies name/value pairs.

//! Retrieve and manage cookies during a cgi call.
class HTTPSocket;

/**
 * HTTP Cookie parse/container class.
 * \sa HttpdSocket
 * \sa HttpdForm
 * \ingroup webserver
 */
class HttpdCookies
{
    /**
     * list of key/value structs.
     */
    using  cookie_v = std::list<std::pair<std::string, std::string> >;

public:
    HttpdCookies();
    HttpdCookies(const std::string& query_string);
    ~HttpdCookies();

    void add(const std::string& s);

    bool getvalue(const std::string&, std::string&) const;
    void replacevalue(const std::string&, const std::string&);
    void replacevalue(const std::string&, long);
    void replacevalue(const std::string&, int);
    size_t getlength(const std::string&) const;

    void setcookie(HTTPSocket *, const std::string& d, const std::string& p, const std::string& c, const std::string& v);
    void setcookie(HTTPSocket *, const std::string& d, const std::string& p, const std::string& c, long v);
    void setcookie(HTTPSocket *, const std::string& d, const std::string& p, const std::string& c, int v);

    const std::string& expiredatetime() const;

    cookie_v& GetHttpdCookies()
    {
        return m_cookies;
    }

    void Reset();

private:
    cookie_v            m_cookies;
    mutable std::string m_date;
};

}//namespace dai

#endif//_HTTPD_COOKIES_H_INCLUDE

