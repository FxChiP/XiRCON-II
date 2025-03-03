#include <windows.h>
#include <tcl.h>

/* Protos */
Tcl_PanicProc MyPanic;

/* Globals */
Tcl_Interp *globalInterp;
HWND mainHWnd;


void
MyPanic TCL_VARARGS_DEF(char const *, arg1)
{
    va_list argList;
    char buf[1024];
    char const *format;
    
    format = TCL_VARARGS_START(char const *, arg1, argList);
    vsprintf(buf, format, argList);

    MessageBeep(MB_ICONSTOP);
    MessageBox(NULL, buf, "Fatal Error in doomed.exe",
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
    wc.lpszClassName = "DoomedMainWindow";
    RegisterClass(&wc);
}


void
InitMainWindow (HINSTANCE hInstance, INT iCmdShow)
{
    mainHWnd = CreateWindowEx(
	    0L,
	    "DoomedMainWindow",
	    "DoomedMainWindow",
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
    char appname[MAX_PATH+1];

    InitInstance(hInstance);
    InitMainWindow(hInstance, iCmdShow);

    // Start Tcl.
    GetModuleFileName(0L, appname, sizeof(appname));
    Tcl_FindExecutable(appname);
    Tcl_SetPanicProc(MyPanic);

    globalInterp = Tcl_CreateInterp();
    if (Tcl_Init(globalInterp) != TCL_OK) {
	char *info;

	// just make sure globalInterp->result is set.
	Tcl_GetStringResult(globalInterp);

	info = Tcl_GetVar(globalInterp, "errorInfo", TCL_GLOBAL_ONLY);
	MessageBox(NULL, info, globalInterp->result,
		MB_ICONSTOP | MB_OK | MB_TASKMODAL | MB_SETFOREGROUND);
	return 777;
    }

    // Source some stuff.
    if (Tcl_EvalFile(globalInterp, "doomed.tcl") != TCL_OK) {
	char *info;

	// just make sure globalInterp->result is set.
	Tcl_GetStringResult(globalInterp);

	info = Tcl_GetVar(globalInterp, "errorInfo", TCL_GLOBAL_ONLY);
	MessageBox(NULL, info, globalInterp->result,
		MB_ICONSTOP | MB_OK | MB_TASKMODAL | MB_SETFOREGROUND);
    }

    while (GetMessage(&msg, 0L, 0, 0)) {
	TranslateMessage(&msg);
	DispatchMessage(&msg);

	// Flush Tcl's event loop.
	// I could add another flush with a WM_TIMER, too, but that's just more
	// flushing on top of flushing.  Kind of like polling a socket for
	// becoming readable...
	Tcl_DoOneEvent(TCL_DONT_WAIT);
    }

    Tcl_DeleteInterp(globalInterp);
    Tcl_Finalize();

    return msg.wParam;
}

