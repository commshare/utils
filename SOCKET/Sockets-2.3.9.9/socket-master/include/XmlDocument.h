
#ifndef _XML_DOCUMENT_H_INCLUDE
#define _XML_DOCUMENT_H_INCLUDE

#include "sockets-config.h"

#ifdef ENABLE_XML

#include <string>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

namespace dai {

class XmlDocument
{
public:
    XmlDocument(const std::string& filename,
                const std::string& verify_ns = "",
                const std::string& verify_root = "");
    virtual ~XmlDocument();

    /** Document parse successful. */
    bool IsOk()
    {
        return m_ok;
    }

    operator xmlDocPtr();

protected:
    // copy constructor
    XmlDocument(const XmlDocument&) {}

private:
    // assignment operator
    XmlDocument& operator=(const XmlDocument&)
    {
        return *this;
    }

    xmlDocPtr m_doc;
    bool      m_ok;
};

}//namespace dai

#endif//ENABLE_XML

#endif//_XML_DOCUMENT_H_INCLUDE
