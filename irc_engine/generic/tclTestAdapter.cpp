/*
 ------------------------------------------------------------------------------
 * tclTestAdapter.cpp --
 *
 *   Tcl glue code for the test suite.
 *
 * Copyright (c) 1999-2002 Tomasoft Engineering.
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 ------------------------------------------------------------------------------
 * RCS: @(#) $Id: tclTestAdapter.cpp,v 1.3 2006/09/24 07:00:35 davygrvy Exp $
 ------------------------------------------------------------------------------
 */
#include <tcl.h>
#include <cpptcl.hpp>
#include "IRCParse.hpp"
#include "IRCSplitAndQ.hpp"
#include "IRCUtils.hpp"


class tclTestAdapter
    : private Tcl::Adapter<tclTestAdapter>, QCallbackBase
{
    Tcl_Encoding encodings[11];

    virtual void DoInterpDataCleanup() {
	for (int i = 0; i < 11; i++) {
	    Tcl_FreeEncoding(encodings[i]);
	}
    }

public:
    tclTestAdapter(Tcl_Interp *interp)
	: Tcl::Adapter<tclTestAdapter>(interp)
    {
	NewTclCmd("IRC::testparse", &tclTestAdapter::ParseCmd);
	NewTclCmd("IRC::testsplit", &tclTestAdapter::SplitCmd);
	NewTclCmd("IRC::testuniquote", &tclTestAdapter::UniQuoteCmd);
	NewTclCmd("IRC::testlayered", &tclTestAdapter::LayeredCmd);

	// we'll connect this to an external variable in the furure.
	encodings[0] = Tcl_GetEncoding(interp, "iso8859-1");

	encodings[1] = Tcl_GetEncoding(interp, "iso8859-1");
	encodings[2] = Tcl_GetEncoding(interp, "iso8859-2");
	encodings[3] = Tcl_GetEncoding(interp, "iso8859-3");
	encodings[4] = Tcl_GetEncoding(interp, "iso8859-4");
	encodings[5] = Tcl_GetEncoding(interp, "iso8859-5");
	encodings[6] = Tcl_GetEncoding(interp, "iso8859-6");
	encodings[7] = Tcl_GetEncoding(interp, "iso8859-7");
	encodings[8] = Tcl_GetEncoding(interp, "iso8859-8");
	encodings[9] = Tcl_GetEncoding(interp, "iso8859-9");
	encodings[10] = Tcl_GetEncoding(interp, "utf-8");

	Tcl_PkgProvide(interp, "IRC_Engine_TEST", "0.1");
    }

private:
    int ParseCmd (int objc, struct Tcl_Obj * const objv[])
    {
	Tcl_Obj *list[8];

	if (objc != 2) {
	    Tcl_WrongNumArgs(interp, 1, objv, "line");
	    return TCL_ERROR;
	}

	IRCParse line(objv[1]);

	if (line.Status() == TCL_ERROR) return TCL_ERROR;

	list[0] = line.GetFullAddress();
	list[1] = line.GetNick();
	list[2] = line.GetUser();
	list[3] = line.GetHost();
	list[4] = line.GetEvent();
	list[5] = line.GetArgs();
	list[6] = line.GetRawArgs();
	list[7] = line.GetRawLine();

	Tcl_SetObjResult(interp, Tcl_NewListObj(8, list));
	return TCL_OK;
    }

    int SplitCmd (int objc, struct Tcl_Obj * const objv[])
    {
	if (objc != 2) {
	    Tcl_WrongNumArgs(interp, 1, objv, "line");
	    return TCL_ERROR;
	}
	IRCParse line(objv[1]);
	IRCSplitAndQ(line, this, encodings, 0L);
	return TCL_OK;
    }

    virtual void QCallback (const IRCParse *line)
    {
	Tcl_Obj *result = Tcl_GetObjResult(interp);
	Tcl_Obj *list[8];

	list[0] = line->GetFullAddress();
	list[1] = line->GetNick();
	list[2] = line->GetUser();
	list[3] = line->GetHost();
	list[4] = line->GetEvent();
	list[5] = line->GetArgs();
	list[6] = line->GetRawArgs();
	list[7] = line->GetRawLine();

	Tcl_ListObjAppendElement(0L, result, Tcl_NewListObj(8, list));
	delete line;
    }
    int UniQuoteCmd (int objc, struct Tcl_Obj * const objv[])
    {
	Tcl_Obj *data;

	if (objc != 2) {
	    Tcl_WrongNumArgs(interp, 1, objv, "line");
	    return TCL_ERROR;
	}

	// We need a Tcl_Obj that isn't shared.
	if (Tcl_IsShared(objv[1])) {
	    data = Tcl_DuplicateObj(objv[1]);
	} else {
	    data = objv[1];
	}

	CTCP2UniQuote(&data, encodings[10]);
	Tcl_SetObjResult(interp, data);
	return TCL_OK;
    }

    int LayeredCmd (int objc, struct Tcl_Obj * const objv[])
    {
	Tcl_DString ds;
	const char *s;
	int len;
	Tcl_Encoding enc;

	if (objc != 3) {
	    Tcl_WrongNumArgs(interp, 1, objv, "encoding text");
	    return TCL_ERROR;
	}

	enc = Tcl_GetEncoding(interp, Tcl_GetString(objv[1]));
	if (enc == 0L) {
	    Tcl_SetObjResult(interp,
		Tcl_NewStringObj("specified encoding could not be loaded",-1));
	    return TCL_ERROR;
	}

	s = Tcl_GetStringFromObj(objv[2], &len);
	IRC_UtfToLayeredExternalDString(enc, s, len, &ds);
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj(Tcl_DStringValue(&ds), Tcl_DStringLength(&ds)));
	Tcl_FreeEncoding(enc);
	Tcl_DStringFree(&ds);
	return TCL_OK;
    }
};

#undef TCL_STORAGE_CLASS
#define TCL_STORAGE_CLASS DLLEXPORT

EXTERN int
Test_Init (Tcl_Interp *interp)
{
#ifdef USE_TCL_STUBS
    if (Tcl_InitStubs(interp, "8.3", 0) == 0L) {
	return TCL_ERROR;
    }
#endif
    new tclTestAdapter(interp);
    return TCL_OK;
}
