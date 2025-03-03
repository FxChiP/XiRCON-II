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
 * RCS: @(#) $Id: StatBoxImps.cpp,v 1.1 2003/11/14 10:57:46 davygrvy Exp $
 ------------------------------------------------------------------------------
 */
#include "StatBoxImps.hpp"

extern HINSTANCE hInstance;
extern HWND hClient;
extern HWND hFrame;

// use message crackers
#include <windowsx.h>

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
	wndcls.style = MDIS_ALLCHILDSTYLES | CS_HREDRAW | CS_VREDRAW;
	wndcls.cbClsExtra = 0;
	wndcls.cbWndExtra = sizeof (HANDLE);
	wndcls.hInstance = hInstance;
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
AllWin32StatBoxImp::Echo (wchar_t *text, size_t len)
{
    pGUI->Echo (text, len);
};
void
AllWin32StatBoxImp::SetTitle (wchar_t *text, size_t len)
{
};

bool AllWin32StatBoxImp::first = true;


AllWin32StatBoxThreadedGUI::AllWin32StatBoxThreadedGUI ()
{
}

AllWin32StatBoxThreadedGUI::~AllWin32StatBoxThreadedGUI ()
{
    ::DeleteObject (hBrush);
}

unsigned
AllWin32StatBoxThreadedGUI::ThreadHandlerProc ()
{
    MSG msg;

    hwndStatBox = ::CreateMDIWindow (
	    "StatBox",
	    "This is a Status window under test",
	    0,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
	    hClient, hInstance, reinterpret_cast <LONG_PTR>(this)
	    );

    // process windows messages in our little lightweight mini application thread.
    //
    while (::GetMessage (&msg, NULL, 0, 0))
    {
	::TranslateMessage (&msg);
	::DispatchMessage (&msg);
    }
    return msg.wParam;
}

// Tcl's thread enters here
//
void
AllWin32StatBoxThreadedGUI::Echo (wchar_t *text, size_t len)
{
    std::wstring *str = new std::wstring(text);
    m_echoQ.Put(str);
}

HWND AllWin32StatBoxThreadedGUI::GetHwnd ()
{
    return hwndStatBox;
}


void
AllWin32StatBoxThreadedGUI::OnSize
  (HWND hwnd, UINT fwSizeType, int nWidth, int nHeight)
{
    ::MoveWindow (pView->GetHwnd(), 0, 0, nWidth, nHeight - pCmd->Getcy(),
	    TRUE);
    ::MoveWindow (pCmd->GetHwnd(), 0, nHeight - pCmd->Getcy(), nWidth,
	    pCmd->Getcy(), TRUE);
}


void
AllWin32StatBoxThreadedGUI::OnClose (HWND hwnd)
{
    // Ouch!  How do we handle this?  We have to notify Tcl that
    // this object is going away and let Tcl destroy it instead.
    // This is wrong for now, as Tcl doesn't know we went away, but
    // a start at least.
    ::DestroyWindow(hwnd);
}


void
AllWin32StatBoxThreadedGUI::OnDestroy (HWND hwnd)
{
    delete pView, pCmd;
}


LRESULT CALLBACK
StatBoxWndProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    MDICREATESTRUCT *lpCreateParams;

    AllWin32StatBoxThreadedGUI *pStatGUI =
	    reinterpret_cast <AllWin32StatBoxThreadedGUI *>
	    (::GetWindowLongPtr (hwnd, GWLP_USERDATA));

    switch (iMsg)
    {
	case WM_CREATE:
	    // move the AllWin32StatBoxThreadedGUI class pointer into the
	    // userdata of the window.
	    //
	    lpCreateParams = (MDICREATESTRUCT *)((CREATESTRUCT *)lParam)->lpCreateParams;

	    ::SetWindowLongPtr (hwnd, GWLP_USERDATA,
		    static_cast <LONG_PTR>(lpCreateParams->lParam));

	    // Supposedly, lpCreateParams has a DWORD alignment problem.
	    // I don't know what this means for pointers.  I doubt much.
	    //
	    pStatGUI = reinterpret_cast <AllWin32StatBoxThreadedGUI *>
		    (lpCreateParams->lParam);

	    pStatGUI->pView = new AllWin32ViewBoxImp (hwnd, pStatGUI->m_echoQ);
	    pStatGUI->pCmd = new AllWin32CmdBoxImp (hwnd);

	    pStatGUI->hBrush = CreateSolidBrush (RGB(0, 0, 0) /*Black*/);

	    return 0;

	HANDLE_MSG (hwnd, WM_CLOSE, pStatGUI->OnClose);
	HANDLE_MSG (hwnd, WM_SIZE, pStatGUI->OnSize);
	HANDLE_MSG (hwnd, WM_DESTROY, pStatGUI->OnDestroy);

	case WM_CTLCOLOREDIT:
	{
	    HDC hdc = reinterpret_cast < HDC > (wParam);

	    //::DefMDIChildProc(hwnd, iMsg, wParam, lParam);
	    //::DefWindowProc (hwnd, iMsg, wParam, lParam);
	    ::SetTextColor (hdc, RGB(192, 192, 192) /*light gray*/);
	    ::SetBkColor (hdc, RGB(0, 0, 0) /*Black*/);
	    return reinterpret_cast < LRESULT > (pStatGUI->hBrush);
	}

	case WM_MDIACTIVATE: {
	  if (lParam == (LPARAM) hwnd) {
	    //::SendMessage(hClient, WM_MDISETMENU, (WPARAM) hMenuStat, (LPARAM) hMenuStatWindow) ;
	  } else {
	    //::SendMessage(hClient, WM_MDISETMENU, (WPARAM) hMenuStat, (LPARAM) hMenuStatWindow) ;
	  }
	  //::DrawMenuBar (hFrame) ;
	  return 0 ;
	}
    }
    return ::DefMDIChildProc(hwnd, iMsg, wParam, lParam);
    //return ::DefWindowProc (hwnd, iMsg, wParam, lParam);
}

