#include "FrameWndProc.hpp"
#include <commctrl.h>
#include "resources/resource.h"
#include <afxres.h>

#define HELPFILE TEXT("xircon-ii.hlp")

// use message crackers!
#include <windowsx.h>

/* globals */
extern HINSTANCE hInstance;
extern HWND hClient;
extern HWND hFrame;
extern HWND hStatBar;
extern HWND hToolBar;
int StatBarVisible = 1;
int ToolBarVisible = 1;

// local protos
static BOOL OnCreate(HWND, LPCREATESTRUCT);
static BOOL OnDestroy(HWND);
static INT_PTR CALLBACK ConnectDlgProc (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
HBITMAP SetToolBarBmpColor(HBITMAP hBitmap);


LRESULT CALLBACK
FrameWndProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_SIZE:
	{
	    RECT rct;
	    int tbH=0, sbH=0;

	    if (wParam == SIZE_MINIMIZED) break;
	    ::MoveWindow(hStatBar, 0, 0, 0, 0, TRUE);

	    if (ToolBarVisible) {
		::SetWindowPos(hToolBar, NULL, 0, 0,
			LOWORD(lParam), HIWORD(lParam),
			SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
		::GetWindowRect(hToolBar, &rct);
		tbH = rct.bottom - rct.top;
	    }
	    if (StatBarVisible) {
		::GetWindowRect(hStatBar, &rct);
		sbH = rct.bottom - rct.top;
	    }

	    // get the actual rather than using cy.
	    ::GetClientRect(hFrame, &rct);
	    sbH = rct.bottom - tbH - sbH;

	    ::MoveWindow(hClient, 0, tbH, rct.right, sbH, TRUE);
	    return 1;
	}

    case WM_COMMAND:
	{
	    HWND w;

	    switch (LOWORD(wParam))
	    {
	    case ID_APP_EXIT:
		::PostMessage (hwnd, WM_SYSCOMMAND, SC_CLOSE, 0L);
		return FALSE;

#ifdef _MSC_VER
    // Yes, I really do understand operator precedence.
#   pragma warning (disable:4554)
#endif

	    case ID_VIEW_TOOLBAR:
		ToolBarVisible ^= 1;
		if ((::IsWindowVisible(hToolBar) == 0) ^ (ToolBarVisible == 0)) {
		    ::ShowWindow(hToolBar, (ToolBarVisible ? SW_SHOW : SW_HIDE));
		    ::CheckMenuItem(
			    ::GetSubMenu(::GetMenu(hwnd), 2),
			    ID_VIEW_TOOLBAR,
			    (ToolBarVisible ? MF_CHECKED : MF_UNCHECKED)
			    );
		    ::PostMessage(hwnd, WM_SIZE, 0, 0);
		}
		return FALSE;

	    case ID_VIEW_STATUS_BAR:
		StatBarVisible ^= 1;
		if ((::IsWindowVisible(hStatBar) == 0) ^ (StatBarVisible == 0)) {
		    ::ShowWindow(hStatBar, (StatBarVisible ? SW_SHOW : SW_HIDE));
		    ::CheckMenuItem(
			    ::GetSubMenu(::GetMenu(hwnd), 2),
			    ID_VIEW_STATUS_BAR,
			    (StatBarVisible ? MF_CHECKED : MF_UNCHECKED)
			    );
		    ::PostMessage(hwnd, WM_SIZE, 0, 0);
		}
		return FALSE;

	    case ID_SERV_CONNECT:
		DialogBoxParam(hInstance, MAKEINTRESOURCE(IDR_CONNECT), hwnd, ConnectDlgProc, NULL);
		return FALSE;

#ifdef _MSC_VER
#   pragma warning (default:4554)
#endif

	    case ID_WINDOW_TILE_HORZ:
		::SendMessage (hClient, WM_MDITILE, MDITILE_HORIZONTAL, 0);
		return FALSE;
	    case ID_WINDOW_TILE_VERT:
		::SendMessage (hClient, WM_MDITILE, MDITILE_VERTICAL, 0);
		return FALSE;
	    case ID_WINDOW_CASCADE:
		::SendMessage (hClient, WM_MDICASCADE, 0, 0);
		return FALSE;
	    case ID_WINDOW_ARRANGE:
		::SendMessage (hClient, WM_MDIICONARRANGE, 0, 0);
		return FALSE;
	    case ID_WINDOW_NEXT:
		::SendMessage (hClient, WM_MDINEXT, NULL, 0);
		return FALSE;
	    case ID_WINDOW_CLOSEALL:
		while (w = (HWND) ::SendMessage(hClient, WM_MDIGETACTIVE, 0, 0))
		    ::SendMessage(w, WM_CLOSE, 0, 0);
		return FALSE;
	    case ID_WINDOW_SPLIT:
	    case ID_WINDOW_SHOWCONSOLE:
	    case ID_APP_ABOUT:
		// dunno yet...
		return FALSE;

	    case ID_HELP_FINDER:
		::WinHelp(hwnd, HELPFILE, HELP_FINDER, 0);
		return FALSE;
	    case ID_HELP_SEARCH:
		::WinHelp(hwnd, HELPFILE, HELP_KEY, 0L);
		return FALSE;
	    default:
		w = (HWND) ::SendMessage(hClient, WM_MDIGETACTIVE, 0, 0);
		if (IsWindow(w)) {
		    ::SendMessage(w, WM_COMMAND, wParam, lParam);
		}
	    }
	}
	break;

	HANDLE_MSG(hwnd, WM_CREATE,  OnCreate);
	HANDLE_MSG(hwnd, WM_DESTROY, OnDestroy);
    }
    return ::DefFrameProc(hwnd, hClient, uMsg, wParam, lParam);
}


BOOL
OnCreate (HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
    CLIENTCREATESTRUCT ccs;
    HMENU hMenu;
    HBITMAP hTbBmp;
    TBBUTTON tbb[16];
    DWORD index = 0;

    hStatBar = ::CreateStatusWindow(
	    SBARS_SIZEGRIP|WS_CHILD|WS_VISIBLE,
	    TEXT("Big Moma!"),
	    hwnd,
	    AFX_IDW_STATUS_BAR
	    );

    ::MoveWindow(hStatBar, 0, 0, 0, 0, TRUE);

    hTbBmp = ::LoadBitmap(hInstance, MAKEINTRESOURCE(IDR_TOOLBAR1));
    hTbBmp = SetToolBarBmpColor(hTbBmp);

#define TBbutton(bmpID, WM_cmd, iStr) \
    tbb[index].fsState = TBSTATE_ENABLED; \
    tbb[index].dwData = 0; \
    tbb[index].iString = iStr; \
    tbb[index].iBitmap = bmpID; \
    tbb[index].idCommand = WM_cmd; \
    tbb[index++].fsStyle = TBSTYLE_BUTTON | BTNS_SHOWTEXT;

#define TBblank \
    tbb[index].fsState = TBSTATE_ENABLED; \
    tbb[index].dwData = 0; \
    tbb[index].iString = 0; \
    tbb[index].iBitmap = 0; \
    tbb[index].idCommand = 0; \
    tbb[index++].fsStyle = TBSTYLE_SEP;

    TBbutton(0,  ID_SERV_CONNECT, IDS_CONNECT)
    TBbutton(1,  ID_BUTTON40003, IDS_DISCONN)
    TBblank
    TBbutton(2,  ID_BUTTON40004, 0)
    TBbutton(3,  ID_BUTTON40005, 0)
    TBbutton(4,  ID_BUTTON40006, 0)
    TBblank
    TBbutton(5,  ID_BUTTON40007, 0)
    TBbutton(6,  ID_BUTTON40008, 0)
    TBblank
    TBbutton(7,  ID_TOOLS_PREFS, 0)
    TBbutton(8,  ID_TOOLS_USERS, 0)
    TBblank
    TBbutton(9,  ID_WINDOW_CASCADE, 0)
    TBbutton(10, ID_WINDOW_TILE_VERT, 0)
    TBbutton(11, ID_WINDOW_TILE_HORZ, 0)

#undef TBbutton
#undef TBblank

    hToolBar = ::CreateToolbarEx(
	    hwnd,
	    WS_CHILD | WS_VISIBLE | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS,
	    0,	    // identifier wanted?
	    12,	    // how many button images in the bitmap?
	    0L,
	    (UINT_PTR) hTbBmp,
	    tbb,   // TBBUTTON array
	    16,	    // how many TBBUTTONs in that array?
	    16,
	    16,
	    16,
	    16,
	    sizeof(TBBUTTON));

    ::SendMessage(hToolBar, TB_SETEXTENDEDSTYLE, 0, (LPARAM)(DWORD)TBSTYLE_EX_MIXEDBUTTONS);

    hMenu = ::LoadMenu(
	    lpCreateStruct->hInstance,
	    MAKEINTRESOURCE(IDR_MAINMENU)
	    );

    //::MoveWindow(hToolBar, 0, 0, 0, 0, TRUE);

    ::SetMenu(hwnd, hMenu);
    ccs.hWindowMenu = ::GetSubMenu(hMenu, 3);	// get the handle to the window menu
    ccs.idFirstChild = 400;       // for some unknown reason we start at 400

    //SendMessage(API->hStatBar, SB_SIMPLE, 0, 0);

    hClient = ::CreateWindowEx (
	    WS_EX_CLIENTEDGE,	// sunken inside edge
	    TEXT("MDICLIENT"),
	    0L,
	    WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL,
	    0,
	    0,
	    0,
	    0,
	    hwnd,
	    NULL,
	    lpCreateStruct->hInstance,
	    &ccs
	    );

    ::ShowWindow(hClient, SW_SHOW);

    // Indicate success.  the SDK docs differ on this.
    // The docs say to return FALSE.  That was wrong!
    return TRUE;
}


BOOL
OnDestroy (HWND hwnd)
{
    ::PostQuitMessage(0);
    return FALSE;
}

INT_PTR CALLBACK
ConnectDlgProc (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    BOOL ret = FALSE;
    TCHAR dlgTitle[25];

    switch (uMsg) {
	case WM_INITDIALOG:
	    LoadString(hInstance, IDS_CONNECT, dlgTitle, 25);
	    SendMessage(hDlg, WM_SETTEXT, 0, (WPARAM)&dlgTitle);
	    ret = TRUE;
	    break;

	case WM_CLOSE:
	    EndDialog(hDlg, 0);
	    break;

	case WM_COMMAND:
	    switch (LOWORD(wParam)) {
		case IDCANCEL:
		    EndDialog(hDlg,0);
	    }
	    break;
    }

    return ret;
}

HBITMAP
SetToolBarBmpColor(HBITMAP hBitmap)
{
    HDC mDC;
    HBRUSH hBrush, hOldBrush;
    HBITMAP hOldBmp, hReturn;

    mDC = CreateCompatibleDC(NULL);
    hOldBmp = (HBITMAP) SelectObject(mDC, hBitmap);
    hBrush = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
    hOldBrush = (HBRUSH) SelectObject(mDC, hBrush);
    ExtFloodFill(mDC, 1, 1, GetPixel(mDC, 1, 1), FLOODFILLSURFACE);
    SelectObject(mDC, hOldBrush);
    DeleteObject(hBrush);

    hReturn = (HBITMAP) SelectObject(mDC, hBitmap);
    DeleteDC(mDC);

    return hReturn;
}
