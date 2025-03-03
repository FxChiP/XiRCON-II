/*
 ------------------------------------------------------------------------------
 * StatBoxImps.hpp --
 *
 *   Xterminal specific implimentation of the component class.
 *
 * Copyright (c) 2002 Tomasoft Engineering
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: StatBoxImps.hpp,v 1.1 2003/11/14 10:36:21 davygrvy Exp $
 ------------------------------------------------------------------------------
 */
#ifndef INC_StatBoxImps_hpp__
#define INC_StatBoxImps_hpp__

#include "StatBoxCommonImp.hpp"
#include "CmdBoxImps.hpp"
#include "ViewBoxImps.hpp"

class XterminalStatBoxImp
    : public StatBoxCommonImp, private XtWindow
{
public:
    XterminalStatBoxImp(XRect __r);
    ~XterminalStatBoxImp();
    virtual void Echo(wchar_t *text, size_t len);
    virtual void SetTitle(wchar_t *text, size_t len);
private:
    XterminalViewBox *vb;
    XterminalCmdBox *cb;
};

#endif  // #ifndef INC_StatBoxImps_hpp__

