
#include "win32_pipestream.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include <QDebug>

/* The official documentation states that the name of a given named
 * pipe cannot be more than 256 characters long.
 */


#define INVALIDATE_PIPE_NAME      "PIPE-UNKNOWN"

Win32PipeStream::Win32PipeStream(const char* pipename) :
    m_pipe(INVALID_HANDLE_VALUE)
{
    sprintf(m_pipeName, "\\\\.\\pipe\\kaopu001-%s", pipename);
    m_pipeName[MAX_PATH-1] = '\0';
}

Win32PipeStream::Win32PipeStream(HANDLE pipe) :
    m_pipe(pipe)
{
}

Win32PipeStream::~Win32PipeStream()
{
    if (m_pipe != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_pipe);
        m_pipe = INVALID_HANDLE_VALUE;
    }
}

Win32PipeStream *Win32PipeStream::accept()
{
    Win32PipeStream*  clientStream;
    HANDLE pipe;

    pipe = ::CreateNamedPipeA(
                m_pipeName,                // pipe name
                PIPE_ACCESS_DUPLEX,  // read-write access
                PIPE_TYPE_BYTE |     // byte-oriented writes
                PIPE_READMODE_BYTE | // byte-oriented reads
                PIPE_WAIT,           // blocking operations
                PIPE_UNLIMITED_INSTANCES, // no limit on clients
                4096,                // input buffer size
                4096,                // output buffer size
                0,                   // client time-out
                NULL);               // default security attributes

    if (pipe == INVALID_HANDLE_VALUE)
    {
        qDebug() <<"[Win32Pipe]: CreateNamedPipe failed" << GetLastError();
        return NULL;
    }

    // Stupid Win32 API design: If a client is already connected, then
    // ConnectNamedPipe will return 0, and GetLastError() will return
    // ERROR_PIPE_CONNECTED. This is not an error! It just means that the
    // function didn't have to wait.
    //
    if (::ConnectNamedPipe(pipe, NULL) == 0 && GetLastError() != ERROR_PIPE_CONNECTED)
    {
        qDebug() <<"[Win32Pipe]: ConnectNamedPipe failed: " << GetLastError();
        CloseHandle(pipe);
        return NULL;
    }

    clientStream = new Win32PipeStream(pipe);
    return clientStream;
}

int Win32PipeStream::connect(void)
{
    HANDLE pipe;
    int    tries = 10;
    int    retval= 0;

    /* We're going to loop in order to wait for the pipe server to
     * be setup properly.
     */
    for (; tries > 0; tries--)
    {
        pipe = ::CreateFileA(
                    m_pipeName,                          // pipe name
                    GENERIC_READ | GENERIC_WRITE,  // read & write
                    0,                             // no sharing
                    NULL,                          // default security attrs
                    OPEN_EXISTING,                 // open existing pipe
                    0,                             // default attributes
                    NULL);                         // no template file

        /* If we have a valid pipe handle, break from the loop */
        if (pipe != INVALID_HANDLE_VALUE)
        {
            break;
        }

        /* We can get here if the pipe is busy, i.e. if the server hasn't
         * create a new pipe instance to service our request. In which case
         * GetLastError() will return ERROR_PIPE_BUSY.
         *
         * If so, then use WaitNamedPipe() to wait for a decent time
         * to try again.
         */
        if (GetLastError() != ERROR_PIPE_BUSY)
        {
            /* Not ERROR_PIPE_BUSY */
            retval = GetLastError();
            qDebug() <<"[Win32Pipe]: CreateFile failed: " << retval;
            errno = EINVAL;

            return retval;
        }

        /* Wait for 10 seconds */
        if ( !WaitNamedPipeA(m_pipeName, 10000) )
        {
            retval = GetLastError();
            qDebug() <<"[Win32Pipe]: WaitNamedPipe failed: " << retval;
            errno = EINVAL;
            return retval;
        }
    }

    m_pipe = pipe;
    return retval;
}

int Win32PipeStream::writefully(void *buf, size_t size)
{
    if (m_pipe == INVALID_HANDLE_VALUE)
        return ERROR_INVALID_HANDLE;

    size_t res = size;
    int retval = 0;

    while (res > 0)
    {
        DWORD  written;
        if (! ::WriteFile(m_pipe, (const char *)buf + (size - res), res, &written, NULL)) {
            retval =  (int)GetLastError();
            qDebug() <<"[Win32Pipe]: writefully failed: " << retval;
            break;
        }
        res -= written;
    }
    return retval;
}

int Win32PipeStream::readfully(void *buf, size_t len)
{
    int retval = 0;

    if (m_pipe == INVALID_HANDLE_VALUE)
        return ERROR_INVALID_HANDLE;

    if (!buf)
    {
        return ERROR_INVALID_HANDLE;  // do not allow NULL buf in that implementation
    }

    size_t res = len;
    while (res > 0) {
        DWORD  readcount = 0;
        if (! ::ReadFile(m_pipe, (char *)buf + (len - res), res, &readcount, NULL) || readcount == 0)
        {
            errno = (int)GetLastError();
            retval = errno;
            qDebug() <<"[Win32Pipe]: readfully failed: " << retval;
            return retval;
        }
        res -= readcount;
    }

    return retval;
}
