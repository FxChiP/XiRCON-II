/*
 * -----------------------------------------------------------------------
 * TclConsoleRedirect.cpp --
 *
 *   Base class for managing std channel redirection to a UI component
 *   and logic for evaluating input.
 *
 * Copyright (c) 2003 Tomasoft Engineering.
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: TclConsoleRedirect.cpp,v 1.2 2003/12/09 22:19:01 davygrvy Exp $
 * -----------------------------------------------------------------------
 */
#include "TclConsoleRedirect.hpp"
#include <errno.h>
#include <string>

static Tcl_ExitProc ConsoleRedirectExit;

static Tcl_DriverCloseProc Tcl::XConsoleClose;
static Tcl_DriverInputProc Tcl::XConsoleInput;
static Tcl_DriverOutputProc Tcl::XConsoleOutput;
//static Tcl_DriverSetOptionProc XConsoleSetOpt;
//static Tcl_DriverGetOptionProc XConsoleGetOpt;
//static Tcl_DriverWatchProc XConsoleWatch;
//static Tcl_DriverGetHandleProc XConsoleHandle;

typedef struct {
    Tcl::ConsoleRedirect *con;
    int where;
} TclConState;

/*
 * This structure describes the channel type structure for file based I/O:
 */

static Tcl_ChannelType xconsoleChannelType = {
    "xconsole",		/* Type name. */
    NULL,		/* Always non-blocking.*/
    Tcl::XConsoleClose,	/* Close proc. */
    Tcl::XConsoleInput,	/* Input proc. */
    Tcl::XConsoleOutput,/* Output proc. */
    NULL,		/* Seek proc. */
    NULL,		/* Set option proc. */
    NULL,		/* Get option proc. */
    NULL,		/* Watch for events on console. */
    NULL,		/* Get a handle from the device. */
    NULL		/* Close2Proc */
};

Tcl::ConsoleRedirect::ConsoleRedirect (
    Tcl_Interp *_interp,
    int needExit,
    ConsoleRedirectColorScheme _scheme
    ) :
    interp(_interp),
    commandPtr(0L),
    scheme(_scheme)
{
    TclConState *state;

    state = (TclConState *) ckalloc(sizeof(TclConState));
    state->con = this;
    state->where = TCL_STDOUT;
    StdOut = Tcl_CreateChannel(&xconsoleChannelType, "xconsole1",
	    state, TCL_WRITABLE);
    if (StdOut != NULL) {
	Tcl_SetChannelOption(NULL, StdOut,
		"-blocking", "off");
	Tcl_SetChannelOption(NULL, StdOut,
		"-translation", "lf");
	Tcl_SetChannelOption(NULL, StdOut,
		"-buffering", "line");
	Tcl_SetChannelOption(NULL, StdOut,
		"-encoding", "unicode");
    }
    Tcl_SetStdChannel(StdOut, state->where);

    state = (TclConState *) ckalloc(sizeof(TclConState));
    state->con = this;
    state->where = TCL_STDERR;
    StdErr = Tcl_CreateChannel(&xconsoleChannelType, "xconsole2",
	    state, TCL_WRITABLE);
    if (StdErr != NULL) {
	Tcl_SetChannelOption(NULL, StdErr,
		"-blocking", "off");
	Tcl_SetChannelOption(NULL, StdErr,
		"-translation", "lf");
	Tcl_SetChannelOption(NULL, StdErr,
		"-buffering", "line");
	Tcl_SetChannelOption(NULL, StdErr,
		"-encoding", "unicode");
    }
    Tcl_SetStdChannel(StdErr, state->where);

    if (needExit) {
	Tcl_CreateExitHandler(ConsoleRedirectExit, this);
    }
}

Tcl::ConsoleRedirect::~ConsoleRedirect()
{
    Tcl_DeleteExitHandler(ConsoleRedirectExit, this);
    if (StdOut) Tcl_Close(NULL, StdOut);
    if (StdErr) Tcl_Close(NULL, StdErr);
}

void
Tcl::ConsoleRedirect::OneLineForEval (wchar_t *line, size_t len)
{
    // Must be Tcl's thread!

    if (commandPtr == 0L) {
	commandPtr = Tcl_NewUnicodeObj(line, len);
	Tcl_IncrRefCount(commandPtr);
    } else {
	if (Tcl_IsShared(commandPtr)) {
	    Tcl_DecrRefCount(commandPtr);
	    commandPtr = Tcl_DuplicateObj(commandPtr);
	    Tcl_IncrRefCount(commandPtr);
	}
	Tcl_AppendUnicodeToObj(commandPtr, line, len);
    }

    Tcl_AppendUnicodeToObj(commandPtr, L"\n", 1);

    std::wstring s(scheme.defaultStr);
    s.append(line, len);

    if (!Tcl_CommandComplete(Tcl_GetString(commandPtr))) {
	s.append(scheme.continueStr);
	PushLineToDisplay(s.c_str(), s.length());
    } else {
	int result;
	PushLineToDisplay(s.c_str(), s.length());
	result = Tcl_RecordAndEvalObj(interp, commandPtr, TCL_EVAL_GLOBAL);
	Tcl_DecrRefCount(commandPtr);
	commandPtr = 0L;
	if (result == TCL_ERROR) {
	    s.assign(scheme.errStr);
	} else {
	    s.assign(scheme.resultStr);
	}
	s.append(Tcl_GetUnicode(Tcl_GetObjResult(interp)));
	PushLineToDisplay(s.c_str(), s.length());
    }
}

const Tcl::ConsoleRedirectColorScheme Tcl::ConsoleRedirect::defScheme =
{
    // perfect for me with my IRC client.  Not perfect for anyone else.
    L"\006C6\006",
    L"\006C5\006 >",
    L"\6XT11\6",
    L"\6XT12\6",
    L"",
    L"\006C4\006"
};

void
ConsoleRedirectExit (ClientData clientData)
{
    delete reinterpret_cast <Tcl::ConsoleRedirect *> (clientData);
}

/*
 *----------------------------------------------------------------------
 *
 * XConsoleClose --
 *
 *	Closes the IO channel.
 *
 * Results:
 *	Always returns 0 (success).
 *
 * Side effects:
 *	No Implimentation for this.
 *
 *----------------------------------------------------------------------
 */
static int
Tcl::XConsoleClose (
    ClientData instanceData,
    Tcl_Interp *interp)
{
    TclConState *state = (TclConState *) instanceData;
    if (state->where == TCL_STDOUT) {
	state->con->StdOut = NULL;
    } else {
	state->con->StdErr = NULL;
    }
    ckfree((char *)instanceData);
    return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * XConsoleInput --
 *
 *	Read input from the display?  Not implemented.
 *
 * Results:
 *	Always returns EOF.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */
static int
Tcl::XConsoleInput (
    ClientData instanceData,
    char *buf,
    int bufSize,
    int *errorCode)
{
    *errorCode = EFAULT;
    return 0;	/* Always return EOF. */
}

/*
 * -----------------------------------------------------------------------
 *
 * XConsoleOutput --
 *
 *	Writes the given output to the display.
 *
 * Results:
 *	A count of how many characters were written is returned and an
 *	error indication is returned in an output argument.
 *
 * Side effects:
 *	
 * -----------------------------------------------------------------------
 */
static int
Tcl::XConsoleOutput (
    ClientData instanceData,	    /* Indicates which device to use. */
    CONST char *buf,		    /* The data buffer. */
    int toWrite,		    /* How many bytes to write? */
    int *errorCode)		    /* Where to store error code. */
{
    *errorCode = 0;
    std::wstring s;
    TclConState *state = (TclConState *) instanceData;

    if (state->where == TCL_STDOUT) {
	s.append(state->con->scheme.stdoutStr);
    } else {
	s.append(state->con->scheme.stderrStr);
    }
    s.append((wchar_t *) buf, toWrite / sizeof(wchar_t));
    state->con->PushLineToDisplay(s.c_str(), s.length());
    return toWrite;
}
