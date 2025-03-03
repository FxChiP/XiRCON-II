/*
 ------------------------------------------------------------------------------
 * ViewBoxImps.hpp --
 *
 *   Xterminal specific implimentation class of the component.
 *
 * Copyright (c) 2002 Tomasoft Engineering
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
#include <xterminal.h>

class XterminalViewBoxImp
    : public ViewBoxCommonImp, private XtWindow
{
public:
    XterminalViewBoxImp();
    ~XterminalViewBoxImp();
    void PushLine(std::wstring *line);
private:
    XtScrollBar *sb;
};

#endif  // #ifndef INC_ViewBoxImps_hpp
