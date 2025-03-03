/*
 ------------------------------------------------------------------------------
 * CmdBoxImps.cpp --
 *
 *   Xterminal specific implimentation of the component class.
 *
 * Copyright (c) 2002 Tomasoft Engineering
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: CmdBoxImps.cpp,v 1.1 2003/11/14 10:36:21 davygrvy Exp $
 ------------------------------------------------------------------------------
 */

#include "CmdBoxImps.hpp"

XterminalCmdBoxImp::XterminalCmdBoxImp(XRect __r)
  : XtInputStringField(__r, "", IF_MAXSTRLEN, FIELD_NO_CLEAR|FIELD_NO_MARGIN)
{
    strcpy(ObjClass, "CmdBox");
}

XterminalCmdBoxImp::~XterminalCmdBoxImp()
{
}

bool
XterminalCmdBoxImp::ProcessKeyboardEvent
   (XKeyboardEvent *__Event)
{
    switch(__Event->KeyCode) {
    case KB_UP:
    {
	const std::wstring *hist;

	if (bHistSaveTemp) {
	    bHistoryTemp = false;
	    if (sHistoryTemp) delete sHistoryTemp;
	    sHistoryTemp = new std::wstring(GetValue());
	}
	hist = GetHist(up);
	if (hist) {
	    SetValue(hist->c_str());
	}
	return true;
    }
    case KB_DOWN:
    {
	const std::wstring *hist;

	// going down from modified text, i would assume, means you
	// don't want the buffer anymore.
	if (bHistSaveTemp && sHistoryTemp) {
	    delete sHistoryTemp;
	    sHistoryTemp = 0L;
	    bHistSaveTemp = false;
	}
	hist = GetHist(down);
	if (hist) {
	    SetValue(hist->c_str());
	}
	return true;
    }
    case KB_ENTER:
    {
	std::wstring *s;

	bHistSaveTemp = false;
	s = new std::wstring(GetValue());
	ClearValue();
	AddHist(s);
	// do an alias call here i guess (?)
	return true;
    }
    default:
	key = __Event->KeyCode;
	if (key >= 0x20 && key <= 0xff) bHistoryTemp = true;
    }
    return false;
}
