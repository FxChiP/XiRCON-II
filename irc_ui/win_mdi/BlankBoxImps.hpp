/*
 ------------------------------------------------------------------------------
 * BlankBoxImps.hpp --
 *
 *   Platform specific implimentation class of the component
 *
 * Copyright (c) 1999-2000 Tomahawk Software Group
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: BlankBoxImps.hpp,v 1.4 2003/11/23 21:46:55 davygrvy Exp $
 ------------------------------------------------------------------------------
 */
#ifndef INC_BlankBoxImps_hpp__
#define INC_BlankBoxImps_hpp__

#include <BlankBoxCommonImp.hpp>
#include <windows.h>

class AllWin32BlankBoxImp : public BlankBoxCommonImp {
public:
    AllWin32BlankBoxImp();
    ~AllWin32BlankBoxImp();
    void *GetWindowId();
private:
    friend LRESULT CALLBACK BlankBoxWndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

    static bool first;
    HWND hwndBlankBox;
    HWND hContainee;
    int UserChangingSize, ExitSizeMove;
};

#endif  // #ifndef INC_BlankBoxImps_hpp__
