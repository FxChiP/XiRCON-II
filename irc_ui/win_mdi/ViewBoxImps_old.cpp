/*
 ------------------------------------------------------------------------------
 * ViewBoxImps.cpp --
 *
 *   Platform specific implimentation class of the component.
 *
 * Copyright (c) 1999-2000 Tomahawk Software Group
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: ViewBoxImps_old.cpp,v 1.1 2003/11/14 10:36:21 davygrvy Exp $
 ------------------------------------------------------------------------------
 */
#include "ViewBoxImps.hpp"

// use the message crackers
#include <windowsx.h>

#ifdef _DEBUG
#pragma comment(lib,"mclsd.lib")
#else
#pragma comment(lib,"mcls.lib")
#endif

#pragma warning (disable:4786)  // I don't care to know about name truncation

// declared in IRCWindowsSystemFactory.cpp
extern const HINSTANCE hAppInst;

AllWin32ViewBoxImp::AllWin32ViewBoxImp
(HWND parent, CMclQueue<std::string *> &echoQ)
  : m_echoQ(echoQ), bSBLock(false), iSBPos(0), iSBMax(0), iSBPage(0),
    iSBBaseLine(0)
{

  hSBAlert = ::CreateSolidBrush(RGB(255,0,0));

  if(first) {
    WNDCLASSEX wndcls;

    first = false;

    wndcls.cbSize        = sizeof(WNDCLASSEX);
    wndcls.style         = CS_HREDRAW | CS_VREDRAW;
    wndcls.cbClsExtra    = 0;
    wndcls.hInstance     = hAppInst;
    wndcls.hCursor       = ::LoadCursor(NULL, IDC_ARROW);
    wndcls.lpszMenuName  = NULL;
    wndcls.hbrBackground = ::CreateSolidBrush(0x000000);
    wndcls.hIcon         = NULL;
    wndcls.hIconSm       = NULL;
    wndcls.lpfnWndProc   = ViewBoxWndProc;
    wndcls.cbWndExtra    = 0;
    wndcls.lpszClassName = "ViewBox";
    ::RegisterClassEx (&wndcls);
  }

  echoQReaderThread = new CMclThread(this);

  hwndViewBox = 
    ::CreateWindowEx (
        WS_EX_CLIENTEDGE | WS_EX_NOPARENTNOTIFY,
        "ViewBox", NULL,
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | WS_VSCROLL,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        parent, NULL, hAppInst, static_cast<LPVOID>(this)
      );

  UpdateScroll();
};

AllWin32ViewBoxImp::~AllWin32ViewBoxImp() {
  DWORD exitcode;

  ceStop.Set();
  do {
    ::Sleep(30);   // be friendly :)
    echoQReaderThread->GetExitCode(&exitcode);
  } while (exitcode == STILL_ACTIVE);

  ::DeleteObject(hSBAlert);
};

void AllWin32ViewBoxImp::Echo(char *text, int len) {};
void AllWin32ViewBoxImp::SetTitle(char *text, int len) {};
HWND AllWin32ViewBoxImp::GetHwnd() { return hwndViewBox; };
bool AllWin32ViewBoxImp::first = true;

// this function is run in a seperate thread from the rest of this class
//
unsigned AllWin32ViewBoxImp::ThreadHandlerProc(void) {
  std::string *text;

  // wait forever for an echo to come down the Queue
  // and punch it, ...repeat :)
  //
  while (m_echoQ.Get(text, INFINITE, &ceStop)) {
    csLines.Enter();
    vLines.push_back(new Win32TextLine(text, hwndViewBox));
    iSBMax = vLines.size();
    csLines.Leave();
    if (!bSBLock) {
      iSBPos = iSBMax;
      ::InvalidateRect(hwndViewBox, NULL, TRUE);
      ::UpdateWindow(hwndViewBox);
    }
    UpdateScroll();
    delete text;
  }
  return 0;
}


void AllWin32ViewBoxImp::UpdateScroll () {
  SCROLLINFO si;

  si.cbSize = sizeof(SCROLLINFO);
  si.fMask = SIF_DISABLENOSCROLL | SIF_ALL;
  si.nMin = 0;
  si.nMax = iSBMax;
  si.nPos = iSBPos; //(iSBPage > 1 ? (iSBMax - iSBPage - 1) : iSBPos);
  si.nPage = 1; //iSBPage;
  ::SetScrollInfo(hwndViewBox, SB_VERT, &si, TRUE);
}


void
AllWin32ViewBoxImp::OnSize
(HWND hwnd, UINT fwSizeType, int nWidth, int nHeight)
{
  if (fwSizeType != SIZE_MINIMIZED)
  {
    cx = nHeight;
    if (nWidth != cy)
    {
      std::vector<Win32TextLine *>::reverse_iterator viLines;

      cy = nWidth;

      csLines.Enter();
      for (
        viLines = vLines.rbegin();
        viLines != vLines.rend();
        viLines++)
      {
        // invalidate all the cached bitmaps
        //
        (*viLines)->InvalidateY();
      }
      csLines.Leave();
    }
  }
};


void
AllWin32ViewBoxImp::OnPaint
(HWND hwnd)
{
  int baseline = cx;
  std::vector<Win32TextLine *>::reverse_iterator viLines;
  bool bDone = false;
  PAINTSTRUCT ps;
  HDC hdc;


  csLines.Enter();

  for (
    viLines = (bSBLock ? vLines.rend() - iSBBaseLine : vLines.rbegin()),
//      iSBPage = 0,
      hdc = ::BeginPaint(hwnd, &ps);
    viLines != vLines.rend() && !bDone;
    viLines++)
  {
//    iSBPage++;
    bDone = (*viLines)->Draw(hdc, cy, &baseline);
  };

  csLines.Leave();
  UpdateScroll();
  ::EndPaint(hwnd, &ps);
}


void
AllWin32ViewBoxImp::OnVScroll
(HWND hwnd, HWND hwndScrollBar, int nScrollCode, short int nPos)
{
  int yInc;

  switch (nScrollCode) {
    case SB_TOP:
      yInc = -iSBPos;
      break;
    case SB_BOTTOM:
      yInc = iSBMax - iSBPos;
      break;
    case SB_LINEUP:
      yInc = -1;
      break;
    case SB_LINEDOWN:
      yInc = 1;
      break;
    case SB_PAGEUP:
      yInc = min (-1, -iSBPage);
      break;
    case SB_PAGEDOWN:
      yInc = max (1, iSBPage);
      break;
    case SB_THUMBTRACK:
      yInc = nPos - iSBPos;
      break;
    default:
      // ignore the SB_ENDSCROLL for now
      return;
  }
  yInc = max(-iSBPos, min(yInc, iSBMax - iSBPos));
  if (yInc != 0) {
    iSBPos += yInc;
    ::InvalidateRect(hwnd, NULL, TRUE);
    ::UpdateWindow(hwnd);
  }
  UpdateScroll();
}


HBRUSH
AllWin32ViewBoxImp::OnCtlColor
(HWND hwnd, HDC hdc, HWND hwndChild, int type)
{
  return hSBAlert;
}


static LRESULT CALLBACK
ViewBoxWndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
  AllWin32ViewBoxImp *pViewBoxImp = 
      reinterpret_cast<AllWin32ViewBoxImp *>(
        ::GetWindowLongPtr(hwnd, GWLP_USERDATA)
      );

  switch (iMsg) {
  case WM_CREATE:
    ::SetWindowLongPtr(hwnd, GWLP_USERDATA,
        reinterpret_cast<LONG_PTR>(
          (((LPCREATESTRUCT) lParam)->lpCreateParams)
        )
    );
    return 0;

  HANDLE_MSG(hwnd, WM_SIZE,    pViewBoxImp->OnSize);
  HANDLE_MSG(hwnd, WM_PAINT,   pViewBoxImp->OnPaint);
  HANDLE_MSG(hwnd, WM_VSCROLL, pViewBoxImp->OnVScroll);
//  HANDLE_MSG(hwnd, WM_CTLCOLORSCROLLBAR, pViewBoxImp->OnCtlColor);

  }
  return ::DefWindowProc(hwnd, iMsg, wParam, lParam);
}

