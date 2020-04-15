/********************************************************************
//  作者:           LiJY
//  CopyRight(c)    2013 靠谱工作室 All Rights Reserved 
//  创建时间:       2013/11/15 14:28
//  类描述:      	靠谱助手的管道通讯类（管道部分参考 kpadb项目）
//  修改时间:       
//  修改目的:       
*********************************************************************/
#ifndef _CNAMEPIPE_H
#define _CNAMEPIPE_H

#include <WTypes.h>
#include <tchar.h>

//管道相关参数配合
//#define CS_PIPE_NAME			TString(_T("\\\\.\\Pipe\\KpzsCmdPipe"))		//管道名
#define CS_PIPE_NAME			TString(_T("\\\\.\\Pipe\\TianTianPlayerPipe"))		//管道名
#define MAX_PIPE_PARAM_LEN		4095								//一次性接收或发送的最大长度
#define WAIT_NAMED_PIPE_TIME	NMPWAIT_NOWAIT						//等待管道连接的时间 永远WAIT_NAMED_PIPE_TIME 或者不等

#include   <string>
#ifndef TString
#ifdef _UNICODE
#define TString std::wstring
#define TCHAR wchar_t
#else
#define TString std::string
#define TCHAR char
#endif
#endif


#ifndef BOOL
#define BOOL    int
#define TRUE    1
#define FALSE   0
#endif

#ifndef ASSERT
#define ASSERT
#endif

#ifndef TRACE
#define TRACE printf
#endif

//判断是否是引用该DLL
#ifndef __NO_USE_NAMEPIPE_DLL
#ifndef    DLLNAMEPIPE_EXPORTS
#define  __DEC_DLL_PORT   __declspec(dllimport) 
#else
#define  __DEC_DLL_PORT   __declspec(dllexport) 
#endif
#else
#define __DEC_DLL_PORT
#endif

class __DEC_DLL_PORT CNamePipeBase
{
protected:
    CNamePipeBase(void);
public:
    virtual ~CNamePipeBase(void);

	virtual BOOL Start() = 0;
	virtual BOOL Read(TString& szRead) = 0;
	virtual BOOL Write(const TString &zsWrite) = 0;
	virtual BOOL End() = 0;
    void setPipeName(TString pipeName){m_szPipeName = pipeName;}
protected:

	BOOL ReadDefault(TString& szRead);
	BOOL WriteDefault(const TString & zsWrite);

    TString m_szPipeName;
	HANDLE m_hPipe;
    int m_value;
};

//  类描述:服务端(它需要等待连接，一次通话后，可断开链接或继续连接（取决于客户端）)
class __DEC_DLL_PORT CServerPipe :public CNamePipeBase
{
public:
    explicit CServerPipe(){;}
    virtual ~CServerPipe(){;}

    virtual BOOL Start();
    BOOL WaitConnect();
    virtual BOOL Read(TString& szRead){ return ReadDefault(szRead);}
    virtual BOOL Write(const TString & zsWrite) { return WriteDefault(zsWrite);}
    BOOL BreakConnect();
    virtual BOOL End();
};


//  类描述:客户端
class __DEC_DLL_PORT CClientPipe :public CNamePipeBase
{
public:
    explicit CClientPipe(){;}
    virtual ~CClientPipe(){;}

    virtual BOOL Start();
    virtual BOOL Read(TString& szRead){ return ReadDefault(szRead);}
    virtual BOOL Write(const TString & zsWrite) { return WriteDefault(zsWrite);}
    virtual BOOL End();
};


#endif /*_CNAMEPIPE_H*/
