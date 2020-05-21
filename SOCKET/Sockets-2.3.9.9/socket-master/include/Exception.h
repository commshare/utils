#ifndef _EXCEPTION_H_INCLUDE
#define _EXCEPTION_H_INCLUDE

#include "sockets-config.h"
#include <string>

namespace dai {

class Exception
{
public:
    Exception(const std::string& description);
    virtual ~Exception() {}

    virtual const std::string ToString() const;
    virtual const std::string Stack() const;

    // copy constructor
    Exception(const Exception&) {}

    // assignment operator
    Exception& operator=(const Exception&)
    {
        return *this;
    }

private:
    std::string m_description;
    std::string m_stack;
};

}//namespace dai

#endif//_EXCEPTION_H_INCLUDE
