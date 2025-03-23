#include <thread>
#include <mutex>
#include <condition_variable>

// FinalCut TUI widgets
// https://github.com/gansm/finalcut/wiki/First-steps
//
#include <final/final.h>
using namespace finalcut;

#define USE_TCL_STUBS
#include "TclEventSystem.hpp"

extern "C" Tcl_AppInitProc Tui_irc_Init;

// Direct access to these is limited to Tcl's thread only.
Tcl_Interp* globalInterp;
const char* rcScript = "./xirconrc.tcl";


void
XircPanic TCL_VARARGS_DEF(const char*, arg1)
{
    va_list argList;
    char buf[1024];
    const char* format;

    format = TCL_VARARGS_START(const char*, arg1, argList);
    sprintf_s(buf, format, argList);
    // todo
}

class TclUp : public TclAsyncJob
{
public:
    TclUp() { TclEventSystem::Instance()->QueueJob(this); }
    ~TclUp() {}
    bool AsyncProc() { return false; }
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
	if (Tui_irc_Init(globalInterp) != TCL_OK) {
	    Tcl_Panic(Tcl_GetStringResult(globalInterp));
	}
	tclStubsPtr->tcl_StaticPackage(globalInterp, "IRC_UserInterface", Tui_irc_Init, Tui_irc_Init);

	// Source some stuff.
	Tcl_SetVar(globalInterp, "argc", "1", TCL_GLOBAL_ONLY);
	Tcl_SetVar(globalInterp, "argv", "xircon.tcl", TCL_GLOBAL_ONLY);
	Tcl_SetVar(globalInterp, "argv0", "tkcon.tcl", TCL_GLOBAL_ONLY);
	if (Tcl_EvalFile(globalInterp, "tkcon.tcl") != TCL_OK) {
	    Tcl_Panic(Tcl_GetStringResult(globalInterp));
	}
    }
};


auto main(int argc, char* argv[]) -> int
{
    int result;
    TclEventSystem* Tcl;
    std::condition_variable cv;
    std::mutex mtx;
    FApplication app(argc, argv);

    // now crank-up Tcl in it's own thread.  Use this tcl .so if specified
    // on the commandline (first) or ini file (second).
    Tcl = TclEventSystem::Instance("tcl8.4.16.so", XircPanic);

    // Send Tcl its first job of creating the global interp, 
    // setting up the environment for the internal IRC_UserInterface
    // extension, and loading the external IRC_Engine extension
    new TclUp;

    // Run XiRCON!
    result = app.exec();

    // delete the global interp
    new TclDown(cv);

    // todo
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock);

    // close the notifier thread
    Tcl->Shutdown();
    catch {delete Tcl;}

    return result;
}