
#ifndef _STD_OUT_LOG_H_INCLUDE
#define _STD_OUT_LOG_H_INCLUDE

#include "sockets-config.h"
#include "StdLog.h"

namespace dai {

/**
 * StdLog implementation, logs to stdout.
 * \ingroup logging
 */
class StdoutLog : public StdLog
{
public:
    StdoutLog(loglevel_t min_level = LOG_LEVEL_INFO) : m_min_level(min_level) {}
    void error(ISocketHandler *, Socket *, const std::string& call, int err, const std::string& sys_err, loglevel_t);

private:
    loglevel_t m_min_level;
};

}//namespace dai

#endif//_STD_OUT_LOG_H_INCLUDE

