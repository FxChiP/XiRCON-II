#include "winTclEventSystemInt.hpp"
#include <string>
#include <crtdbg.h>

// We need at least the Tcl_Obj interface that was started in 8.0
#if TCL_MAJOR_VERSION < 8
#   error "we need Tcl 8.0 or greater to build this"

// Check for Stubs compatibility when asked for it.
#elif defined(USE_TCL_STUBS) && TCL_MAJOR_VERSION == 8 && \
	(TCL_MINOR_VERSION == 0 || \
        (TCL_MINOR_VERSION == 1 && TCL_RELEASE_LEVEL != TCL_FINAL_RELEASE))
#   error "Stubs interface doesn't work in 8.0 and alpha/beta 8.1; only 8.1.0+"
#endif

#ifdef _MSC_VER
    // Only do this when MSVC++ is compiling us.
#   ifdef USE_TCL_STUBS
	// Mark this .obj as needing tcl's Stubs library.
#	pragma comment(lib, "tclstub" \
	    STRINGIFY(JOIN(TCL_MAJOR_VERSION,TCL_MINOR_VERSION)) ".lib")
#	if !defined(_MT) || !defined(_DLL) || defined(_DEBUG)
	    // This fixes a bug with how the Stubs library was compiled.
	    // The requirement for msvcrt.lib from tclstubXX.lib should
	    // be removed.
#	    pragma comment(linker, "-nodefaultlib:msvcrt.lib")
#	endif
#   else
	// Mark this .obj needing the import library
#	pragma comment(lib, "tcl" \
	    STRINGIFY(JOIN(TCL_MAJOR_VERSION,TCL_MINOR_VERSION)) ".lib")
#   endif
#endif

class TclEventLoop : public CMclThreadHandler
{
public:
    TclEventLoop (
	    Tcl_AsyncHandler *_token,
	    CMclEvent &_ready,
	    const char *_app,
	    const char *_tclVer,
	    int _exact,
	    TclEventSystemPlatInt* _that,
	    Tcl_PanicProc *_fatal,
	    Tcl_PanicProc *_nfatal
	)
	: done(0), token(_token), ready(_ready), app(_app), tclVer(_tclVer),
	  exact(_exact), that(_that), fatal(_fatal), nfatal(_nfatal)
    {
    }
    void BreakOut (void)
    {
	done = 1;
    }
private:
    unsigned ThreadHandlerProc (void)
    {
	typedef Tcl_Interp *(*LPFN_createInterpProc) ();
	LPFN_createInterpProc createInterpProc;
	Tcl_Interp *interp;
	char appname[MAX_PATH+1];
	char *library;


	try {
	    library = findTcl(tclVer, exact,
#ifdef _DEBUG
		    1);
#else
		    0);
#endif
	    // load it.
	    hTclMod = LoadLibrary(library);

	    if (hTclMod == 0L)
	    {
		std::string err(library);
		err.append(" failed to load.");
		throw err;
	    }

	    // LoadLibrary() loaded the module correctly.
	    // get the location of Tcl_CreateInterp
	    createInterpProc =
		(LPFN_createInterpProc) GetProcAddress(hTclMod, "Tcl_CreateInterp");

	    // What?? no Tcl_CreateInterp export.. ditch out-a-here!
	    if (createInterpProc == 0L)
	    {
		std::string err("The Tcl_CreateInterp export was not found in ");
		err.append(library);
		err.append(".");
		throw err;
	    }

	    interp = createInterpProc();
	    if (Tcl_InitStubs(interp, tclVer, exact) == 0L)
	    {
		throw std::string(Tcl_GetStringResult(interp));
	    }
	}
	catch (std::string &err)
	{
	    nfatal("Couldn't load Tcl: %s", err.c_str());
	    return 0x666;
	}

	// This needs to RaiseException(TCL_PANIC_UNWIND)
	Tcl_SetPanicProc(fatal);

	// Discover the calling application.
	if (app == 0L) {
	    GetModuleFileNameA(0L, appname, sizeof(appname));
	    app = appname;
	}

	_ASSERTE(app != 0L || *app != '\0');
	Tcl_FindExecutable(app);

	// we're done initializing the core, and now don't need this
	// interp anymore.
	Tcl_DeleteInterp(interp);

        *token = Tcl_AsyncCreate(TclEventSystemInt::MainAsyncProc, that);

	// Now that the async token is ready, unblock the wait in the
	// calling thread.
	ready.Set();

	return RunTclLoop();
    }

    unsigned RunTclLoop (void)
    {
	EXCEPTION_RECORD exceptRec;
	char buffer[578];
	
	exceptRec.ExceptionCode = 0;

	// enter Tcl's notifier and don't fall-out until the thread is
	// told to quit.

	// If a fatal exception happens, catch it.
	//
	__try {
	    __try {
		while (!done) {
		    // *ALL* of Tcl runs from this loop.
		    Tcl_DoOneEvent(TCL_ALL_EVENTS);
		}
		Tcl_AsyncDelete(*token);
		Tcl_Finalize();
	    }
	    __finally {
		// Be careful..  Tcl's DllMain() can lock-up.
		// Leave this here anyways as Tcl _should_ be OK.
		// Go fix the DllMain() bug instead!
		FreeLibrary(hTclMod);
		// Be explicit and reset the Stubs library just
		// cause we should.
		tclStubsPtr = 0L;
		// Reset the async token, too.  Renders QueueJob()
		// non-functional.
		*token = 0L;
	    }
	} __except (
	    exceptRec = *(GetExceptionInformation())->ExceptionRecord,
	    EXCEPTION_EXECUTE_HANDLER)
	{
	    if (exceptRec.ExceptionCode != TES_PANIC_UNWIND) {
		// Something from Tcl's execution tossed a big one.
		wsprintf(buffer,
			"Tcl has crashed with exception [0x%X] (%s) "
			"at address 0x%X", exceptRec.ExceptionCode,
			GetExceptionString(exceptRec.ExceptionCode),
			exceptRec.ExceptionAddress);
		// We don't need RaiseException() for this call.  We're
		// already unwound.
		nfatal(buffer);
	    }
	}
	return exceptRec.ExceptionCode;
    }

    LPTSTR GetExceptionString( DWORD dwCode )
    {
#	define EXCEPTION(x) case EXCEPTION_##x: return #x;

	switch (dwCode)
	{
	    EXCEPTION(ACCESS_VIOLATION)
	    EXCEPTION(DATATYPE_MISALIGNMENT)
	    EXCEPTION(BREAKPOINT)
	    EXCEPTION(SINGLE_STEP)
	    EXCEPTION(ARRAY_BOUNDS_EXCEEDED)
	    EXCEPTION(FLT_DENORMAL_OPERAND)
	    EXCEPTION(FLT_DIVIDE_BY_ZERO)
	    EXCEPTION(FLT_INEXACT_RESULT)
	    EXCEPTION(FLT_INVALID_OPERATION)
	    EXCEPTION(FLT_OVERFLOW)
	    EXCEPTION(FLT_STACK_CHECK)
	    EXCEPTION(FLT_UNDERFLOW)
	    EXCEPTION(INT_DIVIDE_BY_ZERO)
	    EXCEPTION(INT_OVERFLOW)
	    EXCEPTION(PRIV_INSTRUCTION)
	    EXCEPTION(IN_PAGE_ERROR)
	    EXCEPTION(ILLEGAL_INSTRUCTION)
	    EXCEPTION(NONCONTINUABLE_EXCEPTION)
	    EXCEPTION(STACK_OVERFLOW)
	    EXCEPTION(INVALID_DISPOSITION)
	    EXCEPTION(GUARD_PAGE)
	    EXCEPTION(INVALID_HANDLE)
	}

	// If not one of the "known" exceptions, try to get the string
	// from NTDLL.DLL's message table.

	static CHAR szBuffer[512] = { 0 };

	FormatMessage(FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_HMODULE,
		GetModuleHandle("NTDLL.DLL"), dwCode, 0, szBuffer,
		sizeof(szBuffer), 0);

	return szBuffer;
    }

    int done;
    HMODULE hTclMod;
    Tcl_AsyncHandler *token;
    CMclEvent &ready;
    const char *app;
    const char *tclVer;
    int exact;
    TclEventSystemPlatInt *that;
    Tcl_PanicProc *fatal;
    Tcl_PanicProc *nfatal;
};


void
TclEventSystemPlatInt::ShutDown (void)
{
    DWORD exitcode;

    if (loop) {
	loop->BreakOut();
    } else {
	return;
    }

    TclEventLoopThread->GetExitCode(&exitcode);
    if (!TclEventLoopThread.IsNull() &&
	CMclIsValidHandle(TclEventLoopThread.GetHandle()) &&
	exitcode == STILL_ACTIVE)
    {
	DWORD dwWait;

	// Tell the Tcl notifier to close down.
	PostThreadMessage(TclEventLoopThread->GetThreadId(), WM_QUIT, 0L, 0L);
	// block waiting for the thread to close.
	dwWait = TclEventLoopThread->Wait(100);
	// If we timeout, just kill it as the last resort.
	if (CMclWaitTimeout(dwWait)) {
	    TclEventLoopThread->Terminate(666);
	}
    }

    delete loop;
    loop = 0L;
}


TclEventSystemPlatInt::TclEventSystemPlatInt (
    const char *tclVer,
    int exact,
    Tcl_PanicProc *fatal,
    Tcl_PanicProc *nfatal,
    const char *appfile)
{
    CMclEvent ready;

    // kickstart Tcl's thread.
    // from now on, all access to Tcl is through QueueJob()
    TclEventLoopThread = new CMclThread (
	    loop = new TclEventLoop(&m_hAsync, ready, appfile, tclVer,
		exact, this, fatal, nfatal));

    // block waiting until the Async token is created.
    ready.Wait(INFINITE);
}


TclEventSystemPlatInt::~TclEventSystemPlatInt()
{
    ShutDown();
}

int
TclEventSystemPlatInt::Q_Get(const TclAsyncJob *&aj)
{
    return workQ.Get(aj,0);
}

void
TclEventSystemPlatInt::Q_Put(const TclAsyncJob *aj)
{
    workQ.Put(aj);
}
