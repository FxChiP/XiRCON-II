/*
 ------------------------------------------------------------------------------
 * CmdBoxImps.hpp --
 *
 *   Xterminal specific implimentation class of the component.
 *
 * Copyright (c) 2002 Tomasoft Engineering
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
#include <xterminal.h>

class XterminalCmdBoxImp
    : public CmdBoxCommonImp, XtInputStringField
{
public:
    XterminalCmdBoxImp(XRect __r);
    ~XterminalCmdBoxImp();

    // Process keyboard events
    virtual bool ProcessKeyboardEvent(XKeyboardEvent *__Event);
};

#endif  // #ifndef INC_CmdBoxImps_hpp__

