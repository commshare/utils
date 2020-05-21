
#ifdef _WIN32
#   ifdef _MSC_VER
#       pragma warning(disable:4786)
#       pragma warning(disable:4503)
#   endif
#else
#   include <cerrno>
#   include <cctype>
#endif

#include <sys/types.h>
#include <sys/stat.h>

#include "ISocketHandler.h"
#include "Lock.h"
#include "File.h"
#include "HttpPostSocket.h"

namespace dai {


int HttpPostSocket::m_boundary_count = 0;
Mutex HttpPostSocket::m_boundary_mutex;


HttpPostSocket::HttpPostSocket(ISocketHandler& h) : HttpClientSocket(h), m_bMultipart(false)
{
}

HttpPostSocket::HttpPostSocket(ISocketHandler& h, const std::string& url_in) :
    HttpClientSocket(h, url_in),
    m_bMultipart(false)
{
    Lock lock(m_boundary_mutex);

    m_boundary = "----";
    for (int i = 0; i < 12; i++)
    {
        char c = m_boundary_count++ % 128;
        while (!isalnum(c))
            c = m_boundary_count++ % 128;
        m_boundary += c;
    }
    m_boundary += "__" + Utility::l2string(m_boundary_count++);
}



void HttpPostSocket::AddField(const std::string& name, const std::string& value)
{
    std::list<std::string> vec;
    vec.push_back(value);
    AddMultilineField(name, vec);
}


void HttpPostSocket::AddMultilineField(const std::string& name, std::list<std::string>& values)
{
    m_fields[name] = values;
}


void HttpPostSocket::AddFile(const std::string& name, const std::string& filename, const std::string& type)
{
    struct stat st{};
    if (!stat(filename.c_str(), &st))
    {
        m_files[name] = filename;
        m_content_length[filename] = st.st_size;
        m_content_type[filename] = type;
        m_bMultipart = true;
    }
    else
    {
        Handler().LogError(this, "AddFile", Errno, StrError(Errno), LOG_LEVEL_FATAL);
        SetCloseAndDelete();
    }
}


void HttpPostSocket::Open()
{
    // why do I have to specify TcpSocket:: to get to the Open() method??
    TcpSocket::Open(GetUrlHost(), GetUrlPort());
}


void HttpPostSocket::OnConnect()
{
    if (m_bMultipart)
    {
        DoMultipartPost();
    }
    else
    {
        std::string body;

        // only fields, no files, add urlencoding
        for (auto& m_field : m_fields)
        {
            std::string name = m_field.first;
            std::list<std::string>& ref = m_field.second;
            if (body.size())
            {
                body += '&';
            }
            body += name + "=";
            bool first = true;
            for (auto value : ref)
            {
                if (!first)
                {
                    body += "%0d%0a"; // CRLF
                }
                body += Utility::rfc1738_encode(value);
                first = false;
            }
        }

        // build header, send body
        SetMethod("POST");
        SetHttpVersion("HTTP/1.1");
        AddResponseHeader("Host", GetUrlHost()); // oops - this is actually a request header that we're adding..
        AddResponseHeader("User-agent", MyUseragent());
        AddResponseHeader("Accept", "text/html, text/plain, */*;q=0.01");
        AddResponseHeader("Connection", "close");
        AddResponseHeader("Content-type", "application/x-www-form-urlencoded");
        AddResponseHeader("Content-length", Utility::l2string((long) body.size()));
        SendRequest();

        // send body
        Send(body);
    }
}


void HttpPostSocket::DoMultipartPost()
{
    long length = 0; // calculate content_length of our post body
    std::string tmp;

    // fields
    {
        for (auto& m_field : m_fields)
        {
            std::string name = m_field.first;
            std::list<std::string>& ref = m_field.second;
            tmp = "--" + m_boundary + "\r\n"
                  "content-disposition: form-data; name=\"" + name + "\"\r\n"
                  "\r\n";
            for (auto value : ref)
            {
                tmp += value + "\r\n";
            }
            length += (long) tmp.size();
        }
    }

    // files
    {
        for (auto& m_file : m_files)
        {
            std::string name = m_file.first;
            std::string filename = m_file.second;
            long content_length = m_content_length[filename];
            std::string content_type = m_content_type[filename];
            tmp = "--" + m_boundary + "\r\n"
                  "content-disposition: form-data; name=\"" + name + "\"; filename=\"" +
                  filename + "\"\r\n"
                  "content-type: " + content_type + "\r\n"
                  "\r\n";
            length += (long) tmp.size();
            length += content_length;
            length += 2; // crlf after file
        }
    }

    // end
    tmp = "--" + m_boundary + "--\r\n";
    length += (long) tmp.size();

    // build header, send body
    SetMethod("POST");
    SetHttpVersion("HTTP/1.1");
    AddResponseHeader("Host", GetUrlHost()); // oops - this is actually a request header that we're adding..
    AddResponseHeader("User-agent", MyUseragent());
    AddResponseHeader("Accept", "text/html, text/plain, */*;q=0.01");
    AddResponseHeader("Connection", "close");
    AddResponseHeader("Content-type", "multipart/form-data; boundary=" + m_boundary);
    AddResponseHeader("Content-length", Utility::l2string(length));

    SendRequest();

    // send fields
    {
        for (auto& m_field : m_fields)
        {
            std::string name = m_field.first;
            std::list<std::string>& ref = m_field.second;
            tmp = "--" + m_boundary + "\r\n"
                  "content-disposition: form-data; name=\"" + name + "\"\r\n"
                  "\r\n";
            for (auto value : ref)
            {
                tmp += value + "\r\n";
            }
            Send(tmp);
        }
    }

    // send files
    {
        for (auto& m_file : m_files)
        {
            std::string name = m_file.first;
            std::string filename = m_file.second;
            std::string content_type = m_content_type[filename];
            tmp = "--" + m_boundary + "\r\n"
                  "content-disposition: form-data; name=\"" + name + "\"; filename=\"" +
                  filename + "\"\r\n"
                  "content-type: " + content_type + "\r\n"
                  "\r\n";
            Send(tmp);
            {
                std::unique_ptr<IFile> fil = std::unique_ptr<IFile>(new File);
                if (fil->fopen(filename, "rb"))
                {
                    char slask[2000]; // for fread
                    size_t n;
                    while ((n = fil->fread(slask, 1, 2000)) > 0)
                    {
                        SendBuf(slask, n);
                    }
                    fil->fclose();
                }
            }
            Send("\r\n");
        }
    }

    // end of send
    Send("--" + m_boundary + "--\r\n");
}

void HttpPostSocket::SetMultipart()
{
    m_bMultipart = true;
}

}//namespace dai
