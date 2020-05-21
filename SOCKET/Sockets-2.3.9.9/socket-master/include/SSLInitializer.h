
#ifndef _SSL_INITIALIZER_H_INCLUDE
#define _SSL_INITIALIZER_H_INCLUDE

#include "sockets-config.h"

//#define HAVE_OPENSSL
#ifdef HAVE_OPENSSL

#ifdef _WIN32
#   include <winsock2.h>
#endif
#include <openssl/ssl.h>
#include <string>
#include <map>

#include "Mutex.h"

namespace dai {

class SSLInitializer
{
public:
    /**
     * init openssl
     * bio_err
     * create random file
     */
    SSLInitializer();

    /**
     * remove random file
     */
    ~SSLInitializer();

    void DeleteRandFile();

    /** SSL; mutex locking function callback. */
    static void SSL_locking_function(int mode, int n, const char *file, int line);

    /** Return thread id. */
    static unsigned long SSL_id_function();

    BIO *bio_err;

private:
    std::string                     m_rand_file;
    long                            m_rand_size;
    static std::map<int, IMutex *> *m_mmap;
    static Mutex                   *m_mmap_mutex;

    static std::map<int, IMutex *>& MMap();
    static Mutex& MMapMutex();

};

}//namespace dai

#endif//HAVE_OPENSSL

#endif//_SSL_INITIALIZER_H_INCLUDE

