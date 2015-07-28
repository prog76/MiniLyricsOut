// HTTPReader.cpp: implementation of the CHTTPReader class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "HTTPReader.h"

#include <string.h>
#include <tchar.h>
#include <vector>
#include <fstream> 

bool proxyChanged=false;
tstring globalProxy=TEXT("system");

CHTTPReader::CHTTPReader(LPCTSTR lpszServerName,bool bUseSSL)
: m_hInternet(NULL),
  m_hConnection(NULL),
  m_hRequest(NULL),
  m_lpszDefaultHeader(NULL),
  m_lpszDataBuffer(NULL),
  m_dwBufferSize(0),
  m_bUseSSL(bUseSSL),
  m_dwLastError(0)
{
	proxy=globalProxy;
	int MaxCon=100;
	Flags=0;
	InternetSetOption(NULL,INTERNET_OPTION_MAX_CONNS_PER_SERVER,&MaxCon,sizeof(MaxCon));
	SetDefaultHeader(TEXT("Content-Type: application/x-www-form-urlencoded\r\n")
		TEXT("Accept-Language:ru\r\n")
		TEXT("Accept-Encoding:gzip, deflate"));
}

CHTTPReader::~CHTTPReader()
{
	delete[] m_lpszDataBuffer;
	SetDefaultHeader(NULL);
}

bool CHTTPReader::downloadFile(LPCTSTR file){
	char buf[1024];
	std::ofstream stream;
	TCHAR outFile[1024];
	ExpandEnvironmentStrings(file,outFile,sizeof(outFile)/sizeof(outFile[0]));
	stream.open(outFile,stream.binary);
	DWORD rlen;
	while(GetData(buf,sizeof(buf)-1,&rlen))stream.write(buf,rlen);	
	stream.close();
	return true;
}

bool CHTTPReader::CheckError(bool bTest)
{
	if (bTest == false) {
		m_dwLastError = ::GetLastError();
	}
	return bTest;
}

void CHTTPReader::SetTimeOut(int timeOut){
	if(pTimeOut!=timeOut){
		CloseInternet();
		pTimeOut=timeOut;
	}
}

bool CHTTPReader::OpenInternet(LPCTSTR lpszAgent)
{
	if(proxy.compare(globalProxy)!=0){
		CloseInternet();
		proxy=globalProxy;
	}
    
	if (m_hInternet == NULL){
		DWORD inetType;
		if (proxy.size()==0)inetType=INTERNET_OPEN_TYPE_DIRECT;
		else if(proxy.compare(TEXT("system"))==0)inetType=INTERNET_OPEN_TYPE_PRECONFIG;
		else inetType=INTERNET_OPEN_TYPE_PROXY;
		m_hInternet = ::InternetOpen(
			lpszAgent,
			inetType,
			proxy.c_str(),
			NULL,
			Flags);
		if(pTimeOut!=0){
			InternetSetOption(m_hInternet,INTERNET_OPTION_CONNECT_TIMEOUT, &pTimeOut,sizeof(pTimeOut));
			InternetSetOption(m_hInternet,INTERNET_OPTION_SEND_TIMEOUT, &pTimeOut,sizeof(pTimeOut));
			InternetSetOption(m_hInternet,INTERNET_OPTION_RECEIVE_TIMEOUT, &pTimeOut,sizeof(pTimeOut));
		}
	}
	return CheckError(m_hInternet != NULL);
}

void CHTTPReader::CloseInternet (){
	CloseConnection();
	if (m_hInternet)::InternetCloseHandle(m_hInternet);
	m_hInternet = NULL;
}

bool CHTTPReader::OpenConnection (tstring serverName){
	int port=m_bUseSSL? INTERNET_DEFAULT_HTTPS_PORT: INTERNET_DEFAULT_HTTP_PORT;
	int pos=serverName.find_first_of(TEXT(":"));
	if(pos>0){
		port=tcstol(serverName.substr(pos+1).c_str(),NULL,10);
		serverName=serverName.substr(0,pos);
	}
	if (OpenInternet() && m_hConnection == NULL)
		m_hConnection = ::InternetConnect(
			m_hInternet,
			serverName.length()>0?
				serverName.c_str():
					TEXT("localhost"),
			port,
			NULL,
			NULL,
			INTERNET_SERVICE_HTTP,
			0,
			1u);
	return CheckError(m_hConnection != NULL);
}

void CHTTPReader::CloseConnection (){
	CloseRequest();
	if (m_hConnection)::InternetCloseHandle(m_hConnection);
	m_hConnection = NULL;
}

bool CHTTPReader::SendRequest (LPCTSTR lpszVerb,
							   LPCTSTR lpszAction,
							   std::string *Data,
							   LPCTSTR lpszReferer){
	if (OpenConnection()) {
		CloseRequest();
		LPCTSTR AcceptTypes[] = { TEXT("*/*"), NULL};
		m_hRequest = ::HttpOpenRequest(
			m_hConnection,
			lpszVerb,
			lpszAction,
			NULL,
			lpszReferer,
			AcceptTypes,
			(m_bUseSSL? INTERNET_FLAG_SECURE|INTERNET_FLAG_IGNORE_CERT_CN_INVALID: 0)
				| INTERNET_FLAG_KEEP_CONNECTION,
			1);
		if (m_hRequest != NULL) {
			if (::HttpSendRequest(
					m_hRequest,
					m_lpszDefaultHeader,
					-1,
					Data? (LPVOID)Data->c_str():0,
					Data? Data->size(): 0) == FALSE) {
				CheckError(false);
				CloseRequest();
				return false;
			}
		}
	}
	return CheckError(m_hRequest != NULL);
}

bool CHTTPReader::Get (LPCTSTR lpszAction,LPCTSTR lpszReferer){
	return SendRequest(TEXT("GET"),lpszAction,NULL,lpszReferer);
}

bool CHTTPReader::Post (LPCTSTR lpszAction,std::string *Data,LPCTSTR lpszReferer){
	return SendRequest(TEXT("POST"),lpszAction,Data,lpszReferer);
}

void CHTTPReader::CloseRequest (){
	if (m_hRequest)::InternetCloseHandle(m_hRequest);
	m_hRequest = NULL;
}

void CHTTPReader::StrDup (LPTSTR& lpszDest,LPCTSTR lpszSource){
	delete[] lpszDest;
	if (lpszSource == NULL) {
		lpszDest = NULL;
	} else {
		lpszDest = new TCHAR[_tcslen(lpszSource)+1];
		_tcscpy(lpszDest,lpszSource);
	}
}

void CHTTPReader::SetDefaultHeader (LPCTSTR lpszDefaultHeader){
	StrDup(m_lpszDefaultHeader,lpszDefaultHeader);
}

char *CHTTPReader::GetData (char *lpszBuffer,DWORD dwSize,DWORD *lpdwBytesRead){
	DWORD dwBytesRead;
	if (lpdwBytesRead == NULL)
		lpdwBytesRead = &dwBytesRead;
	*lpdwBytesRead = 0;

	if (m_hRequest) {
		bool bRead = ::InternetReadFile(
			m_hRequest,
			lpszBuffer,
			dwSize,
			lpdwBytesRead) != FALSE;
		lpszBuffer[*lpdwBytesRead] = 0;

		return CheckError(bRead) && *lpdwBytesRead? lpszBuffer: NULL;
	}

	return NULL;
}

char *CHTTPReader::GetData (DWORD *lpdwBytesRead){
	DWORD readed,totalReaded,bufCnt;;
	static int const bufLen=1024;
	totalReaded=0;
	char *buffer;
	bool bRead;
	bufCnt=0;
	totalReaded=0;
	std::vector<char*> data;
	do{
		buffer=new char[bufLen];
		int bufPos=0;
		data.push_back(buffer);
		do{
			bRead=InternetReadFile(m_hRequest, &buffer[bufPos], bufLen-bufPos, &readed);
			if(bRead)bufPos+=readed;
		}while((bRead)&&(bufPos<bufLen)&&(readed>0));
		readed=bufPos;
		totalReaded+=readed;
		bufCnt++;
	}while ((bRead)&&(readed==bufLen));
	if(lpdwBytesRead)*lpdwBytesRead=totalReaded;
	SetDataBuffer(bufCnt*bufLen+1);
	for (int i=0;i<data.size();i++){
		if(bRead)
			memcpy(&m_lpszDataBuffer[i*bufLen],data[i],bufLen);
		delete[] data[i];
	}
	m_lpszDataBuffer[totalReaded]=0;

	return CheckError(bRead)? m_lpszDataBuffer: NULL;
}

DWORD CHTTPReader::GetDataSize (){
	if (m_hRequest) {
		DWORD dwDataSize = 0;
		DWORD dwLengthDataSize = sizeof(dwDataSize);
		BOOL bQuery = ::HttpQueryInfo(
			m_hRequest,
			HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER,
			&dwDataSize,
			&dwLengthDataSize,
			NULL);
		return bQuery? dwDataSize: 0;
	}
	return 0;
}

void CHTTPReader::SetDataBuffer (DWORD dwBufferSize)
{
	if (dwBufferSize > m_dwBufferSize) {
		delete[] m_lpszDataBuffer;
		m_lpszDataBuffer = new char[(m_dwBufferSize = dwBufferSize) + 1];
	}
}

bool CHTTPReader::GetUrl(tstring url){
	OpenInternet();
	m_hRequest=InternetOpenUrl(m_hInternet, url.c_str(), 0, 0, INTERNET_FLAG_EXISTING_CONNECT, 0);
	if(m_hRequest==0){
		printf("HTTP:: Get %s Err:%d\n",url.c_str(),GetLastError());
		CloseConnection();
		return false;
	}
	return true;
}
