/*
 ------------------------------------------------------------------------------
 * CmdBoxImps.cpp --
 *
 *   Platform specific implimentation class of the component.
 *
 * Copyright (c) 1999-2000 Tomahawk Software Group
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: CmdBoxImps.cpp,v 1.1 2003/11/14 10:57:46 davygrvy Exp $
 ------------------------------------------------------------------------------
 */
#include "CmdBoxImps.hpp"
#include <string>

// must have 3.0!
#define _RICHEDIT_VER	0x0300
#include <richedit.h>

extern HINSTANCE hInstance;


AllWin32CmdBoxImp::AllWin32CmdBoxImp(HWND parent)
  : bHistSaveTemp(false)
{
    CHARFORMAT2 defFmt;

    ::LoadLibrary("riched20.dll");

    hwndCmdBox =
	::CreateWindowEx (
	    WS_EX_CLIENTEDGE |
	    ES_EX_NOCALLOLEINIT |
	    0,		// zero gets or'd with above
	    RICHEDIT_CLASS, 0L,
//	    ES_AUTOVSCROLL |
//	    ES_DISABLENOSCROLL |
//	    ES_MULTILINE |
	    ES_WANTRETURN |
	    ES_NOHIDESEL |
	    ES_SAVESEL |
//	    ES_SELECTIONBAR |
	    WS_CHILD |
	    WS_CLIPCHILDREN |
	    WS_VISIBLE |
//	    WS_VSCROLL |
	    0,		// zero gets or'd with above
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
	    parent, 0L, hInstance, this
	);

    ::ZeroMemory(&defFmt, sizeof(CHARFORMAT2));
    defFmt.cbSize = sizeof(CHARFORMAT2);
    defFmt.dwMask = 
	    CFM_BACKCOLOR |
	    CFM_FACE |
	    CFM_UNDERLINETYPE |
	    CFM_UNDERLINE |
	    CFM_BOLD |
	    CFM_COLOR |
//	    CFM_SIZE |
//	    CFM_PROTECTED |
	    0;		// zero gets or'd with above

    defFmt.dwEffects =
//	    CFE_PROTECTED |
	    0;		// zero gets or'd with above

    // make the background black.
    ::SendMessage(hwndCmdBox, EM_SETBKGNDCOLOR, 0, RGB(0, 0, 0));

    // Set global styles of the control
    ::SendMessage(hwndCmdBox, EM_SETEDITSTYLE, SES_EXTENDBACKCOLOR | SES_EMULATESYSEDIT,
	    SES_EXTENDBACKCOLOR | SES_EMULATESYSEDIT);

    // turn on advanced typo stuff..  which does exactly what???
    ::SendMessage(hwndCmdBox, EM_SETTYPOGRAPHYOPTIONS, TO_ADVANCEDTYPOGRAPHY, TO_ADVANCEDTYPOGRAPHY);

    // clear extended bkg colors.
    ::SendMessage(hwndCmdBox, EM_SETEDITSTYLE, SES_EXTENDBACKCOLOR, 0);

    // make sure auto URL detection is off.
    ::SendMessage(hwndCmdBox, EM_AUTOURLDETECT, FALSE, 0);

    // we want courier new and ~12 point.
    strcpy(defFmt.szFaceName, "Courier New");
    //defFmt.yHeight = 220;

    // light grey text color
    defFmt.crTextColor = RGB(192, 192, 192);

    // re-iterate a black background.
    defFmt.crBackColor = RGB(0, 0, 0);

    // standard underline style.
    defFmt.bUnderlineType = CFU_UNDERLINE;

    // set the default CHARFORMAT2 info for the control.
    ::SendMessage(hwndCmdBox, EM_SETCHARFORMAT, 0, reinterpret_cast<LPARAM>(&defFmt));


    // Super-class it.
    ::SetWindowLongPtr(hwndCmdBox, GWLP_USERDATA,
	    reinterpret_cast<LONG_PTR>(this));

    fnBaseEdit = 
	reinterpret_cast<WNDPROC>(
	    ::SetWindowLongPtr(
		hwndCmdBox, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(CmdBoxWndProc)
	    )
	);

    InitMetrics();
};

AllWin32CmdBoxImp::~AllWin32CmdBoxImp() { };

HWND AllWin32CmdBoxImp::GetHwnd() { return hwndCmdBox; };
int AllWin32CmdBoxImp::Getcy() { return cy; };

bool AllWin32CmdBoxImp::first = true;
WNDPROC AllWin32CmdBoxImp::fnBaseEdit;

void AllWin32CmdBoxImp::InitMetrics () {
    TEXTMETRIC tm;
    HDC hdc = ::GetDC(hwndCmdBox);

    ::GetTextMetrics(hdc, &tm);
    cy = tm.tmHeight + 2 * ::GetSystemMetrics(SM_CYFRAME);
    ::ReleaseDC(hwndCmdBox, hdc);
}

LRESULT CALLBACK
CmdBoxWndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    AllWin32CmdBoxImp *pCmdBoxImp = 
	reinterpret_cast<AllWin32CmdBoxImp *>(
	    GetWindowLongPtr(hwnd, GWLP_USERDATA)
	);

#define CONTROL(c)      (c - 'a' + 1)
#define ATTR_REV        "\026"
#define ATTR_UNDR       "\037"
#define ATTR_MIRC       "\003"

    switch (iMsg) {
	case WM_CHAR:
	{
	    const char *szChar = 0L;
	    char buff[2] = {0, 0};
	    pCmdBoxImp->bHistSaveTemp = true;

	    switch (wParam) {
		case CONTROL('a'):	// ctcp
		case CONTROL('b'):	// ircii bold
		case CONTROL('c'):	// unix style, mirc color (ctrl+k)
		case CONTROL('f'):	// ctcp/2 format character
		case CONTROL('o'):	// ircii normal
		case 31:		// unix style, ctrl+_
		    buff[0] = (char) wParam;
		    szChar = buff;
		    break;
		case CONTROL('r'):	// ircii reverse
		    szChar = ATTR_REV;
		    break;
		case CONTROL('u'):	// ircii underline
		    szChar = ATTR_UNDR;
		    break;
		case CONTROL('k'):	// mirc color
		    szChar = ATTR_MIRC;
		    break;
		case VK_RETURN:
		{
		    WCHAR *szInput = new WCHAR [510];
		    std::wstring *s;
		    GETTEXTEX gte;

		    pCmdBoxImp->bHistSaveTemp = false;
		    gte.cb = 510;
		    gte.codepage = 1200; //unicode, no translation.
		    gte.flags = GT_USECRLF;
		    gte.lpDefaultChar = 0L;
		    gte.lpUsedDefChar = 0L;
		    ::SendMessage(hwnd, EM_GETTEXTEX, reinterpret_cast<WPARAM>(&gte),
			reinterpret_cast<LPARAM>(szInput));

		    SetWindowText(hwnd, "");
		    s = new std::wstring(szInput);
		    pCmdBoxImp->AddHist(s);
		    //pCmdBoxImp->Type(buf);
		    // do an alias call here i guess (?)
		    delete [] szInput;
		    return 0;
		}
	    }
	    if (szChar) {
		::SendMessage(hwnd, EM_REPLACESEL, 0,
		    reinterpret_cast<LPARAM>(szChar));
		return 0;    
	    }
	    break;
	}

	case WM_KEYDOWN:
	{
	    if (GetKeyState (VK_CONTROL) < 0) {
		switch (wParam) {
		    case VK_HOME:
			//SendMessage (pCmdBoxImp->hwndEcho, WM_VSCROLL, MAKELONG(SB_TOP, 0), 0);
			return 0;
		    case VK_END:
			//SendMessage (pCmdBoxImp->hwndEcho, WM_VSCROLL, MAKELONG(SB_BOTTOM, 0), 0);
			return 0;
		    case VK_DOWN:
			//SendMessage (pCmdBoxImp->hwndEcho, WM_VSCROLL, MAKELONG(SB_LINEDOWN, 0), 0);
			return 0;
		    case VK_UP:
			//SendMessage (pCmdBoxImp->hwndEcho, WM_VSCROLL, MAKELONG(SB_LINEUP, 0), 0);
			return 0;
		}
	    }
	    switch (wParam) {
		case VK_RETURN:
		    return 0;
		case VK_PRIOR:
		    //SendMessage (pCmdBoxImp->hwndEcho, WM_VSCROLL, MAKELONG(SB_PAGEUP, 0), 0);
		    return 0;
		case VK_NEXT:
		    //SendMessage (pCmdBoxImp->hwndEcho, WM_VSCROLL, MAKELONG(SB_PAGEDOWN, 0) 0);
		    return 0;
		case VK_UP:
		{
		    const std::wstring *hist;

		    // if the text was changed, save the buffer.
		    if (pCmdBoxImp->bHistSaveTemp) {
			int len = GetWindowTextLength(hwnd);
			WCHAR *p = new WCHAR [len + 1];

			pCmdBoxImp->bHistSaveTemp = false;
			GetWindowTextW(hwnd, p, len + 1);
			if (pCmdBoxImp->sHistoryTemp) delete pCmdBoxImp->sHistoryTemp;
			pCmdBoxImp->sHistoryTemp = new std::wstring(p);
			delete [] p;
		    }
		    hist = pCmdBoxImp->GetHist(AllWin32CmdBoxImp::up);
		    if (hist) {
			SETTEXTEX stx;
			stx.flags = ST_DEFAULT;
			stx.codepage = 1200;
			::SendMessage(
			    hwnd, EM_SETTEXTEX,
			    reinterpret_cast<WPARAM>(&stx),
			    reinterpret_cast<LPARAM>(hist->c_str())
			);
		    }
		    return 0;
		}
		case VK_DOWN:
		{
		    const std::wstring *hist;

		    // going down from modified text, i would assume, means you
		    // don't want the buffer anymore.
		    if (pCmdBoxImp->bHistSaveTemp && pCmdBoxImp->sHistoryTemp) {
			delete pCmdBoxImp->sHistoryTemp;
			pCmdBoxImp->sHistoryTemp = NULL;
			pCmdBoxImp->bHistSaveTemp = false;
		    }
		    hist = pCmdBoxImp->GetHist(AllWin32CmdBoxImp::down);
		    if (hist) {
			SETTEXTEX stx;
			stx.flags = ST_DEFAULT;
			stx.codepage = 1200;
			::SendMessage(
			    hwnd, EM_SETTEXTEX,
			    reinterpret_cast<WPARAM>(&stx),
			    reinterpret_cast<LPARAM>(hist->c_str())
			);
		    }
		    return 0;
		}
	    }
	}
    }
    return CallWindowProc(pCmdBoxImp->fnBaseEdit, hwnd, iMsg, wParam, lParam);
}
