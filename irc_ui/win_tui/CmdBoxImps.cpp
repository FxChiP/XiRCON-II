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
 * RCS: @(#) $Id: CmdBoxImps.cpp,v 1.1 2003/11/14 10:36:21 davygrvy Exp $
 ------------------------------------------------------------------------------
 */
#include "CmdBoxImps.hpp"
#include <string>

// declared in IRCWindowsSystemFactory.cpp
extern const HINSTANCE hAppInst;


AllWin32CmdBoxImp::AllWin32CmdBoxImp(HWND parent)
  : bHistSaveTemp(false)
{
  /*
  int rtn;

  if (first) {
    WNDCLASSEX wndcls;

    first = false;

    // superclass the edit control
    ::GetClassInfoEx(NULL, "edit", &wndcls);
    wndcls.hInstance = hAppInst;
    wndcls.lpszClassName = "CmdBox";
    fnBaseEdit = wndcls.lpfnWndProc;
    wndcls.lpfnWndProc = CmdBoxWndProc;
    rtn = (int) ::RegisterClassEx(&wndcls);

    if (rtn == 0) {
      DWORD err = GetLastError();
      char *lpMsgBuf;
      char buff[33];
      std::string msg("CmdBox couldn't be registered.  GetLastError reports: ");

      sprintf(buff,"%d",err);
      msg.append(buff);
      msg.append(" : ");

      ::FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL,
        err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
        (LPTSTR) &lpMsgBuf, 0, NULL );

      msg.append(lpMsgBuf);
      ::LocalFree(lpMsgBuf);

      ::MessageBox(NULL, msg.c_str(), "Fatal Error in viewbox.dll",
	        MB_ICONSTOP | MB_OK | MB_TASKMODAL | MB_SETFOREGROUND);

      ::ExitProcess(1);
    }
  }
*/
  hwndCmdBox =
      CreateWindowEx(
        WS_EX_CLIENTEDGE | WS_EX_NOPARENTNOTIFY,
        "edit",
        NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER |
        ES_LEFT | ES_AUTOHSCROLL | ES_NOHIDESEL,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        parent, NULL, hAppInst, NULL
      );

  ::SetWindowLongPtr(hwndCmdBox, GWLP_USERDATA,
      reinterpret_cast<LONG_PTR>(this));

  fnBaseEdit = 
      reinterpret_cast<WNDPROC>(
        ::SetWindowLongPtr(
          hwndCmdBox, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(CmdBoxWndProc)
        )
      );

  InitMetrics();
//  SetFont(hwndCmdBox);
};

AllWin32CmdBoxImp::~AllWin32CmdBoxImp() { };

HWND AllWin32CmdBoxImp::GetHwnd() { return hwndCmdBox; };
int AllWin32CmdBoxImp::Getcy() { return cy; };

bool AllWin32CmdBoxImp::first = true;
WNDPROC AllWin32CmdBoxImp::fnBaseEdit;

void AllWin32CmdBoxImp::InitMetrics () {
  TEXTMETRIC tm;
  HDC hdc = ::GetDC(hwndCmdBox);

//  InitFont(hdc);
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
#define ATTR_REV        '\026'
#define ATTR_UNDR       '\037'
#define ATTR_MIRC       '\003'
#define INPUT_BUFSIZE 512

  switch (iMsg) {
    /*
  case WM_CREATE:
    ::SetWindowLongPtr(hwnd, GWLP_USERDATA,
        reinterpret_cast<LONG_PTR>(
          (((LPCREATESTRUCT) lParam)->lpCreateParams)
        )
    );
    break;
    */
  case WM_CHAR:
    {
      char szChar [2] = {0, 0};
      pCmdBoxImp->bHistSaveTemp = true;
      switch (wParam) {
      case CONTROL('a'):
      case CONTROL('b'):
      case CONTROL('g'):
      case CONTROL('o'):
        *szChar = (char) (wParam);
        break;
      case CONTROL('r'):
        *szChar = ATTR_REV;
        break;
      case CONTROL('u'):
        *szChar = ATTR_UNDR;
        break;
      case CONTROL('k'):
        *szChar = ATTR_MIRC;
        break;
      case VK_RETURN:
        {
            int iLen = GetWindowTextLength(hwnd) + 1;
            char *szInput = new char [iLen];
            std::wstring *s;

            pCmdBoxImp->bHistSaveTemp = false;
            GetWindowText(hwnd, szInput, iLen);
            SetWindowText(hwnd, "");
            s = new std::wstring(szInput);
            pCmdBoxImp->AddHist(s);
            //pCmdBoxImp->Type(buf);
            // do an alias call here i guess (?)
            delete [] szInput;
            return 0;
        }
      }
      if (*szChar) {
        SendMessage (hwnd, EM_REPLACESEL, 0, (LPARAM) szChar);
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
          /*
        case VK_RETURN:
          {
            int iLen = GetWindowTextLength(hwnd) + 1;
            char *szInput = new char [iLen];
            std::string *s;

            GetWindowText(hwnd, szInput, iLen);
            SetWindowText(hwnd, "");
            s = new std::string(szInput);
            pCmdBoxImp->AddHist(s);
            //pCmdBoxImp->Type(buf);
            // do an alias call here i guess (?)
            // same as above.
            delete [] szInput;
            return 0;
          }
          */
        }
      }
      switch (wParam) {
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
            char *p = new char [len + 1];

            pCmdBoxImp->bHistSaveTemp = false;
            GetWindowText(hwnd, p, len + 1);
            if (pCmdBoxImp->sHistoryTemp) delete pCmdBoxImp->sHistoryTemp;
            pCmdBoxImp->sHistoryTemp = new std::wstring(p);
            delete [] p;
          }
          hist = pCmdBoxImp->GetHist(AllWin32CmdBoxImp::up);
          if (hist) SetWindowText(hwnd, hist->c_str());
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
          if (hist) SetWindowText(hwnd, hist->c_str());
          return 0;
        }
      }
    }
  }
  return CallWindowProc(pCmdBoxImp->fnBaseEdit, hwnd, iMsg, wParam, lParam);
}

