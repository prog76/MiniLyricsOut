// MLyricsOutPlugin.h: interface for the MLyricsOutPlugin class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MLYRICSOUTPLUGIN_H__AC6B10FD_1404_49F5_84C6_FBBF78039A44__INCLUDED_)
#define AFX_MLYRICSOUTPLUGIN_H__AC6B10FD_1404_49F5_84C6_FBBF78039A44__INCLUDED_

#include "../MPShared/LyricsOutPlugin.h"

class CG15LCDLyricsOutPlugin : public ILyricsOut
{
public:
	CG15LCDLyricsOutPlugin();
	~CG15LCDLyricsOutPlugin();

	virtual void Config(HWND hWndParent);
	virtual void About(HWND hWndParent);
	virtual LPCTSTR GetDescription();

	virtual bool OnInit(unsigned int *uNotifyFlag);

	virtual void OnCurLineChanged(int nLine, LPCTSTR szLyrics, int nBegTime, int nEndTime);
	virtual void OnHalfOfCurLine(int nLine);
	virtual void OnPlayPos(int uPos);
	virtual void OnLyricsChanged();
	virtual void OnSongChanged(LPCTSTR szArtist, LPCTSTR szTitle, int nDuration);
	virtual void OnQuit();

};

#endif // !defined(AFX_MLYRICSOUTPLUGIN_H__AC6B10FD_1404_49F5_84C6_FBBF78039A44__INCLUDED_)
