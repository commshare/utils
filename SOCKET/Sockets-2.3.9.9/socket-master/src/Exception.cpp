#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include "Exception.h"
#include "Utility.h"

namespace dai {

Exception::Exception(const std::string& description) :
    m_description(description),
    m_stack(Utility::Stack())
{
}

const std::string Exception::ToString() const
{
    return m_description;
}

const std::string Exception::Stack() const
{
    return m_stack;
}

}//namespace dai

