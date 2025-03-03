/*
 ------------------------------------------------------------------------------
 * main.cpp --
 *
 *	DirectDraw Example for TES.  Uses GDI+ and gives it a Tcl interface
 *	for 2D vector graphics to show how Tcl can be run in a thread seperate
 *	from the main application's message pump thread and *hopefully* how
 *	TES can be used for a simpler way to embed Tcl into a windows GUI app.
 *
 * For more on GDI+, see:
 * http://msdn.microsoft.com/library/en-us/gdicpp/GDIPlus/GDIPlus.asp
 *
 * Copyright (c) 2001 Tomasoft Engineering.
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: main.cpp,v 1.7 2004/09/17 06:41:51 davygrvy Exp $
 ------------------------------------------------------------------------------
 */

#define INITGUID
#include <tcl.h>
#include <windows.h>
#include <windowsx.h>
#include <gdiplus.h>
#pragma comment (lib, "gdiplus.lib")
#include <ddraw.h>
#pragma comment (lib, "ddraw.lib")
#include <crtdbg.h>

#include <TclEventSystem.hpp>
#include "../../mcl/include/Cmcl.h"
#ifdef _MSC_VER
#   ifdef _DEBUG
#	pragma comment (lib, "../../lib/tesd.lib")
#	pragma comment (lib, "../../mcl/lib/mcld.lib")
#   else
#	pragma comment (lib, "../../lib/tes.lib")
#	pragma comment (lib, "../../mcl/lib/mcl.lib")
#   endif
#endif
#include "../common/FastInvoke.hpp"

// This extension is internal.
#define STATIC_BUILD

// let the EXTERN macro know we are declaring these for export.
#undef TCL_STORAGE_CLASS
#define TCL_STORAGE_CLASS DLLEXPORT
#include "CustomGDIPAdapter.hpp"


// Global variables
LPDIRECTDRAW7        g_pDD = 0L;          // DirectDraw object
LPDIRECTDRAWSURFACE7 g_pDDSPrimary = 0L;  // DirectDraw primary surface
LPDIRECTDRAWSURFACE7 g_pDDSBack = 0L;     // DirectDraw back surface
Tcl_Interp *globalInterp = 0L;
DWORD mainThreadID;

// prototype.
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
Tcl_PanicProc DDrawPanic;
Tcl_PanicProc DDrawPanicUnwind;
void DDrawPanicVA(const char *format, va_list argList);

void
DDrawPanic TCL_VARARGS_DEF(const char *, arg1)
{
    va_list argList;
    const char *format;
    
    format = TCL_VARARGS_START(const char *, arg1, argList);
    DDrawPanicVA(format, argList);
    va_end(argList);
}

void
DDrawPanicUnwind TCL_VARARGS_DEF(const char *, arg1)
{
    va_list argList;
    const char *format;
    
    format = TCL_VARARGS_START(const char *, arg1, argList);
    DDrawPanicVA(format, argList);
    va_end(argList);
    RaiseException(TES_PANIC_UNWIND, EXCEPTION_NONCONTINUABLE, 0, 0L);
}

void
DDrawPanicVA(const char *format, va_list argList)
{
    char buf[1024];
    vsprintf(buf, format, argList);

    // Kill the display.
    g_pDDSPrimary->Release();
    g_pDDSPrimary = 0L;
    g_pDD->Release();
    g_pDD = 0L;

    MessageBeep(MB_ICONSTOP);
    MessageBox(NULL, buf, "Fatal Error in Tcl",
	    MB_ICONSTOP | MB_OK | MB_TASKMODAL | MB_SETFOREGROUND);

    // Cause the main thead to quit the message pump, thus exit.
    PostThreadMessage(mainThreadID, WM_QUIT, 0, 0);
}


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
	globalInterp = Tcl_CreateInterp();
	Tcl_Init(globalInterp);

	// Intialize our internal Gdiplus extension.
	Gdiplus_Init(globalInterp);
	Tcl_StaticPackage(globalInterp, "Gdiplus", Gdiplus_Init, Gdiplus_SafeInit);

	// Source the [Incr Tcl] class interface for the extension.
        if (Tcl_EvalFile(globalInterp, "../common/gdiplus.itcl") != TCL_OK) {
	    goto error;
	}

	// Source the asteroids game classes.
        if (Tcl_EvalFile(globalInterp, "../common/toids.itcl") != TCL_OK) {
	    goto error;
	}

	// Source the procs we call through EvalProcNoArgs
        if (Tcl_EvalFile(globalInterp, "ddraw.itcl") != TCL_OK) {
	    goto error;
	}

	return;
    error:
	{
	    // just make sure globalInterp->result is set.
	    Tcl_GetStringResult(globalInterp);
	    Tcl_Panic(globalInterp->result);
	}
    }
};


class TclDown : public TclAsyncJob
{
    CMclEvent &_isDown;
public:
    TclDown (CMclEvent &isDown) : _isDown(isDown)
    {
	if (TclEventSystem::Instance()->QueueJob(this) == TCL_ERROR) {
	    isDown.Set();
	}
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


class EvalProcNoArgs : public TclAsyncJob
{
    char *_proc;

public:
    EvalProcNoArgs (char *proc) : _proc(proc)
    {
	if (TclEventSystem::Instance()->QueueJob(this) == TCL_ERROR) {
	    delete this;
	}
    }
    ~EvalProcNoArgs() {}
    bool AsyncProc() { return false; }
    void EventProc()
    {
	if (globalInterp == 0L) return;
	if (Tcl::FastInvoke(globalInterp, _proc, (char *)0L) != TCL_OK) {
	    // just make sure globalInterp->result is set.
	    Tcl_GetStringResult(globalInterp);
	    Tcl_Panic(globalInterp->result);
	}
    }
};


class EvalMousePos : public TclAsyncJob
{
    int _click,_x,_y;
    Tcl_Obj *click,*x,*y;

public:
    EvalMousePos (bool click_, int x_, int y_) : _click(click_), _x(x_), _y(y_)
    {
	if (TclEventSystem::Instance()->QueueJob(this) == TCL_ERROR) {
	    delete this;
	}
    }
    ~EvalMousePos() {}
    bool AsyncProc()
    {
	// remember, only memory allocation routines are allowed here.
	click = Tcl_NewBooleanObj(_click);
	x = Tcl_NewIntObj(_x);
	y = Tcl_NewIntObj(_y);
	return false;
    }
    void EventProc()
    {
	int result;

	if (globalInterp == 0L) return;
	result = Tcl::FastInvoke(globalInterp, "MouseMove", click, x, y, 0L);

	if (result != TCL_OK) {
	    const char *info;

	    // just make sure globalInterp->result is set.
	    Tcl_GetStringResult(globalInterp);

	    info = Tcl_GetVar(globalInterp, "errorInfo", TCL_GLOBAL_ONLY);
	    MessageBox(NULL, info, globalInterp->result,
		    MB_ICONSTOP | MB_OK | MB_TASKMODAL | MB_SETFOREGROUND);
	}
    }
};


void InitializeDirectDraw(HWND hWnd)
{
    DDSURFACEDESC2   ddsd;
    DDSCAPS2	    ddscaps;
    HRESULT	    ddresult;
    
    // Create the main DirectDraw object.
    ddresult = DirectDrawCreateEx(NULL, (VOID**)&g_pDD, IID_IDirectDraw7, NULL);
    _ASSERT(ddresult == DD_OK);
    
    // Get exclusive mode.
    ddresult = g_pDD->SetCooperativeLevel(hWnd, DDSCL_EXCLUSIVE | 
	    DDSCL_FULLSCREEN | DDSCL_MULTITHREADED);
    _ASSERT(ddresult == DD_OK);
    
    // Set the video mode to 640x480x32.
    ddresult = g_pDD->SetDisplayMode(640, 480, 32, 0, 0);
    _ASSERT(ddresult == DD_OK);
    
    // Prepare to create the primary surface by initializing
    // the fields of a DDSURFACEDESC2 structure.
    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP |
	    DDSCAPS_COMPLEX;
    ddsd.dwBackBufferCount = 1;
    
    // Create the primary surface.
    ddresult = g_pDD->CreateSurface(&ddsd, &g_pDDSPrimary, NULL);
    _ASSERT(ddresult == DD_OK);
    
    // Get a pointer to the back buffer.
    ZeroMemory(&ddscaps, sizeof(ddscaps));
    ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
    g_pDDSPrimary->GetAttachedSurface(&ddscaps, &g_pDDSBack);
    
}

void InitInstance (HINSTANCE hInstance)
{
    WNDCLASS  wc;
    // Make the application window class
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = "";
    wc.lpszClassName = "DirectDrawAndGDI+WithTcl";
    RegisterClass(&wc);
}

void InitWindow(HINSTANCE hInstance, INT iCmdShow)
{
    HWND	hWnd;

    // Create the main window
    hWnd = CreateWindowEx(
	    0,
	    "DirectDrawAndGDI+WithTcl",
	    "DirectDrawAndGDI+WithTcl",
	    WS_POPUP,
	    0,
	    0,
	    GetSystemMetrics(SM_CXSCREEN),
	    GetSystemMetrics(SM_CYSCREEN),
	    NULL,
	    NULL,
	    hInstance,
	    NULL);

    ShowWindow(hWnd, iCmdShow);
    UpdateWindow(hWnd);
    SetFocus(hWnd);

    InitializeDirectDraw(hWnd);
}

INT WINAPI
WinMain (HINSTANCE hInstance, HINSTANCE, PSTR szCmdLine, INT iCmdShow)
{
    MSG msg;
    ULONG_PTR gdiplusToken;
    const TclEventSystem *TES;
    CMclEvent isDown;

    // Initialize GDI+
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // Save this for our Tcl_PanicProcs.
    mainThreadID = GetCurrentThreadId();

    // Start Tcl.
    TES = TclEventSystem::Instance(
	    "8.1",		// tcl version requested
	    0,			// exact?
	    DDrawPanicUnwind,	// unwinding panic
	    DDrawPanic		// non-unwinding panic
	    );

    if (TES == 0L) {
	msg.wParam = 1;
	goto barf;
    }

    InitInstance(hInstance);
    InitWindow(hInstance, iCmdShow);

    // Send Tcl its first job.
    //
    // Create the main interp, init the internal extension and source
    // a couple scripts.
    new TclUp;

    while (GetMessage(&msg, NULL, 0, 0) > 0) {
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
    delete TES;

    if (g_pDDSPrimary) g_pDDSPrimary->Release();
    if (g_pDD) g_pDD->Release();

barf:
    Gdiplus::GdiplusShutdown(gdiplusToken);
    return msg.wParam;
}


LRESULT CALLBACK
WndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_KEYDOWN:
	switch (wParam)
	{

	// Send Tcl some jobs.
	case '1':
	    new EvalProcNoArgs("Do1");
            return 0;
	case '2':
	    new EvalProcNoArgs("Do2");
            return 0;
	case '3':
	    new EvalProcNoArgs("Do3");
            return 0;
	case '4':
	    new EvalProcNoArgs("Do4");
            return 0;
	case '5':
	    new EvalProcNoArgs("Do5");
            return 0;
	case '6':
	    new EvalProcNoArgs("Do6");
            return 0;
	case '7':
	    new EvalProcNoArgs("Do7");
            return 0;
	case '8':
	    new EvalProcNoArgs("Do8");
            return 0;
	case '9':
	    new EvalProcNoArgs("Do9");
            return 0;
	case '0':
	    new EvalProcNoArgs("Do0");
            return 0;

	// Beep, sleep, and beep again.
	case 'S':
	    Beep(880, 100);
	    Sleep(5000);
	    Beep(2200, 100);
	    return 0;

	case VK_RIGHT:
	    // ignore repeats.
	    if (lParam & 1<<30) break;
	    new EvalProcNoArgs("MoveCw");
            return 0;

	case VK_LEFT:
	    // ignore repeats.
	    if (lParam & 1<<30) break;
	    new EvalProcNoArgs("MoveCcw");
            return 0;

	case 'Z':
	    // ignore repeats.
	    if (lParam & 1<<30) break;
	    new EvalProcNoArgs("HyperSpace");
            return 0;

	case 'X':
	    // ignore repeats.
	    if (lParam & 1<<30) break;
	    new EvalProcNoArgs("DoThrust");
            return 0;

	case VK_SPACE:
	    // ignore repeats.
	    if (lParam & 1<<30) break;
	    new EvalProcNoArgs("Fire");
            return 0;

	case VK_ESCAPE:
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            return 0;
	}
	break;

    case WM_KEYUP:
	switch (wParam)
	{
	case 'X':
	    new EvalProcNoArgs("StopThrust");
            return 0;

	case VK_LEFT:
	case VK_RIGHT:
	    new EvalProcNoArgs("StopMove");
            return 0;
	}
	break;

    case WM_MOUSEMOVE:
	new EvalMousePos((wParam & MK_LBUTTON), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	return 0;

    case WM_DESTROY:
	PostQuitMessage(0);
	return 0;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}
