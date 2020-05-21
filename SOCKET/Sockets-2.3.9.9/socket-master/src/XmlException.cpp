
#include "XmlException.h"

#ifdef ENABLE_XML

namespace dai {

XmlException::XmlException(const std::string& descr): Exception(descr)
{
}

}//namespace dai

#endif // ENABLE_XML
