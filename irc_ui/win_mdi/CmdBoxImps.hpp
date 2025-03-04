/*
 ------------------------------------------------------------------------------
 * CmdBoxImps.hpp --
 *
 *   Platform specific implimentation class of the component.
 *
 * Copyright (c) 1999-2000 Tomahawk Software Group
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: CmdBoxImps.hpp,v 1.1 2003/11/14 10:36:21 davygrvy Exp $
 ------------------------------------------------------------------------------
 */
#ifndef INC_CmdBoxImps_hpp__
#define INC_CmdBoxImps_hpp__

#include <CmdBoxCommonImp.hpp>
#include <windows.h>

class AllWin32CmdBoxImp
    : public CmdBoxCommonImp
{
public:
    AllWin32CmdBoxImp(HWND parent);
    ~AllWin32CmdBoxImp();
    HWND GetHwnd();
    int Getcy();
private:
    friend LRESULT CALLBACK
	CmdBoxWndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
    void InitMetrics();
    HWND hwndCmdBox;
    static WNDPROC fnBaseEdit;
    static bool first;
    int cy;
    bool bHistSaveTemp;
};

#endif  // #ifndef INC_CmdBoxImps_hpp__

