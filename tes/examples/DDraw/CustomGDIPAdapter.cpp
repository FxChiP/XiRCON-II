// M$ broke how winsock(2) is included from windows.h, again!
#define _WINSOCKAPI_
#include <windows.h>
#include <ddraw.h>
#include "../common/GdiplusAdapter.hpp"

// This extension is internal.
#define STATIC_BUILD

// let the EXTERN macro know we are declaring these for export.
#undef TCL_STORAGE_CLASS
#define TCL_STORAGE_CLASS DLLEXPORT
#include "CustomGDIPAdapter.hpp"

extern LPDIRECTDRAW7	    g_pDD;          // DirectDraw object
extern LPDIRECTDRAWSURFACE7 g_pDDSPrimary;  // DirectDraw primary surface
extern LPDIRECTDRAWSURFACE7 g_pDDSBack;     // DirectDraw back surface


// We override the creation and deletion of the Graphics class
// for the needs of the application.

class DDrawGDIPAdapter : public GdiplusAdapter
{
public:
    DDrawGDIPAdapter(Tcl_Interp *interp) : GdiplusAdapter(interp) {}

private:
    ~DDrawGDIPAdapter() {
	// Do not access the interp from this function.
	Gdiplus::Graphics *graphics;
	Gdiplus::Pen *pen;
	Gdiplus::Brush *brush;
	Gdiplus::Font *font;
	Gdiplus::Matrix *matrix;
	int result;

	for (
	    result = GraphicsHash.Top(&graphics);
	    result != TCL_ERROR;
	    result = GraphicsHash.Next(&graphics)
	) {
	    delete graphics;
	}
	for (
	    result = PenHash.Top(&pen);
	    result != TCL_ERROR;
	    result = PenHash.Next(&pen)
	) {
	    delete pen;
	}
	for (
	    result = BrushHash.Top(&brush);
	    result != TCL_ERROR;
	    result = BrushHash.Next(&brush)
	) {
	    delete brush;
	}
	for (
	    result = FontHash.Top(&font);
	    result != TCL_ERROR;
	    result = FontHash.Next(&font)
	) {
	    delete font;
	}
	for (
	    result = MatrixHash.Top(&matrix);
	    result != TCL_ERROR;
	    result = MatrixHash.Next(&matrix)
	) {
	    delete matrix;
	}
    }

    HDC hdc;

    virtual int GraphicsConstructCmd (int objc, struct Tcl_Obj * CONST objv[])
    {
	ItclObject *GraphicsItclObj;
	Gdiplus::Graphics *graphicsPtr;
	Gdiplus::Status result;
	DDBLTFX ddbltfx;

	if (objc != 1) {
	    Tcl_WrongNumArgs(interp, 1, objv, "");
	    return TCL_ERROR;
	}

	if (GetItclObj(&GraphicsItclObj, objv[0]) != TCL_OK) return TCL_ERROR;

	// Prepare to clear the back buffer by initializing
	// the fields of a DDBLTFX structure.
	ZeroMemory(&ddbltfx, sizeof(ddbltfx));
	ddbltfx.dwSize = sizeof(ddbltfx);
	ddbltfx.dwFillColor = 0;

	// Clear the back buffer.
	g_pDDSBack->Blt(0L, 0L, 0L, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);

	// Get a device context for the back buffer.
	g_pDDSBack->GetDC(&hdc);

	graphicsPtr = Gdiplus::Graphics::FromHDC(hdc);

	if ((result = graphicsPtr->GetLastStatus()) != Gdiplus::Ok) {
	    SetTclErrorMsg(result);
	    g_pDDSBack->ReleaseDC(hdc);
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

	if (GraphicsHash.Find(ItclObj, &graphicsPtr) == TCL_OK) {

	    // Itcl's destructor may be called even though the Itcl class
	    // constructor returned with an error!  Thus, no Gdiplus::Graphics
	    // instance exists in the hash table even though an Itcl object
	    // context exists.  Only delete what we know is there.

	    delete graphicsPtr;
	    GraphicsHash.Delete(ItclObj);

	    // Release the device context for the back buffer.
	    g_pDDSBack->ReleaseDC(hdc);
      
	    // Bring the back buffer into view.
	    g_pDDSPrimary->Flip(0L, 0);
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
    new DDrawGDIPAdapter(interp);
    Tcl_PkgProvide(interp, "Gdiplus", "1.0");
    return TCL_OK;
}

EXTERN int
Gdiplus_SafeInit (Tcl_Interp *interp)
{
    return Gdiplus_Init(interp);
}
