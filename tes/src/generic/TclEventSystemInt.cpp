/*
 ------------------------------------------------------------------------------
 * TclEventSystem.cpp --
 *
 *   Generic implementation of the TES classes.
 *
 * Copyright (c) 2001 Tomahawk Software Group.
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: TclEventSystemInt.cpp,v 1.5 2003/12/23 21:38:02 davygrvy Exp $
 ------------------------------------------------------------------------------
 */
#include "TclEventSystemInt.hpp"

#ifdef __WIN32__
#   include "../win/winTclEventSystemInt.hpp"
#elif MAC_TCL
#   error "Mac not supported yet"
#else
#   error "<unknown> platform not supported yet"
#endif


TclEventSystem *TclEventSystem::m_instance = 0L;

const
TclEventSystem *TclEventSystem::Instance (
    const char *libToUse,
    Tcl_PanicProc *fatal)
{
    if (m_instance == 0L) {
	m_instance = new TclEventSystem(libToUse, fatal);
    }
    return m_instance;
}

const
TclEventSystem *TclEventSystem::Instance (void)
{
    if (m_instance != 0L) return m_instance;
    return 0L;
}

TclEventSystem::TclEventSystem (
    const char *libToUse,
    Tcl_PanicProc *fatal)
{
    privateImp = new TclEventSystemPlatInt(libToUse, fatal);
}


int
TclEventSystemInt::QueueJob(TclAsyncJob *aj)
{
    if (m_hAsync != 0L) {
	Q_Put(aj);
	Tcl_AsyncMark(m_hAsync);
	return TCL_OK;
    } else {
	delete const_cast<TclAsyncJob *>(aj);
	return TCL_ERROR;
    }
}


// This structure is what gets queued into Tcl's event loop
typedef struct
{
    Tcl_Event header;
    TclAsyncJob *aj;
} ASYNCEVENT, *LPASYNCEVENT;


int
TclEventSystemInt::MainAsyncProc (ClientData cData, Tcl_Interp *, int code)
{
    TclEventSystemPlatInt *esi = static_cast<TclEventSystemPlatInt *>(cData);
    TclAsyncJob *aj = 0L;
    LPASYNCEVENT ae;

    // Someone called QueueJob.  There must be at least one entry
    // on the WorkQueue, but pull them all off while we're here.  Don't
    // let a good context go to waste.
    while (esi->Q_Get(aj)) {

	// Run it's custom AsyncProc.
	// Returning true means done, so don't queue it.
	if (aj->AsyncProc()) {
	    delete aj;
	    continue;
	};

	ae = (LPASYNCEVENT) Tcl_Alloc (sizeof(ASYNCEVENT));
	ae->header.proc = MainEventProc;
	ae->aj = aj;

	// Queue it into Tcl's event loop.
	Tcl_QueueEvent((Tcl_Event *)ae, TCL_QUEUE_TAIL);
    };
    return code;
};


int
TclEventSystemInt::MainEventProc (Tcl_Event *evPtr, int flags)
{
    LPASYNCEVENT ae = (LPASYNCEVENT) evPtr;

    // we only handle file-type events here.
    if (!(flags & TCL_FILE_EVENTS)) return 0;

    ae->aj->EventProc();

    // delete the TclAsyncJob instance
    delete const_cast<TclAsyncJob *>(ae->aj);

    return 1;
};
