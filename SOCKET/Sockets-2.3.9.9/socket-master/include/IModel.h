
#ifndef _I_MODEL_H_INCLUDE
#define _I_MODEL_H_INCLUDE

#include "sockets-config.h"

namespace dai {

class Json;

/**
 * IModel interface.
 * \ingroup tools
 */
class IModel
{
public:
    virtual ~IModel() = default;

    virtual void IModel_ToJson(Json& json) const   = 0;
    virtual void IModel_FromJson(const Json& json) = 0;
};

}//namespace dai

#endif//_I_MODEL_H_INCLUDE

