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
 * RCS: @(#) $Id: StatBoxImps.hpp,v 1.2 2003/11/23 21:46:55 davygrvy Exp $
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
    void Echo(char16_t *text, size_t len);
    HWND GetHwnd();
    AllWin32ViewBoxImp *pView;

private:
    friend LRESULT CALLBACK StatBoxWndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

    CMclQueue<std::u16string *> m_echoQ;
    HWND hwndStatBox;
    HBRUSH hBrush;
    AllWin32CmdBoxImp *pCmd;
};

class AllWin32StatBoxImp : public StatBoxCommonImp
{
public:
    AllWin32StatBoxImp();
    ~AllWin32StatBoxImp();
    virtual void Echo(char16_t *text, size_t len);
    virtual void SetTitle(char16_t *text, size_t len);
private:
    static bool first;
    CMclThreadAutoPtr pGUIThread;
    AllWin32StatBoxThreadedGUI *pGUI;
};

#endif  // #ifndef INC_StatBoxImps_hpp__

