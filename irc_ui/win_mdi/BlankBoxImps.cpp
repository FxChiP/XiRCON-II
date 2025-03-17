/*
 ------------------------------------------------------------------------------
 * BlankBoxImps.cpp --
 *
 *   Platform specific implimentation class of the component.
 *
 * Copyright (c) 1999-2003 Tomasoft Engineering.
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: BlankBoxImps.cpp,v 1.4 2003/11/23 21:46:55 davygrvy Exp $
 ------------------------------------------------------------------------------
 */
#include "BlankBoxImps.hpp"
#include "resources/resource.h"

extern HINSTANCE hInstance;
extern HWND hClient;
extern HWND hFrame;

// use message crackers
#include <windowsx.h>

LRESULT CALLBACK BlankBoxWndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);


AllWin32BlankBoxImp::AllWin32BlankBoxImp ()
    : hContainee(NULL), UserChangingSize(0), ExitSizeMove(0)
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
	wndcls.style = MDIS_ALLCHILDSTYLES;
	wndcls.cbClsExtra = 0;
	wndcls.cbWndExtra = sizeof (HANDLE);
	wndcls.hInstance = hInstance;
	wndcls.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wndcls.lpszMenuName = NULL;
	wndcls.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1);
	wndcls.hIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TK));
	wndcls.hIconSm = wndcls.hIcon;
	wndcls.lpfnWndProc = BlankBoxWndProc;
	wndcls.lpszClassName = TEXT("BlankBox");

	::RegisterClassEx (&wndcls);
    }

    // create a new statbox window in its own thread w/ message pump
    //
//    pGUI = new AllWin32BlankBoxThreadedGUI();

    hwndBlankBox = ::CreateMDIWindow (
	    TEXT("BlankBox"),
	    TEXT("This is a Blank window under test"),
	    0,
	    CW_USEDEFAULT, CW_USEDEFAULT, 0xC7, 0xC7,
	    hClient, hInstance, reinterpret_cast <LONG_PTR>(this)
	    );
}

AllWin32BlankBoxImp::~AllWin32BlankBoxImp () {
    if (hwndBlankBox) ::SendMessage(hwndBlankBox, WM_CLOSE, 0, 0);
}

void *AllWin32BlankBoxImp::GetWindowId() {
    return hwndBlankBox;
}

bool AllWin32BlankBoxImp::first = true;


LRESULT CALLBACK
BlankBoxWndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    LPMDICREATESTRUCT lpCreateParams;
	int cx, cy;

    AllWin32BlankBoxImp *pBlank = reinterpret_cast <AllWin32BlankBoxImp*>
	    (::GetWindowLongPtr(hwnd, GWLP_USERDATA));

    switch (iMsg)
    {
	case WM_CREATE:
	    // move the AllWin32StatBoxThreadedGUI class pointer into the
	    // userdata of the window.
	    //
	    lpCreateParams = (LPMDICREATESTRUCT)
		    ((LPCREATESTRUCT)lParam)->lpCreateParams;

	    ::SetWindowLongPtr (hwnd, GWLP_USERDATA,
		    static_cast <LONG_PTR>(lpCreateParams->lParam));

	    // Supposedly, lpCreateParams has a DWORD alignment problem.
	    // I don't know what this means for pointers.  I doubt much.
	    //
	    pBlank = reinterpret_cast <AllWin32BlankBoxImp *>
		    (lpCreateParams->lParam);
	    return 0;

	case WM_QUERYENDSESSION:
	case WM_CLOSE:
	    ::DestroyWindow(hwnd);
	    break;

	case WM_DESTROY:
	    ::SendMessage(hClient, WM_MDIDESTROY, (WPARAM) hwnd, 0);
	    pBlank->hwndBlankBox = NULL;
	    delete pBlank;
	    break;

// ---------  Begin Tk container specials ----------

#define TK_CLAIMFOCUS     (WM_USER)
#define TK_GEOMETRYREQ    (WM_USER+1)
#define TK_ATTACHWINDOW   (WM_USER+2)
#define TK_DETACHWINDOW   (WM_USER+3)

	case TK_CLAIMFOCUS:
	    if (wParam || (GetFocus() != NULL)) {
		::SetFocus(pBlank->hContainee);
	    }
	    return 0;

	case TK_GEOMETRYREQ:
	    if (!pBlank->UserChangingSize) {
		/*
		 * Make the client area the size given, but we
		 * need to move the whole window.  Calc the
		 * additions to get us there.
		 */
		cx = (int)wParam + (2 * GetSystemMetrics(SM_CXEDGE) + 3);
		cy = (int)lParam + (GetSystemMetrics(SM_CYCAPTION) +
			(2 * GetSystemMetrics(SM_CYEDGE) + 3));
		::SetWindowPos(hwnd, NULL, 0, 0, cx, cy,
			SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
	    }
	    return 0;

	case TK_ATTACHWINDOW:
	    pBlank->hContainee = (HWND)wParam;
	    ::SetParent(pBlank->hContainee, hwnd);
	    return 0;

	case TK_DETACHWINDOW:
	    pBlank->hContainee = NULL;
	    ::PostMessage(hwnd, WM_CLOSE, 0, 0);
	    return 0;

// ---------  End Tk container specials ----------

	case WM_ENTERSIZEMOVE:
	    pBlank->UserChangingSize = 1;
	    break;

	case WM_EXITSIZEMOVE:
	    pBlank->UserChangingSize = 0;
	    break;

	case WM_SIZE:
	    if (wParam != SIZE_MINIMIZED && pBlank->hContainee) {
		::SetWindowPos(pBlank->hContainee, NULL, 0, 0,
			LOWORD(lParam), HIWORD(lParam),
			SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
	    }
	    // must fall to DefMDIChildProc(), or bad things happen.
	    //
	    break;

	case WM_PARENTNOTIFY:
	    switch (LOWORD(wParam)) {
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
		    ::SetFocus(pBlank->hContainee);
		    return 0;

		case WM_DESTROY:
		    ::PostMessage(pBlank->hwndBlankBox, WM_CLOSE, 0, 0);
		    return 0;
	    }
	    break;

	case WM_ERASEBKGND:
	    // Avoid flicker.
	    if (pBlank->UserChangingSize) return 1;
	    break;
    }

    return ::DefMDIChildProc(hwnd, iMsg, wParam, lParam);
}
