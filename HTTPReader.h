// HTTPReader.h: interface for the CHTTPReader class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __HTTPREADER_H__
#define __HTTPREADER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef _WININET_
# ifndef _WINDOWS_
#  include <windows.h>
# endif
# include <wininet.h>
#endif
#include <string>
#ifdef _UNICODE 
	typedef std::wstring tstring; 
	#define tcstol wcstol
#else 
	typedef std::string tstring; 
	#define tcstol strtol
#endif

class CHTTPReader
{
public:
	CHTTPReader(LPCTSTR lpszServerName=NULL,bool bUseSSL=false);
	~CHTTPReader();
	tstring proxy;
	bool  OpenInternet     (LPCTSTR lpszAgent=TEXT("RSDN HTTP Reader"));
	void  CloseInternet    ();
	bool  OpenConnection   (tstring serverName=TEXT(""));
	void  CloseConnection  ();
    int Flags;
	bool  Get              (LPCTSTR lpszAction,LPCTSTR lpszReferer=NULL);
	bool  Post             (LPCTSTR lpszAction,std::string *Data,LPCTSTR lpszReferer=NULL);
	void  CloseRequest     ();
	bool GetUrl(tstring url);
	void SetTimeOut(int timeOut);

	char *GetData          (char *lpszBuffer,DWORD dwSize,DWORD *lpdwBytesRead=NULL);
	char *GetData          (DWORD *lpdwBytesRead=NULL);
	DWORD GetDataSize      ();
	void  SetDataBuffer    (DWORD dwBufferSize);
	bool downloadFile(LPCTSTR file);

	void  SetDefaultHeader (LPCTSTR lpszDefaultHeader);
	DWORD GetError         () const { return m_dwLastError; }

protected:
	int pTimeOut;
	bool CheckError (bool bTest);
	void StrDup     (LPTSTR& lpszDest,LPCTSTR lpszSource);
	bool SendRequest(LPCTSTR lpszVerb,LPCTSTR lpszAction, std::string *Data,LPCTSTR lpszReferer);

	HINTERNET m_hInternet;
	HINTERNET m_hConnection;
	HINTERNET m_hRequest;

	DWORD     m_dwLastError;
	bool      m_bUseSSL;
	LPTSTR    m_lpszDefaultHeader;
	char     *m_lpszDataBuffer;
	DWORD     m_dwBufferSize;
};

#endif // __HTTPREADER_H__
