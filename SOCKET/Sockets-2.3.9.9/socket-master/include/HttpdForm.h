
#ifndef _HTTPD_FORM_H_INCLUDE
#define _HTTPD_FORM_H_INCLUDE

#include "sockets-config.h"

#include <string>
#include <list>
#include <cstdio>
#include <cstring>
#include <vector>

namespace dai {

class IFile;
class IFileUpload;
class IStream;

/**
 * Parse/store a http query_string/form-data body.
 * \ingroup webserver
 */
class HttpdForm
{
    /**
     * Store the name/value pairs from a GET/POST operation.
     * "name" does not have to be unique.
     * \ingroup webserver
     */
    struct CGI
    {
        CGI(const std::string& n, const std::string& v) : name(n), value(v) {}
        CGI(const std::string& n, const std::string& v, const std::string& p) : name(n), value(v), path(p) {}

        std::string name;
        std::string value;
        std::string path;
    };

    // list of key/value pairs.
    using cgi_v = std::list<CGI *>;


public:
    /**
     * Default constructor
     */
    HttpdForm(FILE *);

    /**
     * Constructor (used in POST operations).
     * Input is read from stdin. Number of characters to read
     * can be found in the environment variable CONTENT_LENGTH.
     */
    HttpdForm(IFile *, const std::string& content_type, size_t content_length);

    /**
     * Another constructor (used in GET operations).
     * Input is read from the environment variable QUERY_STRING.
     * @param query_string The httpd server provided QUERY_STRING
     * @param length Query string length.
     */
    HttpdForm(const std::string& query_string, size_t length);

    ~HttpdForm();

    /**
     * Parses the following content-types
     *   "multipart/form-data"
     *   "x-www-form-urlencoded"
     * Any other content-type is left alone and nothing is read from input stream
    */
    void ParseFormData(IFile *, const std::string&, size_t);

    void ParseQueryString(const std::string& query_string, size_t length);

    void EnableRaw(bool);

    /** Encode characters '<' '>' '&' as &lt; &gt; &amp; */
    void strcpyval(std::string&, const char *) const;

    /* get names */
    bool getfirst(std::string& n) const;

    bool getnext(std::string& n) const;

    /* get names and values */
    bool getfirst(std::string& n, std::string& v) const;

    bool getnext(std::string& n, std::string& v) const;

    /* get value */
    int getvalue(const std::string&, std::string&) const;

    std::string getvalue(const std::string&) const;

    size_t getlength(const std::string&) const;

    cgi_v& getbase();

    const std::string& GetBoundary() const;

    /** Enable IFileUpload callback */
    void SetFileUpload(IFileUpload& cb);

    bool ContentAvailable() const
    {
        return !m_content.empty();
    }

    const std::vector<char>& GetContent() const
    {
        return m_content;
    }

private:
    HttpdForm(const HttpdForm&) {}

    HttpdForm& operator=(const HttpdForm&)
    {
        return *this;
    }

    cgi_v                         m_cgi;
    mutable cgi_v::const_iterator m_current;
    std::string                   m_strBoundary;
    bool                          raw;
    IFileUpload                  *m_file_upload;
    IStream                      *m_upload_stream;
    std::vector<char>             m_content;
};

}//namespace dai

#endif//_HTTPD_FORM_H_INCLUDE

