/*
 ------------------------------------------------------------------------------
 * StatBoxImps.cpp --
 *
 *   Platform specific implimentation class of the component.
 *
 * Copyright (c) 1999-2000 Tomahawk Software Group
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: StatBoxImps.cpp,v 1.1 2003/11/14 10:36:21 davygrvy Exp $
 ------------------------------------------------------------------------------
 */
#include "StatBoxImps.hpp"

// declared in IRCWindowsSystemFactory.cpp
extern const HINSTANCE hAppInst;

// use message crackers
#include <windowsx.h>

#ifdef _DEBUG
#pragma comment (lib, "mclsd.lib")
#else
#pragma comment (lib, "mcls.lib")
#endif

AllWin32StatBoxImp::AllWin32StatBoxImp ()
{

  // do first time inits ?
  //
  if (first)
  {
    WNDCLASSEX wndcls;

    first = false;

    // register our StatBox window class
    //
    wndcls.cbSize = sizeof (WNDCLASSEX);
    wndcls.style = CS_HREDRAW | CS_VREDRAW;
    wndcls.cbClsExtra = 0;
    wndcls.cbWndExtra = sizeof (HANDLE);
    wndcls.hInstance = hAppInst;
    wndcls.hCursor = LoadCursor (NULL, IDC_ARROW);
    wndcls.lpszMenuName = NULL;
    wndcls.hbrBackground = CreateSolidBrush (0x00000000);
    wndcls.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wndcls.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wndcls.lpfnWndProc = StatBoxWndProc;
    wndcls.lpszClassName = "StatBox";

    ::RegisterClassEx (&wndcls);
  }

  // create a new statbox window in its own thread w/ message pump
  //
  pGUI = new AllWin32StatBoxThreadedGUI ();
  pGUIThread = new CMclThread (pGUI);
};

AllWin32StatBoxImp::~AllWin32StatBoxImp ()
{
  DWORD exitcode;

  ::SendMessage (pGUI->GetHwnd (), WM_DESTROY, 0, 0);
  ::PostThreadMessage (pGUIThread->GetThreadId (), WM_QUIT, 0, 0);

  do
  {
    ::Sleep (30);               // be friendly :)
    pGUIThread->GetExitCode (&exitcode);
  }
  while (exitcode == STILL_ACTIVE);

  delete pGUI;
};

void
AllWin32StatBoxImp::Echo (char *text, int len)
{
  pGUI->Echo (text, len);
};
void
AllWin32StatBoxImp::SetTitle (char *text, int len)
{
};

bool AllWin32StatBoxImp::first = true;


AllWin32StatBoxThreadedGUI::AllWin32StatBoxThreadedGUI ()
{
}

AllWin32StatBoxThreadedGUI::~AllWin32StatBoxThreadedGUI ()
{
  ::DeleteObject (hBrush);
};

unsigned
AllWin32StatBoxThreadedGUI::ThreadHandlerProc ()
{
  MSG msg;

  hwndStatBox =::CreateWindowEx (WS_EX_WINDOWEDGE,
                                 "StatBox",
                                 "This is a Status window under test",
                                 WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN |
                                 WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT,
                                 CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL,
                                 hAppInst, static_cast < LPVOID > (this));

  // process windows messages in our little lightweight mini application
  //
  while (GetMessage (&msg, NULL, 0, 0))
  {
    TranslateMessage (&msg);
    DispatchMessage (&msg);
  }

  return msg.wParam;
};

// Tcl's thread enters here
//
void
AllWin32StatBoxThreadedGUI::Echo (char *text, int len)
{
  std::string * str;

  str = new std::string (text);
  m_echoQ.Put (str);
};

HWND AllWin32StatBoxThreadedGUI::GetHwnd ()
{
  return hwndStatBox;
};


void
AllWin32StatBoxThreadedGUI::OnSize
  (HWND hwnd, UINT fwSizeType, int nWidth, int nHeight)
{
  ::MoveWindow (pView->GetHwnd (), 0, 0, nWidth, nHeight - pCmd->Getcy (),
                TRUE);
  ::MoveWindow (pCmd->GetHwnd (), 0, nHeight - pCmd->Getcy (), nWidth,
                pCmd->Getcy (), TRUE);
};


void
AllWin32StatBoxThreadedGUI::OnClose (HWND hwnd)
{
  // Ouch!  How do we handle this?  We have to notify Tcl that
  // this object is going away and let Tcl destroy it instead.
  // This is wrong for now, as Tcl doesn't know we went away, but
  // a start at least.
  ::DestroyWindow (hwnd);
};


void
AllWin32StatBoxThreadedGUI::OnDestroy (HWND hwnd)
{
  delete pView, pCmd;
};


LRESULT
  CALLBACK StatBoxWndProc (HWND hwnd, UINT iMsg, WPARAM wParam,
                           LPARAM lParam)
{
  AllWin32StatBoxThreadedGUI *pStatGUI =
    reinterpret_cast < AllWin32StatBoxThreadedGUI * >
    (::GetWindowLongPtr (hwnd, GWLP_USERDATA));

  switch (iMsg)
  {
  case WM_CREATE:
    // move the AllWin32StatBoxThreadedGUI class pointer into the
    // userdata of the window.
    //
    ::SetWindowLongPtr
      (hwnd, GWLP_USERDATA, reinterpret_cast < LONG_PTR >
       ((((LPCREATESTRUCT) lParam)->lpCreateParams)));

    // Supposedly, lpCreateParams has a DWORD alignment problem.
    // I don't know what this means for pointers.  I doubt much.
    //
    pStatGUI =
      reinterpret_cast < AllWin32StatBoxThreadedGUI * >
      ((((LPCREATESTRUCT) lParam)->lpCreateParams));

    pStatGUI->pView = new AllWin32ViewBoxImp (hwnd, pStatGUI->m_echoQ);
    pStatGUI->pCmd = new AllWin32CmdBoxImp (hwnd);

    pStatGUI->hBrush = CreateSolidBrush (0x00000000);

    return 0;

    HANDLE_MSG (hwnd, WM_CLOSE, pStatGUI->OnClose);
    HANDLE_MSG (hwnd, WM_SIZE, pStatGUI->OnSize);
    HANDLE_MSG (hwnd, WM_DESTROY, pStatGUI->OnDestroy);

  case WM_CTLCOLOREDIT:
    {
      HDC hdc = reinterpret_cast < HDC > (wParam);

      //::DefMDIChildProc(hwnd, iMsg, wParam, lParam);
      ::DefWindowProc (hwnd, iMsg, wParam, lParam);
      ::SetTextColor (hdc, 0x00C0C0C0);
      ::SetBkMode (hdc, TRANSPARENT);
      return reinterpret_cast < LRESULT > (pStatGUI->hBrush);
    }

    //case WM_MDIACTIVATE: {
    //  if (lParam == (LPARAM) hwnd) {
    //    ::SendMessage(hwndClient, WM_MDISETMENU, (WPARAM) hMenuStat, (LPARAM) hMenuStatWindow) ;
    //  } else {
    //    ::SendMessage(hwndClient, WM_MDISETMENU, (WPARAM) hMenuStat, (LPARAM) hMenuStatWindow) ;
    //  }
    //  ::DrawMenuBar (hwndFrame) ;
    //  return 0 ;
    //}
  }
  //return ::DefMDIChildProc(hwnd, iMsg, wParam, lParam);
  return::DefWindowProc (hwnd, iMsg, wParam, lParam);
}

