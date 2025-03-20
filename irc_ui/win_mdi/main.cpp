#include <windows.h>
#include <commctrl.h>

#include <string>

#include "tcl.h"
#include "TclEventSystem.hpp"
#include "FrameWndProc.hpp"
#include "resources/resource.h"

#include "Cmcl.h"


// -------------  globals  -----------------
const TclEventSystem *Tcl;
HINSTANCE hInstance;
HWND hClient;
HWND hFrame;
HWND hStatBar;
HWND hToolBar;
HACCEL hAccelTbl = NULL;
const TCHAR *szDisplayName = TEXT("XiRCON-II Alpha5 : " __TIMESTAMP__ " PDT");

extern "C" Tcl_AppInitProc Gui_irc_Init;

// Direct access to these is limited to Tcl's thread only.
Tcl_Interp *globalInterp;
const char *rcScript = "./xirconrc.tcl";


// -----------  local protos  --------------
static void setargv (int *, char ***);


void
XircPanic TCL_VARARGS_DEF(const char *, arg1)
{
    va_list argList;
    char buf[1024];
    const char *format;
    
    format = TCL_VARARGS_START(const char *, arg1, argList);
    vsprintf_s(buf, format, argList);

    MessageBeep(MB_ICONSTOP);
    MessageBoxA(NULL, buf, "Fatal Error in XiRCON-II",
	    MB_ICONSTOP | MB_OK | MB_TASKMODAL | MB_SETFOREGROUND);
#ifdef _MSC_VER
    //DebugBreak();
#endif
    ExitProcess(1);
}


class TclUp : public TclAsyncJob
{
public:
    TclUp () { TclEventSystem::Instance()->QueueJob(this); }
    ~TclUp() {}
    bool AsyncProc() {return false;}
    void EventProc() {
	// This is running in Tcl's thread.
	// NOT the main application thread.
	Tcl_SetPanicProc(XircPanic);
	globalInterp = Tcl_CreateInterp();
	
	//Tcl_SetVar(globalInterp, "tcl_rcFileName", rcScript, TCL_GLOBAL_ONLY);
	//Tcl_SetVar(globalInterp, "tcl_interactive", "1", TCL_GLOBAL_ONLY);

	if (Tcl_Init(globalInterp) != TCL_OK) {
	    Tcl_Panic(Tcl_GetStringResult(globalInterp));
	}
	//Tcl_SourceRCFile(globalInterp);
	if (Gui_irc_Init(globalInterp) != TCL_OK) {
	    Tcl_Panic(Tcl_GetStringResult(globalInterp));
	}
	tclStubsPtr->tcl_StaticPackage(globalInterp, "IRC_UserInterface", Gui_irc_Init, Gui_irc_Init);

	// Source some stuff.
	Tcl_SetVar(globalInterp, "argc", "1", TCL_GLOBAL_ONLY);
	Tcl_SetVar(globalInterp, "argv", "xircon.tcl", TCL_GLOBAL_ONLY);
	Tcl_SetVar(globalInterp, "argv0", "tkcon.tcl", TCL_GLOBAL_ONLY);
	if (Tcl_EvalFile(globalInterp, "tkcon.tcl") != TCL_OK) {
	    Tcl_Panic(Tcl_GetStringResult(globalInterp));
	}
    }
};

class TclDown : public TclAsyncJob
{
    CMclEvent &_isDown;
public:
    TclDown (CMclEvent &isDown) : _isDown(isDown) {
	TclEventSystem::Instance()->QueueJob(this);
    }
    ~TclDown() {}
    bool AsyncProc() {return false;}
    void EventProc() {
	Tcl_DeleteInterp(globalInterp);
	globalInterp = 0L;
	_isDown.Set();
    }
};


int APIENTRY
wWinMain(
    _In_     HINSTANCE	hInst, 
    _In_opt_ HINSTANCE	hPrevInstance,
    _In_     LPWSTR	lpCmdLine,
    _In_     int	nCmdShow)
{
    MSG msg;
    WNDCLASSEX wc;
    int argc, arg;
    char **argvUTF;
#ifdef _DEBUG
    const char* tclLibUTF = "tcl86tg.dll";	    //default location
#elif
    const char* tclLibUTF = "C:\\Users\\davygrvy\\XiRCON-II\\x64\\Release\\Tcl\\bin\\tcl86t.dll";   //default location
#endif
    CMclEvent isDown;

    // set global.
    hInstance = hInst;

    // translate the raw commandline
    setargv(&argc, &argvUTF);

    // parse the commandline
    if (argc > 1)
	for (arg = 1; arg < argc; arg++)
	    if (!strncmp(argvUTF[arg], "-", 1))
	    {
		switch (tolower(*(argvUTF[arg] + 1)))
		{
		    case 'l':	// -l[ib] <library> : use this Tcl library
				// ex:  xircon-ii.exe -L "tcl86.dll" or
				//      xircon-ii.exe -L "c:\\progra~1\\tcl\\bin\\tcl90.dll"
			tclLibUTF = argvUTF[++arg];
			break;
		    case 'r':	// -r[c] <rcScript> : use this rcscript
				// ex:  xircon-ii.exe -r "c:/mySpecialRC.tcl"
			rcScript = argvUTF[++arg];
			break;
		}
	    }

    INITCOMMONCONTROLSEX iccce;
    iccce.dwSize = sizeof(INITCOMMONCONTROLSEX);
    iccce.dwICC =
	    ICC_LISTVIEW_CLASSES |
	    ICC_TREEVIEW_CLASSES |
	    ICC_BAR_CLASSES |
	    ICC_HOTKEY_CLASS |
	    ICC_USEREX_CLASSES |
	    ICC_STANDARD_CLASSES;

    // This is actually an OS function.
    ::InitCommonControlsEx(&iccce);

    hAccelTbl = ::LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));

    // prepare the instance
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_BYTEALIGNWINDOW;
    wc.lpfnWndProc = FrameWndProc;
    wc.cbClsExtra = NULL;
    wc.cbWndExtra = NULL;
    wc.hInstance = hInstance;
    wc.hbrBackground = NULL;      // Stops flickering
    wc.lpszMenuName = NULL;
    wc.lpszClassName = TEXT("XircMDIFrame");
    wc.hIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDR_MAINICON));
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.hIconSm = wc.hIcon;

    ::RegisterClassEx(&wc);

    hFrame = ::CreateWindowEx(
	    WS_EX_LEFT,
	    wc.lpszClassName,
	    szDisplayName,
	    WS_OVERLAPPEDWINDOW,
	    CW_USEDEFAULT, CW_USEDEFAULT,
	    CW_USEDEFAULT, CW_USEDEFAULT,
	    NULL, NULL,
	    hInstance, NULL);

    if (hFrame == 0L) {
	return EXIT_FAILURE;
    }

    ::ShowWindow(hFrame, nCmdShow);
    ::UpdateWindow(hFrame);

    // Do we have an entry in the ini file for a Tcl dll?
    if (!tclLibUTF) {
	// TODO! check ini file
	// tclLibUTF = ...
    }

    // now crank-up Tcl in it's own thread.  Use this tcl dll if specified
    // on the commandline (first) or ini file (second).
    Tcl = TclEventSystem::Instance(tclLibUTF, XircPanic);

    // Send Tcl it's first job of creating the global interp and 
    // setting up the environment for the internal IRC_UserInterface
    // extension and the external IRC_Engine
    new TclUp;


    while (::GetMessage(&msg, NULL, 0, 0)) {
	if (!::TranslateMDISysAccel(hClient, &msg) &&
		!::TranslateAccelerator(hFrame, hAccelTbl, &msg)) {
	    ::TranslateMessage(&msg);
	    ::DispatchMessage(&msg);
	}
    }

    // Delete our global interp
    new TclDown(isDown);

    // block waiting for the job to finish.
    isDown.Wait(INFINITE);

    // post a WM_QUIT to Tcl's notifier to 
    // fall into Tcl_Finalize and shutdown the thread
    Tcl->ShutDown();
    //delete Tcl;

    return (int) msg.wParam;
}


/*
 *-------------------------------------------------------------------------
 *
 * setargv --
 *
 *    Parse the Windows command line string into argc/argv.  Done here
 *    because we don't trust the builtin argument parser in crt0.  
 *    Windows applications are responsible for breaking their command
 *    line into arguments.
 *
 *    2N backslashes + quote -> N backslashes + begin quoted string
 *    2N + 1 backslashes + quote -> literal
 *    N backslashes + non-quote -> literal
 *    quote + quote in a quoted string -> single quote 
 *    quote + quote not in quoted string -> empty string 
 *    quote -> begin quoted string
 *
 * Results:
 *    Fills argcPtr with the number of arguments and argvPtr with the
 *    array of arguments.
 *
 * Side effects:
 *    Memory allocated.
 *
 *--------------------------------------------------------------------------
 */

static void
setargv (int *argcPtr, char ***argvUTFPtr)
{
    char *p, *arg, *argSpace;
    char **argv;
    int argc, size, inquote, copy, slashes;
    WCHAR *cmdLineUni;
    CHAR *cmdLine;

    // Get the unicode commandline.
    cmdLineUni = GetCommandLineW();
    // Get the size needed for the conversion to UTF-8.
    size = ::WideCharToMultiByte(CP_UTF8, 0, cmdLineUni, -1, 0, 0, NULL, NULL);
    // alloc it.
    cmdLine = new CHAR [size];
    // do it.
    ::WideCharToMultiByte(CP_UTF8, 0, cmdLineUni, -1, cmdLine, size, NULL, NULL);

    // Precompute an overly pessimistic guess at the number of arguments
    // in the command line by counting non-space spans.

    size = 2;
    for (p = cmdLine; *p != '\0'; p++)
	if ((*p == ' ') || (*p == '\t')) {  // INTL: ISO space. 
	    size++;
	    while ((*p == ' ') || (*p == '\t'))	p++; // INTL: ISO space.
	    if (*p == '\0') break;
	}
	
    argSpace = new char[(size * sizeof(char *) + strlen(cmdLine) + 1)];
    argv = (char **) argSpace;
    argSpace += size * sizeof(char *);
    size--;
	
    p = cmdLine;
    for (argc = 0; argc < size; argc++) {
	argv[argc] = arg = argSpace;
	while ((*p == ' ') || (*p == '\t')) p++;  // INTL: ISO space.
	if (*p == '\0') break;
	
	inquote = 0;
	slashes = 0;
	while (1) {
	    copy = 1;
	    while (*p == '\\') {
		slashes++;
		p++;
	    }
	    if (*p == '"') {
		if ((slashes & 1) == 0)	{
		    copy = 0;
		    if ((inquote) && (p[1] == '"')) {
			p++;
			copy = 1;
		    } else {
			inquote = ~inquote;
		    }
		}
		slashes >>= 1;
	    }

	    while (slashes) {
		*arg = '\\';
		arg++;
		slashes--;
	    }

	    if ((*p == '\0') || (!inquote && ((*p == ' ') || (*p == '\t'))))
		break;                  // INTL: ISO space. 
	    if (copy != 0) {
		*arg = *p;
		arg++;
	    }
	    p++;
	}
	*arg = '\0';
	argSpace = arg + 1;
    }
    argv[argc] = NULL;
    delete [] cmdLine;

    *argcPtr = argc;
    *argvUTFPtr = argv;
}
