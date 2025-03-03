/*
 ------------------------------------------------------------------------------
 * main.cpp --
 *
 *	Main file for Toids.  An asteroids clone.
 *
 * For more on GDI+, see:
 * http://msdn.microsoft.com/library/en-us/gdicpp/GDIPlus/GDIPlus.asp
 *
 * Copyright (c) 2001 Tomahawk Software Group.
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: main.cpp,v 1.11 2004/09/20 22:58:23 davygrvy Exp $
 ------------------------------------------------------------------------------
 */

#include <windows.h>
#include <gdiplus.h>
#pragma comment (lib, "gdiplus.lib")

#include "../../include/TclEventSystem.hpp"
#include "../../mcl/include/Cmcl.h"
#ifdef _DEBUG
#   pragma comment (lib, "../../lib/tesd.lib")
#   pragma comment (lib, "../../mcl/lib/mcld.lib")
#else
#   pragma comment (lib, "../../lib/tes.lib")
#   pragma comment (lib, "../../mcl/lib/mcl.lib")
#endif

#include <tcl.h>
#include "../common/FastInvoke.hpp"

// This extension is internal.
#define STATIC_BUILD

// let the EXTERN macro know we are declaring these for export.
#undef TCL_STORAGE_CLASS
#define TCL_STORAGE_CLASS DLLEXPORT
#include "CustomGDIPAdapter.hpp"

#include "resource.h"

Tcl_Interp *globalInterp = 0L;
HWND hWndMain;
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

	if (Tcl_Init(globalInterp) != TCL_OK) {
	    Tcl_GetStringResult(globalInterp);
	    MessageBox(NULL, globalInterp->result, "Tcl_Init() failed",
		    MB_ICONSTOP | MB_OK | MB_TASKMODAL | MB_SETFOREGROUND);
	    return;
	}

	// Intialize our internal Gdiplus extension.
	Gdiplus_Init(globalInterp);
	Tcl_StaticPackage(globalInterp, "Gdiplus", Gdiplus_Init, Gdiplus_SafeInit);

	// Source some stuff.
	if (Tcl_EvalFile(globalInterp, "toids.tcl") != TCL_OK) {
	    CONST char *info;

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

class EvalProcNoArgs : public TclAsyncJob
{
    char *_proc;

public:
    EvalProcNoArgs (char *proc) : _proc(proc)
    {
	TclEventSystem::Instance()->QueueJob(this);
    }
    ~EvalProcNoArgs() {}
    bool AsyncProc() { return false; }
    void EventProc()
    {
	if (globalInterp == 0L) return;
	if (Tcl::FastInvoke(globalInterp, _proc, (char *)0L) != TCL_OK) {
	    CONST char *info;

	    // just make sure globalInterp->result is set.
	    Tcl_GetStringResult(globalInterp);

	    info = Tcl_GetVar(globalInterp, "errorInfo", TCL_GLOBAL_ONLY);
	    MessageBox(NULL, info, globalInterp->result,
		    MB_ICONSTOP | MB_OK | MB_TASKMODAL | MB_SETFOREGROUND);
	}
    }
};


void InitInstance (HINSTANCE hInstance)
{
    WNDCLASS wc;

    // Make the application window class
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
    wc.lpszClassName = "GDI+WithTcl";
    RegisterClass(&wc);
}

void InitWindow(HINSTANCE hInstance, INT iCmdShow)
{

    // Create the main window
    hWndMain = CreateWindowEx(
	    0,
	    "GDI+WithTcl",
	    "T`oids -- Tcl Asteroids",
	    WS_POPUPWINDOW | WS_CAPTION | WS_MINIMIZEBOX,
	    CW_USEDEFAULT,
	    CW_USEDEFAULT,
	    646,
	    524,
	    NULL,
	    NULL,
	    hInstance,
	    NULL);

    ShowWindow(hWndMain, iCmdShow);
    UpdateWindow(hWndMain);
    SetFocus(hWndMain);
}

INT WINAPI
WinMain (HINSTANCE hInstance, HINSTANCE, PSTR szCmdLine, INT iCmdShow)
{
    MSG       msg;
    ULONG_PTR gdiplusToken;
    const TclEventSystem *TES;
    CMclEvent isDown;

    // Initialize GDI+
    Gdiplus::GdiplusStartupInput gdiplusStartupInput; 
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    InitInstance(hInstance);
    InitWindow(hInstance, iCmdShow);

    // Save this for our Tcl_PanicProcs.
    mainThreadID = GetCurrentThreadId();

    // Start Tcl.
    TES = TclEventSystem::Instance(
	    "8.1",		// tcl version requested
	    0,			// exact?
	    DDrawPanicUnwind,	// unwinding panic
	    DDrawPanic		// non-unwinding panic
	    );


    // Send Tcl its first job.
    //
    // Create the main interp, init the internal extension and source
    // a couple scripts.
    new TclUp;

    while (GetMessage(&msg, NULL, 0, 0)) {
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
	case 'P':
	    // ignore repeats.
	    if (lParam & 1<<30) break;
	    new EvalProcNoArgs("Pause");
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

	case VK_RIGHT:
	case VK_LEFT:
	    new EvalProcNoArgs("StopMove");
            return 0;
	}
	break;

    case WM_COMMAND:

	// Test for the identifier of the menu item.
	switch (LOWORD(wParam)) 
        {
	case IDM_EXIT:
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            return 0;

	case IDM_NEWGAME:
	case IDM_HIGHSCORES:
	default:
	    // fall-out for other menus.
	    break;
	};
	break;

    case WM_DESTROY:
	PostQuitMessage(0);
	return 0;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}
