
// FinalCut TUI widgets
// https://github.com/gansm/finalcut/wiki/First-steps
//
#include <final/final.h>
using namespace finalcut;

#define USE_TCL_STUBS
#include "TclEventSystem.hpp"


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

auto main(int argc, char* argv[]) -> int
{
    int result;
    TclEventSystem* Tcl;
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
    new TclDown;

    // todo, needs a wait() here

    // close the notifier thread
    Tcl->Shutdown();
    catch {delete Tcl;}

    return result;
}