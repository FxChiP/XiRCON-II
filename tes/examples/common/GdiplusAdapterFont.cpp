#include "GdiplusAdapter.hpp"

int GdiplusAdapter::FontConstructCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    ItclObject *FontItclObj;
    Gdiplus::Status result;
    Gdiplus::Font *fontPtr;
    double point;

    if (objc != 3) {
	Tcl_WrongNumArgs(interp, 1, objv, "family point");
	return TCL_ERROR;
    }

    if (GetItclObj(&FontItclObj, objv[0]) != TCL_OK) {
	return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &point) != TCL_OK) {
	return TCL_ERROR;
    }

    fontPtr = new Gdiplus::Font(
	    Tcl_GetUnicode(objv[1]),
	    static_cast<Gdiplus::REAL>(point),
	    Gdiplus::FontStyleBold,
	    Gdiplus::UnitPixel
	    );

    if ((result = fontPtr->GetLastStatus()) != Gdiplus::Ok) {
	delete fontPtr;
	SetTclErrorMsg(result);
	return TCL_ERROR;
    }
    FontHash.Add(FontItclObj, fontPtr);
    return TCL_OK;
}

int
GdiplusAdapter::FontDestructCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    ItclObject *FontItclObj;
    Gdiplus::Font *fontPtr;

    if (objc != 1) {
	Tcl_WrongNumArgs(interp, 1, objv, "");
	return TCL_ERROR;
    }

    if (GetItclObj(&FontItclObj, objv[0]) != TCL_OK) {
	return TCL_ERROR;
    }

    if (FontHash.Extract(FontItclObj, &fontPtr) == TCL_OK) {
	delete fontPtr;
    }
    return TCL_OK;
}

int
GdiplusAdapter::FontGetHeightCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    ItclObject *FontItclObj;
    ItclObject *GraphicsItclObj;
    Gdiplus::Font *fontPtr;
    Gdiplus::Graphics *graphicsPtr;
    Gdiplus::REAL height;

    if (objc != 2) {
	Tcl_WrongNumArgs(interp, 1, objv, "graphics");
	return TCL_ERROR;
    }

    if (GetItclObj(&FontItclObj, objv[0]) != TCL_OK) {
	return TCL_ERROR;
    }

    if (FontHash.Find(FontItclObj, &fontPtr) != TCL_OK) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("Gdiplus::Font instance lost!", -1));
	return TCL_ERROR;
    }

    // look for the graphics ItclObject* from the commandname
    Itcl_FindObject(interp, Tcl_GetString(objv[1]), &GraphicsItclObj);
    if (GraphicsItclObj == 0L) {
	Tcl_AppendStringsToObj(Tcl_GetObjResult(interp), "\"",
		Tcl_GetString(objv[1]), "\"",
		" was not a Gdiplus::Graphics instance.", 0L);
	return TCL_ERROR;
    }

    // Get the Gdiplus::Graphics* from the Itcl object context.
    if (GraphicsHash.Find(GraphicsItclObj, &graphicsPtr) != TCL_OK) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("Gdiplus::Graphics instance lost!", -1));
	return TCL_ERROR;
    }

    height = fontPtr->GetHeight(graphicsPtr);
    Tcl_SetObjResult(interp, Tcl_NewDoubleObj(height));

    return TCL_OK;
}

int
GdiplusAdapter::FontGetUnitCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    ItclObject *FontItclObj;
    Gdiplus::Font *fontPtr;

    if (objc != 1) {
	Tcl_WrongNumArgs(interp, 1, objv, "");
	return TCL_ERROR;
    }

    if (GetItclObj(&FontItclObj, objv[0]) != TCL_OK) {
	return TCL_ERROR;
    }

    if (FontHash.Find(FontItclObj, &fontPtr) != TCL_OK) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("Gdiplus::Font instance lost!", -1));
	return TCL_ERROR;
    }

    UnitToResult(fontPtr->GetUnit());
    return TCL_OK;
}
