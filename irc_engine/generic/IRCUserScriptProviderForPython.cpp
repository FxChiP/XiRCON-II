/*-----------------------------------------------------------------------
 *  IRCUserScriptProviderForPython.cpp --
 *
 *  Makes the jump to Python.  Both Tcl and Python calls are used here.
 *
 *-----------------------------------------------------------------------
 * RCS: @(#) $Id: IRCUserScriptProviderForPython.cpp,v 1.8 2002/08/06 18:05:45 davygrvy Exp $
 *-----------------------------------------------------------------------
 */

#include "IRCUserScriptProvider.hpp"

#if IRCE_USEPYTHON

#include "PythonCore.hpp"

class IRCUserScriptProviderForPythonPrivateImp
    : public IRCUserScriptProviderPrvImpBase
{
    static Tcl_ObjType *tclBooleanTypePtr;
    static Tcl_ObjType *tclDoubleTypePtr;
    static Tcl_ObjType *tclIntTypePtr;

    static Tcl_Obj *
    TranslateObj (PyObject *pyObj)
    {
	PyObject *strob;
	if (pyObj == 0L) goto nodice;

	if (PyUnicode_Check(pyObj))
	    return Tcl_NewUnicodeObj(PyUnicode_AS_UNICODE(pyObj),
		    PyUnicode_GET_SIZE(pyObj));
	else if (PyString_Check(pyObj))
	    return Tcl_NewStringObj(PyString_AS_STRING(pyObj),
		    PyString_GET_SIZE(pyObj));
	else if (PyInt_Check(pyObj))
	    return Tcl_NewLongObj(PyInt_AS_LONG(pyObj));
	else if (PyFloat_Check(pyObj))
	    return Tcl_NewDoubleObj(PyFloat_AS_DOUBLE(pyObj));

	// Convert the remainder up to a unicode string.
	//
	strob = PyUnicode_FromObject(pyObj);
	if (strob != 0L) {
	    Tcl_Obj *ret = Tcl_NewUnicodeObj(PyUnicode_AsUnicode(strob),
		    PyUnicode_GetSize(strob));
	    Py_DECREF(strob);
	    return ret;
	}
    nodice:
	return Tcl_NewStringObj("UNABLE TO ACQUIRE A VALUE!!", -1);
    }
    
    static PyObject *
    TranslateObj (Tcl_Obj *TclObj)
    {
	if (TclObj->typePtr != 0L) {
	    if (TclObj->typePtr == tclBooleanTypePtr ||
		    TclObj->typePtr == tclIntTypePtr)
		return PyInt_FromLong(TclObj->internalRep.longValue);
	    else if (TclObj->typePtr == tclDoubleTypePtr)
		return PyFloat_FromDouble(TclObj->internalRep.doubleValue);
	}

	// Convert the remainder up to a unicode string.  This allows us
	// not to be concerned about silly things like utf-8 decoding and
	// 64-bit integer support :)
	//
	return PyUnicode_FromWideChar(Tcl_GetUnicode(TclObj),
		Tcl_GetCharLength(TclObj));
    }
    
    static void
    WriteStdOut (PyObject *text)
    {
	Tcl_Channel stdOut = Tcl_GetStdChannel(TCL_STDOUT);
	Tcl_WriteObj(stdOut, TranslateObj(text));
	Tcl_Flush(stdOut);
    }

    static void
    WriteStdErr (PyObject *text)
    {
	Tcl_Channel stdErr = Tcl_GetStdChannel(TCL_STDERR);
	Tcl_WriteObj(stdErr, TranslateObj(text));
	Tcl_Flush(stdErr);
    }

    Py::Interp *pyPtr;
    Tcl_Interp *globalInterp;
    int *status;

public:
    IRCUserScriptProviderForPythonPrivateImp(
	const char *scriptFileName,
	int objc,
	struct Tcl_Obj * const objv[],
	Tcl_Interp *_globalInterp,
	int useSafe,
	int *_status)
	:
	status(_status),
	globalInterp(_globalInterp)
    {
	// do only once.
	if (tclBooleanTypePtr == 0L) {
	    tclBooleanTypePtr = Tcl_GetObjType("boolean");
	    tclDoubleTypePtr = Tcl_GetObjType("double");
	    tclIntTypePtr = Tcl_GetObjType("int");  // really a long.
	}

	try {
	    pyPtr = new Py::Interp(WriteStdOut, WriteStdErr);
	}
	catch (char *err) {
	    Tcl_SetObjResult(globalInterp, Tcl_NewStringObj(err, -1));
	    *status =  TCL_ERROR;
	    return;
	}
	//while (objc--) pyPtr->???(Tcl_GetUnicode(objv[objc]));
	//pyPtr->Load(scriptFileName);
	//PyRun_File(0L, scriptFileName, Py_file_input, ?, ?);
    }
    ~IRCUserScriptProviderForPythonPrivateImp()
    {
	delete pyPtr;
    }

    virtual CompletionStatus IncomingEvent(const IRCParse *line)
    {
	// TODO!
	return 0;
    }
    virtual CompletionStatus RunAlias(const IRCParse* line)
    {
	return IRCUserScriptProvider::NOTCOMPLETED;
    }


};

Tcl_ObjType *IRCUserScriptProviderForPythonPrivateImp::tclBooleanTypePtr = 0L;
Tcl_ObjType *IRCUserScriptProviderForPythonPrivateImp::tclDoubleTypePtr = 0L;
Tcl_ObjType *IRCUserScriptProviderForPythonPrivateImp::tclIntTypePtr = 0L;


IRCUserScriptProviderForPython::IRCUserScriptProviderForPython(
    const char *scriptFileName,
    const char *instanceName,
    Tcl_Interp *globalInterp,
    int useSafe = 0);
    :
    IRCUserScriptProvider()
{
    _imp = new IRCUserScriptProviderForPythonPrivateImp(scriptFileName,
	    instanceName, globalInterp, useSafe, &status);
}

#endif
