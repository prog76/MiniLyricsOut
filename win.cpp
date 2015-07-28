// GT_HelloWorldWin32.cpp
// compile with: /D_UNICODE /DUNICODE /DWIN32 /D_WINDOWS /c

#include "stdafx.h"
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>

static TCHAR szWindowClass[] = _T("win32app");


static TCHAR szTitle[] = _T("Win32 Guided Tour Application");

HINSTANCE hInst;
RECT rect, lineSize;
HFONT bigFont=0, curFont=0;
HWND hWnd;
int fontSize = 10;

TCHAR *str1=0,*str2=0;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void Refresh(){
  InvalidateRect(hWnd,&rect, true);
  }

int getIni(LPTSTR varName, int defVal)
  {
  TCHAR buff[512];
  GetPrivateProfileString(_T("mlpTranslate"), varName, _T(""), buff, sizeof(buff)/sizeof(buff[0]), _T("mlpTranslate.ini"));
  int v=_ttol(buff);
  if (v==0) v=defVal;
  return v;
  }

void setIni(LPTSTR varName, int value)
  {
  TCHAR buff[512];
  _ltot(value, buff, 10);
  WritePrivateProfileString(_T("mlpTranslate"), varName, buff, _T("mlpTranslate.ini"));
  }

void savePosition()
  {
  GetWindowRect(hWnd, &rect); 
  setIni(_T("X"), rect.left);
  setIni(_T("Y"), rect.top);
  }

HFONT getFont(HFONT oldFont,double factor,double factor2)
  {
  LOGFONT font = { 0 };
  GetObject(oldFont, sizeof(LOGFONT), &font);
  HFONT new_font=CreateFont(font.lfHeight*factor*fontSize/10, font.lfWidth*factor*fontSize/10, font.lfEscapement,font.lfOrientation, font.lfWeight*factor2, font.lfItalic, font.lfUnderline, font.lfStrikeOut,font.lfCharSet,font.lfOutPrecision, font.lfClipPrecision, NONANTIALIASED_QUALITY, font.lfPitchAndFamily, TEXT("Tahoma"));
  return new_font;
  }

void initFont(HDC hdc){
  HFONT currentFont;
  curFont=CreateFont(-20, 0, 0, 0, FW_NORMAL, 0, 0, 0, RUSSIAN_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, NONANTIALIASED_QUALITY, VARIABLE_PITCH|FF_DONTCARE, L"Times New Roman");
  currentFont = (HFONT)GetCurrentObject(hdc, OBJ_FONT);
  bigFont   = getFont(currentFont,1.2,2.5);
  TCHAR str[] = _T("WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW");
  SelectObject(hdc,bigFont);
  DrawText(hdc, str, _tcslen(str), &lineSize, DT_CALCRECT);
  SetWindowPos(hWnd, 0, getIni(_T("X"), 10), getIni(_T("Y"), 10), lineSize.right, lineSize.bottom*2.5, 0);
  }

void outTxt1(HDC hdc,RECT mrect,int x, int y,const TCHAR *txt){
  mrect.bottom+=y-1;
  mrect.top+=y+1;
  mrect.left+=x-1;
  mrect.right+=x+1;
  DrawText(hdc, txt,_tcslen(txt), &mrect, DT_CENTER | DT_VCENTER);
  }

void outTxt(HDC hdc,RECT rect,int line,const TCHAR *txt){
  if(txt==0)return;
  rect.right;
  rect.top=line*lineSize.bottom+lineSize.bottom*0.2;
  rect.bottom=rect.top+lineSize.bottom*1.2;
  SelectObject(hdc,bigFont);
  SetTextColor(hdc, RGB(10, 10, 10));
  SetBkMode(hdc, TRANSPARENT);
  outTxt1(hdc,rect,1,1,txt);
  outTxt1(hdc,rect,-1,-1,txt);
  outTxt1(hdc,rect,-1,1,txt);
  outTxt1(hdc,rect,1,-1,txt);
  SetTextColor(hdc, RGB(255, 255, 10));
  outTxt1(hdc,rect,0,0,txt);
  }

int WINAPI WinMain(HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPSTR lpCmdLine,
  int nCmdShow)
  {
  WNDCLASSEX wcex;

  wcex.cbSize = sizeof(WNDCLASSEX);
  wcex.style          = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc    = WndProc;
  wcex.cbClsExtra     = 0;
  wcex.cbWndExtra     = 0;
  wcex.hInstance      = hInstance;
  wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
  wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
  wcex.lpszMenuName   = NULL;
  wcex.lpszClassName  = szWindowClass;
  wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

  if (!RegisterClassEx(&wcex))
    {
    MessageBox(NULL,
      _T("Call to RegisterClassEx failed!"),
      _T("Win32 Guided Tour"),
      NULL);

    return 1;
    }

  hInst = hInstance; // Store instance handle in our global variable
  hWnd = CreateWindowEx(WS_EX_LAYERED | WS_THICKFRAME*0 | WS_EX_TOPMOST, szWindowClass, NULL, WS_POPUP, 10,10,10,10, NULL, NULL, hInstance, NULL);
  if (!hWnd){
    MessageBox(NULL,
      _T("Call to CreateWindow failed!"),
      _T("Win32 Guided Tour"),
      NULL);

    return 1;
    }

  int style = GetWindowLong(hWnd, GWL_STYLE);
  SetWindowLong(hWnd, GWL_STYLE, (style & ~WS_CAPTION));
  SetLayeredWindowAttributes( hWnd,         // handle to window to modify
    RGB(255,255,255),  // colorkey
    0,            // 0=transparent, 255=completely solid
    LWA_COLORKEY);

  SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_TOPMOST);
  ShowWindow(hWnd,nCmdShow);
  UpdateWindow(hWnd);

  // Main message loop:
  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0))
    {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
    if(msg.message == WM_CLOSE)break;
    }

  return (int) msg.wParam;
  }

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
  {
  PAINTSTRUCT ps;
  HDC hdc;

  switch (message)
    {
  case WM_MOUSEWHEEL:
    hdc = BeginPaint(hWnd, &ps);
    if(GET_WHEEL_DELTA_WPARAM(wParam) > 0)
      fontSize++;
    else
      fontSize--;
    savePosition();
    setIni(_T("Font"), fontSize);
    initFont(hdc);
  case WM_PAINT:
    GetClientRect(hWnd,&rect);
    hdc = BeginPaint(hWnd, &ps);
    if(curFont==0){
      fontSize = getIni(_T("Font"), 10);
      initFont(hdc);
      }
    outTxt(hdc,rect,0,str1);
    outTxt(hdc,rect,1,str2);
    EndPaint(hWnd, &ps);
    break;
  case WM_NCHITTEST:
    return HTCAPTION;

  case WM_CLOSE:
    savePosition();
    PostQuitMessage(0);
    break;
  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
    break;
    }

  return 0;
  }

DWORD WINAPI Thread(LPVOID lpParam ){
  WinMain(NULL,NULL,NULL,SW_SHOW);
  return 0;
  }


class TWnd{
public:
  TWnd(){
    HANDLE Handle = CreateThread( NULL, 0, Thread, NULL, 0, NULL);  
    }
  }wnd;