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
 * RCS: @(#) $Id: StatBoxImps.cpp,v 1.2 2003/11/23 21:46:55 davygrvy Exp $
 ------------------------------------------------------------------------------
 */
#include "StatBoxImps.hpp"

extern HINSTANCE hInstance;
extern HWND hClient;
extern HWND hFrame;
extern HACCEL hAccelTbl;

// use message crackers
#include <windowsx.h>

LRESULT CALLBACK StatBoxWndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);


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
	wndcls.hbrBackground = NULL;
	wndcls.hIcon = LoadIcon (NULL, IDI_APPLICATION);
	wndcls.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
	wndcls.lpfnWndProc = StatBoxWndProc;
	wndcls.lpszClassName = TEXT("StatBox");

	::RegisterClassEx (&wndcls);
    }

    // create a new statbox window in its own thread w/ message pump
    //
    pGUI = new AllWin32StatBoxThreadedGUI();
    pGUIThread = new CMclThread(pGUI);
}

AllWin32StatBoxImp::~AllWin32StatBoxImp ()
{
    ::SendMessage(pGUI->GetHwnd(), WM_CLOSE, 0, 0);
    ::PostThreadMessage(pGUIThread->GetThreadId(), WM_QUIT, 0, 0);
    pGUIThread->Wait(INFINITE);
    delete pGUI;
}

void
AllWin32StatBoxImp::Echo (char16_t *text, size_t len)
{
    pGUI->Echo (text, len);
}

void
AllWin32StatBoxImp::SetTitle (char16_t *text, size_t len)
{
    int length;
    TCHAR *space = NULL;
    length = ::WideCharToMultiByte(CP_OEMCP, 0, (LPWCH)text, len, space, 0, NULL, NULL);
    space = new TCHAR[length];
    ::WideCharToMultiByte(CP_OEMCP, 0, (LPWCH)text, len, space, length, NULL, NULL);
    ::SetWindowText(pGUI->GetHwnd(), space);
    delete space;
}

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
	    TEXT("StatBox"),
	    TEXT("This is a Status window under test"),
	    0,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
	    hClient, hInstance, reinterpret_cast <LONG_PTR>(this)
	    );

    // process windows messages in our little lightweight mini application thread.
    //
    while (::GetMessage(&msg, NULL, 0, 0) > 0) {
	if (!::TranslateMDISysAccel(hClient, &msg) &&
		!::TranslateAccelerator(hFrame, hAccelTbl, &msg)) {
	    ::TranslateMessage(&msg);
	    ::DispatchMessage(&msg);
	}
    }
    return (int) msg.wParam;
}

// Tcl's thread enters here
//
void
AllWin32StatBoxThreadedGUI::Echo (char16_t *text, size_t len)
{
    std::u16string *str = new std::u16string(text, len);
    m_echoQ.Put(str);
}

HWND AllWin32StatBoxThreadedGUI::GetHwnd ()
{
    return hwndStatBox;
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

	case WM_SIZE:
	    if (wParam == SIZE_MINIMIZED) break;
	    ::MoveWindow (pStatGUI->pView->GetHwnd(), 0, 0, LOWORD(lParam),
		    HIWORD(lParam) - pStatGUI->pCmd->Getcy(), TRUE);
	    ::MoveWindow (pStatGUI->pCmd->GetHwnd(), 0,
		    HIWORD(lParam) - pStatGUI->pCmd->Getcy(),
		    LOWORD(lParam), pStatGUI->pCmd->Getcy(), TRUE);
	    // Must fall to DefMDIChildProc or bad things happen.
	    break;

	case WM_CLOSE:
	    ::SendMessage(hClient, WM_MDIDESTROY, (WPARAM) hwnd, 0);
	    // Ouch!  How do we handle this?  We have to notify Tcl that
	    // this object is going away and let Tcl destroy it instead.
	    // This is wrong for now, as Tcl doesn't know we went away, but
	    // a start at least.
	    ::DestroyWindow(hwnd);
	    return 0;

	case WM_DESTROY:
	    delete pStatGUI->pView, pStatGUI->pCmd;
	    return 0;

	case WM_ERASEBKGND:
	    // Avoid flicker.
	    return 1;

	case WM_CTLCOLOREDIT:
	{
	    HDC hdc = reinterpret_cast <HDC> (wParam);
	    ::SetTextColor (hdc, RGB(192, 192, 192) /*light gray*/);
	    ::SetBkColor (hdc, RGB(0, 0, 0) /*Black*/);
	    return reinterpret_cast <LRESULT> (pStatGUI->hBrush);
	}

	case WM_MDIACTIVATE:
	    if ((HWND) lParam == hwnd) {
		//::SendMessage(hClient, WM_MDISETMENU, (WPARAM) hMenuStat,
		//    (LPARAM) hMenuStatWindow);
	    }
	    //::DrawMenuBar(hFrame);
	    return 0;
    }

    return ::DefMDIChildProc(hwnd, iMsg, wParam, lParam);
}

