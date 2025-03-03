/*
 ------------------------------------------------------------------------------
 * ChanBoxImps.cpp --
 *
 *   Xterminal specific implimentation of the component class.
 *
 * Copyright (c) 2002 Tomasoft Engineering
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: ChanBoxImps.cpp,v 1.1 2003/11/14 10:36:21 davygrvy Exp $
 ------------------------------------------------------------------------------
 */
#include "ChanBoxImps.hpp"

XterminalChanBoxImp::XterminalChanBoxImp (XRect __r)
    : XtWindow(__r, "channel window under test")
{
}

XterminalChanBoxImp::~XterminalChanBoxImp ()
{
}

void
XterminalChanBoxImp::Echo(wchar_t *text, size_t len)
{
}

void
XterminalChanBoxImp::SetTitle(wchar_t *text, size_t len)
{
    XtWindow::SetTitle(text->c_str());
}
