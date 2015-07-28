// MLyricsOutPlugin.cpp: implementation of the MLyricsOutPlugin class.
//
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <tchar.h>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include "codecvt.h"
#include "MLyricsOutPlugin.h"

HINSTANCE				g_hInst = NULL;
IMLyrOutHost			*g_pMLyrOutHost;
CG15LCDLyricsOutPlugin	g_g15LcdPlugin;

#include "HTTPReader.h"
extern std::wstring gTranslate(const TCHAR *from,const TCHAR *to, const TCHAR *txt);
extern TCHAR *str1,*str2;
extern void Refresh();
extern HWND hWnd;

tstring fileName;
typedef std::vector<std::wstring> TWLines;
TWLines wlines;

int str2No;

extern "C"
{
	__declspec (dllexport) int InitMLyricsOutPlugin(HINSTANCE hDllInstance, IMLyrOutHost *pMLyrOutHost, ILyricsOut **ppLyricsOut)
	{
		g_hInst = hDllInstance;
		*ppLyricsOut = &g_g15LcdPlugin;
		g_pMLyrOutHost = pMLyrOutHost;

		return ML_LYR_OUT_VERSION;
	}
};

CG15LCDLyricsOutPlugin::CG15LCDLyricsOutPlugin()
{

}

CG15LCDLyricsOutPlugin::~CG15LCDLyricsOutPlugin()
{

}

void CG15LCDLyricsOutPlugin::Config(HWND hWndParent)
{
	MessageBox(hWndParent, L"Configuration...", L"G15", MB_OK);
}

void CG15LCDLyricsOutPlugin::About(HWND hWndParent)
{
	MessageBox(hWndParent, L"About...", L"G15", MB_OK);
}

LPCTSTR CG15LCDLyricsOutPlugin::GetDescription()
{
	return L"Your plugin's description...";
}

bool CG15LCDLyricsOutPlugin::OnInit(unsigned int *uNotifyFlag)
{
	*uNotifyFlag = NOTIF_CUR_LINE | NOTIF_HALF_CUR_LINE;

	return true;
}

void setStr(TCHAR* &str, const TCHAR *sstr){
	if(str)free(str);
	str=wcsdup(sstr);
}

void CG15LCDLyricsOutPlugin::OnCurLineChanged(int nLine, LPCTSTR szLyrics, int nBegTime, int nEndTime)
{
	if(nLine<wlines.size()){
		setStr(str1,wlines[nLine].c_str());
		if(str2No==nLine){
			setStr(str2,L"");
		}
		Refresh();
	}
}

void CG15LCDLyricsOutPlugin::OnHalfOfCurLine(int nLine)
{
	TCHAR		szLyrics[512] = L"";
	int			nBegTime, nEndTime;
	bool		bRet;
	nLine++;
	bRet = g_pMLyrOutHost->GetLyricsOfLine(nLine, szLyrics, sizeof(szLyrics)/sizeof(TCHAR), nBegTime, nEndTime);
	if (bRet)
	{
		// show Next line?
		if(nLine<wlines.size()){
			setStr(str2,wlines[nLine].c_str());
			str2No=nLine;
			Refresh();
		}
	}
}

void CG15LCDLyricsOutPlugin::OnPlayPos(int uPos)
{

}

tstring getFile(){
	TCHAR fil[1024];
	tstring res;
	g_pMLyrOutHost->GetMediaFile(fil,sizeof(fil));
	fil[0]='d';
	res=fil;
	res+=TEXT(".rus");
	return res;
}

void CG15LCDLyricsOutPlugin::OnLyricsChanged()
{
	if(fileName.compare(getFile())==0){
		DeleteFile(fileName.c_str());
		OnSongChanged(NULL,NULL,0);
	}
}

void CG15LCDLyricsOutPlugin::OnSongChanged(LPCTSTR szArtist, LPCTSTR szTitle, int nDuration){
	TCHAR line[1024];
	wlines.clear();
	int tmp;
	std::wstringstream strm;
	std::wstring str;
	fileName=getFile();
	FILE *file=_tfopen(fileName.c_str(),TEXT("r"));
	if(file==NULL){
		for(int i=0;i<g_pMLyrOutHost->GetLineCount();i++){
			g_pMLyrOutHost->GetLyricsOfLine(i,line,sizeof(line),tmp,tmp);
			strm<<line<<std::endl;
		}
		str=gTranslate(TEXT("auto"),TEXT("Ru"),strm.str().c_str());
		if(str.size()>0){
			file=_tfopen(fileName.c_str(),TEXT("w+b"));
			if(file)//fputws(str.c_str(),file);
				fwrite(str.c_str(), str.length(),sizeof(wchar_t), file);
		}
	}
	if(file)fclose(file);

	wchar_t wline[1024];
	std::wifstream wifstrm(fileName.c_str(), std::wifstream::in|std::ios::binary);
	wifstrm.imbue(std::locale(wifstrm.getloc(), new ucs2_conversion()));
	while(wifstrm.getline(wline,sizeof(wline)/2))wlines.push_back(wline);
	wifstrm.close();
}

void CG15LCDLyricsOutPlugin::OnQuit()
{
	SendMessage(hWnd, WM_CLOSE,0,0);
}

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}
