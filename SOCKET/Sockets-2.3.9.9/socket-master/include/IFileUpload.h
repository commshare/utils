
#ifndef _I_FILE_UPLOAD_H_INCLUDE
#define _I_FILE_UPLOAD_H_INCLUDE

#include "sockets-config.h"
#include <string>

namespace dai {

class IStream;

/**
 * Multipart form file upload callback.
 * \ingroup webserver
 */
class IFileUpload
{
public:
    virtual ~IFileUpload() {}

    virtual IStream& IFileUploadBegin( const std::string& input_name,
                                       const std::string& filename,
                                       const std::string& content_type) = 0;
    virtual void IFileUploadEnd() = 0;
};

}//namespace dai

#endif//_I_FILE_UPLOAD_H_INCLUDE

