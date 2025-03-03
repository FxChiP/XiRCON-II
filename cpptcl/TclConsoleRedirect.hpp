/*
 ------------------------------------------------------------------------------
 * TclConsoleRedirect.hpp --
 *
 *   Base class for managing std channel redirection to a UI component
 *   and logic for evaluating input.
 *
 * Copyright (c) 2003 Tomasoft Engineering.
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: TclConsoleRedirect.hpp,v 1.3 2003/12/22 03:39:03 davygrvy Exp $
 ------------------------------------------------------------------------------
 */
#ifndef INC_TclConsoleRedirect_hpp__
#define INC_TclConsoleRedirect_hpp__

#include "tcl.h"

namespace Tcl {

    struct ConsoleRedirectColorScheme
    {
	wchar_t *defaultStr;
	wchar_t *continueStr;
	wchar_t *resultStr;
	wchar_t *errStr;
	wchar_t *stdoutStr;
	wchar_t *stderrStr;
    };

    class ConsoleRedirect
    {
	friend Tcl_DriverOutputProc XConsoleOutput;
	friend Tcl_DriverInputProc XConsoleInput;
	friend Tcl_DriverCloseProc XConsoleClose;
    public:

	ConsoleRedirect (
	    Tcl_Interp *_interp,    // interp used for Tcl_Eval().
	    int needExit = 0,	    // needExit must be set if operating
				    // from an extension that will be
				    // unloaded prior to TclFinalizeIOSubsystem()
				    // or else this will crash.  Not likely
				    // to happen as this class is intended
				    // for an application that hosts Tcl.
	    ConsoleRedirectColorScheme _scheme = defScheme
				    // Color codes for the display.
	    );

	~ConsoleRedirect();

	// When XConsoleOutput in the driver is called with data, we call
	// this from the base class to get the data out to the display.
	//
	virtual void PushLineToDisplay (const wchar_t *line, size_t len) = 0;

	// Incoming text to be evaluated (and displayed).
	//
	void OneLineForEval (wchar_t *line, size_t len);

    protected:
	Tcl_Interp *interp;

    private:
	Tcl_Obj *commandPtr;
	Tcl_Channel StdOut;
	Tcl_Channel StdErr;
	static const ConsoleRedirectColorScheme defScheme;
	ConsoleRedirectColorScheme &scheme;
    };

}  // namespace Tcl


#endif // #ifndef INC_TclConsoleRedirect_hpp__