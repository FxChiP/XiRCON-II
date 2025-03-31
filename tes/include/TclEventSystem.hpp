/*
 ------------------------------------------------------------------------------
 * TclEventSystem.cpp --
 *
 *   Public interface for TES.
 *
 * Copyright (c) 2001-2003 Tomasoft Engineering.
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: TclEventSystem.hpp,v 1.8 2003/12/23 22:25:05 davygrvy Exp $
 ------------------------------------------------------------------------------
 */

#ifndef INC_TclEventSystem_hpp__
#define INC_TclEventSystem_hpp__

#ifndef USE_TCL_STUBS
#   error "This library was designed to be used with Stubs linkage only"
#endif

#include <tcl.h>


// base class for all work to be handed over to Tcl.
class TclAsyncJob
{
public:
    // Returning true means an event does not need to be queue'd
    // and the only use was to yield Tcl for just the AsyncProc.
    // NOTE: Only Tcl_* allocation functions are allowed to be
    // called from this function (ie. pretty much anything that
    // doesn't use an interp pointer).
    virtual bool AsyncProc (void) = 0;

    // Call any Tcl function in here.
    virtual void EventProc (void) = 0;
};



// Used internally as a bridge pattern to hide our boring internals
// from the outside.
class TclEventSystemIntBase
{
public:
    virtual int QueueJob (TclAsyncJob *ai) = 0;
    virtual void ShutDown(void) = 0;
};


// Singleton design pattern to ensure there's only one instance
// for the lifetime of the application.
class TclEventSystem
{
public:
    // Creates the instance on the first call.
    static const TclEventSystem *Instance (
	const char *libToUse,		//  Tcl library to load.
	Tcl_PanicProc* fatal);		//  fatal message proc	

    // Used to grab the one and only instance of the class.
    static const TclEventSystem *Instance (void);

    // Send a Job "packet" to Tcl.
    int QueueJob (TclAsyncJob *aj) const
    {
	return privateImp->QueueJob(aj);
    }

    // Unwinds the notifier, event loop, and cleans up Tcl.
    void ShutDown(void) const
    {
	privateImp->ShutDown();
    }

    ~TclEventSystem ()
    {
	delete privateImp;
	m_instance = 0L;
    };

protected:
    TclEventSystem(
	const char* libToUse,
	Tcl_PanicProc* fatal
    );
private:
    static TclEventSystem *m_instance;
    TclEventSystemIntBase *privateImp;
};

#endif  /* INC_TclEventSystem_hpp__ */