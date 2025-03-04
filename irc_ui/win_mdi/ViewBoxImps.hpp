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


class AllWin32ViewBoxImp : public ViewBoxCommonImp, CMclThreadHandler
{
public:
    AllWin32ViewBoxImp(HWND parent, CMclQueue<std::u16string *> &echoQ);
    ~AllWin32ViewBoxImp();
    HWND GetHwnd();
    unsigned ThreadHandlerProc(void);

private:
    CMclQueue<std::u16string *> &m_echoQ;
    CMclThreadAutoPtr echoQReaderThread;
    CMclEvent ceStop;

    HWND hwndViewBox;  // our ViewBox window
    HFONT font;
    HMODULE reDll;
};

#endif  // #ifndef INC_ViewBoxImps_hpp
