
#ifndef _XML_EXCEPTION_H_INCLUDE
#define _XML_EXCEPTION_H_INCLUDE

#include "sockets-config.h"
#include "Exception.h"

#ifdef ENABLE_XML

namespace dai {

class XmlException : public Exception
{
public:
    XmlException(const std::string& descr);

    // copy constructor
    XmlException(const XmlException& x) : Exception(x) {}

private:
    // assignment operator
    XmlException& operator=(const XmlException&)
    {
        return *this;
    }

};


}//namespace dai

#endif//ENABLE_XML
#endif//_XML_EXCEPTION_H_INCLUDE
