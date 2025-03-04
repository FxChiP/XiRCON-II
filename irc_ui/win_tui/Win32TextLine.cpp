/*
 ------------------------------------------------------------------------------
 * Win32TextLine.cpp --
 *
 *   Platform specific implimentation class of the component
 *
 * Copyright (c) 1999-2000 Tomahawk Software Group
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: Win32TextLine.cpp,v 1.1 2003/11/14 10:36:21 davygrvy Exp $
 ------------------------------------------------------------------------------
 */
#include "Win32TextLine.hpp"

#pragma warning (disable:4786)  // I don't care to know about name truncation


// does not take ownership of the string class.  It's the caller responsibility
// to delete it, not us.  And we might write to it, too.  So it's not a const
Win32TextLine::Win32TextLine(std::string *line, HWND hwndViewBox)
  : bBMPInvalid(true)
{
  int parseFrmChar = 0, parseEndChar = 0, parseStart = 0;
  int encoding = 28591,   //  Latin-1/iso8859-1
      boldCnt = 0,
      revCnt = 0,
      overCnt = 0,
      underCnt = 0,
      italicCnt = 0,
      blinkCnt = 0,
      urlCnt = 0;
  int fntVal, fntDir = 1, fntSize = 0;
  COLORREF fg, fgDef, fgLast, bg, bgDef, bgLast;
  fg = fgDef = fgLast = RGB(204, 204, 204);  // light-grey
  bg = bgDef = bgLast = RGB(0, 0, 0);        // black

  LOGFONT lf =
  {
    9,                           // lfHeight
    0,                           // lfWidth
    0,                           // lfEscapement
    0,                           // lfOrientation
    FW_NORMAL,                   // lfWeight
    FALSE,                       // lfItalic
    FALSE,                       // lfUnderline
    FALSE,                       // lfStrikeOut
    DEFAULT_CHARSET,             // lfCharSet
    OUT_TT_PRECIS,               // lfOutPrecision
    CLIP_DEFAULT_PRECIS,         // lfClipPrecision
    PROOF_QUALITY,               // lfQuality
    DEFAULT_PITCH | FF_DONTCARE, // lfPitchAndFamily
    "Courier New"                // lfFaceName[LF_FACESIZE]
  };

  
  // fill up the vDrawables vector from the ctcp/2 encoded text.


  // pre-parse and replace other attribute standards with
  // the ctcp2 equivalents
  mirc2ctcp(line);
  ircii2ctcp(line);
  ansi2ctcp(line);
  

  // find opening ctcp/2 format character (^F)
  parseFrmChar = line->find_first_of("\006");

  // do we have any?
  while (parseFrmChar != std::string::npos) {
    std::string *segment;

    // find the closing ctcp/2 format character (^F)
    parseEndChar = line->find_first_of("\006", parseFrmChar+1);

    if (parseEndChar == std::string::npos) {
      // an opening without a closing is bogus
      // so remove it and fallout of the while loop
      line->replace(parseFrmChar, 1, "");
      break;
    }

    // check for text before this format character
    // and after the last format character (or index 0 for
    // the first iteration.
    if (parseFrmChar > parseStart) {
      int pushFont = 0;
  
      segment = new std::string(line->substr(parseStart, parseFrmChar));

      // yes.. we got a text segment ready to push.
      // never push-out ALL the attributes... only the CHANGES.

      if ((boldCnt > 0) ^ (lf.lfWeight == FW_BOLD)) {
        pushFont = 1;
        lf.lfWeight = (boldCnt ? FW_BOLD : FW_NORMAL);
      }

      if ((overCnt > 0) ^ (lf.lfStrikeOut == TRUE)) {
        pushFont = 1;
        lf.lfStrikeOut = (overCnt ? TRUE : FALSE);
      }

      if ((underCnt > 0) ^ (lf.lfUnderline == TRUE)) {
        pushFont = 1;
        lf.lfUnderline = (underCnt ? TRUE : FALSE);
      }

      if ((italicCnt > 0) ^ (lf.lfItalic == TRUE)) {
        pushFont = 1;
        lf.lfItalic = (italicCnt ? TRUE : FALSE);
      }

      // do font size stuff here

      // do font width stuff here

      if (pushFont)
          vDrawables.push_back(new FontChange(&lf));

      if (fg ^ fgLast) {
        fgLast = fg;
        vDrawables.push_back(new FGColorChange(fg));
      }

      if (bg ^ bgLast) {
        bgLast = bg;
        vDrawables.push_back(new BGColorChange(bg));
      }

      if (urlCnt)
        // make sure the encoding is Latin-1,  URLs might be defined
        // for 7-bit though, not sure.
//        vDrawables.push_back(new URLGlyph(new std::string(segment)));
__asm nop;
      else
        vDrawables.push_back(
          new Win32TextSegment(new Win32wstring(encoding, segment), hwndViewBox));
  
      delete segment;
    }

    // now, interpret this format code and change the attributes
    parseFrmChar++;
    switch (line->at(parseFrmChar)) {

#define CTCP2_TOGGLE(ch, var)  \
    case (ch): \
      decodeToggle(line, &parseFrmChar, &parseEndChar, &(var)); \
      break;

    // bold
    CTCP2_TOGGLE('B', boldCnt);

    // reverse
    CTCP2_TOGGLE('V', revCnt);

    // underline
    CTCP2_TOGGLE('U', underCnt);

    // over-strike
    CTCP2_TOGGLE('S', overCnt);

    // italics
    CTCP2_TOGGLE('I', italicCnt);

    // URL
    CTCP2_TOGGLE('L', urlCnt);

    // color
    case 'C':
      parseFrmChar++;   // consume it
      if (parseFrmChar < parseEndChar) {
        if (line->at(parseFrmChar) == '.') {
          fg = fgDef;
          parseFrmChar++;  // consume it
        }
        else if (line->at(parseFrmChar) != '-')
            decodeColor(line, &parseFrmChar, &parseEndChar, &fg);

        // background color?
        if (parseFrmChar < parseEndChar) {
          if (line->at(parseFrmChar) == '.') {
            bg = bgDef;
            parseFrmChar++;  // consume it
          }
          else if (line->at(parseFrmChar) != '-')
              decodeColor(line, &parseFrmChar, &parseEndChar, &bg);
        }
        else
            parseFrmChar--;   // rollback one
      } else {
        // no colors were specified: reset to normal
        fg = fgDef;
        bg = bgDef;
      }
      // if fg/bg are too similar, reset to defaults.
      // (this is in the ctcp2 draft, so don't bitch if this
      //    happens to your colors!)
      {
        int R1, R2, G1, G2, B1, B2;

        R1 = fg & 0x000000FF;
        R2 = bg & 0x000000FF;
        G1 = (fg & 0x0000FF00) >> 8;
        G2 = (bg & 0x0000FF00) >> 8;
        B1 = (fg & 0x00FF0000) >> 16;
        B2 = (bg & 0x00FF0000) >> 16;
        // these calculations are DIRECTLY from ctcp2's draft
        if ((R1 >= R2 - 8) && (R1 <= R2 + 8) &&
            (G1 >= G2 - 8) && (G1 <= G2 + 8) &&
            (B1 >= B2 - 8) && (B1 <= B2 + 8))
        {
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
        else fntDir = -1;
        parseFrmChar++;
        if (parseFrmChar < parseEndChar) {
          fntVal = line->at(parseFrmChar) - 48;
          if (fntVal > 4) fntVal = 4;
        } else {
          // no amount -- assume 1
          fntVal = 1;
          parseFrmChar--;
        }
        fntSize += (fntDir*fntVal);
      } else {
        // no parameters -- reset to normal
        fntSize = 0;
        parseFrmChar--;
      }
      break;

    // character spacing
    case 'P':
      parseFrmChar++;   // consume it
      // ...  add code here
      break;

    // text encoding
    case 'E':
      parseFrmChar++;   // consume it
      switch (line->at(parseFrmChar)) {
      case '1':
        if (line->at(parseFrmChar+1) == '0') {parseFrmChar++;}; // encoding = 28591; what's iso8859-10 ???
        /*else*/ encoding = 28591;
        parseFrmChar++; break;
      case '2':
        encoding = 28592; parseFrmChar++; break;
      case '3':
        encoding = 28593; parseFrmChar++; break;
      case '4':
        encoding = 28594; parseFrmChar++; break;
      case '5':
        encoding = 28595; parseFrmChar++; break;
      case '6':
        encoding = 28596; parseFrmChar++; break;
      case '7':
        encoding = 28597; parseFrmChar++; break;
      case '8':
        encoding = 28598; parseFrmChar++; break;
      case '9':
        encoding = 28599; parseFrmChar++; break;
      case 'U':
        encoding = CP_UTF8; parseFrmChar++; break;
      default:
        // error..
        __asm nop;
      }
      break;

    // revert to normal
    case 'N':
      parseFrmChar++;   // consume it
      boldCnt = revCnt = overCnt = underCnt = italicCnt = 0;
      encoding = 28591;
      break;

    // client extensions
    case 'X':
	parseFrmChar++;
	switch (line->at(parseFrmChar)) {

	// put kano's ideas here...

	// blink
	CTCP2_TOGGLE('K', blinkCnt);

	default:
	    // rollback to show the error.
	    parseFrmChar -= 2;
	}
	break;

    // error condition...
    default:
      // rollback one.  unrecognized format specifier
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
          std::string("\006[") +
          line->substr(parseFrmChar, parseEndChar-parseFrmChar) +
          std::string("]"));
      parseEndChar = parseFrmChar;
    }

    // save old end position
    parseStart = parseEndChar+1;

    // got any more?
    parseFrmChar = line->find_first_of("\006", parseStart);
  };



  // anything left?
  if (parseEndChar < line->length()) {
    int pushFont = 0;

    // last text segment is ready to push.
    // never push-out ALL the attributes... only the CHANGES.

    if ((boldCnt > 0) ^ (lf.lfWeight == FW_BOLD)) {
      pushFont = 1;
      lf.lfWeight = (boldCnt ? FW_BOLD : FW_NORMAL);
    }

    if ((overCnt > 0) ^ (lf.lfStrikeOut == TRUE)) {
      pushFont = 1;
      lf.lfStrikeOut = (overCnt ? TRUE : FALSE);
    }

    if ((underCnt > 0) ^ (lf.lfUnderline == TRUE)) {
      pushFont = 1;
      lf.lfUnderline = (underCnt ? TRUE : FALSE);
    }

    if ((italicCnt > 0) ^ (lf.lfItalic == TRUE)) {
      pushFont = 1;
      lf.lfItalic = (italicCnt ? TRUE : FALSE);
    }

    // do font size stuff here

    // do font width stuff here

    if (pushFont)
        vDrawables.push_back(new FontChange(&lf));

    if (fg ^ fgLast) {
      fgLast = fg;
      vDrawables.push_back(new FGColorChange(fg));
    }

    if (bg ^ bgLast) {
      bgLast = bg;
      vDrawables.push_back(new BGColorChange(bg));
    }

    if (urlCnt)
        // make sure the encoding is Latin-1,  URLs might be defined
        // for 7-bit though, not sure.
//        vDrawables.push_back(new URLGlyph(new std::string(segment)));
__asm nop;
    else
      vDrawables.push_back(
        new Win32TextSegment(
          new Win32wstring(encoding, line->substr(parseEndChar)), hwndViewBox
        )
      );
  }
};

Win32TextLine::~Win32TextLine() {
  std::list<DrawableItem *>::iterator pDraw;

  DeleteBMPs();
  for (
    pDraw = vDrawables.begin();
    pDraw != vDrawables.end();
    pDraw++
  ) delete (*pDraw);
  //vDrawables.erase(vDrawables.begin(), vDrawables.end());
}

const COLORREF Win32TextLine::indexColor[16] = {
    RGB(0, 0, 0),         // black
    RGB(0, 0, 127),       // blue
    RGB(0, 127, 0),       // green
    RGB(0, 127, 127),     // cyan
    RGB(127, 0, 0),       // red
    RGB(127, 0, 127),     // purple
    RGB(127, 127, 0),     // brown
    RGB(204, 204, 204),   // light gray
    RGB(127, 127, 127),   // gray
    RGB(0, 0, 255),       // light blue
    RGB(0, 255, 0),       // light green
    RGB(0, 255, 255),     // light cyan
    RGB(255, 0, 0),       // light red
    RGB(255, 0, 255),     // light magenta
    RGB(255, 255, 0),     // yellow
    RGB(255, 255, 255)    // white
    };

bool Win32TextLine::Draw(HDC hdc, int maxy, int *startX) {
  HDC hdcMem = ::CreateCompatibleDC(hdc);
  std::list<HBITMAP *>::reverse_iterator pBMP;
  int lastX = *startX;

  if (bBMPInvalid) DrawBMPs(hdc, maxy);

  for (
    pBMP = hbRows.rbegin();
    pBMP != hbRows.rend();
    pBMP++
  ) {
    ::SelectObject(hdcMem, *pBMP);
    ::BitBlt(hdc, 0, 0/*cyOffset*/, 0/*cxEcho*/, 0/*cyEcho*/, hdcMem, 0,
        0, SRCCOPY);
    if (lastX <= 0) return true;  // all finished!
  }
  return false;
};

void Win32TextLine::DrawBMPs(HDC hdc, int maxy) {
  HDC hdcMem = ::CreateCompatibleDC(hdc);
  std::list<DrawableItem *>::iterator pDraw;

  DeleteBMPs();

  for (
    pDraw = vDrawables.begin();
    pDraw != vDrawables.end();
    pDraw++
  ) {
    SIZE size;

    // build the width and height for the glyphs then draw 'em
    (*pDraw)->CalcExtent(hdc, &size, maxy);
  }
};

