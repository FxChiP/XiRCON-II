/*
 ------------------------------------------------------------------------------
 * CTCP2Parse.hpp  --
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
 * RCS: @(#) $Id: CTCP2Parse.hpp,v 1.12 2004/12/12 19:15:40 davygrvy Exp $
 ------------------------------------------------------------------------------
 */


#ifndef INC_CTCP2Parse_hpp__
#define INC_CTCP2Parse_hpp__

#include "tcl.h"
#include <string>
#include <vector>

typedef unsigned char byte;

// Mirc to CTCP/2 attribute convertor
class mircAttrCvt
{
public:
    void mirc2ctcp(std::u16string *in);
private:
    static const char16_t tokens[2];
    static const char16_t *mirc2ctcpmap[17];
    size_t extractcolor (std::u16string *in, size_t start, short *fg, short *bg);
};


// ircII to CTCP/2 attribute convertor
class irciiAttrCvt
{
public:
    void ircii2ctcp(std::u16string *in);
private:
    static const char16_t tokens[6];
};


// HydraIRC attribute convertor
class hydraAttrCvt
{
public:
    void hydra2ctcp(std::u16string *in);
private:
    static const char16_t tokens[2];
};


// ANSI escape sequences to CTCP/2 attribute convertor
class ansiAttrCvt
{
public:
    void ansi2ctcp(std::u16string *in);
private:
    void do_m ();
    void do_C ();
	
    const static char16_t *escSeq;
    const static char16_t *fgColorMap[16];
    const static char16_t *bgColorMap[8];
	
    size_t pos;
    int bold, underline, reverse, blink;
	
    std::u16string *_in;
	
    //  A simple ansi escape sequence extractor class
    class AnsiSeq
    {
	std::u16string *_seq;
	std::vector<int> _params;
	char16_t _verb;
	static const char16_t *_notOf;
		
    public:
	AnsiSeq (std::u16string *in, size_t pos);
	~AnsiSeq ();
		
	const char16_t verb () { return _verb; }
	const char16_t *seq () { return _seq->c_str(); }
	size_t paramc () { return _params.size(); }
	size_t param (size_t index) { return _params[index]; }
    };
	
    AnsiSeq *_seq;
};


// CTCP/2 embedded attribute parser and action invoker.
//
class CTCP2Parse : protected mircAttrCvt, irciiAttrCvt, hydraAttrCvt, ansiAttrCvt
{
public:

    // Mode for the parser.
    enum ParseMode {
	Gateway,	// Parser has no knowledge of default colors or
			// spacing (pass-thru).  Parser is not being used
			// for rendering.

	Render		// Parser knows default colors and forces color
			// change rules.  Parser is being used for
			// rendering.
    };
    // Character spacing.
    enum CharSpacingType {
	Fixed,		// monospaced.
	Proportional	// not monospaced.
    };

    class DefaultData
    {
    public:
	// default foreground color.
	byte fgR, fgG, fgB;

	// default background color.
	byte bgR, bgG, bgB;

	// default spacing.
	CharSpacingType cspc;
    };

    // 24-bit colors (initialized here as integers for clarity)
    enum ColorType
    {
	Black      = 0x000000,
	Blue       = 0x000080,
	Green      = 0x008000,
	Cyan       = 0x008080,
	Red        = 0x800000,
	Magenta    = 0x800080,
	Yellow     = 0x808000,
	Grey       = 0x808080,
	Lt_Grey    = 0xC0C0C0,
	Lt_Blue    = 0x0000FF,
	Lt_Green   = 0x00FF00,
	Lt_Cyan    = 0x00FFFF,
	Lt_Red     = 0xFF0000,
	Lt_Magenta = 0xFF00FF,
	Lt_Yellow  = 0xFFFF00,
	White      = 0xFFFFFF
    };

protected:

    CTCP2Parse(ParseMode mode = Render);

    void SetDefaults (DefaultData &data);
    void SetDefaults (
	byte fgR, byte fgG, byte fgB,	// default foreground color.
	byte bgR, byte bgG, byte bgB,	// default background color.
	CharSpacingType cspc		// default spacing.
	);

    // The entrance to the parser.
    void ParseIt(std::u16string *line);

    // These can be over-riden by derived classes for adding 
    // support to the parser for extensions.
    virtual int ParseExtension (std::u16string *string, size_t *offset, size_t *endoffset);
    virtual void ClearExtensions();
    virtual void PushForegroundColorReset ();
    virtual void PushBackgroundColorReset ();


    // Notifies the derived class that a segment (and attributes, if any)
    // are about to be pushed.  These are required in derived classes.
    virtual void PushStart () = 0;
    virtual void PushBold (bool) = 0;
    virtual void PushReverse (bool) = 0;
    virtual void PushUnderline (bool) = 0;
    virtual void PushOverstrike (bool) = 0;
    virtual void PushItalic (bool) = 0;
    virtual void PushBlink (bool) = 0;
    virtual void PushURL (bool) = 0;
    virtual void PushSpacing (CharSpacingType) = 0;
    virtual void PushFontSize (int) = 0;
    virtual void PushForegroundColor (byte R, byte G, byte B) = 0;
    virtual void PushBackgroundColor (byte R, byte G, byte B) = 0;
    virtual void PushTextSegment (std::u16string *string) = 0;

    class RGBColor
    {
    public:
	const static int Indeterminate;

	RGBColor() : color(Indeterminate) {}
	RGBColor(RGBColor &cpy) : color(cpy.color) {}
	RGBColor(int val) : color(val) {}
	RGBColor &operator= (const RGBColor &from) {
	    color = from.color;
	    return *this;
	}
	int operator= (int32_t newVal) {
	    color = newVal;
	    return color;
	}
	operator int () { return color; }
	operator int* () { return &color; }
	inline int32_t GetHiR () { return ((color & 0xF00000) >> 20); }
	inline int32_t GetLoR () { return ((color & 0x0F0000) >> 16); }
	inline int32_t GetHiG () { return ((color & 0x00F000) >> 12); }
	inline int32_t GetLoG () { return ((color & 0x000F00) >> 8); }
	inline int32_t GetHiB () { return ((color & 0x0000F0) >> 4); }
	inline int32_t GetLoB () { return (color & 0x00000F); }
	inline int32_t GetR () { return ((color & 0xFF0000) >> 16); }
	inline int32_t GetG () { return ((color & 0x00FF00) >> 8); }
	inline int32_t GetB () { return (color & 0x0000FF);}
	inline void SetHiR (int32_t v) { color &= 0x0FFFFF; color |= ((v & 0x0F) << 20); }
	inline void SetLoR (int32_t v) { color &= 0xF0FFFF; color |= ((v & 0x0F) << 16); }
	inline void SetHiG (int32_t v) { color &= 0xFF0FFF; color |= ((v & 0x0F) << 12); }
	inline void SetLoG (int32_t v) { color &= 0xFFF0FF; color |= ((v & 0x0F) << 8); }
	inline void SetHiB (int32_t v) { color &= 0xFFFF0F; color |= ((v & 0x0F) << 4); }
	inline void SetLoB (int32_t v) { color &= 0xFFFFF0; color |= (v & 0x0F); }
	inline void SetR (int32_t v) { color &= 0x00FFFF; color |= ((v & 0xFF) << 16); }
	inline void SetG (int32_t v) { color &= 0xFF00FF; color |= ((v & 0xFF) << 8); }
	inline void SetB (int32_t v) { color &= 0xFFFF00; color |= (v & 0xFF); }
	inline bool IsUndefined () { return (color == Indeterminate); }
    private:
	int32_t color;
    };

    CharSpacingType cp, cpLast, cpDef;	// storage for font spacing type
    RGBColor	fg, fgLast, fgCache, fgDef, bg, bgLast, bgCache, bgDef;
    static const RGBColor indexColor[16];   // Array for indexing the color values

private:

    inline void decodeToggle
	(std::u16string *line, size_t *parseFrmChar, size_t *parseEndChar, int *varCnt)
    {
	(*parseFrmChar)++;   // consume the attribute
	if (*parseFrmChar < *parseEndChar) {
	    if (line->at(*parseFrmChar) == U'+') {
		(*varCnt)++;
		(*parseFrmChar)++;   // consume the '+'
	    } else if (line->at(*parseFrmChar) == U'-') {
		if ((*varCnt) > 0) (*varCnt)--;
		(*parseFrmChar)++;   // consume the '-'
	    }
	} else {
	    (*varCnt)++;  // default to a '+'
	}
    }

    inline int decodeColor
	(std::u16string *line, size_t *parseFrmChar, size_t *parseEndChar, RGBColor *c)
    {
	RGBColor newColor;

	if (line->at(*parseFrmChar) == u'#') {
	    (*parseFrmChar)++;   // consume it
	    if (*parseFrmChar + 6 <= *parseEndChar) {
		int C = 0;

		// hi-byte red
		if (!fromHex(line->at(*parseFrmChar), &C)) return 0;
		(*parseFrmChar)++;  // consume the hex character
		newColor.SetHiR(C);

		// lo-byte red
		if (!fromHex(line->at(*parseFrmChar), &C)) return 0;
		(*parseFrmChar)++;  // consume the hex character
		newColor.SetLoR(C);

		// hi-byte green
		if (!fromHex(line->at(*parseFrmChar), &C)) return 0;
		(*parseFrmChar)++;  // consume the hex character
		newColor.SetHiG(C);

		// lo-byte green
		if (!fromHex(line->at(*parseFrmChar), &C)) return 0;
		(*parseFrmChar)++;  // consume the hex character
		newColor.SetLoG(C);

		// hi-byte blue
		if (!fromHex(line->at(*parseFrmChar), &C)) return 0;
		(*parseFrmChar)++;  // consume the hex character
		newColor.SetHiB(C);

		// lo-byte blue
		if (!fromHex(line->at(*parseFrmChar), &C)) return 0;
		(*parseFrmChar)++;  // consume the hex character
		newColor.SetLoB(C);
	    } else {
		// thrown an error...
		return 0;
	    }
	    *c = newColor;
	} else {
	    int index = 0;
	    if (!fromHex(line->at(*parseFrmChar), &index)) return 0;
	    (*parseFrmChar)++;  // consume the hex character
	    *c = indexColor[index];
	}
	return 1;
    }

    inline int fromHex(int h, int *val)
    {
	// 0 - 9
	if (h > 47 && h < 58) *val = (h - 48);
	// A - F
	else if (h > 64 && h < 71) *val = (h - 55);
	else return 0;   // throw a little conversion exception
	return 1;
    }

    inline void PushOut (void)
    {
	fgCache = fg;
	bgCache = bg;

	// notify the derived class, we are about to start a new attribute set.
	PushStart();

	// never push-out ALL the attributes -- only the CHANGED ones.

	if ((boldCnt > 0) ^ boldLast) PushBold (boldLast = !boldLast ? true : false);
	if ((revCnt > 0) ^ revLast) PushReverse (revLast = !revLast ? true : false);
	if ((underCnt > 0) ^ underLast)	PushUnderline (underLast = !underLast ? true : false);
	if ((overCnt > 0) ^ overLast) PushOverstrike (overLast = !overLast ? true : false);
	if ((italicCnt > 0) ^ italicLast) PushItalic (italicLast = !italicLast ? true : false);
	if ((blinkCnt > 0) ^ blinkLast) PushBlink (blinkLast = !blinkLast ? true : false);
	if ((urlCnt > 0) ^ urlLast) PushURL (urlLast = !urlLast ? true : false);
	if (cp ^ cpLast) PushSpacing (cpLast = cp);
	if (fntSize ^ fntLast) PushFontSize (fntLast = fntSize);

	if (fg ^ fgLast) {
	    fgLast = fg;
	    if (fg.IsUndefined()) {
		PushForegroundColorReset();
	    } else {
		PushForegroundColor(fg.GetR(), fg.GetG(), fg.GetB());
	    }
	}
	if (bg ^ bgLast) {
	    bgLast = bg;
	    if (bg.IsUndefined()) {
		PushBackgroundColorReset();
	    } else {
		PushBackgroundColor(bg.GetR(), bg.GetG(), bg.GetB());
	    }
	}

	fg = fgCache;
	bg = bgCache;
    }

    // storage for the toggle states
    int boldCnt,  revCnt,  overCnt,  underCnt,  italicCnt,  blinkCnt,  urlCnt;
    int boldLast, revLast, overLast, underLast, italicLast, blinkLast, urlLast;
    int fntSize, fntLast;

    ParseMode mode;
    std::u16string *segment;
};

#endif  // INC_CTCP2Parse_hpp__