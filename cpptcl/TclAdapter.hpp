/*
 ------------------------------------------------------------------------------
 * TclAdaptor.hpp --
 *
 *	Defines one templated class for doing the grunt work of making
 *	C++ extensions for Tcl.
 *
 * Copyright (c) 1999-2001 TomaSoft Engineering
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: TclAdapter.hpp,v 1.7 2003/11/08 01:40:50 davygrvy Exp $
 ------------------------------------------------------------------------------
 */

#ifndef INC_TclAdaptor_hpp__
#define INC_TclAdaptor_hpp__

#include "tcl.h"


/* Get "Stubs" compatibility. */
#if (TCL_MAJOR_VERSION < 8 || \
	(TCL_MAJOR_VERSION == 8 && \
	    (TCL_MINOR_VERSION == 0 || \
		(TCL_MINOR_VERSION == 1 && \
		    TCL_RELEASE_LEVEL != TCL_FINAL_RELEASE) \
	    ) \
	) \
    )
#   define HAS_TCL_STUBS_SUPPORT    0
#   ifdef USE_TCL_STUBS
#	error Stubs doesn't work in alpha/beta 8.1 or less; only 8.1.0+
#   else
	/* Tcl_InitStubs doesn't exist, so fake it. */
#	define Tcl_InitStubs(i,v,e) Tcl_PkgRequire(i,"Tcl",TCL_VERSION,1)
#   endif
#else
#   define HAS_TCL_STUBS_SUPPORT    1
#endif


#ifdef _MSC_VER
    /* Only do this when MSVC++ is compiling us. */
#   ifdef USE_TCL_STUBS
	/* Mark this .obj as needing tcl's Stubs library. */
#	pragma comment(lib, "tclstub" \
	    STRINGIFY(JOIN(TCL_MAJOR_VERSION,TCL_MINOR_VERSION)) ".lib")
#	if !defined(_MT) || !defined(_DLL) || defined(_DEBUG)
	    /* This fixes a bug with how the Stubs library was compiled.
	     * The requirement for msvcrt.lib from tclstubXX.lib should
	     * be removed. */
#	    pragma comment(linker, "-nodefaultlib:msvcrt.lib")
#	endif
#   else
	/* Mark this .obj needing the import library */
#	pragma comment(lib, "tcl" \
	    STRINGIFY(JOIN(TCL_MAJOR_VERSION,TCL_MINOR_VERSION)) ".lib")
#   endif
#endif

// We only need one Win32 API in here so we can be super defensive
// with the destructor.
#ifdef __WIN32__
#   include <windows.h>
#   ifdef _MSC_VER
#	pragma comment (lib, "kernel32.lib")
#   endif
#endif


// The short cut for creating a command.
#if TCL_MAJOR_VERSION > 7
#   define NewTclCmd(a,b) \
	Tcl_CreateObjCommand(interp,(a),CmdDemux,CmdInfo((b),this),CmdDelete)
#else
#   define NewTclCmd(a,b) \
	Tcl_CreateCommand(interp,(a),CmdDemux,CmdInfo((b),this),CmdDelete)
#endif


namespace Tcl {

template <class T>
    class Adapter
{
    typedef struct {
	T *ext;
#if TCL_MAJOR_VERSION > 7
	int (T::*cmd)(int, struct Tcl_Obj * const []);
#else
	int (T::*cmd)(int, char **);
#endif
    } MPLEXDATA, *LPMPLEXDATA;

public:
    Tcl_Interp *interp;

protected:
    Adapter(Tcl_Interp *_interp);
    Adapter();
    virtual ~Adapter();

    virtual void DoInterpDataCleanup(void) {};
    virtual void DoCommandCleanup(void) {};

    // delay start entry when the interp isn't known for the constructor.
    //
    void InitBaseForAutoDestruct();

    // Create the multiplexor data that we save in the ClientData
    // portion of the Tcl command.
    //
#if TCL_MAJOR_VERSION > 7
    ClientData CmdInfo(int(T::*cmd)(int,struct Tcl_Obj *const[]),T *that)
#else
    ClientData CmdInfo(int(T::*cmd)(int,char **),T *that)
#endif
    {
	LPMPLEXDATA mplex = new MPLEXDATA;
	
	mplex->ext = that;
	mplex->cmd = cmd;
	return static_cast <ClientData>(mplex);
    }
    static Tcl_InterpDeleteProc InterpDeleting;
    static Tcl_ExitProc Exiting;
    static Tcl_ObjCmdProc CmdDemux;
    static Tcl_CmdDeleteProc CmdDelete;
};


template <class T>
    Adapter<T>::Adapter(Tcl_Interp *_interp)
    : interp(_interp)
{
    InitBaseForAutoDestruct();
}

template <class T>
    Adapter<T>::Adapter()
    : interp(0L)
{
}

template <class T> void
    Adapter<T>::InitBaseForAutoDestruct()
{
    Tcl_CallWhenDeleted(interp, InterpDeleting, this);
    Tcl_CreateExitHandler(Exiting, this);
}

template <class T>
    Adapter<T>::~Adapter()
{
    if (interp != 0L) DoCommandCleanup();
    Tcl_DeleteExitHandler(Exiting, this);
}


template <class T> void
    Adapter<T>::InterpDeleting (ClientData clientData, Tcl_Interp *)
{
    Adapter<T> *adapt = reinterpret_cast <Adapter<T> *>(clientData);
    adapt->DoInterpDataCleanup();

    // The use of the interp* is not allowed from the context of the
    // destructor.  Ensure that it can't be used by removing it now.
    //
    adapt->interp = 0L;
    delete adapt;
}


template <class T> void
    Adapter<T>::Exiting (ClientData clientData)
{
    Adapter<T> *adapt = reinterpret_cast <Adapter<T> *>(clientData);
#ifdef __WIN32__
    // It can happen that the HEAP could have already been unloaded
    // from an awkward teardown caused by a Ctrl+C or other.  Win32
    // seems to do a teardown in reverse order and by the time Tcl
    // knows what's going on and Tcl_Finalize calls the exit
    // handlers, this extension's data (heap?) has already been
    // unloaded by the OS.  Do a quick legal check on the pointer
    // first.
    //
    if (IsBadReadPtr(adapt, sizeof(T *))) return;
#endif
    // The use of the interp* is not allowed from the context of an
    // exit handler, so make sure it can't be used by removing it.
    //
    adapt->interp = 0L;
    delete adapt;
}


template <class T> int
#if TCL_MAJOR_VERSION > 7
    Adapter<T>::CmdDemux (ClientData clientData, Tcl_Interp *, int objc,
			  struct Tcl_Obj * const objv[])
#else
    Adapter<T>::CmdDemux (ClientData clientData, Tcl_Interp *, int objc,
			  char *objv[])
#endif
{
    LPMPLEXDATA demux = static_cast <LPMPLEXDATA>(clientData);
    // We aleady know what the interp pointer is (saved in the class
    // instance), so don't bother with it here. Call the member function
    // of the extension instance as saved in the MPLEXDATA struct when we
    // created this command using the somewhat obscure C++ pointer-to-
    // member method.
    //
    // This is a demultiplexor or 'demux' for short.
    //
    return ((demux->ext) ->* (demux->cmd)) (objc,objv);
}


template <class T> void
    Adapter<T>::CmdDelete (ClientData clientData)
{
    // clean-up the MPLEXDATA structure from the commands.
    //
    delete static_cast <LPMPLEXDATA>(clientData);
}

} // namespace Tcl

#endif
