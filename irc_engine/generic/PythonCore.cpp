/*-----------------------------------------------------------------------
 *  PythonCore.cpp --
 *
 *  Classes for wrapping-up Python into a single Py::Interp class.
 *  No use of Tcl is made here.
 *
 *-----------------------------------------------------------------------
 * RCS: @(#) $Id: PythonCore.cpp,v 1.3 2002/07/24 00:47:05 davygrvy Exp $
 *-----------------------------------------------------------------------
 */

#if IRCE_USEPYTHON

#include "PythonCore.hpp"

#define PyAppend(s,o) PyString_ConcatAndDel((o), PyString_FromString(s))

PyObject *
Py::Except::GetExceptInfo (void)
{
    PyObject *exc_typ = 0L, *exc_val = 0L, *exc_tb = 0L;
    PyObject *errObj = Py_None;

    PyErr_Fetch(&exc_typ, &exc_val, &exc_tb);
    if (exc_typ == 0L) return Py_None;
    PyErr_NormalizeException(&exc_typ, &exc_val, &exc_tb);
    PySys_SetObject("last_type", exc_typ);
    PySys_SetObject("last_value", exc_val);
    PySys_SetObject("last_traceback", exc_tb);

    if (exc_tb && exc_tb != Py_None) {
	DoTraceback(exc_tb, &errObj);
    }
    if (PyErr_GivenExceptionMatches(exc_typ, PyExc_SyntaxError)) {
	DoSyntaxError(exc_val, &errObj);
    } 
    if (PyClass_Check(exc_typ)) {
	DoClassError(exc_typ, &errObj);
    }
    if (exc_val && exc_val != Py_None) {
	PyObject *s = PyObject_Str(exc_val);
	// only print colon if the str() of the
	// object is not the empty string
	if (s != 0L) {
	    PyString_ConcatAndDel(&errObj, PyString_FromString(": "));
	}
	PyString_ConcatAndDel(&errObj, s);
    }
    Py_XDECREF(exc_typ);
    Py_XDECREF(exc_val);
    Py_XDECREF(exc_tb);
    return errObj;
}

void
Py::Except::DoSyntaxError (PyObject *exc_val, PyObject **errObj)
{
    PyObject *message;
    char *filename, *text;
    int lineno, offset;

    if (!parse_syntax_error(exc_val, &message, &filename, &lineno, &offset,
	    &text)) {
	PyErr_Clear();
	return;
    } else {
	char buf[10];

	PyAppend("  File \"", errObj);
	if (filename == 0L) {
	    PyAppend("<string>", errObj);
	} else {
	    PyAppend(filename, errObj);
	}
	PyAppend("\", line ", errObj);
	sprintf(buf, "%d", lineno);
	PyAppend(buf, errObj);
	PyAppend("\n", errObj);
	if (text != 0L)
	    print_error_text(errObj, offset, text);
    }
}

void
Py::Except::DoClassError (PyObject *exc_typ, PyObject **errObj)
{
    PyClassObject* exc = (PyClassObject*)exc_typ;
    PyObject* className = exc->cl_name;
    PyObject* moduleName = PyDict_GetItemString(exc->cl_dict, "__module__");

    if (moduleName == 0L) {
	PyAppend("<unknown>", errObj);
    } else {
	char* modstr = PyString_AsString(moduleName);
	if (modstr && strcmp(modstr, "exceptions")) {
	    PyAppend(modstr, errObj);
	    PyAppend(".", errObj);
	}
    }
    if (className == NULL) {
	PyAppend("<unknown>", errObj);
    } else {
	PyString_ConcatAndDel(errObj, PyObject_Str(className));
    }
}

// Obtains an object from a Python traceback.
// This is the exact same string as "traceback.print_exc" would return.
//
// Pass in a Python traceback object (probably obtained from PyErr_Fetch())
// Result is a PyObject*.
//
void
Py::Except::DoTraceback(PyObject *exc_tb, PyObject **errObj)
{
    PyObject *modStringIO = NULL;
    PyObject *modTB = NULL;
    PyObject *obFuncStringIO = NULL;
    PyObject *obStringIO = NULL;
    PyObject *obFuncTB = NULL;
    PyObject *argsTB = NULL;

#   define TRACEBACK_FETCH_ERROR(what) \
	    {PyAppend(what, errObj); goto done;}

    // Import the modules we need - cStringIO and traceback
    modStringIO = PyImport_ImportModule("cStringIO");
    if (modStringIO==NULL)
	    TRACEBACK_FETCH_ERROR("can't import cStringIO\n");

    modTB = PyImport_ImportModule("traceback");
    if (modTB==NULL)
	    TRACEBACK_FETCH_ERROR("can't import traceback\n");

    // Construct a cStringIO object
    obFuncStringIO = PyObject_GetAttrString(modStringIO, "StringIO");
    if (obFuncStringIO==NULL)
	    TRACEBACK_FETCH_ERROR("can't find cStringIO.StringIO\n");
    obStringIO = PyObject_CallObject(obFuncStringIO, NULL);
    if (obStringIO==NULL)
	    TRACEBACK_FETCH_ERROR("cStringIO.StringIO() failed\n");

    // Get the traceback.print_exception function, and call it.
    obFuncTB = PyObject_GetAttrString(modTB, "print_tb");
    if (obFuncTB==NULL)
	    TRACEBACK_FETCH_ERROR("can't find traceback.print_tb\n");

    argsTB = Py_BuildValue("OOO",
	exc_tb  ? exc_tb  : Py_None,
	Py_None,
	obStringIO);
    if (argsTB==NULL)
	    TRACEBACK_FETCH_ERROR("can't make print_tb arguments\n");

    *errObj = PyObject_CallObject(obFuncTB, argsTB);
    if (*errObj==NULL)
	    TRACEBACK_FETCH_ERROR("traceback.print_tb() failed\n");

    // Now call the getvalue() method in the StringIO instance
    Py_DECREF(obFuncStringIO);
    obFuncStringIO = PyObject_GetAttrString(obStringIO, "getvalue");
    if (obFuncStringIO==NULL)
	    TRACEBACK_FETCH_ERROR("can't find getvalue function\n");
    Py_DECREF(*errObj);
    *errObj = PyObject_CallObject(obFuncStringIO, NULL);
    if (*errObj==NULL)
	    TRACEBACK_FETCH_ERROR("getvalue() failed.\n");

    // And it should be a string all ready to go - duplicate it.
    if (!PyString_Check(*errObj))
	    TRACEBACK_FETCH_ERROR("getvalue() did not return a string\n");
    
done:
    Py_XDECREF(modStringIO);
    Py_XDECREF(modTB);
    Py_XDECREF(obFuncStringIO);
    Py_XDECREF(obStringIO);
    Py_XDECREF(obFuncTB);
    Py_XDECREF(argsTB);
}


// Having the need to copy code verbatim from the core makes me unhappy.
// Gosh darn it, make it public.
int
Py::Except::parse_syntax_error(PyObject *err, PyObject **message, char **filename,
		   int *lineno, int *offset, char **text)
{
    long hold;
    PyObject *v;
    
    /* old style errors */
    if (PyTuple_Check(err))
	return PyArg_Parse(err, "(O(ziiz))", message, filename,
	lineno, offset, text);
    
    /* new style errors.  `err' is an instance */
    
    if (! (v = PyObject_GetAttrString(err, "msg")))
	goto finally;
    *message = v;
    
    if (!(v = PyObject_GetAttrString(err, "filename")))
	goto finally;
    if (v == Py_None)
	*filename = NULL;
    else if (! (*filename = PyString_AsString(v)))
	goto finally;
    
    Py_DECREF(v);
    if (!(v = PyObject_GetAttrString(err, "lineno")))
	goto finally;
    hold = PyInt_AsLong(v);
    Py_DECREF(v);
    v = NULL;
    if (hold < 0 && PyErr_Occurred())
	goto finally;
    *lineno = (int)hold;
    
    if (!(v = PyObject_GetAttrString(err, "offset")))
	goto finally;
    if (v == Py_None) {
	*offset = -1;
	Py_DECREF(v);
	v = NULL;
    } else {
	hold = PyInt_AsLong(v);
	Py_DECREF(v);
	v = NULL;
	if (hold < 0 && PyErr_Occurred())
	    goto finally;
	*offset = (int)hold;
    }
    
    if (!(v = PyObject_GetAttrString(err, "text")))
	goto finally;
    if (v == Py_None)
	*text = NULL;
    else if (! (*text = PyString_AsString(v)))
	goto finally;
    Py_DECREF(v);
    return 1;
    
finally:
    Py_XDECREF(v);
    return 0;
}

void
Py::Except::print_error_text(PyObject **errObj, int offset, char *text)
{
    char *nl;
    if (offset >= 0) {
	if (offset > 0 && offset == (int)strlen(text))
	    offset--;
	for (;;) {
	    nl = strchr(text, '\n');
	    if (nl == NULL || nl-text >= offset)
		break;
	    offset -= (nl+1-text);
	    text = nl+1;
	}
	while (*text == ' ' || *text == '\t') {
	    text++;
	    offset--;
	}
    }
    PyAppend("    ", errObj);
    PyAppend(text, errObj);
    if (*text == '\0' || text[strlen(text)-1] != '\n')
	PyAppend("\n", errObj);
    if (offset == -1)
	return;
    PyAppend("    ", errObj);
    offset--;
    while (offset > 0) {
	PyAppend(" ", errObj);
	offset--;
    }
    PyAppend("^\n", errObj);
}


typedef struct {
	PyObject_HEAD
	Py::Std::WriteFunc stdChan;
} StdObject;

staticforward PyTypeObject Std_Type;

static PyObject *
write (PyObject *self, PyObject *args)
{
    PyObject *ob;
    StdObject *std = reinterpret_cast<StdObject *>(self);

    if (!PyArg_ParseTuple(args, "O", &ob)) return NULL;
    std->stdChan(ob);
    Py_INCREF(Py_None);
    return Py_None;
}

struct PyMethodDef std_methods[] = {
    {"write", write, METH_VARARGS},
    {0L, 0L}
};

static PyObject *
std_getattr(StdObject *self, char *name)
{
    return Py_FindMethod(std_methods,
	    reinterpret_cast<PyObject *>(self), name);
}

statichere PyTypeObject Std_Type = {
    /* The ob_type field must be initialized in the module init function
     * to be portable to Windows without using C++. */
    PyObject_HEAD_INIT(NULL)
    0,			/*ob_size*/
    "tcl-std",		/*tp_name*/
    sizeof(StdObject),	/*tp_basicsize*/
    0,			/*tp_itemsize*/
    /* methods */
    (destructor)0L,	/*tp_dealloc*/
    0,			/*tp_print*/
    (getattrfunc)std_getattr, /*tp_getattr*/
    (setattrfunc)0L,	/*tp_setattr*/
    0,			/*tp_compare*/
    0,			/*tp_repr*/
    0,			/*tp_as_number*/
    0,			/*tp_as_sequence*/
    0,			/*tp_as_mapping*/
    0,			/*tp_hash*/
};


Py::Std::Std (WriteFunc _stdOut, WriteFunc _stdErr)
    : stdOut(_stdOut), stdErr(_stdErr)
{
}

Py::Std::~Std()
{
}

int Py::Std::initialized = 0;

// Should only be called from inside an interpreter lock.
void Py::Std::Init()
{
    StdObject *self;

    if (!initialized) {
	// Initialize the type of the new type object here; doing it here
	// is required for portability to Windows without requiring C++.
	Std_Type.ob_type = &PyType_Type;

	// Create the module and add the functions
	Py_InitModule("tcl-std", std_methods);
	initialized = 1;
    }

    self = PyObject_New(StdObject, &Std_Type);
    self->stdChan = stdOut;
    PySys_SetObject("stdout", reinterpret_cast<PyObject *>(self));

    self = PyObject_New(StdObject, &Std_Type);
    self->stdChan = stdErr;
    PySys_SetObject("stderr", reinterpret_cast<PyObject *>(self));
}


int Py::Interp::existingInterpreters = 0;

#ifdef WITH_THREAD
PyThreadState *Py::Interp::globalState = 0L;
#endif

Py::Interp::Interp (void(stdOut)(PyObject*), void(stdErr)(PyObject*))
    : Std(stdOut, stdErr)
{
#ifdef WITH_THREAD
    if (existingInterpreters == 0) {

	// initialize main interpreter
        Py_Initialize();

	// initialize and acquire the global interpreter lock
        PyEval_InitThreads();

	// save the global thread
        globalState = PyThreadState_Swap(0);

    } else {

	// needed in order to be able to create a new interpreter
        PyEval_AcquireLock();
    }

    if ((state = Py_NewInterpreter()) == 0L) {
        PyEval_ReleaseLock();
        throw "could not create a new interpreter";
    }
#else
    if (existingInterpreters > 0) {
        throw  "cannot create several concurrent Python interpreters\n(Python library was compiled without thread support)";
    } else {
	// initialize main interpreter
        Py_Initialize();
    }
#endif

    dict = PyModule_GetDict(PyImport_AddModule("__main__"));

    // initialize the redirection module.
    Std::Init();
    existingInterpreters++;

#ifdef WITH_THREAD
    // release the global interpreter lock.
    PyEval_ReleaseLock();
#endif

}


Py::Interp::~Interp ()
{
#ifdef WITH_THREAD

    // acquire the interpreter lock and make this thread current.
    PyEval_AcquireThread(state);
    Py_EndInterpreter(state);
#endif

    // any remaining sub-interpreters?
    if (--existingInterpreters == 0) {

#ifdef WITH_THREAD
        PyEval_ReleaseLock();

	// required before finalization
        PyEval_AcquireThread(globalState);
        globalState = 0;
#endif
	// clean everything up
        Py_Finalize();
    }
}


int
Py::Interp::run(char *pycode, int startToken, PyObject **result)
{
    int code = 0;

#ifdef WITH_THREAD
    // acquire the global interpreter lock and make this thread current
    PyEval_RestoreThread(state);
#endif

    if ((*result = PyRun_String(pycode, startToken, dict, dict)) == 0L) {
	// an error occured
	code = 1;
	*result = GetExceptInfo();
    }

#ifdef WITH_THREAD
    // eventually restore the previous thread and release the global
    // interpreter lock.
    PyEval_SaveThread();
#endif

    return code;
}

#endif