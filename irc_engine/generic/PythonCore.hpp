/*-----------------------------------------------------------------------
 *  PythonCore.hpp --
 *
 *  Classes for wrapping-up Python into a single Py::Interp class.
 *  No use of Tcl is made here.
 *
 *-----------------------------------------------------------------------
 * RCS: @(#) $Id: PythonCore.hpp,v 1.4 2002/07/24 00:47:05 davygrvy Exp $
 *-----------------------------------------------------------------------
 */

#ifndef INC_PythonCore_hpp__
#define INC_PythonCore_hpp__

#include "Python.h"

namespace Py {

    class Except
    {
    protected:
	static void DoTraceback(PyObject *exc_tb, PyObject **errObj);
	static int parse_syntax_error(PyObject *err, PyObject **message,
		char **filename, int *lineno, int *offset, char **text);
	static void DoSyntaxError (PyObject *exc_val, PyObject **errObj);
	static void DoClassError (PyObject *exc_typ, PyObject **errObj);
	static void print_error_text(PyObject **errObj, int offset, char *text);

    public:
	static PyObject *GetExceptInfo (void);
    };

    class Std
    {
    public:
	typedef void (*WriteFunc) (PyObject *);
	Std (WriteFunc _stdOut, WriteFunc _stdErr);
	~Std ();
    protected:
	// Should only be called from inside an interpreter lock.
	void Init();
    private:
	WriteFunc stdOut;
	WriteFunc stdErr;
	static int initialized;
    };

    class Interp : protected Except, Std
    {
	PyObject *dict;
#   ifdef WITH_THREAD
	static PyThreadState *globalState;
#   endif
	int run (char *pycode, int startToken, PyObject **result);
	static int existingInterpreters;

    protected:
#   ifdef WITH_THREAD
	PyThreadState *state;
#   endif

    public:
	Interp(Std::WriteFunc stdOut, Std::WriteFunc stdErr);
	~Interp();

	// Both of these return zero on success or one on failure.
	inline int exec (char *pyscript, PyObject **result)
	{
	    return run (pyscript, Py_file_input, result);
	}
	inline int eval (char *pyexpression, PyObject **result)
	{
	    return run (pyexpression, Py_eval_input, result);
	}
    };

}	// namespace Py

#endif	// INC_PythonCore_hpp__