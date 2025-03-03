#include <tcl.h>
#include "../common/GdiplusAdapter.hpp"

// This extension is internal.
#define STATIC_BUILD

// let the EXTERN macro know we are declaring these for export.
#undef TCL_STORAGE_CLASS
#define TCL_STORAGE_CLASS DLLEXPORT
#include "CustomGDIPAdapter.hpp"

extern HWND hWndMain;


// We override the creation and deletion of the Graphics class
// for the needs of the application.

class ToidsGDIPAdapter : public GdiplusAdapter
{
public:
    ToidsGDIPAdapter(Tcl_Interp *interp) : GdiplusAdapter(interp) {}

private:

    virtual int GraphicsConstructCmd (int objc, struct Tcl_Obj * CONST objv[])
    {
	ItclObject *GraphicsItclObj;
	Gdiplus::Graphics *graphicsPtr;
	Gdiplus::Status result;

	if (objc != 1) {
	    Tcl_WrongNumArgs(interp, 1, objv, "");
	    return TCL_ERROR;
	}

	if (GetItclObj(&GraphicsItclObj, objv[0]) != TCL_OK) return TCL_ERROR;

	graphicsPtr = Gdiplus::Graphics::FromHWND(hWndMain);

	if ((result = graphicsPtr->GetLastStatus()) != Gdiplus::Ok) {
	    delete graphicsPtr;
	    SetTclErrorMsg(result);
	    return TCL_ERROR;
	}
	//graphicsPtr->SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
	graphicsPtr->SetInterpolationMode(Gdiplus::InterpolationModeHighQuality);
	//graphicsPtr->SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
	graphicsPtr->SetSmoothingMode(Gdiplus::SmoothingModeHighSpeed);

	GraphicsHash.Add(GraphicsItclObj, graphicsPtr);
	return TCL_OK;
    }

    virtual int GraphicsDestructCmd (int objc, struct Tcl_Obj * CONST objv[])
    {
	ItclObject *ItclObj;
	Gdiplus::Graphics *graphicsPtr;

	if (objc != 1) {
	    Tcl_WrongNumArgs(interp, 1, objv, "");
	    return TCL_ERROR;
	}

	if (GetItclObj(&ItclObj, objv[0]) != TCL_OK) return TCL_ERROR;

	if (GraphicsHash.Extract(ItclObj, &graphicsPtr) == TCL_OK) {
	    delete graphicsPtr;
	}
	return TCL_OK;
    }
};


EXTERN int
Gdiplus_Init (Tcl_Interp *interp)
{
#ifdef USE_TCL_STUBS
    if (Tcl_InitStubs(interp, "8.1", 0) == 0L) {
	return TCL_ERROR;
    }
#endif
#ifdef USE_ITCL_STUBS
    if (Itcl_InitStubs(interp, "3.1", 0) == 0L) {
	return TCL_ERROR;
    }
#endif
    new ToidsGDIPAdapter(interp);
    Tcl_PkgProvide(interp, "Gdiplus", "1.0");
    return TCL_OK;
}

EXTERN int
Gdiplus_SafeInit (Tcl_Interp *interp)
{
    return Gdiplus_Init(interp);
}
