#include "stdafx.h"
#include "HTTPReader.h"
#include <sstream>
tstring char2hex( TCHAR dec ){
    TCHAR dig1 = (dec&0xF0)>>4;
    TCHAR dig2 = (dec&0x0F);
    if ( 0<= dig1 && dig1<= 9) dig1+=48;    //0,48inascii
    if (10<= dig1 && dig1<=15) dig1+=97-10; //a,97inascii
    if ( 0<= dig2 && dig2<= 9) dig2+=48;
    if (10<= dig2 && dig2<=15) dig2+=97-10;

    tstring r;
    r.append( &dig1, 1);
    r.append( &dig2, 1);
    return r;
}


tstring urlencode(const tstring &c){
    tstring escaped=TEXT("");
    int max = c.length();
    for(int i=0; i<max; i++)
    {
        if ( (48 <= c[i] && c[i] <= 57) ||//0-9
             (65 <= c[i] && c[i] <= 90) ||//abc...xyz
             (97 <= c[i] && c[i] <= 122) || //ABC...XYZ
             (c[i]=='~' || c[i]=='!' || c[i]=='*' || c[i]=='(' || c[i]==')' || c[i]=='\'')
        )
        {
            escaped.append( &c[i], 1);
        }
        else
        {
            escaped.append(TEXT("%"));
            escaped.append( char2hex(c[i]) );//converts char 255 to string "ff"
        }
    }
    return escaped;
}

std::wstring utf82wchar(const char* str){
	int size = MultiByteToWideChar(CP_UTF8, 0, str, -1, 0, 0);
    wchar_t* resW = new wchar_t[size];
	MultiByteToWideChar(CP_UTF8, 0, str, -1, resW, size);
	std::wstring res=resW;
	delete [] resW;
	return res;
}

std::string tchar2utf8(const TCHAR* strText)
{
        int nSize;

#ifdef _UNICODE
        LPCWSTR pszUnicodeText = strText;
#else
        nSize = ::MultiByteToWideChar(CP_ACP, 0, (LPCSTR)strText, -1, NULL, 0);
        if (nSize == 0)
                return "";

        LPWSTR pszUnicodeText = new WCHAR[nSize];
        ::MultiByteToWideChar(CP_ACP, 0, (LPCSTR)strText, -1, pszUnicodeText, nSize);
#endif

        nSize = ::WideCharToMultiByte(CP_UTF8, 0, pszUnicodeText, -1, NULL, 0, NULL, NULL);
        if (nSize == 0)
        {
#ifndef _UNICODE
                delete[] pszUnicodeText;
#endif
                return "";
        }

        LPSTR pszTextUTF8 = new CHAR[nSize];
        ::WideCharToMultiByte(CP_UTF8, 0, pszUnicodeText, -1, pszTextUTF8, nSize, NULL, NULL);

		std::string res=pszTextUTF8;
        delete[] pszTextUTF8;

#ifndef _UNICODE
        delete[] pszUnicodeText;
#endif
        return res;
}


std::wstring gTranslate(const TCHAR *from,const TCHAR *to, const TCHAR *txt){
	CHTTPReader reader;
	reader.SetDefaultHeader(
			TEXT("Accept-Encoding: deflate\r\n")
			TEXT("Accept-Language: en,ru\r\n")
			TEXT("Accept-Charset: utf-8\r\n")
			TEXT("Content-Type: application/x-www-form-urlencoded; charset=UTF-8\r\n")
	);
	reader.SetTimeOut(15000);
	reader.OpenInternet(TEXT("'Mozilla/5.0 (Windows; U; Windows NT 5.1; ru; rv:1.9.0.8) Gecko/2009032609 Firefox/3.0.8"));
	reader.OpenConnection(TEXT("translate.google.com"));
	std::string res;
	tstring url	= TEXT("/translate_a/t");
	tstring url2= TEXT("client=t&otf=1&pc=1&ie=utf-8&oe=utf-8&sl=");
	url2+=from;
	url2+=TEXT("&tl=");
	url2+=to;
	url2+=TEXT("&text=. ");
	url2+=urlencode(txt);

	std::string data=tchar2utf8(url2.c_str());
	reader.Post(url.c_str(),&data);
	char* cres=reader.GetData();
	reader.CloseConnection();
	if(cres)res=cres;
	else return L"";
	std::wstring wres=utf82wchar(res.c_str());
	std::wstringstream strm;
	std::wstring line;
	int start=0;
	while(((start=(wres.find(L"[\"",start)+2))>2)){
		int end=wres.find(L"\",\"",start);
		line=wres.substr(start,end-start);
		while((start=line.find(L"\\n"))>=0){
			strm<<line.substr(0,start)<<std::endl;
			line.erase(0,start+2);
		}
		if(line.length()>0)strm<<line<<" ";
		start=end;
	}
	return strm.str();
}