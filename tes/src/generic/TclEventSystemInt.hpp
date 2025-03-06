/*
 ------------------------------------------------------------------------------
 * TclEventSystem.cpp --
 *
 *   Private (internal) interface for TES.
 *
 * Copyright (c) 2001 Tomahawk Software Group.  http://www.tomasoft.org
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: TclEventSystemInt.hpp,v 1.4 2003/12/23 21:38:02 davygrvy Exp $
 ------------------------------------------------------------------------------
 */
#include "..\..\include\TclEventSystem.hpp"

#ifndef INC_TclEventSystemInt_hpp__
#define INC_TclEventSystemInt_hpp__

#if defined(_WIN32) && defined(_UNICODE)
#include <wchar.h>
#endif

//#include "TclEventSystem.hpp"

// Be very restrictive about the minimum acceptable version of Tcl.
// Compiling requires 8.1.0+, but running requires 8.4a2+ for the needed
// bug fixes with Tcl_AsyncMark() and friends.
#if TCL_MAJOR_VERSION != 8  || \
    (TCL_MAJOR_VERSION == 8 && \
	(TCL_MINOR_VERSION == 0 || \
	    (TCL_MINOR_VERSION == 1 && TCL_RELEASE_LEVEL != TCL_FINAL_RELEASE)))
#   error "You need the Tcl 8.1.0 headers (or above) from Scriptics for this"
#endif

extern char *findTcl (const char *minVer, int exact, int dbgOnly);

class TclEventSystemInt : public TclEventSystemIntBase
{
public:
    int QueueJob (const TclAsyncJob *ai);
    void ShutDown (void);
    virtual int  Q_Get (const TclAsyncJob *&aj) = 0;
    virtual void Q_Put (const TclAsyncJob *aj) = 0;

    static int MainAsyncProc (ClientData cData, Tcl_Interp *interp, int code);
    static int MainEventProc (Tcl_Event *evPtr, int flags);

    Tcl_AsyncHandler m_hAsync;
};

#endif  /* INC_TclEventSystemInt_hpp__ */