
#ifdef _WIN32
#   ifdef _MSC_VER
#       pragma warning(disable:4786)
#   endif
#   include <io.h>
#endif

#include "SSLInitializer.h"

#ifdef HAVE_OPENSSL

#include <map>
#include <openssl/rand.h>

#include "Lock.h"
#include "Utility.h"

#ifdef _DEBUG
#   define DEB(x) x
#else
#   define DEB(x)
#endif


namespace dai {

std::map<int, IMutex *> *SSLInitializer::m_mmap = NULL;
Mutex                   *SSLInitializer::m_mmap_mutex = NULL;


SSLInitializer::SSLInitializer()
{
    DEB(fprintf(stderr, "SSLInitializer()\n");)

    bio_err = NULL;
    m_rand_size = 1024;

    /* An error write context */
    bio_err = BIO_new_fp(stderr, BIO_NOCLOSE);

    /* Global system initialization*/
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
    CRYPTO_set_locking_callback(SSL_locking_function);
    CRYPTO_set_id_callback(SSL_id_function);

    std::string randfile = Utility::GetEnv("RANDFILE");
    std::string home = Utility::GetEnv("HOME");
    if (randfile.empty() && home.empty())
    {
        std::string homepath = Utility::GetEnv("HOMEPATH");
        if (!homepath.empty())
        {
            Utility::SetEnv("HOME", homepath);
        }
    }
    char path[512];
    *path = 0;
    RAND_file_name(path, 512);
    if (*path)
    {
        m_rand_file = path;
        m_rand_size = 1024;
        RAND_write_file(path);
    }
    else
    {
        DEB(fprintf(stderr, "SSLInitializer: no random file generated\n");)
    }

    /* Load randomness */
    if (!m_rand_file.size() || !RAND_load_file(m_rand_file.c_str(), m_rand_size))
    {
        DEB(fprintf(stderr, "SSLInitializer: PRNG not initialized\n");)
    }

}


SSLInitializer::~SSLInitializer()
{
    DEB(fprintf(stderr, "~SSLInitializer()\n");)
    DeleteRandFile();
    // %! delete mutexes
}


void SSLInitializer::DeleteRandFile()
{
    if (m_rand_file.size())
    {
#ifdef _WIN32
        _unlink(m_rand_file.c_str());
#else
        unlink(m_rand_file.c_str());
#endif
    }
}


void SSLInitializer::SSL_locking_function(int mode, int n, const char *file, int line)
{
    IMutex *mutex = NULL;
    {
        Lock lock(MMapMutex());
        if (MMap().find(n) == MMap().end())
        {
            MMap()[n] = new Mutex;
        }
        mutex = MMap()[n];
    }
    if (mode & CRYPTO_LOCK)
    {
        mutex->Lock();
    }
    else
    {
        mutex->Unlock();
    }
}


unsigned long SSLInitializer::SSL_id_function()
{
    return Utility::ThreadID();
}


std::map<int, IMutex *>& SSLInitializer::MMap()
{
    if (m_mmap == NULL)
        m_mmap = new std::map<int, IMutex *>();
    return *m_mmap;
}


Mutex& SSLInitializer::MMapMutex()
{
    if (m_mmap_mutex == NULL)
        m_mmap_mutex = new Mutex();
    return *m_mmap_mutex;
}


}//namespace dai

#endif // HAVE_OPENSSL

