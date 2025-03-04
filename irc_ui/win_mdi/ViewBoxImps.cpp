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
 * RCS: @(#) $Id: ViewBoxImps.cpp,v 1.1 2003/11/14 10:36:21 davygrvy Exp $
 ------------------------------------------------------------------------------
 */
#include "ViewBoxImps.hpp"
#include "CTCP2Parse.hpp"

// must have 3.0!
#define _RICHEDIT_VER	0x0300
#include <richedit.h>

#pragma warning (disable:4786)  // I don't care to know about name truncation

extern HINSTANCE hInstance;


// CTCP/2 color attribute parser with component specific actions (this RichEdit control)
//
class RichEditCTCP2Action : protected CTCP2Parse
{
public:
    RichEditCTCP2Action(HWND reCtrl, std::u16string *line)
	: CTCP2Parse(), hRE(reCtrl)
    {
	GETTEXTLENGTHEX gtlen;
	LRESULT length;

	::ZeroMemory(&cf2Def, sizeof(CHARFORMAT2));
	cf2Def.cbSize = sizeof(CHARFORMAT2);

	// Always send the richedit unicode strings.  We do the conversion
	// up to unicode ourselves so we can get out of the 255 character
	// limit and up into the 65k glyph range :)
	stx.codepage = 1200;		// 1200 = unicode, UTF-16.
	stx.flags = ST_SELECTION;

	// get the defaults for the control.
	::SendMessage(hRE, EM_GETCHARFORMAT, 0, reinterpret_cast<LPARAM>(&cf2Def));

	// Now set those defaults into the parser.
	CTCP2Parse::SetDefaults (
		// extract the red from the Win32 specific COLORREF structure
		GetRValue(cf2Def.crTextColor),
		// green
		GetGValue(cf2Def.crTextColor),
		// blue
		GetBValue(cf2Def.crTextColor),
		// red
		GetRValue(cf2Def.crBackColor),
		// green
		GetGValue(cf2Def.crBackColor),
		// blue
		GetBValue(cf2Def.crBackColor),
		(cf2Def.bPitchAndFamily & FIXED_PITCH ? Fixed : Proportional)
		);

	// Assert those defaults.
	//
	::SendMessage(hRE, EM_SETSEL, -1, -1);
	::SendMessage(hRE, EM_SETCHARFORMAT, SCF_SELECTION,
		reinterpret_cast<LPARAM>(&cf2Def));

	gtlen.flags = GTL_CLOSE;
	gtlen.codepage = 1200; //unicode, UTF-16.
	length = ::SendMessage(hRE, EM_GETTEXTLENGTHEX,
		reinterpret_cast<WPARAM>(&gtlen), 0L);

	// pre-pend a linefeed if the control is not empty.
	//
	if (length) {
	    ::SendMessage(
		    hRE, EM_SETTEXTEX,
		    reinterpret_cast<WPARAM>(&stx),
		    reinterpret_cast<LPARAM>(U"\n")
		    );
	}

	::CopyMemory(&cf2, &cf2Def, sizeof(CHARFORMAT2));

	// Crank it through.
	//
	CTCP2Parse::ParseIt(line);
    }

    ~RichEditCTCP2Action ()
    {
	// return to where we started.
	//
	::SendMessage(hRE, EM_SETSEL, -1, -1);
	::SendMessage(hRE, EM_SETCHARFORMAT, SCF_SELECTION,
		reinterpret_cast<LPARAM>(&cf2Def));
	//::HideCaret(hRE);
    }

private:
    virtual void PushStart ()
    {
	// A segment's attributes (if any) are about to be pushed.
	// Do what we need to prepare our component.

	changed = false;
    }

    virtual void PushBold (bool doit)
    {
	changed = true;
	cf2.dwMask |= CFM_BOLD;
	if (doit) cf2.dwEffects |= CFE_BOLD;	// or it into
	else cf2.dwEffects &= ~(CFE_BOLD);	// suck it out of
    }

    virtual void PushReverse (bool doit)
    {
	// TODO: not supported yet...
	PushBold(doit);
    }

    virtual void PushUnderline (bool doit)
    {
	changed = true;
	cf2.dwMask |= CFM_UNDERLINE;
	if (doit) cf2.dwEffects |= CFE_UNDERLINE;
	else cf2.dwEffects &= ~(CFE_UNDERLINE);
    }

    virtual void PushOverstrike (bool doit)
    {
	changed = true;
	cf2.dwMask |= CFM_STRIKEOUT;
	if (doit) cf2.dwEffects |= CFE_STRIKEOUT;
	else cf2.dwEffects &= ~(CFE_STRIKEOUT);
    }

    virtual void PushItalic (bool doit)
    {
	changed = true;
	cf2.dwMask |= CFM_ITALIC;
	if (doit) cf2.dwEffects |= CFE_ITALIC;
	else cf2.dwEffects &= ~(CFE_ITALIC);
    }

    virtual void PushBlink (bool doit)
    {
	// Blink is ugly... remap it to bold.
	PushBold(doit);
    }

    virtual void PushURL (bool doit)
    {
	changed = true;
	cf2.dwMask |= CFM_LINK;
	if (doit) cf2.dwEffects |= CFE_LINK;
	else cf2.dwEffects &= ~(CFE_LINK);
    }

    virtual void PushSpacing (CharSpacingType sp)
    {
	// TODO: not supported at this time...
    }

    virtual void PushFontSize (int size)
    {
	// TODO: not supported at this time...
    }

    virtual void PushForegroundColor (byte R, byte G, byte B)
    {
	changed = true;
	cf2.dwMask |= CFM_COLOR;
	cf2.crTextColor = RGB(R, G, B);		// convert to a Win32 COLORREF
    }

    virtual void PushBackgroundColor (byte R, byte G, byte B)
    {
	changed = true;
	cf2.dwMask |= CFM_BACKCOLOR;
	cf2.crBackColor = RGB(R, G, B);		// convert to a Win32 COLORREF
    }

    virtual void PushTextSegment (std::u16string *segment)
    {
	DWORD start;

	// move to the end.
	::SendMessage(hRE, EM_SETSEL, -1, -1);

	// Where is the end?
	::SendMessage(hRE, EM_GETSEL, 0L,
		reinterpret_cast<LPARAM>(&start));

	::SendMessage(
		hRE, EM_SETTEXTEX,
		reinterpret_cast<WPARAM>(&stx),

		// WCHAR data type is native UTF-16, codepage 1200
		reinterpret_cast<LPARAM>((WCHAR *)segment->c_str())
		);

	//if (changed) {
	    ::SendMessage(hRE, EM_SETSEL, start, -1);
	    ::SendMessage(hRE, EM_SETCHARFORMAT, SCF_SELECTION,
		    reinterpret_cast<LPARAM>(&cf2));
	//}
    }

    virtual int ParseExtension (std::u16string *string, int *offset, int *endoffset)
    {
	int index, start;
	char16_t ch;

	if (*offset < *endoffset) {
	    switch (string->at(*offset)) {
		// Tags (dynamic color settings)
		case 'T':
		    start = (*offset)++;
		    ch = string->at(*offset);
		    if (string->at(*offset) >= U'0' && string->at(*offset) <= U'9') {
			if (string->at((*offset)+1) >= U'0' && string->at((*offset)+1) <= U'9') {
			    // two digit
			    index = (((string->at(*offset) - U'0') * 10) + (string->at((*offset)+1) - U'0'));
			    *offset += 2;
			} else {
			    // one digit.
			    index = string->at(*offset) - U'0';
			    (*offset)++;
			}
			if (index > 16) {
			    // malformed.
			    *offset = start;
			    return 1;
			}

			// These colors are hard coded for now.
			switch (index) {
			case 0:	    // default.
			case 1:	    // public.
			case 2:	    // private.
			    fg = fgDef;
			    bg = bgDef;
			    break;
			case 3:	    // action.
			    fg = 0xFF7F00;
			    bg = bgDef;
			    break;
			case 4:	    // notice.
			    fg = 0x7F3F00;
			    bg = bgDef;
			    break;
			case 5:	    // ctcp.
			    fg = Red;
			    bg = bgDef;
			    break;
			case 6:	    // change.
			    fg = 0xA000A0;
			    bg = bgDef;
			    break;
			case 7:	    // join.
			case 8:	    // part.
			    fg = Cyan;
			    bg = bgDef;
			    break;
			case 9:	    // quit.
			case 10:    // kick.
			    fg = Green;
			    bg = bgDef;
			    break;
			case 11:    // highlight.
			    fg = Lt_Magenta;
			    bg = bgDef;
			    break;
			case 12:    // error.
			    fg = Lt_Yellow;
			    bg = Lt_Red;
			    break;
			case 13:    // nick.
			    fg = Lt_Cyan;
			    bg = bgDef;
			    break;
			case 14:    // channel.
			    fg = Lt_Green;
			    bg = bgDef;
			    break;
			case 15:    // mode.
			    fg = Lt_Yellow;
			    bg = bgDef;
			    break;
			case 16:    // socket.
			    fg = White;
			    bg = Grey;
			    break;
			}
		    } else {
			// malformed.
			*offset = start;
			return 1;
		    }
		    break;
		default:
		    // error.
		    return 1;
	    }
	    return 0;
	} else {
	    // malformed.
	    return 1;
	}
    }

    virtual void ClearExtensions ()
    {
    }

    HWND hRE;
    CHARFORMAT2 cf2, cf2Def;
    SETTEXTEX stx;
    bool changed;
};

AllWin32ViewBoxImp::AllWin32ViewBoxImp
    (HWND parent, CMclQueue<std::u16string *> &echoQ)
    : m_echoQ(echoQ)
{
    CHARFORMAT2 defFmt;
    PARAFORMAT2 paraFmt;

    reDll = ::LoadLibrary(TEXT("riched20.dll"));

    hwndViewBox = 
	::CreateWindowEx (
	    WS_EX_CLIENTEDGE |
	    ES_EX_NOCALLOLEINIT |
	    0,		// zero gets or'd with above
	    RICHEDIT_CLASS, 0L,
	    ES_AUTOVSCROLL |
	    ES_DISABLENOSCROLL |
	    ES_MULTILINE |
	    ES_NOHIDESEL |
	    ES_SAVESEL |
//	    ES_SELECTIONBAR |
	    WS_CHILD |
	    WS_CLIPCHILDREN |
	    WS_VISIBLE |
	    WS_VSCROLL |
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
	    CFM_PROTECTED |
	    CFM_WEIGHT |
	    0;		// zero gets or'd with above

    defFmt.dwEffects =
	    CFE_PROTECTED |
	    0;		// zero gets or'd with above

    // make the background black.
    ::SendMessage(hwndViewBox, EM_SETBKGNDCOLOR, 0, RGB(0, 0, 0));

    // Set control for read-only
    ::SendMessage(hwndViewBox, EM_SETOPTIONS, ECOOP_OR, ECO_READONLY);

    // turn on advanced typo stuff..  which does exactly what???
    ::SendMessage(hwndViewBox, EM_SETTYPOGRAPHYOPTIONS, TO_ADVANCEDTYPOGRAPHY, TO_ADVANCEDTYPOGRAPHY);

    // Make sure 'extended bkg colors' is cleared.
    ::SendMessage(hwndViewBox, EM_SETEDITSTYLE, 0, SES_EXTENDBACKCOLOR);

    // make sure auto URL detection is on; let's try it.
    // make sure auto URL detection is off; I don't like it.
    ::SendMessage(hwndViewBox, EM_AUTOURLDETECT, FALSE, 0);

    // we want courier new and ~12 point.
    _tcscpy_s(defFmt.szFaceName, 32, /*"code2001"*/ TEXT("Courier New"));

    //defFmt.yHeight = -MulDiv(18, GetDeviceCaps(hDC, LOGPIXELSY), 72);
    //defFmt.yHeight = 512;
    defFmt.wWeight = FW_EXTRALIGHT;

    // light grey text color
    defFmt.crTextColor = RGB(192, 192, 192);

    // re-iterate a black background.
    defFmt.crBackColor = RGB(0, 0, 0);

    // standard underline style.
    defFmt.bUnderlineType = CFU_UNDERLINE;

    // set the default CHARFORMAT2 info for the control.
    ::SendMessage(hwndViewBox, EM_SETCHARFORMAT, 0,
	    reinterpret_cast<LPARAM>(&defFmt));

    // Turn on hanging indent mode.
    //
    ::ZeroMemory(&paraFmt, sizeof(PARAFORMAT2));
    paraFmt.cbSize = sizeof(PARAFORMAT2);
    paraFmt.dwMask = PFM_OFFSET;
    paraFmt.dxOffset = 600;
    ::SendMessage(hwndViewBox, EM_SETPARAFORMAT, 0L,
	    reinterpret_cast<LPARAM>(&paraFmt));

    // Wrap at word boundaries.
    //
    ::SendMessage(hwndViewBox, EM_SETWORDWRAPMODE,
	    WBF_WORDBREAK|WBF_WORDWRAP, 0L);

    // Set language options.
    //
    ::SendMessage(hwndViewBox, EM_SETLANGOPTIONS, 0L,
	    IMF_AUTOFONT|IMF_AUTOFONTSIZEADJUST|IMF_DUALFONT);

    echoQReaderThread = new CMclThread(this);
}

AllWin32ViewBoxImp::~AllWin32ViewBoxImp() {
    ceStop.Set();
    echoQReaderThread->Wait(INFINITE);
    ::FreeLibrary(reDll);
}

HWND AllWin32ViewBoxImp::GetHwnd() { return hwndViewBox; };

// this function is run in a seperate thread from the rest of this class
//
unsigned AllWin32ViewBoxImp::ThreadHandlerProc(void) {
    std::u16string *line;

    // wait forever for an echo to come down the Queue
    // and punch it, ...repeat :)
    //
    while (m_echoQ.Get(line, INFINITE, &ceStop))
    {
	RichEditCTCP2Action(hwndViewBox, line);
	delete line;
    }
    return 0;
}
