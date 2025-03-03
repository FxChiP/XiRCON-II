/*
 ------------------------------------------------------------------------------
 * ChanBoxImps.hpp --
 *
 *   Xterminal specific implimentation of the component class.
 *
 * Copyright (c) 2002 Tomasoft Engineering.
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: ChanBoxImps.hpp,v 1.1 2003/11/14 10:36:21 davygrvy Exp $
 ------------------------------------------------------------------------------
 */
#ifndef INC_ChanBoxImps_hpp__
#define INC_ChanBoxImps_hpp__

#include <xterminal.h>
#include "ChanBoxCommonImp.hpp"
#include "ViewBoxImp.hpp"
#include "CmdBoxImp.hpp"
#include "NickListBoxImp.hpp"

class XterminalChanBoxImp
    : public ChanBoxCommonImp, private XtWindow
{
public:
    XterminalChanBoxImp (XRect __r);
    ~XterminalChanBoxImp ();
    virtual void Echo (wchar_t *text, size_t len);
    virtual void SetTitle (wchar_t *text, size_t len);
private:
    XterminalViewBoxImp *vb;
    XterminalCmdBoxImp *cb;
    XterminalNickListBoxImp *nlb;
};

#endif  // #ifndef INC_ChanBoxImps_hpp__

