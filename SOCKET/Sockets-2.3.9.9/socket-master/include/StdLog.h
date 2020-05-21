
#ifndef _STD_LOG_H_INCLUDE
#define _STD_LOG_H_INCLUDE

#include "sockets-config.h"
#include <string>

namespace dai {

/** error level enum. */
typedef enum
{
    LOG_LEVEL_INFO = 0,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL
} loglevel_t;


class ISocketHandler;

class Socket;

/**
 * \defgroup logging Log help classes
 */

/**
 * Log class interface.
 * \ingroup logging
 */
class StdLog
{
public:
    virtual ~StdLog() {}

    virtual void error(ISocketHandler *,
                       Socket *,
                       const std::string& user_text,
                       int err,
                       const std::string& sys_err,
                       loglevel_t = LOG_LEVEL_WARNING) = 0;
};

}//namespace dai

#endif // _STD_LOG_H_INCLUDE

