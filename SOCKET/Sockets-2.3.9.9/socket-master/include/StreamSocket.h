
#ifndef _STREAM_SOCKET_H_INCLUDE
#define _STREAM_SOCKET_H_INCLUDE

#include "Socket.h"

namespace dai {

/**
 * SOCK_STREAM Socket base class.
 * \ingroup basic
 */
class StreamSocket : public Socket
{
public:
    StreamSocket(ISocketHandler&);
    ~StreamSocket();

    /**
     * Socket should Check Connect on next write event from select().
     */
    void SetConnecting(bool = true);

    /**
     * Check connecting flag.
     * \return true if the socket is still trying to connect
     */
    bool Connecting();

    /**
     * Returns true when socket file descriptor is valid,
     * socket connection is established, and socket is not about to
     * be closed.
     */
    bool Ready();

    /**
     * Set timeout to use for connection attempt.
     * \param x Timeout in seconds
     */
    void SetConnectTimeout(int x);

    /**
     * Return number of seconds to wait for a connection.
     * \return Connection timeout (seconds)
     */
    int GetConnectTimeout();

    /**
     * Set flush before close to make a tcp socket completely empty its
     * output buffer before closing the connection.
     */
    void SetFlushBeforeClose(bool = true);

    /**
     * Check flush before status.
     * \return true if the socket should send all data before closing
     */
    bool GetFlushBeforeClose();

    /**
     * Define number of connection retries (tcp only).
     * n = 0 - no retry
     * n > 0 - number of retries
     * n = -1 - unlimited retries
     */
    void SetConnectionRetry(int n);

    /**
     * Get number of maximum connection retries (tcp only).
     */
    int GetConnectionRetry();

    /**
     * Increase number of actual connection retries (tcp only).
     */
    void IncreaseConnectionRetries();

    /**
     * Get number of actual connection retries (tcp only).
     */
    int GetConnectionRetries();

    /**
     * Reset actual connection retries (tcp only).
     */
    void ResetConnectionRetries();

    /**
     * Called after OnRead if socket is in line protocol mode.
     * \sa SetLineProtocol
     */
    /**
     * Enable the OnLine callback. Do not create your own OnRead
     * callback when using this.
     */
    virtual void SetLineProtocol(bool = true);

    /**
     * Check line protocol mode.
     * \return true if socket is in line protocol mode
     */
    bool LineProtocol();

    /**
     * Set shutdown status.
     */
    void SetShutdown(int);

    /**
     * Get shutdown status.
     */
    int GetShutdown();

    /**
     * Returns IPPROTO_TCP or IPPROTO_SCTP
     */
    virtual int Protocol() = 0;

protected:
    // copy constructor
    StreamSocket(const StreamSocket& src) : Socket(src) {}

private:
    // assignment operator
    StreamSocket& operator=(const StreamSocket&)
    {
        return *this;
    }

    bool m_bConnecting;        ///< Flag indicating connection in progress
    int  m_connect_timeout;    ///< Connection timeout (seconds)
    bool m_flush_before_close; ///< Send all data before closing (default true)
    int  m_connection_retry;   ///< Maximum connection retries (tcp)
    int  m_retries;            ///< Actual number of connection retries (tcp)
    bool m_line_protocol;      ///< Line protocol mode flag
    int  m_shutdown;           ///< Shutdown status
};

}//namespace dai

#endif // _STREAM_SOCKET_H_INCLUDE

