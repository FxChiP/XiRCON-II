/*
 ------------------------------------------------------------------------------
 * CmdBoxCommonImp.cpp --
 *
 *   Base class for the internal implimentation without any platform
 *   specifics.
 *
 * Copyright (c) 1999-2000 Tomahawk Software Group
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: CmdBoxCommonImp.cpp,v 1.1 2003/11/14 10:36:20 davygrvy Exp $
 ------------------------------------------------------------------------------
 */
#include <CmdBoxCommonImp.hpp>


CmdBoxCommonImp::CmdBoxCommonImp()
    : iMaxHistory(50), iHistPos(0), sHistoryTemp(0)
{
    empty = new std::u16string(u"");
}


CmdBoxCommonImp::~CmdBoxCommonImp()
{
    std::vector<std::u16string *>::iterator pHist;

    for (pHist = vHistory.begin(); pHist != vHistory.end(); pHist++) {
	delete *pHist;
    }
    delete empty;
}


void
CmdBoxCommonImp::AddHist(const std::u16string*line)
{
    vHistory.push_back(const_cast<std::u16string *>(line));
    if (vHistory.size() > iMaxHistory) {
	delete vHistory.front();
	vHistory.erase(vHistory.begin());
    }
    iHistPos = vHistory.size() ;
}


const std::u16string *
CmdBoxCommonImp::GetHist(direction d)
{
    size_t iHistSize = vHistory.size();
    switch(d) {
    case up:
	// we're already at the bottom, return nothing.
	if (iHistPos == 0) return 0;
	else iHistPos--;
	break;
    case down:
	// increment when less than the top.
	if (iHistPos < iHistSize) iHistPos++;
	// increment when our sHistoryTemp is valid and at the top.
	else if (sHistoryTemp && iHistPos == iHistSize) iHistPos++;
	// a weird case logic fix.
	else if (!sHistoryTemp && iHistPos > iHistSize) iHistPos--;
	// we're already at the top, return nothing.
	else return 0;
    }
    
    if (iHistPos == iHistSize + 1) {
	return empty;
    } else if (iHistPos == iHistSize) {
	// return the temporary if valid
	return (sHistoryTemp ? sHistoryTemp : empty);
    } else {
	return vHistory[iHistPos];
    }
}

