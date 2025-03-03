/*
 ------------------------------------------------------------------------------
 * CTCP2Parse.cpp  --
 *
 *	a C++ implementation of the Text attribute section of the CTCP/2
 *	draft by Robey Pointer (that eggdrop dude) and friends.  This code
 *	is based directly from Robey's Java example.  This code includes
 *	full support for decoding mIrc, ANSI, HydraIRC/Besirc and ircII
 *	attributes as well.
 *	
 *	    http://www.lag.net/~robey/ctcp
 *
 *	Depends: STL's string template, no OS specifics.
 *
 *	This source is under BSD license and has no restrictions the way
 *	the GPL does.  Sell it for all I care.  Just be nice and give me
 *	credit for it.  Although it's not a requirement.
 *
 *	Author: David Gravereaux <davygrvy@pobox.com>
 *		Tomasoft Engineering
 *
 *	Original conversion routines donated by
 *	    Keith Lea <keith@cs.oswego.edu> aka kano
 *
 * RCS: @(#) $Id: CTCP2Parse.cpp,v 1.14 2006/09/24 06:38:57 davygrvy Exp $
 ------------------------------------------------------------------------------
 */


#include "CTCP2Parse.hpp"
#include <locale>
#include <codecvt>

// an ISO hack for VisualC++
#ifdef _MSC_VER
#define   swprintf	_snwprintf
#else
int
_wtoi (const wchar_t *str)
{
    return (int)wcstol(str, 0, 10);
}
#endif

CTCP2Parse::CTCP2Parse (ParseMode _mode)
    : mode(_mode), segment(0L)
{
    boldCnt = revCnt = overCnt = underCnt = italicCnt = blinkCnt = urlCnt = 0;
    boldLast = revLast = overLast = underLast = italicLast = blinkLast = urlLast = 0;
    fntSize = fntLast = 0;
    
    if (mode == Render) {
	fg = fgLast = fgDef = Lt_Grey;
	bg = bgLast = bgDef = Black;
    } else {
	fg = fgLast = fgDef = RGBColor::Indeterminate;
	bg = bgLast = bgDef = RGBColor::Indeterminate;
    }
    cp = cpLast = cpDef = Fixed;
}

void
CTCP2Parse::SetDefaults (
    byte fgR, byte fgG, byte fgB,	// default foreground color
    byte bgR, byte bgG, byte bgB,	// default background color
    CharSpacingType cpDef_)
{
    
    // reassemble the seperate RGB values into our internal rep.
    //
    if (mode == Render) {
	fg = fgLast = fgDef = (fgR << 16) + (fgG << 8) + fgB;
	bg = bgLast = bgDef = (bgR << 16) + (bgG << 8) + bgB;
    } else {
	fg = fgLast = fgDef = RGBColor::Indeterminate;
	bg = bgLast = bgDef = RGBColor::Indeterminate;
    }
    
    cp = cpLast = cpDef = cpDef_;
}

void
CTCP2Parse::SetDefaults (DefaultData &data)
{
    SetDefaults(data.fgR, data.fgG, data.fgB, data.bgR, data.bgG, data.bgB, data.cspc);
}


void CTCP2Parse::ParseIt (std::u16string *line)
{
    register size_t parseFrmChar = 0, parseEndChar = 0, parseStart = 0;
    int fntVal, fntDir = 1;
    size_t needReset = 0, colorStart, setFgColor, setBgColor, colorFail;
    RGBColor fgTemp, bgTemp;

    // Pre-parse and replace other attribute standards with
    // the ctcp2 equivalents for display (or pass-thru in gateway mode).
    //
    mirc2ctcp(line);
    ircii2ctcp(line);
    hydra2ctcp(line);
    ansi2ctcp(line);


    // find an opening ctcp/2 format character (^F)
    parseFrmChar = line->find_first_of(u"\006");

    // do we have any?
    while (parseFrmChar != std::string::npos) {
	// find the closing ctcp/2 format character (^F)
	parseEndChar = line->find_first_of(u"\006", parseFrmChar+1);

	if (parseEndChar == std::string::npos) {
	    // an opening without a closing is bogus
	    // so remove it and fallout of the while loop
	    line->replace(parseFrmChar, 1, u"");
	    break;
	}

	// Resetting does not produce a segment, but we must push it.
	//
	if (parseFrmChar > parseStart || needReset) {
	    PushOut();
	}
	
	// Check for text before this format character and after the
	// last format character (or index 0 for the first iteration.
	//
	if (parseFrmChar > parseStart) {
	    // yes.. we got a text segment ready to push.
	    segment = new std::u16string(line->substr(parseStart, (parseFrmChar - parseStart)));
	    PushTextSegment(segment);
	    delete segment;
	}

	// now, interpret this format code and decode this attribute
	// (and options, if any).

	parseFrmChar++;  // consume the first ^F.
	
	switch (line->at(parseFrmChar)) {
	    
#define CTCP2_TOGGLE(ch, var) \
	case (ch): \
	    decodeToggle(line, &parseFrmChar, &parseEndChar, &(var)); \
	    break;
    
	// bold
	CTCP2_TOGGLE('B', boldCnt);
    
	// reverse
	CTCP2_TOGGLE('V', revCnt);

	// underline
	CTCP2_TOGGLE('U', underCnt);

	// overstrike
	CTCP2_TOGGLE('S', overCnt);

	// italics
	CTCP2_TOGGLE('I', italicCnt);

	// blink
	CTCP2_TOGGLE('K', blinkCnt);

	// URL
	CTCP2_TOGGLE('L', urlCnt);

#undef CTCP2_TOGGLE
	    
	// color
	case 'C':
	    colorStart = parseFrmChar++;   // consume the 'C'
	    colorFail = 0;
	    if (parseFrmChar < parseEndChar) {
		setFgColor = 1;
		if (line->at(parseFrmChar) == '.') {
		    // reset to default
		    fg = fgDef;
		    setFgColor = 0;
		    parseFrmChar++;  // consume the '.'
		} else if (line->at(parseFrmChar) == '-') {
		    // ignore
		    setFgColor = 0;
		    parseFrmChar++;   // consume the '-'
		} else {
		    if (!decodeColor(line, &parseFrmChar, &parseEndChar,
			    &fgTemp)) {
			// rollback the error to the 'C'
			setFgColor = 0;
			colorFail = 1;
			parseFrmChar = colorStart - 1;
			break;
		    }
		}

		// background color?
		if (parseFrmChar < parseEndChar) {
		    setBgColor = 1;
		    if (line->at(parseFrmChar) == '.') {
			// return to default
			setBgColor = 0;
			bg = bgDef;
			parseFrmChar++;  // consume the '.'
		    } else if (line->at(parseFrmChar) == '-') {
			// ignore
			setBgColor = 0;
			parseFrmChar++;   // consume the '-'
		    } else {
			if (!decodeColor(line, &parseFrmChar,
				&parseEndChar, &bgTemp)) {
			    // rollback the error to the 'C'
			    parseFrmChar = colorStart - 1;
			    setBgColor = 0;
			    colorFail = 1;
			    break;
			}
		    }
		} else {
		    setBgColor = 0;
		    parseFrmChar--;	// rollback one.
		}

		if (setFgColor && !colorFail) fg = fgTemp;
		if (setBgColor && !colorFail) bg = bgTemp;

	    } else {
		// no colors were specified: reset to normal
		if (mode == Gateway) {
		    fg = RGBColor::Indeterminate;
		    bg = RGBColor::Indeterminate;
		} else {
		    fg = fgDef;
		    bg = bgDef;
		}
	    }
	    
	    // if fg/bg are too similar, reset to defaults.
	    // (this is in the ctcp2 draft).  These calculations
	    // are DIRECTLY from ctcp2's draft.
	    if (
		(!fg.IsUndefined() && !bg.IsUndefined()) &&
		(fg.GetR() >= bg.GetR() - 8) && (fg.GetR() <= bg.GetR() + 8) &&
		(fg.GetG() >= bg.GetG() - 8) && (fg.GetG() <= bg.GetG() + 8) &&
		(fg.GetB() >= bg.GetB() - 8) && (fg.GetB() <= bg.GetB() + 8)
		)
	    {
		if (mode == Gateway) {
		    fg = RGBColor::Indeterminate;
		    bg = RGBColor::Indeterminate;
		} else {
		    fg = fgDef;
		    bg = bgDef;
		}
	    }
	    break;
	    
	    // font size
	case 'F':
	    parseFrmChar++;
	    if (parseFrmChar < parseEndChar) {
		if (line->at(parseFrmChar) == '+') fntDir = 1;
		else if (line->at(parseFrmChar) == '-') fntDir = -1;
		else fntDir = 0;
		parseFrmChar++;
		if (parseFrmChar < parseEndChar) {
		    fntVal = line->at(parseFrmChar) - 48;
		    if (fntVal > 4) fntVal = 4;
		} else {
		    // no amount -- assume 1
		    fntVal = 1;
		    parseFrmChar--;
		}
		// If no direction was specified, make sure we
		// return to zero.
		//
		fntSize = (fntDir*fntVal == 0 ? 0 : fntSize+(fntDir*fntVal));
	    } else {
		// no parameters -- reset to normal
		fntSize = 0;
		parseFrmChar--;
	    }
	    break;
	    
	// character spacing
	case 'P':
	    parseFrmChar++;   // consume it
	    if (parseFrmChar < parseEndChar) {
		switch (line->at(parseFrmChar)) {
		case 'P':
		    cp = Proportional; parseFrmChar++; break;
		case 'F':
		    cp = Fixed; parseFrmChar++; break;
		default:
		    // rollback to show the error.
		    parseFrmChar -= 2;
		}
	    } else {
		// no parameters -- reset to normal
		cp = cpDef;
	    }
	    break;
	    
	// revert to normal
	case 'N':
	    parseFrmChar++;   // consume it
	    boldCnt = revCnt = overCnt = underCnt = italicCnt = blinkCnt = urlCnt = 0;
	    ClearExtensions();
	    fg = fgDef;
	    bg = bgDef;
	    cp = cpDef;
	    fntSize = 0;
	    needReset = 1;
	    break;

	// client extensions
	case 'X':
	    parseFrmChar++;   // consume it
	    if (ParseExtension(line, &parseFrmChar, &parseEndChar)) {
		//rollback to show the error.
		parseFrmChar -= 2;
	    }
	    break;

	// error condition...
	default:
	    // rollback one.  unrecognized format specifier.
	    parseFrmChar--;
	}
	
	// extra params? include them in the text surrounded by "[ ]"
	// (This is recommended, in case someone chooses to "hide" a
	//    message by doing "^FNsell yoyodyne stocks!^F" -- this code
	//    effectively "unhides" it, and is [yet again] recommended
	//    by the CTCP2 draft.)

	parseFrmChar++;

	if (parseFrmChar < parseEndChar) {
	    line->replace(parseFrmChar, parseEndChar - parseFrmChar,
		    std::u16string(u"\006[") +
		    line->substr(parseFrmChar, parseEndChar-parseFrmChar) +
		    std::u16string(u"]"));
		    
	    parseEndChar = parseFrmChar;
	}

	// save old end position
	parseStart = parseEndChar+1;
	
	// got any more?
	parseFrmChar = line->find_first_of(u"\006", parseStart);
    }

    // anything left?
    if (parseStart < line->length()) {
        // last text segment is ready to push.
	if (parseStart == 0) {
	    segment = line;
	    PushOut();
	    PushTextSegment(segment);
	} else {
	    segment = new std::u16string(line->substr(parseStart));
	    PushOut();
	    PushTextSegment(segment);
	    delete segment;
	}
    }

/*
    if (mode == Literal || mode == TagReset) {
	// Clear everything that is different than default.
	boldCnt = revCnt = overCnt = underCnt = italicCnt = blinkCnt = urlCnt = 0;
	ClearExtensions();
	if (mode == TagReset) {
	    fg = RGBColor::Undefined;
	    bg = RGBColor::Undefined;
	} else {
	    fg = fgDef;
	    bg = bgDef;
	}
	cp = cpDef;
	fntSize = 0;
	PushOut();
    }
*/
}

int
CTCP2Parse::ParseExtension (std::u16string *string, size_t *offset, size_t *endoffset)
{
    // always return an error.
    return 1;
}

void CTCP2Parse::ClearExtensions () {}
void CTCP2Parse::PushForegroundColorReset () {}
void CTCP2Parse::PushBackgroundColorReset () {}


const CTCP2Parse::RGBColor CTCP2Parse::indexColor[16] = {
    RGBColor(Black),
    RGBColor(Blue),
    RGBColor(Green),
    RGBColor(Cyan),
    RGBColor(Red),
    RGBColor(Magenta),
    RGBColor(Yellow),
    RGBColor(Lt_Grey),
    RGBColor(Grey),
    RGBColor(Lt_Blue),
    RGBColor(Lt_Green),
    RGBColor(Lt_Cyan),
    RGBColor(Lt_Red),
    RGBColor(Lt_Magenta),
    RGBColor(Lt_Yellow),
    RGBColor(White)
};

const int32_t CTCP2Parse::RGBColor::Indeterminate = 0x01000000;

#define CCOLOR	u'\003'  // ctrl+c (keypress is really ctrl+k!)
#define CCTCP2	u'\006'  // ctrl+f
#define MIRC_TRANSPARENT    16
#define isn(ch) ((ch) >= '0' && (ch) <= '9')


const char16_t
mircAttrCvt::tokens[2] = {
    CCOLOR, 0
};


const char16_t *
mircAttrCvt::mirc2ctcpmap[17] = {
    u"F",		// white
    u"0",		// black
    u"1",		// blue
    u"2",		// green
    u"C",		// bright red
    u"4",		// red
    u"#A000A0",		// purple (between norm and hi intensity)
    u"#FF8000",		// orange  (definately not yellow (808000))
    u"E",		// bright yellow
    u"A",		// bright green
    u"3",		// cyan
    u"B",		// bright cyan
    u"9",		// bright blue
    u"#FF00FF",		// pink    (not really bright magenta (FF00FF))
    u"8",		// grey
    u"7",		// light grey
    u"."		// return to default (^K99)
};


void
mircAttrCvt::mirc2ctcp(std::u16string *in)
{
    size_t col;
    std::u16string f;
    bool eqf, eqb;
    short oldfg, oldbg, fg = -1, bg = -1;
    size_t pos;

    pos = in->find_first_of(tokens);

    while (pos != std::string::npos) {
	oldfg = fg;
	oldbg = bg;
	col = extractcolor(in, pos+1, &fg, &bg);
	eqf = (oldfg == fg);
	eqb = (oldbg == bg);

	if (col && (!eqf || !eqb)) {
	    // if the color hasn't changed, we insert a - which means
	    // basically "ignore me"
	    f = std::u16string(u"C") + (eqf ? u"-" : mirc2ctcpmap[fg]) +
		    (eqb ? (eqf ? 0 : u"-") : mirc2ctcpmap[bg]);
	 //   swprintf(f, 20, u"C%s%s", 
	//	    eqf ? u"-" : mirc2ctcpmap[fg], 
	//	    eqb ? (eqf ? 0 : u"-") : mirc2ctcpmap[bg]);

	    in->replace(pos, col+1,
		    std::u16string(u"") + CCTCP2 + f + CCTCP2);

	    pos += (1 + f.length());   // consume the additions
	} else {
	    // reset
	    fg = bg = -1;
	    in->replace(pos, 1, std::u16string(u"") + CCTCP2 + u"C" + CCTCP2);
	    pos += 2;   // consume the additions
	}
	pos = in->find_first_of(tokens, ++pos);
    }
};


size_t
mircAttrCvt::extractcolor (std::u16string *in, size_t start, short *fg, short *bg)
{
    size_t ptr = start, len = in->length(), preColor;

    if (ptr == len) return 0;

    if (isn(in->at(ptr)))
    {
	// there's a foreground

	if (ptr + 1 < len && isn(in->at(ptr + 1))) {
	    // it's 2 digits
	    preColor = (((in->at(ptr) - '0') * 10) + (in->at(ptr + 1) - '0'));
	    if (preColor == 99) *fg = MIRC_TRANSPARENT;
	    else *fg = preColor % 16;
	    ptr += 2;
	} else {
	    // it's 1 digit
	    *fg = in->at(ptr) - '0';
	    ptr++;
	}
	if (ptr + 1 < len && in->at(ptr) == ',' && isn(in->at(ptr + 1))) {
	    // there's a background
	    ptr++; // the comma

	    if (ptr + 1 < len && isn(in->at(ptr + 1))) {
		// it's 2 digits
		preColor = (((in->at(ptr) - '0') * 10) + (in->at(ptr + 1) - '0'));
		if (preColor == 99) *bg = MIRC_TRANSPARENT;
		else *bg = preColor % 16;
		ptr += 2;
	    } else {
		// it's 1 digit
		*bg = in->at(ptr) - '0';
		ptr++;
	    }
	}
    }
    return (ptr - start);
};


// ------------------------------ macro definitions -------------------
#define CBOLD	u'\002'	    // ctrl+b, bold
#define CCTCP2	u'\006'	    // ctrl+f, ctcp/2 format character
#define CSTOP	u'\017'	    // ctrl+o, reset
#define CREV	u'\026'	    // ctrl+v, reverse
#define CUNDR	u'\037'	    // ctrl+_, underline
#define CITAL	u'\035'	    // ctrl+n, italics; used by besirc and hydrairc


const char16_t irciiAttrCvt::tokens[6] = {
    CBOLD, CUNDR, CREV, CSTOP, CITAL, 0
};

void
irciiAttrCvt::ircii2ctcp (std::u16string *in)
{
    size_t pos;
    bool bold, under, rev, italics;

    bold = under = rev = italics = false;

    pos = in->find_first_of(tokens);

    while (pos != std::string::npos) {
	switch (in->at(pos)) {
	case CBOLD:
	    bold = !bold;
	    in->replace(pos, 1,
		std::u16string(u"") + CCTCP2 + u"B" + (bold?u"+":u"-") + CCTCP2);
	    pos += 3;   // consume the additions
	    break;

	case CUNDR:
	    under = !under;
	    in->replace(pos, 1,
		std::u16string(u"") + CCTCP2 + u"U" + (under?u"+":u"-") + CCTCP2);
	    pos += 3;   // consume the additions
	    break;

	case CREV:
	    rev = !rev;
	    in->replace(pos, 1,
		std::u16string(u"") + CCTCP2 + u"V" + (rev?u"+":u"-") + CCTCP2);
	    pos += 3;   // consume the additions
	    break;

	case CITAL:
	    italics = !italics;
	    in->replace(pos, 1,
		std::u16string(u"") + CCTCP2 + u"I" + (italics?u"+":u"-") + CCTCP2);
	    pos += 3;   // consume the additions
	    break;

	case CSTOP:
	    bold = under = rev = italics = false;
	    in->replace(pos, 1, std::u16string(u"") + CCTCP2 + u"N" + CCTCP2);
	    pos += 2;   // consume the additions
	}
	pos = in->find_first_of(tokens,++pos);
    }
}

// ------------------------------ macro definitions -------------------
#define CHICLR	u'\004'	    // hi-color

const char16_t hydraAttrCvt::tokens[2] = {
    CHICLR, 0
};

void
hydraAttrCvt::hydra2ctcp (std::u16string *in)
{
    size_t pos, i, begin;
    std::u16string fore, back;

    pos = in->find_first_of(tokens);

    while (pos != std::string::npos) {
	switch (in->at(pos)) {
	case CHICLR:
	    begin = pos++;	    // consume the \004, but save the
				    // start
	    if (in->at(pos) != ',') {
		// get foreground color if 6 hexidecimal numbers follow
		if (pos + 6 > in->length()) break;
		for (i = 0; i < 6; i++) {
		    if (!isxdigit(in->at(pos + i))) break;
		}
		// no dice, bug out.
		if (i != 6) break;
		fore = std::u16string(u"#") + in->substr(pos, 6);
		pos += 6;   // move the pointer forward.
	    } else {
		fore = std::u16string(u"-");
	    }
	    // check for a ,
	    if (in->at(pos) == ',') {
		pos++;	    // consume the ','
		// get background color if 6 hexidecimal numbers follow
		if (pos + 6 > in->length()) break;
		for (i = 0; i < 6; i++) {
		    if (!isxdigit(in->at(pos + i))) break;
		}
		// no dice, bug out.
		if (i != 6) break;
		back = std::u16string(u"#") + in->substr(pos, 6);
		pos += 6;   // move the pointer forward.
	    } else {
		back = std::u16string(u"-");
	    }
	    std::u16string replacement(std::u16string(u"") + CCTCP2 + u"C" +
		    fore + back + CCTCP2);
	    in->replace(begin, pos-begin, replacement);
	    // set pointer to last character of the new sequence, not
	    // the next character following.
	    pos = begin + replacement.length() - 1;
	    break;
	}
	pos = in->find_first_of(tokens,++pos);
    }
}

#define INVALID_SEQUENCE    -1
const char16_t *ansiAttrCvt::AnsiSeq::_notOf = u"1234567890=;";

ansiAttrCvt::AnsiSeq::AnsiSeq (std::u16string *in, size_t pos)
    : _seq(0), _verb(0)
{
    size_t verbPos;
    size_t seqLen;
    size_t pos1, pos2;

    verbPos = in->find_first_not_of(_notOf, pos + 2);

    if (verbPos == std::string::npos || in->at(verbPos) == u'\033') {
	_verb = INVALID_SEQUENCE;
	in->replace(pos, 2, u"");
	return;
    }

    seqLen = verbPos - pos - 1;

    // get a copy of the ansi sequence without the "esc["
    _seq = new std::u16string(in->substr(pos + 2, seqLen));

    // remove it from the original, now that we have the needed info
    in->replace(pos, verbPos - pos + 1, u"");

    // set the verb
    _verb = _seq->at(seqLen - 1);

    //std::wstring_convert<std::codecvt_utf16<wchar_t, 0x10ffff>, wchar_t>{}.from_bytes()

    for (pos1 = 0, pos2 = 0; pos2 < seqLen; pos2++) {
	if (_seq->at(pos2) != ';' && pos2 < (seqLen - 1)) continue;
	// a bit messy, sorry
	_params.push_back(_wtoi((wchar_t*)_seq->substr(pos1, pos2 - pos1).c_str()));
	pos1 = pos2 + 1;
    }
}


ansiAttrCvt::AnsiSeq::~AnsiSeq ()
{
    if (_seq) delete _seq;
}


#define ANSI_SGM_TA_RESET   0
#define ANSI_SGM_TA_BOLD    1
#define ANSI_SGM_TA_ULINE   4
#define ANSI_SGM_TA_BLINK   5
#define ANSI_SGM_TA_REV     7
#define ANSI_SGM_TA_CONSEAL 8
#define ANSI_SGM_FC_BLK     30
#define ANSI_SGM_FC_RED     31
#define ANSI_SGM_FC_GRN     32
#define ANSI_SGM_FC_YEL     33
#define ANSI_SGM_FC_BLU     34
#define ANSI_SGM_FC_MAG     35
#define ANSI_SGM_FC_CYN     36
#define ANSI_SGM_FC_WHT     37
#define ANSI_SGM_BC_BLK     40
#define ANSI_SGM_BC_RED     41
#define ANSI_SGM_BC_GRN     42
#define ANSI_SGM_BC_YEL     43
#define ANSI_SGM_BC_BLU     44
#define ANSI_SGM_BC_MAG     45
#define ANSI_SGM_BC_CYN     46
#define ANSI_SGM_BC_WHT     47


const char16_t *ansiAttrCvt::escSeq = u"\033\133";

void
ansiAttrCvt::ansi2ctcp (std::u16string *in)
{
    bold = underline = reverse = blink = 0;
    _in = in;
    
    pos = in->find(escSeq);
    
    while (pos != std::string::npos) {
	_seq = new AnsiSeq(in, pos);
	
	switch (_seq->verb()) {
	case INVALID_SEQUENCE:
	    break;
	    
	case 'm':		// set graphic mode
	    do_m();
	    break;
	case 'C':		// move cursor forward
	    do_C();
	    break;
	case 'f':		// move to cursor position
	case 'H':		//          "
	case 'A':		// move cursor up
	case 'B':		// move cursor down
	case 'D':		// move cursor backward
	case 's':		// save cursor position
	case 'u':		// restore cursor position
	case 'J':		// erase display and goto home
	case 'K':		// erase line
	case 'h':		// set display mode
	case 'l':		// reset display mode
	default:
	    std::u16string errmsg(std::u16string(u"[ansi error: '") + _seq->seq() +
		u"' sequence unsupported]");
	    in->insert(pos, errmsg);
	    pos += errmsg.length();
	}
	delete _seq;
	pos = in->find(escSeq, pos);
    }
}

const char16_t *ansiAttrCvt::fgColorMap[16] = {
    u"C0-",		// Black
    u"C4-",		// Red
    u"C2-",		// Green
    u"C6-",		// Yellow
    u"C1-",		// Blue
    u"C5-",		// Magenta
    u"C3-",		// Cyan
    u"C7-",		// Light Grey
    u"C8-",		// Grey
    u"CC-",		// Light Red
    u"CA-",		// Light Green
    u"CE-",		// Light Yellow
    u"C9-",		// Light Blue
    u"CD-",		// Light Magenta
    u"CB-",		// Light Cyan
    u"CF-"		// White
};

const char16_t *ansiAttrCvt::bgColorMap[8] = {
    u"C-0",		// Black
    u"C-4",		// Red
    u"C-2",		// Green
    u"C-6",		// Yellow
    u"C-1",		// Blue
    u"C-5",		// Magenta
    u"C-3",		// Cyan
    u"C-7"		// Light Grey
};

void
ansiAttrCvt::do_m ()
{
    int i;
    for (i = 0; i < _seq->paramc(); i++) {
	switch (_seq->param(i)) {
	    // reset
	case ANSI_SGM_TA_RESET:
	    bold = underline = reverse = blink = 0;
	    _in->insert(pos, std::u16string(u"") + CCTCP2 + u"N" + CCTCP2);
	    pos += 3;
	    break;

	    // bold color, not an \002 (Ctrl+B) bold
	case ANSI_SGM_TA_BOLD:
	    if (!bold) {
		// a second call to bold while already bold 
		// doesn't turn it off in ansi graphics
		bold = 1;
	    }
	    break;

	    // underline
	case ANSI_SGM_TA_ULINE:
	    if (!underline) {
		underline = 1;
		_in->insert(pos, std::u16string(u"") + CCTCP2 + u"U+" + CCTCP2);
		pos += 4;	// consume the additions
	    }
	    break;

	    // blink
	case ANSI_SGM_TA_BLINK:
	    if (!blink) {
		blink = 1;
		_in->insert(pos, std::u16string(u"") + CCTCP2 + u"K+" + CCTCP2);
		pos += 4;	// consume additions.
	    }
	    break;

	    // reverse
	case ANSI_SGM_TA_REV:
	    if (!reverse) {
		reverse = 1;
		_in->insert(pos, std::u16string(u"") + CCTCP2 + u"V+" + CCTCP2);
		pos += 4;	// consume the additions
	    }
	    break;

	    // concealed
	case ANSI_SGM_TA_CONSEAL:
	    // wtf is this?
	    break;

	case ANSI_SGM_FC_BLK:
	case ANSI_SGM_FC_RED:
	case ANSI_SGM_FC_GRN:
	case ANSI_SGM_FC_YEL:
	case ANSI_SGM_FC_BLU:
	case ANSI_SGM_FC_MAG:
	case ANSI_SGM_FC_CYN:
	case ANSI_SGM_FC_WHT:
	    _in->insert(pos, std::u16string(u"") + CCTCP2 +
		fgColorMap[_seq->param(i) - 30 + (bold ? 8 : 0)] +
		CCTCP2);
	    pos += 5;		// consume the additions
	    break;

	case ANSI_SGM_BC_BLK:
	case ANSI_SGM_BC_RED:
	case ANSI_SGM_BC_GRN:
	case ANSI_SGM_BC_YEL:
	case ANSI_SGM_BC_BLU:
	case ANSI_SGM_BC_MAG:
	case ANSI_SGM_BC_CYN:
	case ANSI_SGM_BC_WHT:
	    _in->insert(pos, std::u16string(u"") +
		CCTCP2 + bgColorMap[_seq->param(i) - 40] + CCTCP2);
	    pos += 5;		// consume the additions
	    break;
	}
    }
}

void
ansiAttrCvt::do_C ()
{
    for (std::u16string::size_type i = 0; i < _seq->param(_seq->paramc() - 1); i++)
	_in->insert(pos++, u" ");
}
