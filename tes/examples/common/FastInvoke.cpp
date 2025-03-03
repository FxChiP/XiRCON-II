#include "FastInvoke.hpp"

#ifdef _MSC_VER
#   pragma warning (push)
    // No we aren't using objv/argv before it was initialized, thank you.
#   pragma warning (disable : 4701)
#endif

namespace Tcl {
    
int FastInvoke (Tcl_Interp *interp, const char *cmd, const char *...)
{
    char *arg;
    const char **argv;
    Tcl_Obj **objv;
    int argc, i, max;
    Tcl_CmdInfo info;
    va_list pvar;
    int result;
    
    va_start(pvar, cmd);
    
    if (!Tcl_GetCommandInfo(interp, cmd, &info)) {
	Tcl_Obj *resultObj = Tcl_GetObjResult(interp);
	
	Tcl_AppendStringsToObj(resultObj, "Unknown command \"", cmd, "\"", NULL);
	va_end(pvar);
	return TCL_ERROR;
    }
    
    max = 10;
    argc = 1;
    
    if (info.isNativeObjectProc) {
	objv = (Tcl_Obj **) ckalloc(max * sizeof(Tcl_Obj *));
	objv[0] = Tcl_NewStringObj(cmd, -1);
    } else {
	argv = (const char **) ckalloc(max * sizeof(char *));
	argv[0] = cmd;
    }
    Tcl_ResetResult(interp);
    
    for (;;) {
	arg = va_arg(pvar, char *);
	if (info.isNativeObjectProc) {
	    if (arg == 0L) {
		objv[argc] = 0L;
	    } else {
		objv[argc] = Tcl_NewStringObj(arg, -1);
		Tcl_IncrRefCount(objv[argc]);
	    }
	} else {
	    argv[argc] = arg;
	}
	
	if (arg == NULL) {
	    break;
	}
	argc++;
	
	if (argc >= max) {
	    if (info.isNativeObjectProc) {
		Tcl_Obj **old = objv;
		objv = (Tcl_Obj **) ckalloc(2 * max * sizeof(Tcl_Obj *));
		for (i=0; i<max; i++) {
		    objv[i] = old[i];
		}
		ckfree ((char *) old);
	    } else {
		const char **old = argv;
		argv = (const char **) ckalloc(2 * max * sizeof(char *));
		for (i=0; i<max; i++) {
		    argv[i] = old[i];
		}
		ckfree((char *)old);
	    }
	    max *= 2;
	}
    }
    va_end(pvar);
    
    // invoke the function directly.
    
    if (info.isNativeObjectProc) {
	result = (*info.objProc)(info.objClientData, interp, argc, objv);
	Tcl_GetStringResult(interp);
    } else {
	result = (*info.proc)(info.clientData, interp, argc, argv);
    }
    
    if (info.isNativeObjectProc) {
	for (i=0; i<argc; i++) {
	    Tcl_DecrRefCount(objv[i]);
	}
	ckfree((char *)objv);
    } else {
	ckfree((char *)argv);
    }
    
    return result;
}

int FastInvoke (Tcl_Interp *interp, const char *cmd, Tcl_Obj *...)
{
    const char **argv;
    Tcl_Obj *arg;
    Tcl_Obj **objv;
    int argc, i, max;
    Tcl_CmdInfo info;
    va_list pvar;
    int result;
    
    va_start(pvar, cmd);
    
    if (!Tcl_GetCommandInfo(interp, cmd, &info)) {
	Tcl_Obj *resultObj = Tcl_GetObjResult(interp);
	
	Tcl_AppendStringsToObj(resultObj, "Unknown command \"", cmd, "\"", NULL);
	va_end(pvar);
	return TCL_ERROR;
    }
    
    max = 10;
    argc = 1;
    
    if (info.isNativeObjectProc) {
	objv = (Tcl_Obj **) ckalloc(max * sizeof(Tcl_Obj *));
	objv[0] = Tcl_NewStringObj(cmd, -1);
    } else {
	argv = (const char **) ckalloc(max * sizeof(char *));
	argv[0] = cmd;
    }
    Tcl_ResetResult(interp);
    
    for (;;) {
	arg = va_arg(pvar, Tcl_Obj *);
	if (info.isNativeObjectProc) {
	    if (arg == 0L) {
		objv[argc] = 0L;
	    } else {
		objv[argc] = arg;
		Tcl_IncrRefCount(objv[argc]);
	    }
	} else {
	    argv[argc] = Tcl_GetString(arg);
	}
	
	if (arg == NULL) {
	    break;
	}
	argc++;
	
	if (argc >= max) {
	    if (info.isNativeObjectProc) {
		Tcl_Obj **old = objv;
		objv = (Tcl_Obj **) ckalloc(2 * max * sizeof(Tcl_Obj *));
		for (i=0; i<max; i++) {
		    objv[i] = old[i];
		}
		ckfree ((char *)old);
	    } else {
		const char **old = argv;
		argv = (const char **) ckalloc(2 * max * sizeof(char *));
		for (i=0; i<max; i++) {
		    argv[i] = old[i];
		}
		ckfree((char *)old);
	    }
	    max *= 2;
	}
    }
    va_end(pvar);
    
    // invoke the function directly.
    
    if (info.isNativeObjectProc) {
	result = (*info.objProc)(info.objClientData, interp, argc, objv);
	Tcl_GetStringResult(interp);
    } else {
	result = (*info.proc)(info.clientData, interp, argc, argv);
    }
    
    if (info.isNativeObjectProc) {
	for (i=0; i<argc; i++) {
	    Tcl_DecrRefCount(objv[i]);
	}
	ckfree((char *)objv);
    } else {
	ckfree((char *)argv);
    }
    
    return result;
}


int
FastInvoke (Tcl_Interp *interp, unsigned int objc, struct Tcl_Obj * const objv[])
{
    const char **argv;
    unsigned int i;
    int result;
    Tcl_CmdInfo info;
    const char *cmd;

    cmd = Tcl_GetString(objv[0]);
    if (!Tcl_GetCommandInfo(interp, cmd, &info)) {
	Tcl_Obj *resultObj = Tcl_GetObjResult(interp);
	
	Tcl_AppendStringsToObj(resultObj, "Unknown command \"", cmd, "\"", NULL);
	return TCL_ERROR;
    }
    
    Tcl_ResetResult(interp);

    if (info.isNativeObjectProc) {
	for (i = 0; i < objc; i++) {
	    Tcl_IncrRefCount(objv[i]);
	}
	result = (*info.objProc)(info.objClientData, interp, objc, objv);
	for (i = 0; i < objc; i++) {
	    Tcl_DecrRefCount(objv[i]);
	}
    } else {
	argv = (const char **) ckalloc(objc * sizeof(const char *));
	for (i = 0; i < objc; i++) {
	    Tcl_IncrRefCount(objv[i]);
	    argv[i] = Tcl_GetString(objv[i]);
	}
	result = (*info.proc)(info.clientData, interp, objc, argv);
	for (i = 0; i < objc; i++) {
	    Tcl_DecrRefCount(objv[i]);
	}
	ckfree((char *)argv);
	Tcl_GetStringResult(interp);
    }

    return result;
}

int
FastInvoke (Tcl_Interp *interp, unsigned int argc, const char *argv[])
{
    struct Tcl_Obj **objv;
    unsigned int i;
    int result;
    Tcl_CmdInfo info;
    const char *cmd;

    cmd = argv[0];
    if (!Tcl_GetCommandInfo(interp, cmd, &info)) {
	Tcl_Obj *resultObj = Tcl_GetObjResult(interp);
	
	Tcl_AppendStringsToObj(resultObj, "Unknown command \"", cmd, "\"", NULL);
	return TCL_ERROR;
    }
    
    Tcl_ResetResult(interp);

    if (info.isNativeObjectProc) {
	objv = (struct Tcl_Obj **) ckalloc(argc * sizeof(struct Tcl_Obj *));
	for (i = 0; i < argc; i++) {
	    Tcl_IncrRefCount(objv[i] = Tcl_NewStringObj(argv[i], -1));
	}
	result = (*info.objProc)(info.objClientData, interp, argc, objv);
	for (i = 0; i < argc; i++) {
	    Tcl_DecrRefCount(objv[i]);
	}
	ckfree((char *)objv);
    } else {
	result = (*info.proc)(info.clientData, interp, argc, argv);
	Tcl_GetStringResult(interp);
    }

    return result;
}

} // namespace Tcl

#ifdef _MSC_VER
#   pragma warning (pop)
#endif
