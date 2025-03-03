#include "GdiplusAdapter.hpp"

int
GdiplusAdapter::PenConstructCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    ItclObject *PenItclObj;
    Gdiplus::Pen *pen;
    Gdiplus::Status result;
    Gdiplus::ARGB color;
    double width = 1.0F;

    if (objc < 2 || objc > 3) {
	Tcl_WrongNumArgs(interp, 1, objv, "color ?width?");
	return TCL_ERROR;
    }

    if (GetItclObj(&PenItclObj, objv[0]) != TCL_OK) {
	return TCL_ERROR;
    }

    if (MakeARGB(objv[1], &color) != TCL_OK) {
	return TCL_ERROR;
    }

    if (objc == 3 && Tcl_GetDoubleFromObj(interp, objv[2], &width)
	    != TCL_OK) {
	return TCL_ERROR;
    }

    pen = new Gdiplus::Pen(
	    Gdiplus::Color(color),
	    static_cast<Gdiplus::REAL>(width)
	    );

    if ((result = pen->GetLastStatus()) != Gdiplus::Ok) {
	delete pen;
	SetTclErrorMsg(result);
	return TCL_ERROR;
    }
    PenHash.Add(PenItclObj, pen);

    return TCL_OK;
}

int
GdiplusAdapter::PenDestructCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    ItclObject *PenItclObj;
    Gdiplus::Pen *penPtr;

    if (objc != 1) {
	Tcl_WrongNumArgs(interp, 1, objv, "");
	return TCL_ERROR;
    }

    if (GetItclObj(&PenItclObj, objv[0]) != TCL_OK) {
	return TCL_ERROR;
    }

    if (PenHash.Extract(PenItclObj, &penPtr) == TCL_OK) {
	delete penPtr;
    }
    return TCL_OK;
}

int
GdiplusAdapter::PenMultiplyTransformCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    ItclObject *PenItclObj;
    ItclObject *MatrixItclObj;
    Gdiplus::Pen *penPtr;
    Gdiplus::Matrix *matrixPtr;
    Gdiplus::Status result;

    if (objc != 2) {
	Tcl_WrongNumArgs(interp, 1, objv, "matrix");
	return TCL_ERROR;
    }

    if (GetItclObj(&PenItclObj, objv[0]) != TCL_OK) {
	return TCL_ERROR;
    }
    if (PenHash.Find(PenItclObj, &penPtr) != TCL_OK) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("Gdiplus::Pen instance lost!", -1));
	return TCL_ERROR;
    }

    // look for the matrix ItclObject* from the commandname
    Itcl_FindObject(interp, Tcl_GetString(objv[1]), &MatrixItclObj);
    if (MatrixItclObj == 0L) {
	Tcl_AppendStringsToObj(Tcl_GetObjResult(interp), "\"",
		Tcl_GetString(objv[1]), "\"",
		" was not a Gdiplus::Matrix instance.", 0L);
	return TCL_ERROR;
    }

    // Get the Gdiplus::Matrix* from the Itcl object context.
    if (MatrixHash.Find(MatrixItclObj, &matrixPtr) != TCL_OK) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("Gdiplus::Matrix instance lost!", -1));
	return TCL_ERROR;
    }

    result = penPtr->MultiplyTransform(
	    matrixPtr, Gdiplus::MatrixOrderPrepend);

    if (result != Gdiplus::Ok) {
	SetTclErrorMsg(result);
	return TCL_ERROR;
    }

    return TCL_OK;
}

int
GdiplusAdapter::PenResetTransformCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    ItclObject *PenItclObj;
    Gdiplus::Pen *penPtr;
    Gdiplus::Status result;

    if (objc != 1) {
	Tcl_WrongNumArgs(interp, 1, objv, "");
	return TCL_ERROR;
    }

    if (GetItclObj(&PenItclObj, objv[0]) != TCL_OK) {
	return TCL_ERROR;
    }
    if (PenHash.Find(PenItclObj, &penPtr) != TCL_OK) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("Gdiplus::Pen instance lost!", -1));
	return TCL_ERROR;
    }

    result = penPtr->ResetTransform();

    if (result != Gdiplus::Ok) {
	SetTclErrorMsg(result);
	return TCL_ERROR;
    }

    return TCL_OK;
}

int
GdiplusAdapter::PenRotateTransformCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    ItclObject *PenItclObj;
    Gdiplus::Pen *penPtr;
    Gdiplus::Status result;
    double angle;

    if (objc != 2) {
	Tcl_WrongNumArgs(interp, 1, objv, "angle");
	return TCL_ERROR;
    }

    if (GetItclObj(&PenItclObj, objv[0]) != TCL_OK) {
	return TCL_ERROR;
    }
    if (PenHash.Find(PenItclObj, &penPtr) != TCL_OK) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("Gdiplus::Pen instance lost!", -1));
	return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[1], &angle) != TCL_OK) {
	return TCL_ERROR;
    }

    result = penPtr->RotateTransform(
	static_cast<Gdiplus::REAL>(angle), Gdiplus::MatrixOrderPrepend);

    if (result != Gdiplus::Ok) {
	SetTclErrorMsg(result);
	return TCL_ERROR;
    }

    return TCL_OK;
}

int
GdiplusAdapter::PenScaleTransformCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    ItclObject *PenItclObj;
    Gdiplus::Pen *penPtr;
    Gdiplus::Status result;
    double sx, sy;

    if (objc != 2) {
	Tcl_WrongNumArgs(interp, 1, objv, "sx sy");
	return TCL_ERROR;
    }

    if (GetItclObj(&PenItclObj, objv[0]) != TCL_OK) {
	return TCL_ERROR;
    }
    if (PenHash.Find(PenItclObj, &penPtr) != TCL_OK) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("Gdiplus::Pen instance lost!", -1));
	return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[1], &sx) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetDoubleFromObj(interp, objv[2], &sy) != TCL_OK) {
	return TCL_ERROR;
    }

    result = penPtr->ScaleTransform(
	    static_cast<Gdiplus::REAL>(sx),
	    static_cast<Gdiplus::REAL>(sy),
	    Gdiplus::MatrixOrderPrepend
	    );

    if (result != Gdiplus::Ok) {
	SetTclErrorMsg(result);
	return TCL_ERROR;
    }

    return TCL_OK;
}

int
GdiplusAdapter::PenAlignmentCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    ItclObject *PenItclObj;
    Gdiplus::Pen *penPtr;
    Gdiplus::Status result;
    Gdiplus::PenAlignment align;

    if (objc > 2) {
	Tcl_WrongNumArgs(interp, 1, objv, "?alignment?");
	return TCL_ERROR;
    }

    if (GetItclObj(&PenItclObj, objv[0]) != TCL_OK) {
	return TCL_ERROR;
    }
    if (PenHash.Find(PenItclObj, &penPtr) != TCL_OK) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("Gdiplus::Pen instance lost!", -1));
	return TCL_ERROR;
    }

    if (objc == 1) {
	align = penPtr->GetAlignment();
	if (PenAlignmentToResult(align) != TCL_OK) {
	    return TCL_ERROR;
	}
    } else {
	if (GetPenAlignmentFromObj(objv[1], &align) != TCL_OK) {
	    return TCL_ERROR;
	}
	result = penPtr->SetAlignment(align);
	if (result != Gdiplus::Ok) {
	    SetTclErrorMsg(result);
	    return TCL_ERROR;
	}
    }

    return TCL_OK;
}

int
GdiplusAdapter::PenBrushCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    return TCL_ERROR;
}

int
GdiplusAdapter::PenColorCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    return TCL_ERROR;
}

int
GdiplusAdapter::PenCompoundArrayCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    return TCL_ERROR;
}

int
GdiplusAdapter::PenCustomEndCapCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    return TCL_ERROR;
}

int
GdiplusAdapter::PenCustomStartCapCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    return TCL_ERROR;
}

int
GdiplusAdapter::PenDashCapCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    return TCL_ERROR;
}

int
GdiplusAdapter::PenDashOffsetCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    return TCL_ERROR;
}

int
GdiplusAdapter::PenDashPatternCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    return TCL_ERROR;
}

int
GdiplusAdapter::PenDashStyleCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    return TCL_ERROR;
}

int
GdiplusAdapter::PenEndCapCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    return TCL_ERROR;
}

int
GdiplusAdapter::PenLineJoinCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    return TCL_ERROR;
}

int
GdiplusAdapter::PenMiterLimitCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    return TCL_ERROR;
}

int
GdiplusAdapter::PenStartCapCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    return TCL_ERROR;
}

int
GdiplusAdapter::PenTransformCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    return TCL_ERROR;
}

int
GdiplusAdapter::PenWidthCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    return TCL_ERROR;
}
