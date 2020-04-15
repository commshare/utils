//#include "stdafx.h"
#include "namepipe.h"

#include <iostream>
#include <Windows.h>
#include <string>

#include "logoperation.h"


CNamePipeBase::CNamePipeBase(void)
    :m_szPipeName(CS_PIPE_NAME)
    ,m_hPipe(INVALID_HANDLE_VALUE)
{
}


CNamePipeBase::~CNamePipeBase(void)
{
    //ASSERT(m_hPipe == INVALID_HANDLE_VALUE);
}

BOOL CNamePipeBase::ReadDefault(TString& szRead)
{
    ASSERT(m_hPipe != INVALID_HANDLE_VALUE);
    ASSERT(szRead.length()*sizeof(TCHAR)<=MAX_PIPE_PARAM_LEN);
    if(szRead.length()*sizeof(TCHAR)>MAX_PIPE_PARAM_LEN)
        return FALSE;

    DWORD nBytesRead = 0;
    TCHAR szCmd[MAX_PIPE_PARAM_LEN] = {0};
    if (!ReadFile(m_hPipe, szCmd, (MAX_PIPE_PARAM_LEN-1)*sizeof(TCHAR), &nBytesRead, NULL))
    {
        TRACE("ReadFile:%d\n",GetLastError());
        return FALSE;
    }
    szRead = szCmd;
    return TRUE;
}

BOOL CNamePipeBase::WriteDefault(const TString & szWrite)
{
    ASSERT(m_hPipe != INVALID_HANDLE_VALUE);
    ASSERT(szWrite.length()*sizeof(TCHAR)<=MAX_PIPE_PARAM_LEN);
    if(szWrite.length()*sizeof(TCHAR)>MAX_PIPE_PARAM_LEN)
        return FALSE;

    DWORD nBytesWrite = 0;
    if (!WriteFile(m_hPipe, szWrite.c_str(), szWrite.length()*sizeof(TCHAR), &nBytesWrite, NULL))
    {
        TRACE("WriteFile:%d\n",GetLastError());
        return FALSE;
    }
    ASSERT(nBytesWrite == szWrite.length()*sizeof(TCHAR));
    return nBytesWrite == szWrite.length()*sizeof(TCHAR);
}


BOOL CServerPipe::Start()
{
    ASSERT(m_hPipe == INVALID_HANDLE_VALUE);
    //先创建管道
    m_hPipe = CreateNamedPipe(m_szPipeName.c_str(),
        PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE,
        1, 0, 0, 1000, NULL); // 创建命名管道
    ASSERT(m_hPipe != INVALID_HANDLE_VALUE);
    if(m_hPipe == INVALID_HANDLE_VALUE)
    {
        TRACE("CreateNamedPipe:%d\n",GetLastError());
        CLogOperation::writeLine("TianTian",QtDebugMsg,"CreateNamedPipe failed:"+QString::number(GetLastError()));
        return FALSE;
    }

    return TRUE;
}

BOOL CServerPipe::WaitConnect()
{
    ASSERT(m_hPipe != INVALID_HANDLE_VALUE);

    m_value = 1;
    //连接管道
    if (!ConnectNamedPipe(m_hPipe, NULL)) // 等待客户机的连接
    {
        TRACE("ConnectNamedPipe:%d\n",GetLastError());
        CLogOperation::writeLine("TianTian",QtDebugMsg,"ConnectNamedPipe failed:"+QString::number(GetLastError()));
        return FALSE;
    }

    return TRUE;
}

BOOL CServerPipe::BreakConnect()
{
    ASSERT(m_hPipe != INVALID_HANDLE_VALUE);
    // 终止连接
    if (!DisconnectNamedPipe(m_hPipe))
    {
        TRACE("DisconnectNamedPipe:%d\n",GetLastError());
        CLogOperation::writeLine("TianTian",QtDebugMsg,"DisconnectNamedPipe failed:"+QString::number(GetLastError()));
        return FALSE;
    }

    return TRUE;
}

BOOL CServerPipe::End()
{
    ASSERT(m_hPipe != INVALID_HANDLE_VALUE);

    CloseHandle(m_hPipe);
    m_hPipe = INVALID_HANDLE_VALUE;
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////

BOOL CClientPipe::Start()
{
    ASSERT(m_hPipe == INVALID_HANDLE_VALUE);

    //连接
    if (!WaitNamedPipe(m_szPipeName.c_str(), WAIT_NAMED_PIPE_TIME))
    {
        TRACE("WaitNamedPipe:%d\n",GetLastError());
        return FALSE;
    }

    m_hPipe = CreateFile(m_szPipeName.c_str(), GENERIC_READ | GENERIC_WRITE,
        0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if ( m_hPipe == INVALID_HANDLE_VALUE )
    {
        TRACE("CreateFile:%d\n",GetLastError());
        return FALSE;
    }

    return TRUE;
}

BOOL CClientPipe::End()
{
    ASSERT(m_hPipe != INVALID_HANDLE_VALUE);
    BOOL bRet = CloseHandle(m_hPipe);
    ASSERT(bRet);
    m_hPipe = INVALID_HANDLE_VALUE;
    return bRet;
}
