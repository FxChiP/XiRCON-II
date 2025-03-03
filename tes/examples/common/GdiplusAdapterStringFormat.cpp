#include "GdiplusAdapter.hpp"

int
GdiplusAdapter::StringFormatConstructCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    ItclObject *sfItclObj;
    Gdiplus::StringFormat *sfPtr;
    Gdiplus::Status result;

    if (objc != 1) {
	Tcl_WrongNumArgs(interp, 1, objv, "");
	return TCL_ERROR;
    }

    if (GetItclObj(&sfItclObj, objv[0]) != TCL_OK) {
	return TCL_ERROR;
    }

    sfPtr = new Gdiplus::StringFormat();

    if ((result = sfPtr->GetLastStatus()) != Gdiplus::Ok) {
	delete sfPtr;
	SetTclErrorMsg(result);
	return TCL_ERROR;
    }
    StringFormatHash.Add(sfItclObj, sfPtr);
    return TCL_OK;
}

int
GdiplusAdapter::StringFormatDestructCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    ItclObject *sfItclObj;
    Gdiplus::StringFormat *sfPtr;

    if (objc != 1) {
	Tcl_WrongNumArgs(interp, 1, objv, "");
	return TCL_ERROR;
    }

    if (GetItclObj(&sfItclObj, objv[0]) != TCL_OK) {
	return TCL_ERROR;
    }

    if (StringFormatHash.Extract(sfItclObj, &sfPtr) == TCL_OK) {
	delete sfPtr;
    }
    return TCL_OK;
}
int
GdiplusAdapter::StringFormatAlignmentCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    return TCL_ERROR;
}

int
GdiplusAdapter::StringFormatDigitSubstitutionLanguageCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    return TCL_ERROR;
}

int
GdiplusAdapter::StringFormatFormatFlagsCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    return TCL_ERROR;
}

int
GdiplusAdapter::StringFormatHotkeyPrefixCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    return TCL_ERROR;
}

int
GdiplusAdapter::StringFormatLineAlignmentCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    return TCL_ERROR;
}

int
GdiplusAdapter::StringFormatMeasurableCharacterRangeCountCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    return TCL_ERROR;
}

int
GdiplusAdapter::StringFormatTabStopsCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    return TCL_ERROR;
}

int
GdiplusAdapter::StringFormatTrimmingCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    return TCL_ERROR;
}
