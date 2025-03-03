#include "GdiplusAdapter.hpp"

int
GdiplusAdapter::MatrixConstructCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    ItclObject *MatrixItclObj;
    Gdiplus::Status result;
    Gdiplus::Matrix *matrixPtr;
    double m11, m12, m21, m22, m31, m32;

    if (objc != 7) {
	Tcl_WrongNumArgs(interp, 1, objv, "m11 m12 m21 m22 m31 m32");
	return TCL_ERROR;
    }

    if (GetItclObj(&MatrixItclObj, objv[0]) != TCL_OK) {
	return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[1], &m11) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetDoubleFromObj(interp, objv[2], &m12) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetDoubleFromObj(interp, objv[3], &m21) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetDoubleFromObj(interp, objv[4], &m22) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetDoubleFromObj(interp, objv[5], &m31) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetDoubleFromObj(interp, objv[6], &m32) != TCL_OK) {
	return TCL_ERROR;
    }

    matrixPtr = new Gdiplus::Matrix(
	    static_cast<Gdiplus::REAL>(m11),
	    static_cast<Gdiplus::REAL>(m12),
	    static_cast<Gdiplus::REAL>(m21),
	    static_cast<Gdiplus::REAL>(m22),
	    static_cast<Gdiplus::REAL>(m31),
	    static_cast<Gdiplus::REAL>(m32)
	    );

    if ((result = matrixPtr->GetLastStatus()) != Gdiplus::Ok) {
	delete matrixPtr;
	SetTclErrorMsg(result);
	return TCL_ERROR;
    }
    MatrixHash.Add(MatrixItclObj, matrixPtr);
    return TCL_OK;
}

int
GdiplusAdapter::MatrixDestructCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    ItclObject *MatrixItclObj;
    Gdiplus::Matrix *matrixPtr;

    if (objc != 1) {
	Tcl_WrongNumArgs(interp, 1, objv, "");
	return TCL_ERROR;
    }

    if (GetItclObj(&MatrixItclObj, objv[0]) != TCL_OK) {
	return TCL_ERROR;
    }

    if (MatrixHash.Extract(MatrixItclObj, &matrixPtr) == TCL_OK) {
	delete matrixPtr;
    }
    return TCL_OK;
}

int
GdiplusAdapter::MatrixEqualsCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    return TCL_ERROR;
}

int
GdiplusAdapter::MatrixInvertCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    return TCL_ERROR;
}

int
GdiplusAdapter::MatrixIsIdentityCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    return TCL_ERROR;
}

int
GdiplusAdapter::MatrixIsInvertibleCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    return TCL_ERROR;
}

int
GdiplusAdapter::MatrixMultiplyCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    return TCL_ERROR;
}

int
GdiplusAdapter::MatrixOffsetXCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    return TCL_ERROR;
}

int
GdiplusAdapter::MatrixOffsetYCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    return TCL_ERROR;
}

int
GdiplusAdapter::MatrixRotateCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    return TCL_ERROR;
}

int
GdiplusAdapter::MatrixRotateAtCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    return TCL_ERROR;
}

int
GdiplusAdapter::MatrixScaleCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    return TCL_ERROR;
}

int
GdiplusAdapter::MatrixShearCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    return TCL_ERROR;
}

int
GdiplusAdapter::MatrixTranslateCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    return TCL_ERROR;
}
