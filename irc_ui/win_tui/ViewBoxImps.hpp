/*
 ------------------------------------------------------------------------------
 * ViewBoxImps.hpp --
 *
 *   Platform specific implimentation class of the component.
 *
 * Copyright (c) 1999-2000 Tomahawk Software Group
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: ViewBoxImps.hpp,v 1.1 2003/11/14 10:36:21 davygrvy Exp $
 ------------------------------------------------------------------------------
 */
#ifndef INC_ViewBoxImps_hpp__
#define INC_ViewBoxImps_hpp__

#include <ViewBoxCommonImp.hpp>
#include <windows.h>
#include <CMcl.h>
#include <vector>
//#include "Win32TextLine.hpp"


class AllWin32ViewBoxImp : public ViewBoxCommonImp, CMclThreadHandler
{
public:
  AllWin32ViewBoxImp(HWND parent, CMclQueue<std::string *> &echoQ);
  ~AllWin32ViewBoxImp();
  void Echo(wchar_t *text, size_t len);
  void SetTitle(wchar_t *text, size_t len);
  HWND GetHwnd();
  unsigned ThreadHandlerProc(void);

private:
  static bool first;

  friend LRESULT CALLBACK 
    ViewBoxWndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

  void OnSize
    (HWND hwnd, UINT fwSizeType, int nWidth, int nHeight);
  void OnPaint
    (HWND hwnd);
  void OnVScroll 
    (HWND hwnd, HWND hwndScrollBar, int nScrollCode, short int nPos);
  HBRUSH OnCtlColor
    (HWND hwnd, HDC hdc, HWND hwndChild, int type);

  void UpdateScroll();

  CMclQueue<std::string *> &m_echoQ;
  CMclThreadAutoPtr echoQReaderThread;
  CMclEvent ceStop;
  CMclCritSec csLines;
  std::vector<Win32TextLine *> vLines;

  HWND hwndViewBox;  // our ViewBox window
  int cx, cy;        // dimensions of this ViewBox window

  // scrollbar stuff
  bool bSBLock;
  int iSBPos, iSBMax, iSBPage;
  int iSBBaseLine;  // When in scrollback, this is the baseline to start
                    // drawing from
  HBRUSH hSBAlert;  // paints the scrollbar this color when in scrollback
};

#endif  // #ifndef INC_ViewBoxImps_hpp
