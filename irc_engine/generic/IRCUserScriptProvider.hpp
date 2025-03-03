/*-----------------------------------------------------------------------
 *  IRCUserScriptProvider.hpp --
 *
 *  General abstraction for how scripting hosts are hooked into us.
 *
 *  Because Tcl steps on perl defines and python is stepping on perl
 *  typedefs ..etc.. , we use a bridge pattern and decouple it here to
 *  avoid all the namespace pollution of having to bring in all the
 *  headers into the same place.
 *
 *-----------------------------------------------------------------------
 * RCS: @(#) $Id: IRCUserScriptProvider.hpp,v 1.21 2004/12/12 19:34:03 davygrvy Exp $
 *-----------------------------------------------------------------------
 */

#ifndef INC_IRCUserScriptProvider_hpp__
#define INC_IRCUserScriptProvider_hpp__

#include "cpptcl.hpp"
#include "IRCParse.hpp"
#include "IRCEvents.hpp"

class IRCUserScriptProvider
{
protected:
    int status;
    const char *traceBack;

public:
    IRCUserScriptProvider() : status(TCL_ERROR), traceBack(0L){}
    virtual ~IRCUserScriptProvider() {};

    enum CompletionStatus {NOTCOMPLETED, COMPLETED, COMPLETED_WITHERROR};
    virtual CompletionStatus IncomingEvent (const IRCParse *line) = 0;
    inline int Status() { return status; }
    inline const char *GetTraceBack() { return traceBack; }
};

/*
 *---------------------------------------
 * Nub, a bypass.
 *---------------------------------------
 */
class IRCUserScriptProviderNub
    : public IRCUserScriptProvider
{
    virtual CompletionStatus IncomingEvent (const IRCParse *line);
};

extern IRCUserScriptProviderNub Nub;

/*
 *---------------------------------------
 *  Tcl Stuff.  Direct, no bridge used.
 *---------------------------------------
 */

class IRCUserScriptProviderForTcl
    : public IRCUserScriptProvider,
    Tcl::Adapter<IRCUserScriptProviderForTcl>
{
    // This is what we store in the event hash table.
    typedef IRCEvents<Tcl_Obj *> OurEvents;

    Tcl_Obj *FullAddress;
    Tcl_Obj *Nick;
    Tcl_Obj *User;
    Tcl_Obj *Host;
    Tcl_Obj *Event;
    Tcl_Obj *Args;
    Tcl_Obj *RawArgs;
    Tcl_Obj *RawLine;
    CompletionStatus Completed;
    OurEvents Events;
    OurEvents Aliases;

    int OnCmd		   (int objc, struct Tcl_Obj * const objv[]);
    int AliasCmd	   (int objc, struct Tcl_Obj * const objv[]);
    int FullAddrCmd	   (int objc, struct Tcl_Obj * const objv[]);
    int NickCmd		   (int objc, struct Tcl_Obj * const objv[]);
    int UserCmd		   (int objc, struct Tcl_Obj * const objv[]);
    int HostCmd		   (int objc, struct Tcl_Obj * const objv[]);
    int EventCmd	   (int objc, struct Tcl_Obj * const objv[]);
    int ArgsCmd		   (int objc, struct Tcl_Obj * const objv[]);
    int RawArgsCmd	   (int objc, struct Tcl_Obj * const objv[]);
    int RawLineCmd	   (int objc, struct Tcl_Obj * const objv[]);
    int CompleteCmd	   (int objc, struct Tcl_Obj * const objv[]);

public:
    IRCUserScriptProviderForTcl(
	    const char *scriptFileName,
	    const char *instanceName,
	    Tcl_Interp *globalInterp,
	    int useSafe = 0);

    virtual ~IRCUserScriptProviderForTcl();
    virtual CompletionStatus IncomingEvent (const IRCParse *line);
};


/*
 *---------------------------------------
 *  Dynamic library.
 *---------------------------------------
 */

class IRCUserScriptProviderForDLL
    : public IRCUserScriptProvider
{
    // This is what we store in the event hash table.
    typedef IRCEvents<void *> OurEvents;

    Tcl_Obj *FullAddress;
    Tcl_Obj *Nick;
    Tcl_Obj *User;
    Tcl_Obj *Host;
    Tcl_Obj *Event;
    Tcl_Obj *Args;
    Tcl_Obj *RawArgs;
    Tcl_Obj *RawLine;
    CompletionStatus Completed;
    OurEvents Events;

public:
    IRCUserScriptProviderForDLL(
	    const char *dllFileName,
	    const char *instanceName,
	    Tcl_Interp *globalInterp);

    virtual ~IRCUserScriptProviderForDLL();
    virtual CompletionStatus IncomingEvent (const IRCParse *line);
};


/*
 *-----------------------------------------------
 * Virtual base class for the private (hidden)
 * side of our bridge to the other script engines.
 *-----------------------------------------------
 */
class IRCUserScriptProviderPrvImpBase
{
public:
    virtual ~IRCUserScriptProviderPrvImpBase() {};
    virtual IRCUserScriptProvider::CompletionStatus IncomingEvent(const IRCParse *line) = 0;
};

/*
 *--------------------------------
 *  Python Stuff
 *--------------------------------
 */

#if IRCE_USEPYTHON

class IRCUserScriptProviderForPython
    : public IRCUserScriptProvider
{
    IRCUserScriptProviderPrvImpBase *_imp;

public:
    IRCUserScriptProviderForPython(
	    const char *scriptFileName,
	    const char *instanceName,
	    Tcl_Interp *globalInterp,
	    int useSafe = 0);

    virtual ~IRCUserScriptProviderForPython()
    {
	delete _imp;
    }

    virtual CompletionStatus IncomingEvent(const IRCParse *line)
    {
	return _imp->IncomingEvent(line);
    }
};

#endif	// IRCE_USEPYTHON


/*
 *--------------------------------
 *  Perl Stuff
 *--------------------------------
 */

#if IRCE_USEPERL

class IRCUserScriptProviderForPerl
    : public IRCUserScriptProvider
{
    IRCUserScriptProviderPrvImpBase *_imp;

public:
    IRCUserScriptProviderForPerl(
	    const char *scriptFileName,
	    const char *instanceName,
	    Tcl_Interp *globalInterp,
	    int useSafe = 0);

    virtual ~IRCUserScriptProviderForPerl()
    {
	delete _imp;
    }

    virtual CompletionStatus IncomingEvent(const IRCParse *line)
    {
	return _imp->IncomingEvent(line);
    }
};

#endif	// IRCE_USEPERL


/*
 *--------------------------------
 *  Java Stuff
 *--------------------------------
 */

#if IRCE_USEJAVA

class IRCUserScriptProviderForJava
    : public IRCUserScriptProvider
{
    IRCUserScriptProviderPrvImpBase *_imp;

public:
    IRCUserScriptProviderForJava(
	    const char *scriptFileName,
	    const char *instanceName,
	    Tcl_Interp *globalInterp,
	    int useSafe = 0);

    virtual ~IRCUserScriptProviderForJava()
    {
	delete _imp;
    }

    virtual CompletionStatus IncomingEvent(const IRCParse *line)
    {
	return _imp->IncomingEvent(line);
    }
};

#endif	// IRCE_USEJAVA

#endif	// INC_IRCUserScriptProvider_hpp__
