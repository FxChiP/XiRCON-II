#include "GdiplusAdapter.hpp"

int
GdiplusAdapter::SolidBrushConstructCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    ItclObject *SolidBrushItclObj;
    Gdiplus::Brush *brush;
    Gdiplus::Status result;
    Gdiplus::ARGB color;

    if (objc < 2) {
	Tcl_WrongNumArgs(interp, 1, objv, "color");
	return TCL_ERROR;
    }
    if (GetItclObj(&SolidBrushItclObj, objv[0]) != TCL_OK) {
	return TCL_ERROR;
    }
    if (MakeARGB(objv[1], &color) != TCL_OK) {
	return TCL_ERROR;
    }

    brush = new Gdiplus::SolidBrush(Gdiplus::Color(color));

    if ((result = brush->GetLastStatus()) != Gdiplus::Ok) {
	delete brush;
	SetTclErrorMsg(result);
	return TCL_ERROR;
    }
    BrushHash.Add(SolidBrushItclObj, brush);

    return TCL_OK;
}

int
GdiplusAdapter::SolidBrushDestructCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    ItclObject *SolidBrushItclObj;
    // This is really a Gdiplus::SolidBrush*, but we stored it by its base type.
    Gdiplus::Brush *brushPtr;

    if (objc != 1) {
	Tcl_WrongNumArgs(interp, 1, objv, "");
	return TCL_ERROR;
    }

    if (GetItclObj(&SolidBrushItclObj, objv[0]) != TCL_OK) {
	return TCL_ERROR;
    }

    if (BrushHash.Extract(SolidBrushItclObj, &brushPtr) == TCL_OK) {
	delete brushPtr;
    }
    return TCL_OK;
}

int GdiplusAdapter::TextureBrushConstructCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    // Can't do a TextureBrush without a Gdiplus::Image class..
    // not for today...  maybe some other time...
    return TCL_ERROR;
}

int GdiplusAdapter::TextureBrushDestructCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    // Can't do a TextureBrush without a Gdiplus::Image class..
    // not for today...  maybe some other time...
    return TCL_ERROR;
}

int GdiplusAdapter::LinearGradientBrushConstructCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    ItclObject *LinearGradientBrushItclObj;
    Gdiplus::Brush *brush;
    Gdiplus::Status result;
    Gdiplus::ARGB color1, color2;
    double x, y, width, height, angle;
    int isAngleScalable = 0;

    if (objc < 8 || objc > 9) {
	Tcl_WrongNumArgs(interp, 1, objv,
		"x y width height color1 color2 angle ?scalable?");
	return TCL_ERROR;
    }

    if (GetItclObj(&LinearGradientBrushItclObj, objv[0]) != TCL_OK) {
	return TCL_ERROR;
    }

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

    if (MakeARGB(objv[5], &color1) != TCL_OK) {
	return TCL_ERROR;
    }
    if (MakeARGB(objv[6], &color2) != TCL_OK) {
	return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[7], &angle) != TCL_OK) {
	return TCL_ERROR;
    }

    if (objc == 9 &&
	    Tcl_GetBooleanFromObj(interp, objv[8], &isAngleScalable)
	    != TCL_OK) {
	return TCL_ERROR;
    }

    brush = new Gdiplus::LinearGradientBrush(
	    Gdiplus::RectF(
		    static_cast<Gdiplus::REAL>(x),
		    static_cast<Gdiplus::REAL>(y),
		    static_cast<Gdiplus::REAL>(width),
		    static_cast<Gdiplus::REAL>(height)
		    ),
	    Gdiplus::Color(color1),
	    Gdiplus::Color(color2),
	    static_cast<Gdiplus::REAL>(angle),
	    static_cast<BOOL>(isAngleScalable)
	    );

    if ((result = brush->GetLastStatus()) != Gdiplus::Ok) {
	delete brush;
	SetTclErrorMsg(result);
	return TCL_ERROR;
    }
    BrushHash.Add(LinearGradientBrushItclObj, brush);

    return TCL_OK;
}

int GdiplusAdapter::LinearGradientBrushDestructCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    ItclObject *LinearGradientBrushItclObj;

    // This should be a LinearGradientBrush*, but we stored it by its base type.
    Gdiplus::Brush *brushPtr;

    if (objc != 1) {
	Tcl_WrongNumArgs(interp, 1, objv, "");
	return TCL_ERROR;
    }

    if (GetItclObj(&LinearGradientBrushItclObj, objv[0]) != TCL_OK) {
	return TCL_ERROR;
    }

    if (BrushHash.Extract(LinearGradientBrushItclObj, &brushPtr) == TCL_OK) {
	delete brushPtr;
    }
    return TCL_OK;
}

int GdiplusAdapter::HatchBrushConstructCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    ItclObject *HatchBrushItclObj;
    Gdiplus::Status result;
    Gdiplus::HatchBrush *brush;
    Gdiplus::HatchStyle hatchStyle;
    Gdiplus::ARGB foreColor;
    Gdiplus::ARGB backColor = Gdiplus::Color::Black;

    if (objc < 3 || objc > 4) {
	Tcl_WrongNumArgs(interp, 1, objv, "hatchStyle foreColor ?backColor?");
	return TCL_ERROR;
    }

    if (GetItclObj(&HatchBrushItclObj, objv[0]) != TCL_OK) {
	return TCL_ERROR;
    }

    if (GetHatchStyle(objv[1], &hatchStyle) != TCL_OK) {
	return TCL_ERROR;
    }
    if (MakeARGB(objv[2], &foreColor) != TCL_OK) {
	return TCL_ERROR;
    }
    if (objc == 4 && MakeARGB(objv[3], &backColor) != TCL_OK) {
	return TCL_ERROR;
    }

    brush = new Gdiplus::HatchBrush(
	    hatchStyle,
	    Gdiplus::Color(foreColor),
	    Gdiplus::Color(backColor)
	    );

    if ((result = brush->GetLastStatus()) != Gdiplus::Ok) {
	delete brush;
	SetTclErrorMsg(result);
	return TCL_ERROR;
    }
    BrushHash.Add(HatchBrushItclObj, brush);
    return TCL_OK;
}

int GdiplusAdapter::HatchBrushDestructCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    ItclObject *HatchBrushItclObj;

    // This should be a LinearGradientBrush*, but we stored it by its base type.
    Gdiplus::Brush *brushPtr;

    if (objc != 1) {
	Tcl_WrongNumArgs(interp, 1, objv, "");
	return TCL_ERROR;
    }

    if (GetItclObj(&HatchBrushItclObj, objv[0]) != TCL_OK) {
	return TCL_ERROR;
    }

    if (BrushHash.Extract(HatchBrushItclObj, &brushPtr) == TCL_OK) {
	delete brushPtr;
    }
    return TCL_OK;
}
