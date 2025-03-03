/*
 ------------------------------------------------------------------------------
 * StatBoxImps.hpp --
 *
 *   Platform specific implimentation class of the component.
 *
 * Copyright (c) 1999-2000 Tomahawk Software Group
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: StatBoxImps.hpp,v 1.1 2003/11/14 10:57:46 davygrvy Exp $
 ------------------------------------------------------------------------------
 */
#ifndef INC_StatBoxImps_hpp__
#define INC_StatBoxImps_hpp__

#include <CMcl.h>
#include <StatBoxCommonImp.hpp>
#include "CmdBoxImps.hpp"
#include "ViewBoxImps.hpp"

class AllWin32StatBoxThreadedGUI : public CMclThreadHandler
{
public:
    AllWin32StatBoxThreadedGUI();
    ~AllWin32StatBoxThreadedGUI();
    unsigned ThreadHandlerProc(void);
    void Echo(wchar_t *text, size_t len);
    HWND GetHwnd();
    AllWin32ViewBoxImp *pView;

private:
    friend LRESULT CALLBACK StatBoxWndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

    void OnSize   (HWND hwnd, UINT fwSizeType, int nWidth, int nHeight);
    void OnClose  (HWND hwnd);
    void OnDestroy(HWND hwnd);

    CMclQueue<std::wstring *> m_echoQ;
    HWND hwndStatBox;
    HBRUSH hBrush;
    AllWin32CmdBoxImp *pCmd;
};

class AllWin32StatBoxImp : public StatBoxCommonImp
{
public:
    AllWin32StatBoxImp();
    ~AllWin32StatBoxImp();
    virtual void Echo(wchar_t *text, size_t len);
    virtual void SetTitle(wchar_t *text, size_t len);
private:
    static bool first;
    CMclThreadAutoPtr pGUIThread;
    AllWin32StatBoxThreadedGUI *pGUI;
};

#endif  // #ifndef INC_StatBoxImps_hpp__

