/*
 ------------------------------------------------------------------------------
 * Win32TextLine.hpp --
 *
 *   Platform specific implimentation class what a line of text is and how it
 *   is drawn.
 *
 * Copyright (c) 1999-2000 Tomahawk Software Group
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: Win32TextLine.hpp,v 1.1 2003/11/14 10:36:21 davygrvy Exp $
 ------------------------------------------------------------------------------
 */

#ifndef INC_Win32TextLine_hpp__
#define INC_Win32TextLine_hpp__

//#include <IRCTextLine.hpp>
#include <windows.h>
#include <string>
#include <list>
#include <ctcp2parse.hpp>


class Win32wstring : public std::string {
public:
  Win32wstring(unsigned int codepage, std::string *s) {
    convert(codepage, s->data(), s->length());
  };
  Win32wstring(unsigned int codepage, std::string &s) {
    convert(codepage, s.data(), s.length());
  };
private:
  inline void convert(unsigned int codepage, const char *s, int len) {
    register int length;
    register wchar_t *buff = NULL;

    // calc size needed
    if (!(length = ::MultiByteToWideChar(codepage, MB_USEGLYPHCHARS, s, -1, buff, 0)))
        throw;   // conversion failed!

    buff = new wchar_t [length];

    // do it
    if (!::MultiByteToWideChar(codepage, MB_USEGLYPHCHARS, s, -1, buff, length))
        throw;   // conversion failed!

    // drop into base constructor
    std::wstring(static_cast<wchar_t *>(buff));
    delete [] buff;
  };
};

class DrawableItem {
public:
  virtual ~DrawableItem() {};
  virtual void Draw(HDC dc) = 0;
  // returns true when the current row is full (ready to be drawn)
  // and needs to be split.
  virtual bool CalcExtent(HDC dc, SIZE *row, int maxy) = 0;
};

class FGColorChange : public DrawableItem {
public:
  FGColorChange(COLORREF fg) : m_fg(fg) {};
  ~FGColorChange() {};
  void Draw(HDC dc) {
    ::SetTextColor(dc, m_fg);
  };
  bool CalcExtent(HDC dc, SIZE *row, int maxy) {
    return false;
  };
private:
  COLORREF m_fg;
};

class BGColorChange : public DrawableItem {
public:
  BGColorChange(COLORREF bg) : m_bg(bg) {};
  ~BGColorChange() {};
  void Draw(HDC dc) {
    ::SetBkColor(dc, m_bg);
  };
  bool CalcExtent(HDC dc, SIZE *row, int maxy) {
    return false;
  };
private:
  COLORREF m_bg;
};

class Win32TextSegment : public DrawableItem {
public:
  Win32TextSegment(std::wstring *pText, HWND hwndViewBox) : m_pText(pText) {
    std::wstring::iterator wi;

    // parse and store the indexes for
    // good word-break positions
    for (
      wi = m_pText->begin();
      wi != m_pText->end();
      wi++)
    {
      
    }
  };
  ~Win32TextSegment() { delete m_pText; };
  void Draw(HDC dc) {
    // use cached positions if set by CalcExtent()
    ::TextOutW(dc, 0, 0, m_pText->data(), -1);
    // reset cached postion
  };
  bool CalcExtent(HDC dc, SIZE *row, int maxy) {
    // use cached positions if set by previous CalcExtent()
    ::GetTextExtentPoint32W(dc, m_pText->data(), -1, &size);
    if (row->cy + size.cy < maxy) {
      row->cy += size.cy;
      row->cx = max(row->cx, size.cx);
      return false;
    } else {
      // do something here with caching positions and flags and such
      return true;
    }
  };
private:
  SIZE size;
  std::wstring *m_pText;
};

class FontChange : public DrawableItem {
public:
  FontChange(LOGFONT *lf) {
    zero.cy = 0;
    zero.cx = 0;
    m_hFont = ::CreateFontIndirect(lf);
  };
  ~FontChange() {
    ::DeleteObject(m_hFont);
  };
  void Draw(HDC dc) {
    ::SelectObject(dc, m_hFont);
  };
  bool CalcExtent(HDC dc, SIZE *row, int maxy) {
    ::SelectObject(dc, m_hFont);
    // we don't use any space, but the font change effects
    // the next CalcExtent.
    return false;
  };
private:
  SIZE zero;
  HFONT m_hFont;
};

class URLGlyph : public DrawableItem {
public:
  URLGlyph(std::string *pURL)
    : m_pURL(pURL), m_pLink(0) {};
  URLGlyph(std::string *pURL, std::string *pLink)
    : m_pURL(pURL), m_pLink(pLink) {};
  ~URLGlyph() { delete m_pURL, m_pLink; };
  void Draw(HDC dc) {
//    ::SelectObject(dc, m_hFontUline);
    // inverse fg color here
    ::TextOut(dc, 0, 0, m_pLink->c_str(), m_pLink->length());
//    ::SelectObject(dc, m_hFont);
  };
  bool CalcExtent(HDC dc, SIZE *row, int maxy) {
    return false;
  };
  std::string *GetURL() { return m_pURL; };
  void OnClick() {
//    ::ShellExecute ...
  };
private:
  SIZE size;
  std::string *m_pURL;
  std::string *m_pLink;
};

class Win32TextLine : protected mircAttrCvt, irciiAttrCvt, ansiAttrCvt {
public:
  Win32TextLine(std::string *line, HWND hwndViewBox);
  ~Win32TextLine();
  inline void InvalidateY() {
    bBMPInvalid = true;
  };
  bool Draw(HDC hDc, int maxy, int *startX);
  void OnMouseOver(POINT p);

private:
  inline void decodeToggle
    (std::string *line, int *parseFrmChar, int *parseEndChar, int *varCnt)
  {
    (*parseFrmChar)++;   // consume the attribute
    if (*parseFrmChar < *parseEndChar) {
      if (line->at(*parseFrmChar) == '+') {
        (*varCnt)++;
        (*parseFrmChar)++;   // consume the '+'
      }
      else if (line->at(*parseFrmChar) == '-') {
        if ((*varCnt) > 0)
          (*varCnt)--;
        (*parseFrmChar)++;   // consume the '-'
      }
    }
    else
      (*varCnt)++;  // default to a '+'
  }

  inline int fromHex(int h, int *val) {
    // 0 - 9
    if (h > 47 && h < 58)
      *val += (h - 48);
    // A - F
    else if (h > 64 && h < 71)
      *val += (h - 55);
    // a - f  <- this is a slight addition I'm making to the standard
    //        be liberal in what you receive and be attentive to what you send
    else if (h > 96 && h < 103)
      *val += (h - 87);
    else
      return 0;   // throw a little conversion exception up the stack
    return 1;
  }

  inline int decodeColor
    (std::string *line, int *parseFrmChar, int *parseEndChar, COLORREF *c)
  {
    if (line->at(*parseFrmChar) == '#') {
      (*parseFrmChar)++;   // consume it
      if (*parseFrmChar + 6 <= *parseEndChar) {
        int R = 0, G = 0, B = 0;

        if (!fromHex(line->at((*parseFrmChar)++), &R)) return 0;
        R <<= 4;
        if (!fromHex(line->at((*parseFrmChar)++), &R)) return 0;

        if (!fromHex(line->at((*parseFrmChar)++), &G)) return 0;
        G <<= 4;
        if (!fromHex(line->at((*parseFrmChar)++), &G)) return 0;

        if (!fromHex(line->at((*parseFrmChar)++), &B)) return 0;
        B <<= 4;
        if (!fromHex(line->at((*parseFrmChar)++), &B)) return 0;

        *c = RGB(R, G, B);
      } else {
        // rollback the error...
        (*parseFrmChar)--;
        return 0;
      }
    } else {
      int index = 0;
      if (!fromHex(line->at((*parseFrmChar)++), &index))
        return 0;
      *c = indexColor[index];
    }
    return 1;
  }

  static const COLORREF indexColor[16];

  bool bBMPInvalid;
  void DrawBMPs(HDC hdc, int maxy);
  inline void DeleteBMPs() {
    std::list<HBITMAP *>::iterator pBMP;
    if (!hbRows.empty()) {
      for (pBMP = hbRows.begin(); pBMP != hbRows.end(); pBMP++)
          ::DeleteObject(*pBMP);
      hbRows.erase(hbRows.begin(), hbRows.end());
    };
  };
  std::list<DrawableItem *> vDrawables;

  // Stored in forward order, but displayed (drawn) last to
  // first from the bottom of the client area up.
  std::list<HBITMAP *> hbRows;
};

#endif  // #ifndef INC_Win32TextLine_hpp__

