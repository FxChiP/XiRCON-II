/*
 ------------------------------------------------------------------------------
 * StatBoxImps.cpp --
 *
 *   Xterminal specific implimentation class of the component.
 *
 * Copyright (c) 2002 Tomasoft Engineering
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: StatBoxImps.cpp,v 1.1 2003/11/14 10:36:21 davygrvy Exp $
 ------------------------------------------------------------------------------
 */
#include "StatBoxImps.hpp"

XterminalStatBoxImp::XterminalStatBoxImp (XRect __r)
    : XtWindow(__r, "status window under test")
{
}

XterminalStatBoxImp::~XterminalStatBoxImp ()
{
}

void
AllWin32StatBoxImp::Echo (wchar_t *text, size_t len)
{
    // pass it to the ViewBox!
}

void
AllWin32StatBoxImp::SetTitle (wchar_t *text, size_t len)
{
    SetTitle(text->c_str());
}
