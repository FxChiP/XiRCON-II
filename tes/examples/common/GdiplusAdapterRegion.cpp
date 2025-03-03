#include "GdiplusAdapter.hpp"

int
GdiplusAdapter::RegionConstructCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    ItclObject *regionItclObj;
    Gdiplus::Region *regionPtr;
    Gdiplus::Status result;
    double x, y, height, width;

    if (objc != 1 && objc != 5) {
	Tcl_WrongNumArgs(interp, 1, objv, "?x y height width?");
	return TCL_ERROR;
    }

    if (GetItclObj(&regionItclObj, objv[0]) != TCL_OK) {
	return TCL_ERROR;
    }

    if (objc == 1) {
	regionPtr = new Gdiplus::Region();
    } else {
	if (Tcl_GetDoubleFromObj(interp, objv[1], &x) != TCL_OK) {
	    return TCL_ERROR;
	}
	if (Tcl_GetDoubleFromObj(interp, objv[2], &y) != TCL_OK) {
	    return TCL_ERROR;
	}
	if (Tcl_GetDoubleFromObj(interp, objv[3], &width) != TCL_OK) {
	    return TCL_ERROR;
	}
	if (Tcl_GetDoubleFromObj(interp, objv[4], &height) != TCL_OK) {
	    return TCL_ERROR;
	}

	regionPtr = new Gdiplus::Region(
		Gdiplus::RectF(
		    static_cast<Gdiplus::REAL>(x),
		    static_cast<Gdiplus::REAL>(y),
		    static_cast<Gdiplus::REAL>(width),
		    static_cast<Gdiplus::REAL>(height)
		    )
		);
    }

    if ((result = regionPtr->GetLastStatus()) != Gdiplus::Ok) {
	delete regionPtr;
	SetTclErrorMsg(result);
	return TCL_ERROR;
    }
    RegionHash.Add(regionItclObj, regionPtr);
    return TCL_OK;
}

int
GdiplusAdapter::RegionDestructCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    ItclObject *regionItclObj;
    Gdiplus::Region *regionPtr;

    if (objc != 1) {
	Tcl_WrongNumArgs(interp, 1, objv, "");
	return TCL_ERROR;
    }

    if (GetItclObj(&regionItclObj, objv[0]) != TCL_OK) {
	return TCL_ERROR;
    }

    if (RegionHash.Extract(regionItclObj, &regionPtr) == TCL_OK) {
	delete regionPtr;
    }
    return TCL_OK;
}

int
GdiplusAdapter::RegionTransformCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    return TCL_ERROR;
}

int
GdiplusAdapter::RegionTranslateCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    return TCL_ERROR;
}

int
GdiplusAdapter::RegionUnionCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    return TCL_ERROR;
}

int
GdiplusAdapter::RegionXorCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    return TCL_ERROR;
}
