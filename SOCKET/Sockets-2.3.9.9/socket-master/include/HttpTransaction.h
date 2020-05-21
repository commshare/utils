
#ifndef _HTTP_TRANSACTION_H_INCLUDE
#define _HTTP_TRANSACTION_H_INCLUDE

#include "Utility.h"

namespace dai {

class HttpTransaction
{
public:
    HttpTransaction() = default;
    virtual ~HttpTransaction() = default;

    /** Set / Read http header value */
    void SetHeader(const std::string& key, const std::string& value);
    void SetHeader(const std::string& key, long value);

    [[nodiscard]]
    const std::string& Header(const std::string& key) const;

    void SetAccept(const std::string& value);

    [[nodiscard]]
    const std::string& Accept() const;

    void SetAcceptCharset(const std::string& value);

    [[nodiscard]]
    const std::string& AcceptCharset() const;

    void SetAcceptEncoding(const std::string& value);

    [[nodiscard]]
    const std::string& AcceptEncoding() const;

    void SetAcceptLanguage(const std::string& value);

    [[nodiscard]]
    const std::string& AcceptLanguage() const;

    void SetConnection(const std::string& value);

    [[nodiscard]]
    const std::string& Connection() const;

    void SetContentType(const std::string& value);

    [[nodiscard]]
    const std::string& ContentType() const;

    void SetContentLength(long value);

    [[nodiscard]]
    long ContentLength() const;

    void SetHost(const std::string& value);

    [[nodiscard]]
    const std::string& Host() const;

    [[nodiscard]]
    const std::string HostOnly() const;

    void SetPragma(const std::string& value);

    [[nodiscard]]
    const std::string& Pragma() const;

    void SetReferer(const std::string& value);

    [[nodiscard]]
    const std::string& Referer() const;

    void SetUserAgent(const std::string& value);

    [[nodiscard]]
    const std::string& UserAgent() const;

    [[nodiscard]]
    const Utility::ncmap <std::string>& Headers() const;

    virtual void Reset();

private:
    Utility::ncmap <std::string> m_header;
    std::string                  m_null;

}; // end of class


}//namespace dai

#endif//_HTTP_TRANSACTION_H_INCLUDE

