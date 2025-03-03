/*
 ------------------------------------------------------------------------------
 * NickListBoxImps.cpp --
 *
 *   Platform specific implimentation class of the component.
 *
 * Copyright (c) 2002 Tomasoft Engineering
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: NickListBoxImps.cpp,v 1.1 2003/11/14 10:36:21 davygrvy Exp $
 ------------------------------------------------------------------------------
 */
#include "NickListBoxImps.hpp"


XterminalNickListBoxImp::XterminalNickListBoxImp(XRect __r)
    : XtList(__r, LIST_V_SCROLLBAR)
{
}