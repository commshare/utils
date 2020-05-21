
#include "XmlDocument.h"
#include "XmlException.h"

#ifdef ENABLE_XML

namespace dai {


XmlDocument::XmlDocument(const std::string& filename,
                         const std::string& verify_ns,
                         const std::string& verify_root):
    m_doc(nullptr),
    m_ok(false)
{
    xmlNodePtr cur;
    xmlNsPtr ns;

    if (!(m_doc = xmlParseFile(filename.c_str())))
    {
        throw XmlException("Parse of file failed: " + filename);
    }
    if (!(cur = xmlDocGetRootElement(m_doc)))
    {
        xmlFreeDoc(m_doc);
        m_doc = NULL;
        throw XmlException("Document is empty: " + filename);
    }
    if (verify_ns.size())
    {
        if (!(ns = xmlSearchNsByHref(m_doc, cur, (const xmlChar *) verify_ns.c_str())))
        {
            xmlFreeDoc(m_doc);
            m_doc = NULL;
            throw XmlException("Document namespace != " + verify_ns);
        }
    }
    if (verify_root.size())
    {
        if (xmlStrcmp(cur->name, (const xmlChar *) verify_root.c_str()))
        {
            xmlFreeDoc(m_doc);
            m_doc = NULL;
            throw XmlException("Document root != " + verify_root);
        }
    }
    m_ok = true;
}


XmlDocument::~XmlDocument()
{
    if (m_doc)
    {
        xmlFreeDoc(m_doc);
    }
}


XmlDocument::operator xmlDocPtr()
{
    return m_doc;
}


}//namespace dai

#endif//ENABLE_XML
