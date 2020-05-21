
#ifndef _SMTPD_SOCKET_H_INCLUDE
#define _SMTPD_SOCKET_H_INCLUDE

#include "sockets-config.h"
#include <string>
#include "TcpSocket.h"

namespace dai {


/**
 * Smtp server base class.
 */
class SmtpdSocket : public TcpSocket
{
protected:
    typedef enum
    {
        SMTP_NO_HELLO,
        SMTP_NAME_TOO_LONG,
        SMTP_DOMAIN_TOO_LONG,
        SMTP_QUIT
    } reason_t;


public:
    class EmailAddress
    {
    public:
        EmailAddress(const std::string& str_in)
        {
            std::string str = str_in;
            size_t i = str.find("<");
            if (i != std::string::npos)
                str = str.substr(i + 1);
            i = str.find("@");
            if (i != std::string::npos)
            {
                m_name = str.substr(0, i);
                str = str.substr(i + 1);
                i = str.find(">");
                if (i != std::string::npos)
                    str = str.substr(0, i);
                m_domain = str;
            }
            while (!m_name.empty() && m_name[m_name.size() - 1] == ' ')
                m_name.resize(m_name.size() - 1);
            while (!m_domain.empty() && m_domain[m_domain.size() - 1] == ' ')
                m_domain.resize(m_domain.size() - 1);
            while (!m_name.empty() && m_name[0] == ' ')
                m_name = m_name.substr(1);
            while (!m_domain.empty() && m_domain[0] == ' ')
                m_domain = m_domain.substr(1);
            m_top = m_domain;
            {
                for (size_t i = 0; i < m_domain.size(); i++)
                {
                    if (m_domain[i] == '.')
                    {
                        m_sub = m_top;
                        m_top = m_domain.substr(i + 1);
                    }
                }
            }
        }

        [[nodiscard]]
        const std::string& GetName() const
        {
            return m_name;
        }

        [[nodiscard]]
        const std::string& GetDomain() const
        {
            return m_domain;
        }

        [[nodiscard]]
        const std::string& GetTopDomain() const
        {
            return m_top;
        }

        [[nodiscard]]
        const std::string& GetSubDomain() const
        {
            return m_sub;
        }

        [[nodiscard]]
        std::string ToString() const
        {
            return m_name + "@" + m_domain;
        }

    private:
        std::string m_name;
        std::string m_domain;
        std::string m_top;
        std::string m_sub;
    };

public:
    SmtpdSocket(ISocketHandler&);

    void OnAccept();
    void OnLine(const std::string&);

    /** \return 'false' to abort */
    virtual bool OnHello(const std::string& domain) = 0;

    /** \return 'false' to abort */
    virtual bool OnMailFrom(const EmailAddress& addr) = 0;

    /** \return 'false' to abort */
    virtual bool OnRcptTo(const EmailAddress& addr) = 0;

    virtual void OnHeader(const std::string& key, const std::string& value) = 0;
    virtual void OnHeaderComplete() = 0;
    virtual void OnData(const std::string& line) = 0;

    /** \return 'false' if message write failed (message will probably be resent) */
    virtual bool OnDataComplete() = 0;
    virtual void OnRset() = 0;
    virtual void OnAbort(reason_t) = 0;
    virtual void OnNotSupported(const std::string& cmd, const std::string& arg) = 0;

private:
    bool        m_hello; // we need HELO or EHLO first of all
    bool        m_data;
    bool        m_header;
    std::string m_header_line;
};


}//namespace dai

#endif//_SMTPD_SOCKET_H_INCLUDE

