#ifndef INC_FastInvoke_hpp__
#define INC_FastInvoke_hpp__

#include "tcl.h"

namespace Tcl {

    int FastInvoke (Tcl_Interp *interp, const char *cmd, const char *...);
    int FastInvoke (Tcl_Interp *interp, const char *cmd, Tcl_Obj *...);
    int FastInvoke (Tcl_Interp *interp, unsigned int objc, struct Tcl_Obj * const objv[]);
    int FastInvoke (Tcl_Interp *interp, unsigned int argc, const char *argv[]);

} // namespace Tcl

#endif INC_FastInvoke_hpp__
