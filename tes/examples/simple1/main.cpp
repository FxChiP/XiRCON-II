#include <windows.h>

#include "../../include/TclEventSystem.hpp"
#ifdef _DEBUG
#   pragma comment (lib, "tessd.lib")
#else
#   pragma comment (lib, "tess.lib")
#endif

#define USE_TCL_STUBS
#include <tcl.h>

#include "../../mcl/include/Cmcl.h"
#ifdef _DEBUG
#   pragma comment (lib, "mclsd.lib")
#else
#   pragma comment (lib, "mcls.lib")
#endif

/* Protos */
Tcl_PanicProc MyPanic;

/* Globals */
Tcl_Interp *globalInterp;
HWND mainHWnd;

class TclUp : public TclAsyncJob
{
public:
    TclUp ()
    {
	TclEventSystem::Instance()->QueueJob(this);
    }
    ~TclUp() {}
    bool AsyncProc() {return false;}
    void EventProc()
    {
	// This is running in Tcl's thread.
	// NOT the main application thread.
	Tcl_SetPanicProc(MyPanic);
	globalInterp = Tcl_CreateInterp();
	if (Tcl_Init(globalInterp) != TCL_OK) {
	    goto error;
	}

	// Source some stuff.
        if (Tcl_EvalFile(globalInterp, "simple1.tcl") != TCL_OK) {
	    goto error;
	}

	return;
error:
	{
    	    char *info;

	    // just make sure globalInterp->result is set.
	    Tcl_GetStringResult(globalInterp);

	    info = Tcl_GetVar(globalInterp, "errorInfo", TCL_GLOBAL_ONLY);
	    MessageBox(NULL, info, globalInterp->result,
		    MB_ICONSTOP | MB_OK | MB_TASKMODAL | MB_SETFOREGROUND);
	}
    }
};


class TclDown : public TclAsyncJob
{
    CMclEvent &_isDown;
public:
    TclDown (CMclEvent &isDown) : _isDown(isDown)
    {
	TclEventSystem::Instance()->QueueJob(this);
    }
    ~TclDown() {}
    bool AsyncProc() {return false;}
    void EventProc()
    {
	Tcl_DeleteInterp(globalInterp);
	globalInterp = 0L;
	_isDown.Set();
    }
};


void
MyPanic TCL_VARARGS_DEF(char const *,arg1)
{
    va_list argList;
    char buf[1024];
    char const *format;
    
    format = TCL_VARARGS_START(char const *,arg1,argList);
    vsprintf(buf, format, argList);

    MessageBeep(MB_ICONSTOP);
    MessageBox(NULL, buf, "Fatal Error in simple1.exe",
	    MB_ICONSTOP | MB_OK | MB_TASKMODAL | MB_SETFOREGROUND);
#ifdef _MSC_VER
    DebugBreak();
#endif
    ExitProcess(777);
}


LRESULT CALLBACK
WndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_KEYDOWN:
	switch (wParam)
	{
	case VK_ESCAPE:
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            return 0;
	}
	break;

    case WM_DESTROY:
	PostQuitMessage(0);
	return 0;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}


void
InitInstance (HINSTANCE hInstance)
{
    WNDCLASS  wc;

    // Make the application window class
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0L, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) COLOR_APPWORKSPACE;
    wc.lpszMenuName = "";
    wc.lpszClassName = "Simple1MainWindow";
    RegisterClass(&wc);
}


void
InitMainWindow (HINSTANCE hInstance, INT iCmdShow)
{
    mainHWnd = CreateWindowEx(
	    0L,
	    "Simple1MainWindow",
	    "Simple1MainWindow",
	    WS_OVERLAPPEDWINDOW,
	    CW_USEDEFAULT,
	    CW_USEDEFAULT,
	    CW_USEDEFAULT,
	    CW_USEDEFAULT,
	    0L,
	    0L,
	    hInstance,
	    0L);

    ShowWindow(mainHWnd, iCmdShow);
    UpdateWindow(mainHWnd);
    SetFocus(mainHWnd);
}


INT WINAPI
WinMain (HINSTANCE hInstance, HINSTANCE, PSTR szCmdLine, INT iCmdShow)
{
    MSG msg;
    const TclEventSystem *Tcl;
    CMclEvent isDown;

    InitInstance(hInstance);
    InitMainWindow(hInstance, iCmdShow);

    // Start Tcl.
    Tcl = TclEventSystem::Instance("8.1" /*tcl version*/, 0 /*exact*/);

    // Send Tcl its first job.
    //
    // Create the main interp and source a script.
    new TclUp;

    while (GetMessage(&msg, 0L, 0, 0)) {
	TranslateMessage(&msg);
	DispatchMessage(&msg);
    }

    // Send Tcl its last job.
    //
    // Delete the main interp.
    new TclDown(isDown);

    // block waiting for the job to finish.
    isDown.Wait(INFINITE);

    // Bring down Tcl's event loop, cleanup, and exit its thread.
    Tcl->ShutDown();

    return msg.wParam;
}

