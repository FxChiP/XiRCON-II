#include "GdiplusAdapter.hpp"

int
GdiplusAdapter::GraphicsAddMetafileCommentCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    ItclObject *GraphicsItclObj;
    Gdiplus::Graphics *graphicsPtr;
    Gdiplus::Status result;
    char *utf8String;
    int len;

    if (objc > 2) {
	Tcl_WrongNumArgs(interp, 1, objv, "string");
	return TCL_ERROR;
    }

    if (GetItclObj(&GraphicsItclObj, objv[0]) != TCL_OK) {
	return TCL_ERROR;
    }

    if (GraphicsHash.Find(GraphicsItclObj, &graphicsPtr) != TCL_OK) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("Gdiplus::Graphics instance lost!", -1));
	return TCL_ERROR;
    }

    utf8String = Tcl_GetStringFromObj(objv[1], &len);

    result = graphicsPtr->AddMetafileComment(
	    reinterpret_cast<BYTE *>(utf8String), len);

    if (result != Gdiplus::Ok) {
	SetTclErrorMsg(result);
	return TCL_ERROR;
    }

    return TCL_OK;
}

int
GdiplusAdapter::GraphicsBeginContainerCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    ItclObject *GraphicsItclObj;
    Gdiplus::Graphics *graphicsPtr;
    Gdiplus::GraphicsContainer result;

    if (objc > 1) {
	Tcl_WrongNumArgs(interp, 1, objv, "");
	return TCL_ERROR;
    }

    if (GetItclObj(&GraphicsItclObj, objv[0]) != TCL_OK) {
	return TCL_ERROR;
    }

    if (GraphicsHash.Find(GraphicsItclObj, &graphicsPtr) != TCL_OK) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("Gdiplus::Graphics instance lost!", -1));
	return TCL_ERROR;
    }

    result = graphicsPtr->BeginContainer();
    Tcl_SetObjResult(interp, Tcl_NewLongObj(result));

    return TCL_OK;
}

int
GdiplusAdapter::GraphicsClearCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    ItclObject *GraphicsItclObj;
    Gdiplus::Graphics *graphicsPtr;
    Gdiplus::Status result;
    Gdiplus::ARGB color = Gdiplus::Color::Black;

    if (objc > 2) {
	Tcl_WrongNumArgs(interp, 1, objv, "?color?");
	return TCL_ERROR;
    }

    if (GetItclObj(&GraphicsItclObj, objv[0]) != TCL_OK) {
	return TCL_ERROR;
    }

    if (GraphicsHash.Find(GraphicsItclObj, &graphicsPtr) != TCL_OK) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("Gdiplus::Graphics instance lost!", -1));
	return TCL_ERROR;
    }

    if (objc == 2 && MakeARGB(objv[1], &color) != TCL_OK) {
	return TCL_ERROR;
    }

    result = graphicsPtr->Clear(
	    Gdiplus::Color(color)
	    );

    if (result != Gdiplus::Ok) {
	SetTclErrorMsg(result);
	return TCL_ERROR;
    }

    return TCL_OK;
}

int
GdiplusAdapter::GraphicsDrawArcCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    ItclObject *GraphicsItclObj;
    ItclObject *PenItclObj;
    Gdiplus::Graphics *graphicsPtr;
    Gdiplus::Pen *penPtr;
    Gdiplus::Status result;
    double x, y, width, height, startAngle, sweepAngle;

    if (objc != 8) {
	Tcl_WrongNumArgs(interp, 1, objv, "pen x y width height startAngle sweepAngle");
	return TCL_ERROR;
    }

    if (GetItclObj(&GraphicsItclObj, objv[0]) != TCL_OK) {
	return TCL_ERROR;
    }

    if (GraphicsHash.Find(GraphicsItclObj, &graphicsPtr) != TCL_OK) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("Gdiplus::Graphics instance lost!", -1));
	return TCL_ERROR;
    }

    // Dissect and disseminate all the arguments.

    // look for the pen ItclObject* from the commandname
    Itcl_FindObject(interp, Tcl_GetString(objv[1]), &PenItclObj);
    if (PenItclObj == 0L) {
	Tcl_AppendStringsToObj(Tcl_GetObjResult(interp), "\"",
		Tcl_GetString(objv[1]), "\"",
		" was not a Gdiplus::Pen instance.", 0L);
	return TCL_ERROR;
    }

    // Get the Gdiplus::Pen* from the Itcl object context.
    if (PenHash.Find(PenItclObj, &penPtr) != TCL_OK) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("Gdiplus::Pen instance lost!", -1));
	return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &x) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetDoubleFromObj(interp, objv[3], &y) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetDoubleFromObj(interp, objv[4], &width) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetDoubleFromObj(interp, objv[5], &height) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetDoubleFromObj(interp, objv[6], &startAngle) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetDoubleFromObj(interp, objv[7], &sweepAngle) != TCL_OK) {
	return TCL_ERROR;
    }

    result = graphicsPtr->DrawArc(
	    penPtr,
	    Gdiplus::RectF(
		static_cast<Gdiplus::REAL>(x),
		static_cast<Gdiplus::REAL>(y),
		static_cast<Gdiplus::REAL>(width),
		static_cast<Gdiplus::REAL>(height)
		),
	    static_cast<Gdiplus::REAL>(startAngle),
	    static_cast<Gdiplus::REAL>(sweepAngle)
	    );

    if (result != Gdiplus::Ok) {
	SetTclErrorMsg(result);
	return TCL_ERROR;
    }

    return TCL_OK;
}

int
GdiplusAdapter::GraphicsDrawBezierCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    ItclObject *GraphicsItclObj;
    ItclObject *PenItclObj;
    Gdiplus::Graphics *graphicsPtr;
    Gdiplus::Pen *penPtr;
    Gdiplus::Status result;
    double x1, y1, x2, y2, x3, y3, x4, y4;

    if (objc != 10) {
	Tcl_WrongNumArgs(interp, 1, objv, "pen x1 y1 x2 y2 x3 y3 x4 y4");
	return TCL_ERROR;
    }

    if (GetItclObj(&GraphicsItclObj, objv[0]) != TCL_OK) {
	return TCL_ERROR;
    }

    if (GraphicsHash.Find(GraphicsItclObj, &graphicsPtr) != TCL_OK) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("Gdiplus::Graphics instance lost!", -1));
	return TCL_ERROR;
    }

    // Dissect and disseminate all the arguments.

    // look for the pen ItclObject* from the commandname
    Itcl_FindObject(interp, Tcl_GetString(objv[1]), &PenItclObj);
    if (PenItclObj == 0L) {
	Tcl_AppendStringsToObj(Tcl_GetObjResult(interp), "\"",
		Tcl_GetString(objv[1]), "\"",
		" was not a Gdiplus::Pen instance.", 0L);
	return TCL_ERROR;
    }

    // Get the Gdiplus::Pen* from the Itcl object context.
    if (PenHash.Find(PenItclObj, &penPtr) != TCL_OK) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("Gdiplus::Pen instance lost!", -1));
	return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &x1) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetDoubleFromObj(interp, objv[3], &y1) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetDoubleFromObj(interp, objv[4], &x2) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetDoubleFromObj(interp, objv[5], &y2) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetDoubleFromObj(interp, objv[6], &x3) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetDoubleFromObj(interp, objv[7], &y3) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetDoubleFromObj(interp, objv[8], &x4) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetDoubleFromObj(interp, objv[9], &y4) != TCL_OK) {
	return TCL_ERROR;
    }

    result = graphicsPtr->DrawBezier(
	    penPtr,
	    static_cast<Gdiplus::REAL>(x1),
	    static_cast<Gdiplus::REAL>(y1),
	    static_cast<Gdiplus::REAL>(x2),
	    static_cast<Gdiplus::REAL>(y2),
	    static_cast<Gdiplus::REAL>(x3),
	    static_cast<Gdiplus::REAL>(y3),
	    static_cast<Gdiplus::REAL>(x4),
	    static_cast<Gdiplus::REAL>(y4)
	    );

    if (result != Gdiplus::Ok) {
	SetTclErrorMsg(result);
	return TCL_ERROR;
    }

    return TCL_OK;
}

int
GdiplusAdapter::GraphicsDrawCachedBitmapCmd (int objc, struct Tcl_Obj * CONST objv[])
{
//    Status DrawCachedBitmap(IN CachedBitmap *cb,
//                            IN INT x,
//                            IN INT y)
    return TCL_ERROR;
}

int
GdiplusAdapter::GraphicsDrawClosedCurveCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    ItclObject *GraphicsItclObj;
    ItclObject *PenItclObj;
    Gdiplus::Graphics *graphicsPtr;
    Gdiplus::Pen *penPtr;
    Gdiplus::Status result;
    Gdiplus::PointF *PointFArray;
    double x, y, tension;
    int i, j;

    if (objc < 5 || (objc % 2) != 1) {
	Tcl_WrongNumArgs(interp, 1, objv, "pen tension x1 y1 ?x2 y2? ... ?xN yN?");
	return TCL_ERROR;
    }

    if (GetItclObj(&GraphicsItclObj, objv[0]) != TCL_OK) {
	return TCL_ERROR;
    }

    if (GraphicsHash.Find(GraphicsItclObj, &graphicsPtr) != TCL_OK) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("Gdiplus::Graphics instance lost!", -1));
	return TCL_ERROR;
    }

    // Dissect and disseminate all the arguments.

    // look for the pen ItclObject* from the commandname
    Itcl_FindObject(interp, Tcl_GetString(objv[1]), &PenItclObj);
    if (PenItclObj == 0L) {
	Tcl_AppendStringsToObj(Tcl_GetObjResult(interp), "\"",
		Tcl_GetString(objv[1]), "\"",
		" was not a Gdiplus::Pen instance.", 0L);
	return TCL_ERROR;
    }

    // Get the Gdiplus::Pen* from the Itcl object context.
    if (PenHash.Find(PenItclObj, &penPtr) != TCL_OK) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("Gdiplus::Pen instance lost!", -1));
	return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &tension) != TCL_OK) {
	return TCL_ERROR;
    }

    PointFArray = new Gdiplus::PointF [(objc-3)/2];
    for (i=3, j=0; i < objc; i+=2, j++) {
	if (Tcl_GetDoubleFromObj(interp, objv[i], &x) != TCL_OK) {
	    return TCL_ERROR;
	}
	if (Tcl_GetDoubleFromObj(interp, objv[i+1], &y) != TCL_OK) {
	    return TCL_ERROR;
	}
	PointFArray[j].X = static_cast<Gdiplus::REAL>(x);
	PointFArray[j].Y = static_cast<Gdiplus::REAL>(y);
    }

    result = graphicsPtr->DrawClosedCurve(
	    penPtr,
	    PointFArray,
	    j,
	    static_cast<Gdiplus::REAL>(tension)
	    );

    delete [] PointFArray;

    if (result != Gdiplus::Ok) {
	SetTclErrorMsg(result);
	return TCL_ERROR;
    }

    return TCL_OK;
}

int
GdiplusAdapter::GraphicsDrawCurveCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    ItclObject *GraphicsItclObj;
    ItclObject *PenItclObj;
    Gdiplus::Graphics *graphicsPtr;
    Gdiplus::Pen *penPtr;
    Gdiplus::Status result;
    Gdiplus::PointF *PointFArray;
    double x, y, tension = 0.5f;
    int i, j, offset, numSegments;

    if (objc < 7 || (objc % 2) != 1) {
	Tcl_WrongNumArgs(interp, 1, objv, "pen offset numSegments tension x1 y1 ?x2 y2? ... ?xN yN?");
	return TCL_ERROR;
    }

    if (GetItclObj(&GraphicsItclObj, objv[0]) != TCL_OK) {
	return TCL_ERROR;
    }

    if (GraphicsHash.Find(GraphicsItclObj, &graphicsPtr) != TCL_OK) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("Gdiplus::Graphics instance lost!", -1));
	return TCL_ERROR;
    }

    // Dissect and disseminate all the arguments.

    // look for the pen ItclObject* from the commandname
    Itcl_FindObject(interp, Tcl_GetString(objv[1]), &PenItclObj);
    if (PenItclObj == 0L) {
	Tcl_AppendStringsToObj(Tcl_GetObjResult(interp), "\"",
		Tcl_GetString(objv[1]), "\"",
		" was not a Gdiplus::Pen instance.", 0L);
	return TCL_ERROR;
    }

    // Get the Gdiplus::Pen* from the Itcl object context.
    if (PenHash.Find(PenItclObj, &penPtr) != TCL_OK) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("Gdiplus::Pen instance lost!", -1));
	return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[2], &offset) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetIntFromObj(interp, objv[3], &numSegments) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetDoubleFromObj(interp, objv[4], &tension) != TCL_OK) {
	return TCL_ERROR;
    }

    PointFArray = new Gdiplus::PointF [(objc-5)/2];
    for (i=5, j=0; i < objc; i+=2, j++) {
	if (Tcl_GetDoubleFromObj(interp, objv[i], &x) != TCL_OK) {
	    return TCL_ERROR;
	}
	if (Tcl_GetDoubleFromObj(interp, objv[i+1], &y) != TCL_OK) {
	    return TCL_ERROR;
	}
	PointFArray[j].X = static_cast<Gdiplus::REAL>(x);
	PointFArray[j].Y = static_cast<Gdiplus::REAL>(y);
    }

    result = graphicsPtr->DrawCurve(
	    penPtr,
	    PointFArray,
	    j,
	    offset,
	    numSegments,
	    static_cast<Gdiplus::REAL>(tension)
	    );

    delete [] PointFArray;

    if (result != Gdiplus::Ok) {
	SetTclErrorMsg(result);
	return TCL_ERROR;
    }

    return TCL_OK;
}

int
GdiplusAdapter::GraphicsDrawDriverStringCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    ItclObject *GraphicsItclObj, *FontItclObj, *BrushItclObj, *MatrixItclObj;
    Gdiplus::Graphics *graphicsPtr;
    Gdiplus::Font *fontPtr;
    Gdiplus::Brush *brushPtr;
    Gdiplus::Matrix *matrixPtr;
    Gdiplus::Status result;
    Tcl_UniChar* uniString;
    int uniLen;
    double x, y;

    if (objc != 7) {
	Tcl_WrongNumArgs(interp, 1, objv, "text font brush x y matrix");
	return TCL_ERROR;
    }

    if (GetItclObj(&GraphicsItclObj, objv[0]) != TCL_OK) {
	return TCL_ERROR;
    }

    if (GraphicsHash.Find(GraphicsItclObj, &graphicsPtr) != TCL_OK) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("Gdiplus::Graphics instance lost!", -1));
	return TCL_ERROR;
    }

    uniString = Tcl_GetUnicode(objv[1]);
    uniLen = Tcl_GetCharLength(objv[1]);

    // look for the font ItclObject* from the commandname
    Itcl_FindObject(interp, Tcl_GetString(objv[2]), &FontItclObj);
    if (FontItclObj == 0L) {
	Tcl_AppendStringsToObj(Tcl_GetObjResult(interp), "\"",
		Tcl_GetString(objv[2]), "\"",
		" was not a Gdiplus::Font instance.", 0L);
	return TCL_ERROR;
    }

    // Get the Gdiplus::Font* from the Itcl object context.
    if (FontHash.Find(FontItclObj, &fontPtr) != TCL_OK) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("Gdiplus::Font instance lost!", -1));
	return TCL_ERROR;
    }

    // look for the font ItclObject* from the commandname
    Itcl_FindObject(interp, Tcl_GetString(objv[3]), &BrushItclObj);
    if (BrushItclObj == 0L) {
	Tcl_AppendStringsToObj(Tcl_GetObjResult(interp), "\"",
		Tcl_GetString(objv[3]), "\"",
		" was not a Gdiplus::Brush instance.", 0L);
	return TCL_ERROR;
    }

    // Get the Gdiplus::Brush* from the Itcl object context.
    if (BrushHash.Find(BrushItclObj, &brushPtr) != TCL_OK) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("Gdiplus::Brush instance lost!", -1));
	return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[4], &x) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetDoubleFromObj(interp, objv[5], &y) != TCL_OK) {
	return TCL_ERROR;
    }

    Gdiplus::PointF point(
		static_cast<Gdiplus::REAL>(x),
		static_cast<Gdiplus::REAL>(y)
		);

    // look for the matrix ItclObject* from the commandname
    Itcl_FindObject(interp, Tcl_GetString(objv[6]), &MatrixItclObj);
    if (MatrixItclObj == 0L) {
	Tcl_AppendStringsToObj(Tcl_GetObjResult(interp), "\"",
		Tcl_GetString(objv[6]), "\"",
		" was not a Gdiplus::Matrix instance.", 0L);
	return TCL_ERROR;
    }

    // Get the Gdiplus::Matrix* from the Itcl object context.
    if (MatrixHash.Find(MatrixItclObj, &matrixPtr) != TCL_OK) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("Gdiplus::Matrix instance lost!", -1));
	return TCL_ERROR;
    }

    result = graphicsPtr->DrawDriverString(
	    uniString,
	    uniLen,
	    fontPtr,
	    brushPtr,
	    &point,
	    Gdiplus::DriverStringOptionsCmapLookup,
	    matrixPtr
	    );

    if (result != Gdiplus::Ok) {
	SetTclErrorMsg(result);
	return TCL_ERROR;
    }

    return TCL_OK;
}

int
GdiplusAdapter::GraphicsDrawEllipseCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    ItclObject *GraphicsItclObj;
    ItclObject *PenItclObj;
    Gdiplus::Graphics *graphicsPtr;
    Gdiplus::Pen *penPtr;
    Gdiplus::Status result;
    double x, y, width, height;

    if (objc != 6) {
	Tcl_WrongNumArgs(interp, 1, objv, "pen x y width height");
	return TCL_ERROR;
    }

    if (GetItclObj(&GraphicsItclObj, objv[0]) != TCL_OK) {
	return TCL_ERROR;
    }

    if (GraphicsHash.Find(GraphicsItclObj, &graphicsPtr) != TCL_OK) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("Gdiplus::Graphics instance lost!", -1));
	return TCL_ERROR;
    }

    // Dissect and disseminate all the arguments.

    // look for the pen ItclObject* from the commandname
    Itcl_FindObject(interp, Tcl_GetString(objv[1]), &PenItclObj);
    if (PenItclObj == 0L) {
	Tcl_AppendStringsToObj(Tcl_GetObjResult(interp), "\"",
		Tcl_GetString(objv[1]), "\"",
		" was not a Gdiplus::Pen instance.", 0L);
	return TCL_ERROR;
    }

    // Get the Gdiplus::Pen* from the Itcl object context.
    if (PenHash.Find(PenItclObj, &penPtr) != TCL_OK) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("Gdiplus::Pen instance lost!", -1));
	return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &x) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetDoubleFromObj(interp, objv[3], &y) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetDoubleFromObj(interp, objv[4], &width) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetDoubleFromObj(interp, objv[5], &height) != TCL_OK) {
	return TCL_ERROR;
    }

    // Gdiplus::REAL is really a float.

    result = graphicsPtr->DrawEllipse(
	    penPtr,
	    Gdiplus::RectF(
		static_cast<Gdiplus::REAL>(x),
		static_cast<Gdiplus::REAL>(y),
		static_cast<Gdiplus::REAL>(width),
		static_cast<Gdiplus::REAL>(height)
		)
	    );

    if (result != Gdiplus::Ok) {
	SetTclErrorMsg(result);
	return TCL_ERROR;
    }

    return TCL_OK;
}

int
GdiplusAdapter::GraphicsDrawImageCmd (int objc, struct Tcl_Obj * CONST objv[])
{
//    Status DrawImage(IN Image* image,
//                     IN const PointF& point)
    return TCL_ERROR;
}

int
GdiplusAdapter::GraphicsDrawLineCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    ItclObject *GraphicsItclObj;
    ItclObject *PenItclObj;
    Gdiplus::Graphics *graphicsPtr;
    Gdiplus::Pen *penPtr;
    Gdiplus::Status result;
    double x1, y1, x2, y2;

    if (objc != 6) {
	Tcl_WrongNumArgs(interp, 1, objv, "pen x1 y1 x2 y2");
	return TCL_ERROR;
    }

    if (GetItclObj(&GraphicsItclObj, objv[0]) != TCL_OK) {
	return TCL_ERROR;
    }

    if (GraphicsHash.Find(GraphicsItclObj, &graphicsPtr) != TCL_OK) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("Gdiplus::Graphics instance lost!", -1));
	return TCL_ERROR;
    }

    // Dissect and disseminate all the arguments.

    // look for the pen ItclObject* from the commandname
    Itcl_FindObject(interp, Tcl_GetString(objv[1]), &PenItclObj);
    if (PenItclObj == 0L) {
	Tcl_AppendStringsToObj(Tcl_GetObjResult(interp), "\"",
		Tcl_GetString(objv[1]), "\"",
		" was not a Gdiplus::Pen instance.", 0L);
	return TCL_ERROR;
    }

    // Get the Gdiplus::Pen* from the Itcl object context.
    if (PenHash.Find(PenItclObj, &penPtr) != TCL_OK) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("Gdiplus::Pen instance lost!", -1));
	return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &x1) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetDoubleFromObj(interp, objv[3], &y1) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetDoubleFromObj(interp, objv[4], &x2) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetDoubleFromObj(interp, objv[5], &y2) != TCL_OK) {
	return TCL_ERROR;
    }

    result = graphicsPtr->DrawLine(
	    penPtr,
	    static_cast<Gdiplus::REAL>(x1),
	    static_cast<Gdiplus::REAL>(y1),
	    static_cast<Gdiplus::REAL>(x2),
	    static_cast<Gdiplus::REAL>(y2)
	    );

    if (result != Gdiplus::Ok) {
	SetTclErrorMsg(result);
	return TCL_ERROR;
    }

    return TCL_OK;
}

int
GdiplusAdapter::GraphicsDrawPathCmd (int objc, struct Tcl_Obj * CONST objv[])
{
//    Status DrawPath(IN const Pen* pen,
//                    IN const GraphicsPath* path)
    return TCL_ERROR;
}

int
GdiplusAdapter::GraphicsDrawPieCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    ItclObject *GraphicsItclObj;
    ItclObject *PenItclObj;
    Gdiplus::Graphics *graphicsPtr;
    Gdiplus::Pen *penPtr;
    Gdiplus::Status result;
    double x, y, width, height, startAngle, sweepAngle;

    if (objc != 8) {
	Tcl_WrongNumArgs(interp, 1, objv,
		"pen x y width height startAngle sweepAngle");
	return TCL_ERROR;
    }

    if (GetItclObj(&GraphicsItclObj, objv[0]) != TCL_OK) {
	return TCL_ERROR;
    }

    if (GraphicsHash.Find(GraphicsItclObj, &graphicsPtr) != TCL_OK) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("Gdiplus::Graphics instance lost!", -1));
	return TCL_ERROR;
    }

    // Dissect and disseminate all the arguments.

    // look for the pen ItclObject* from the commandname
    Itcl_FindObject(interp, Tcl_GetString(objv[1]), &PenItclObj);
    if (PenItclObj == 0L) {
	Tcl_AppendStringsToObj(Tcl_GetObjResult(interp), "\"",
		Tcl_GetString(objv[1]), "\"",
		" was not a Gdiplus::Pen instance.", 0L);
	return TCL_ERROR;
    }

    // Get the Gdiplus::Pen* from the Itcl object context.
    if (PenHash.Find(PenItclObj, &penPtr) != TCL_OK) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("Gdiplus::Pen instance lost!", -1));
	return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &x) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetDoubleFromObj(interp, objv[3], &y) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetDoubleFromObj(interp, objv[4], &width) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetDoubleFromObj(interp, objv[5], &height) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetDoubleFromObj(interp, objv[6], &startAngle) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetDoubleFromObj(interp, objv[7], &sweepAngle) != TCL_OK) {
	return TCL_ERROR;
    }

    // Gdiplus::REAL is really a float.  downgrade the double (with
    // possible loss) to a float.

    result = graphicsPtr->DrawPie(
	    penPtr,
	    Gdiplus::RectF(
		static_cast<Gdiplus::REAL>(x),
		static_cast<Gdiplus::REAL>(y),
		static_cast<Gdiplus::REAL>(width),
		static_cast<Gdiplus::REAL>(height)
		),
	    static_cast<Gdiplus::REAL>(startAngle),
	    static_cast<Gdiplus::REAL>(sweepAngle)
	    );

    if (result != Gdiplus::Ok) {
	SetTclErrorMsg(result);
	return TCL_ERROR;
    }

    return TCL_OK;
}

int
GdiplusAdapter::GraphicsDrawPolygonCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    ItclObject *GraphicsItclObj;
    ItclObject *PenItclObj;
    Gdiplus::Graphics *graphicsPtr;
    Gdiplus::Pen *penPtr;
    Gdiplus::Status result;
    Gdiplus::PointF *PointFArray;
    double x, y;
    int i, j;

    if (objc < 4 || (objc % 2) != 0) {
	Tcl_WrongNumArgs(interp, 1, objv, "pen x1 y1 ?x2 y2? ... ?xN yN?");
	return TCL_ERROR;
    }

    if (GetItclObj(&GraphicsItclObj, objv[0]) != TCL_OK) {
	return TCL_ERROR;
    }

    if (GraphicsHash.Find(GraphicsItclObj, &graphicsPtr) != TCL_OK) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("Gdiplus::Graphics instance lost!", -1));
	return TCL_ERROR;
    }

    // Dissect and disseminate all the arguments.

    // look for the pen ItclObject* from the commandname
    Itcl_FindObject(interp, Tcl_GetString(objv[1]), &PenItclObj);
    if (PenItclObj == 0L) {
	Tcl_AppendStringsToObj(Tcl_GetObjResult(interp), "\"",
		Tcl_GetString(objv[1]), "\"",
		" was not a Gdiplus::Pen instance.", 0L);
	return TCL_ERROR;
    }

    // Get the Gdiplus::Pen* from the Itcl object context.
    if (PenHash.Find(PenItclObj, &penPtr) != TCL_OK) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("Gdiplus::Pen instance lost!", -1));
	return TCL_ERROR;
    }

    PointFArray = new Gdiplus::PointF [(objc-2)/2];
    for (i=2, j=0; i < objc; i+=2, j++) {
	if (Tcl_GetDoubleFromObj(interp, objv[i], &x) != TCL_OK) {
	    return TCL_ERROR;
	}
	if (Tcl_GetDoubleFromObj(interp, objv[i+1], &y) != TCL_OK) {
	    return TCL_ERROR;
	}
	PointFArray[j].X = static_cast<Gdiplus::REAL>(x);
	PointFArray[j].Y = static_cast<Gdiplus::REAL>(y);
    }

    // Gdiplus::REAL is really a float.  downgrade the double (with
    // possible loss) to a float.

    result = graphicsPtr->DrawPolygon(
	    penPtr,
	    PointFArray,
	    j
	    );

    delete [] PointFArray;

    if (result != Gdiplus::Ok) {
	SetTclErrorMsg(result);
	return TCL_ERROR;
    }

    return TCL_OK;
}

int
GdiplusAdapter::GraphicsDrawPolygonListCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    ItclObject *GraphicsItclObj;
    ItclObject *PenItclObj;
    Gdiplus::Graphics *graphicsPtr;
    Gdiplus::Pen *penPtr;
    Gdiplus::Status result;
    Gdiplus::PointF *PointFArray;
    double x, y;
    int i, j;

    if (objc < 3 || objc > 3 != 0) {
	Tcl_WrongNumArgs(interp, 1, objv, "pen pointList");
	return TCL_ERROR;
    }

    if (GetItclObj(&GraphicsItclObj, objv[0]) != TCL_OK) {
	return TCL_ERROR;
    }

    if (GraphicsHash.Find(GraphicsItclObj, &graphicsPtr) != TCL_OK) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("Gdiplus::Graphics instance lost!", -1));
	return TCL_ERROR;
    }

    // Dissect and disseminate all the arguments.

    // look for the pen ItclObject* from the commandname
    Itcl_FindObject(interp, Tcl_GetString(objv[1]), &PenItclObj);
    if (PenItclObj == 0L) {
	Tcl_AppendStringsToObj(Tcl_GetObjResult(interp), "\"",
		Tcl_GetString(objv[1]), "\"",
		" was not a Gdiplus::Pen instance.", 0L);
	return TCL_ERROR;
    }

    // Get the Gdiplus::Pen* from the Itcl object context.
    if (PenHash.Find(PenItclObj, &penPtr) != TCL_OK) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("Gdiplus::Pen instance lost!", -1));
	return TCL_ERROR;
    }

    Tcl_ListObjLength(0L, objv[2], &j);
    PointFArray = new Gdiplus::PointF [j];
    for (i=0; i < j; i++) {
	Tcl_Obj *ox, *oy, *points;

	Tcl_ListObjIndex(0L, objv[2], i, &points);
	Tcl_ListObjIndex(0L, points, 0, &ox);
	if (Tcl_GetDoubleFromObj(interp, ox, &x) != TCL_OK) {
	    return TCL_ERROR;
	}
	Tcl_ListObjIndex(0L, points, 1, &oy);
	if (Tcl_GetDoubleFromObj(interp, oy, &y) != TCL_OK) {
	    return TCL_ERROR;
	}
	PointFArray[i].X = static_cast<Gdiplus::REAL>(x);
	PointFArray[i].Y = static_cast<Gdiplus::REAL>(y);
    }

    // Gdiplus::REAL is really a float.  downgrade the double (with
    // possible loss) to a float.

    result = graphicsPtr->DrawPolygon(
	    penPtr,
	    PointFArray,
	    j
	    );

    delete [] PointFArray;

    if (result != Gdiplus::Ok) {
	SetTclErrorMsg(result);
	return TCL_ERROR;
    }

    return TCL_OK;
}

int
GdiplusAdapter::GraphicsDrawRectangleCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    ItclObject *GraphicsItclObj;
    ItclObject *PenItclObj;
    Gdiplus::Graphics *graphicsPtr;
    Gdiplus::Pen *penPtr;
    Gdiplus::Status result;
    double x, y, width, height;

    if (objc != 6) {
	Tcl_WrongNumArgs(interp, 1, objv, "pen x y width height");
	return TCL_ERROR;
    }

    if (GetItclObj(&GraphicsItclObj, objv[0]) != TCL_OK) {
	return TCL_ERROR;
    }

    if (GraphicsHash.Find(GraphicsItclObj, &graphicsPtr) != TCL_OK) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("Gdiplus::Graphics instance lost!", -1));
	return TCL_ERROR;
    }

    // Dissect and disseminate all the arguments.

    // look for the pen ItclObject* from the commandname
    Itcl_FindObject(interp, Tcl_GetString(objv[1]), &PenItclObj);
    if (PenItclObj == 0L) {
	Tcl_AppendStringsToObj(Tcl_GetObjResult(interp), "\"",
		Tcl_GetString(objv[1]), "\"",
		" was not a Gdiplus::Pen instance.", 0L);
	return TCL_ERROR;
    }

    // Get the Gdiplus::Pen* from the Itcl object context.
    if (PenHash.Find(PenItclObj, &penPtr) != TCL_OK) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("Gdiplus::Pen instance lost!", -1));
	return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &x) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetDoubleFromObj(interp, objv[3], &y) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetDoubleFromObj(interp, objv[4], &width) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetDoubleFromObj(interp, objv[5], &height) != TCL_OK) {
	return TCL_ERROR;
    }

    // Gdiplus::REAL is really a float.

    result = graphicsPtr->DrawRectangle(
	    penPtr,
	    Gdiplus::RectF(
		static_cast<Gdiplus::REAL>(x),
		static_cast<Gdiplus::REAL>(y),
		static_cast<Gdiplus::REAL>(width),
		static_cast<Gdiplus::REAL>(height)
		)
	    );

    if (result != Gdiplus::Ok) {
	SetTclErrorMsg(result);
	return TCL_ERROR;
    }

    return TCL_OK;
}

int
GdiplusAdapter::GraphicsDrawStringCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    ItclObject *GraphicsItclObj;
    ItclObject *FontItclObj;
    ItclObject *BrushItclObj;
    Gdiplus::Graphics *graphicsPtr;
    Gdiplus::Font *fontPtr;
    Gdiplus::Brush *brushPtr;
    Gdiplus::Status result;
    Tcl_UniChar *string;
    int length;
    double x, y;

    if (objc != 6) {
	Tcl_WrongNumArgs(interp, 1, objv, "string font x y brush");
	return TCL_ERROR;
    }

    if (GetItclObj(&GraphicsItclObj, objv[0]) != TCL_OK) {
	return TCL_ERROR;
    }

    if (GraphicsHash.Find(GraphicsItclObj, &graphicsPtr) != TCL_OK) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("Gdiplus::Graphics instance lost!", -1));
	return TCL_ERROR;
    }

    string = Tcl_GetUnicode(objv[1]);
    length = Tcl_GetCharLength(objv[1]);
    
    // look for the font ItclObject* from the commandname
    Itcl_FindObject(interp, Tcl_GetString(objv[2]), &FontItclObj);
    if (FontItclObj == 0L) {
	Tcl_AppendStringsToObj(Tcl_GetObjResult(interp), "\"",
		Tcl_GetString(objv[2]), "\"",
		" was not a Gdiplus::Font instance.", 0L);
	return TCL_ERROR;
    }

    // Get the Gdiplus::Font* from the Itcl object context.
    if (FontHash.Find(FontItclObj, &fontPtr) != TCL_OK) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("Gdiplus::Font instance lost!", -1));
	return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[3], &x) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetDoubleFromObj(interp, objv[4], &y) != TCL_OK) {
	return TCL_ERROR;
    }

    // look for the font ItclObject* from the commandname
    Itcl_FindObject(interp, Tcl_GetString(objv[5]), &BrushItclObj);
    if (BrushItclObj == 0L) {
	Tcl_AppendStringsToObj(Tcl_GetObjResult(interp), "\"",
		Tcl_GetString(objv[5]), "\"",
		" was not a Gdiplus::Brush instance.", 0L);
	return TCL_ERROR;
    }

    // Get the Gdiplus::Brush* from the Itcl object context.
    if (BrushHash.Find(BrushItclObj, &brushPtr) != TCL_OK) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("Gdiplus::Brush instance lost!", -1));
	return TCL_ERROR;
    }

    // Create a generic StringFormat object.
    const Gdiplus::StringFormat* pStringFormat =
	    Gdiplus::StringFormat::GenericDefault();

    result = graphicsPtr->DrawString(
	    string,
	    length,
	    fontPtr,
	    Gdiplus::PointF(
		static_cast<Gdiplus::REAL>(x),
		static_cast<Gdiplus::REAL>(y)
		),
	    pStringFormat,
	    brushPtr
	    );

    delete pStringFormat;

    if (result != Gdiplus::Ok) {
	SetTclErrorMsg(result);
	return TCL_ERROR;
    }

    return TCL_OK;
}

int
GdiplusAdapter::GraphicsEndContainerCmd (int objc, struct Tcl_Obj * CONST objv[])
{
//    Status EndContainer(IN GraphicsContainer state)
    return TCL_ERROR;
}

int
GdiplusAdapter::GraphicsEnumerateMetafileCmd (int objc, struct Tcl_Obj * CONST objv[])
{
//    Status
//    EnumerateMetafile(
//        IN const Metafile *        metafile,
//        IN const PointF &          destPoint,
//        IN EnumerateMetafileProc   callback,
//        IN VOID *                  callbackData    = NULL,
//        IN const ImageAttributes *       imageAttributes = NULL
//        )
    return TCL_ERROR;
}

int
GdiplusAdapter::GraphicsExcludeClipCmd (int objc, struct Tcl_Obj * CONST objv[])
{
//    Status ExcludeClip(IN const RectF& rect)
    return TCL_ERROR;
}

int
GdiplusAdapter::GraphicsFillClosedCurveCmd (int objc, struct Tcl_Obj * CONST objv[])
{
//    Status FillClosedCurve(IN const Brush* brush,
//                           IN const Point* points,
//                           IN INT count,
//                           IN FillMode fillMode,
//                           IN REAL tension = 0.5f)
    return TCL_ERROR;
}

int
GdiplusAdapter::GraphicsFillEllipseCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    ItclObject *GraphicsItclObj;
    ItclObject *BrushItclObj;
    Gdiplus::Graphics *graphicsPtr;
    Gdiplus::Brush *brushPtr;
    Gdiplus::Status result;
    double x, y, width, height;

    if (objc != 6) {
	Tcl_WrongNumArgs(interp, 1, objv, "brush x y width height");
	return TCL_ERROR;
    }

    if (GetItclObj(&GraphicsItclObj, objv[0]) != TCL_OK) {
	return TCL_ERROR;
    }

    if (GraphicsHash.Find(GraphicsItclObj, &graphicsPtr) != TCL_OK) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("Gdiplus::Graphics instance lost!", -1));
	return TCL_ERROR;
    }

    // Dissect and disseminate all the arguments.

    // look for the brush ItclObject* from the commandname
    Itcl_FindObject(interp, Tcl_GetString(objv[1]), &BrushItclObj);
    if (BrushItclObj == 0L) {
	Tcl_AppendStringsToObj(Tcl_GetObjResult(interp), "\"",
		Tcl_GetString(objv[1]), "\"",
		" was not a Gdiplus::Brush instance.", 0L);
	return TCL_ERROR;
    }

    // Get the Gdiplus::Brush* from the Itcl object context.
    if (BrushHash.Find(BrushItclObj, &brushPtr) != TCL_OK) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("Gdiplus::Brush instance lost!", -1));
	return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &x) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetDoubleFromObj(interp, objv[3], &y) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetDoubleFromObj(interp, objv[4], &width) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetDoubleFromObj(interp, objv[5], &height) != TCL_OK) {
	return TCL_ERROR;
    }

    // Gdiplus::REAL is really a float.  downgrade the double (with
    // possible loss) to a float.

    result = graphicsPtr->FillEllipse(
	    brushPtr,
	    Gdiplus::RectF(
		static_cast<Gdiplus::REAL>(x),
		static_cast<Gdiplus::REAL>(y),
		static_cast<Gdiplus::REAL>(width),
		static_cast<Gdiplus::REAL>(height)
		)
	    );

    if (result != Gdiplus::Ok) {
	SetTclErrorMsg(result);
	return TCL_ERROR;
    }

    return TCL_OK;
}

int
GdiplusAdapter::GraphicsFillPathCmd (int objc, struct Tcl_Obj * CONST objv[])
{
//    Status FillPath(IN const Brush* brush,
//                    IN const GraphicsPath* path)
    return TCL_ERROR;
}

int
GdiplusAdapter::GraphicsFillPieCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    ItclObject *GraphicsItclObj;
    ItclObject *BrushItclObj;
    Gdiplus::Graphics *graphicsPtr;
    Gdiplus::Brush *brushPtr;
    Gdiplus::Status result;
    double x, y, width, height, startAngle, sweepAngle;

    if (objc != 8) {
	Tcl_WrongNumArgs(interp, 1, objv,
		"brush x y width height startAngle sweepAngle");
	return TCL_ERROR;
    }

    if (GetItclObj(&GraphicsItclObj, objv[0]) != TCL_OK) {
	return TCL_ERROR;
    }

    if (GraphicsHash.Find(GraphicsItclObj, &graphicsPtr) != TCL_OK) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("Gdiplus::Graphics instance lost!", -1));
	return TCL_ERROR;
    }

    // Dissect and disseminate all the arguments.

    // look for the pen ItclObject* from the commandname
    Itcl_FindObject(interp, Tcl_GetString(objv[1]), &BrushItclObj);
    if (BrushItclObj == 0L) {
	Tcl_AppendStringsToObj(Tcl_GetObjResult(interp), "\"",
		Tcl_GetString(objv[1]), "\"",
		" was not a Gdiplus::Brush instance.", 0L);
	return TCL_ERROR;
    }

    // Get the Gdiplus::Pen* from the Itcl object context.
    if (BrushHash.Find(BrushItclObj, &brushPtr) != TCL_OK) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("Gdiplus::Brush instance lost!", -1));
	return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &x) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetDoubleFromObj(interp, objv[3], &y) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetDoubleFromObj(interp, objv[4], &width) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetDoubleFromObj(interp, objv[5], &height) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetDoubleFromObj(interp, objv[6], &startAngle) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetDoubleFromObj(interp, objv[7], &sweepAngle) != TCL_OK) {
	return TCL_ERROR;
    }

    // Gdiplus::REAL is really a float.  downgrade the double (with
    // possible loss) to a float.

    result = graphicsPtr->FillPie(
	    brushPtr,
	    Gdiplus::RectF(
		static_cast<Gdiplus::REAL>(x),
		static_cast<Gdiplus::REAL>(y),
		static_cast<Gdiplus::REAL>(width),
		static_cast<Gdiplus::REAL>(height)
		),
	    static_cast<Gdiplus::REAL>(startAngle),
	    static_cast<Gdiplus::REAL>(sweepAngle)
	    );

    if (result != Gdiplus::Ok) {
	SetTclErrorMsg(result);
	return TCL_ERROR;
    }

    return TCL_OK;
}

int
GdiplusAdapter::GraphicsFillPolygonCmd (int objc, struct Tcl_Obj * CONST objv[])
{
//    Status FillPolygon(IN const Brush* brush,
//                       IN const Point* points,
//                       IN INT count,
//                       IN FillMode fillMode)
    return TCL_ERROR;
}

int
GdiplusAdapter::GraphicsFillRectangleCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    ItclObject *GraphicsItclObj;
    ItclObject *BrushItclObj;
    Gdiplus::Graphics *graphicsPtr;
    Gdiplus::Brush *brushPtr;
    Gdiplus::Status result;
    double x, y, width, height;

    if (objc != 6) {
	Tcl_WrongNumArgs(interp, 1, objv, "brush x y width height");
	return TCL_ERROR;
    }

    if (GetItclObj(&GraphicsItclObj, objv[0]) != TCL_OK) {
	return TCL_ERROR;
    }

    if (GraphicsHash.Find(GraphicsItclObj, &graphicsPtr) != TCL_OK) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("Gdiplus::Graphics instance lost!", -1));
	return TCL_ERROR;
    }

    // Dissect and disseminate all the arguments.

    // look for the brush ItclObject* from the commandname
    Itcl_FindObject(interp, Tcl_GetString(objv[1]), &BrushItclObj);
    if (BrushItclObj == 0L) {
	Tcl_AppendStringsToObj(Tcl_GetObjResult(interp), "\"",
		Tcl_GetString(objv[1]), "\"",
		" was not a Gdiplus::Brush instance.", 0L);
	return TCL_ERROR;
    }

    // Get the Gdiplus::Brush* from the Itcl object context.
    if (BrushHash.Find(BrushItclObj, &brushPtr) != TCL_OK) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("Gdiplus::Brush instance lost!", -1));
	return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &x) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetDoubleFromObj(interp, objv[3], &y) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetDoubleFromObj(interp, objv[4], &width) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetDoubleFromObj(interp, objv[5], &height) != TCL_OK) {
	return TCL_ERROR;
    }

    // Gdiplus::REAL is really a float.  downgrade the double (with
    // possible loss) to a float.

    result = graphicsPtr->FillRectangle(
	    brushPtr,
	    Gdiplus::RectF(
		static_cast<Gdiplus::REAL>(x),
		static_cast<Gdiplus::REAL>(y),
		static_cast<Gdiplus::REAL>(width),
		static_cast<Gdiplus::REAL>(height)
		)
	    );

    if (result != Gdiplus::Ok) {
	SetTclErrorMsg(result);
	return TCL_ERROR;
    }

    return TCL_OK;
}

int
GdiplusAdapter::GraphicsFillRegionCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    ItclObject *GraphicsItclObj;
    ItclObject *BrushItclObj;
    Gdiplus::Graphics *graphicsPtr;
    Gdiplus::Brush *brushPtr;
    Gdiplus::Status result;
    double x, y, width, height;

    if (objc != 6) {
	Tcl_WrongNumArgs(interp, 1, objv, "brush x y width height");
	return TCL_ERROR;
    }

    if (GetItclObj(&GraphicsItclObj, objv[0]) != TCL_OK) {
	return TCL_ERROR;
    }

    if (GraphicsHash.Find(GraphicsItclObj, &graphicsPtr) != TCL_OK) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("Gdiplus::Graphics instance lost!", -1));
	return TCL_ERROR;
    }

    // Dissect and disseminate all the arguments.

    // look for the brush ItclObject* from the commandname
    Itcl_FindObject(interp, Tcl_GetString(objv[1]), &BrushItclObj);
    if (BrushItclObj == 0L) {
	Tcl_AppendStringsToObj(Tcl_GetObjResult(interp), "\"",
		Tcl_GetString(objv[1]), "\"",
		" was not a Gdiplus::Brush instance.", 0L);
	return TCL_ERROR;
    }

    // Get the Gdiplus::Brush* from the Itcl object context.
    if (BrushHash.Find(BrushItclObj, &brushPtr) != TCL_OK) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("Gdiplus::Brush instance lost!", -1));
	return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &x) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetDoubleFromObj(interp, objv[3], &y) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetDoubleFromObj(interp, objv[4], &width) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetDoubleFromObj(interp, objv[5], &height) != TCL_OK) {
	return TCL_ERROR;
    }

    Gdiplus::Region region(
	    Gdiplus::RectF(
		static_cast<Gdiplus::REAL>(x),
		static_cast<Gdiplus::REAL>(y),
		static_cast<Gdiplus::REAL>(width),
		static_cast<Gdiplus::REAL>(height)
		)
	    );

    result =  graphicsPtr->FillRegion(brushPtr, &region);

    if (result != Gdiplus::Ok) {
	SetTclErrorMsg(result);
	return TCL_ERROR;
    }

    return TCL_OK;
}

int
GdiplusAdapter::GraphicsFlushCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    ItclObject *GraphicsItclObj;
    Gdiplus::Graphics *graphicsPtr;

    if (objc != 1) {
	Tcl_WrongNumArgs(interp, 1, objv, "");
	return TCL_ERROR;
    }

    if (GetItclObj(&GraphicsItclObj, objv[0]) != TCL_OK) {
	return TCL_ERROR;
    }

    if (GraphicsHash.Find(GraphicsItclObj, &graphicsPtr) != TCL_OK) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("Gdiplus::Graphics instance lost!", -1));
	return TCL_ERROR;
    }

    graphicsPtr->Flush();
    return TCL_OK;
}

int
GdiplusAdapter::GraphicsClipRegionCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    ItclObject *GraphicsItclObj;
    Gdiplus::Graphics *graphicsPtr;
    Gdiplus::Status result;
    Gdiplus::CombineMode mode;
    double x, y, width, height;

    if (objc != 1 && objc != 5 && objc != 6) {
	Tcl_WrongNumArgs(interp, 1, objv, "x y width height ?CombineMode?");
	return TCL_ERROR;
    }

    if (GetItclObj(&GraphicsItclObj, objv[0]) != TCL_OK) {
	return TCL_ERROR;
    }

    if (GraphicsHash.Find(GraphicsItclObj, &graphicsPtr) != TCL_OK) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("Gdiplus::Graphics instance lost!", -1));
	return TCL_ERROR;
    }

    if (objc == 1) {
	ItclClass *regClsPtr;
	ItclObject *regObjPtr;
	Gdiplus::Region *region = new Gdiplus::Region();
	Gdiplus::Region *regionLast;

	result =  graphicsPtr->GetClip(region);
	if (result != Gdiplus::Ok) {
	    SetTclErrorMsg(result);
	    return TCL_ERROR;
	}

	// Create a Gdiplus::Region Itcl instance, copy the new data
	// into it, and return it by name.

	regClsPtr = Itcl_FindClass(interp, "::Gdiplus::Region", 0);
	Itcl_CreateObject(interp, GenUniqueItclObjName(regClsPtr),
		regClsPtr, 0, 0L, &regObjPtr);

	// Pull-out the new one created by the Itcl constructor and
	// recycle it.
	RegionHash.Extract(regObjPtr, &regionLast);
	delete regionLast;

	// Replace the Gdiplus::Region* with the one modified
	// by graphicsPtr->GetClip.
	RegionHash.Add(regObjPtr, region);

	// Return the Itcl object by name.
	Tcl_SetObjResult(interp, Tcl_NewStringObj(
		Tcl_GetCommandName(interp, regObjPtr->accessCmd), -1));
	return TCL_OK;

    } else {

	if (Tcl_GetDoubleFromObj(interp, objv[2], &x) != TCL_OK) {
	    return TCL_ERROR;
	}
	if (Tcl_GetDoubleFromObj(interp, objv[3], &y) != TCL_OK) {
	    return TCL_ERROR;
	}
	if (Tcl_GetDoubleFromObj(interp, objv[4], &width) != TCL_OK) {
	    return TCL_ERROR;
	}
	if (Tcl_GetDoubleFromObj(interp, objv[5], &height) != TCL_OK) {
	    return TCL_ERROR;
	}

	Gdiplus::Region region(
		Gdiplus::RectF(
		    static_cast<Gdiplus::REAL>(x),
		    static_cast<Gdiplus::REAL>(y),
		    static_cast<Gdiplus::REAL>(width),
		    static_cast<Gdiplus::REAL>(height)
		    )
		);

	if (objc == 6) {
	    ObjToCombineMode(objv[5], &mode);
	} else {
	    mode = Gdiplus::CombineModeReplace;
	}

	result =  graphicsPtr->SetClip(&region, mode);

	if (result != Gdiplus::Ok) {
	    SetTclErrorMsg(result);
	    return TCL_ERROR;
	}

	return TCL_OK;
    }
}

int
GdiplusAdapter::GraphicsCompositingModeCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    // TODO: fill this with some code.
    return TCL_ERROR;
}

int
GdiplusAdapter::GraphicsCompositingQualityCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    // TODO: fill this with some code.
    return TCL_ERROR;
}

int
GdiplusAdapter::GraphicsInterpolationModeCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    // TODO: fill this with some code.
    return TCL_ERROR;
}

int
GdiplusAdapter::GraphicsPageScaleCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    // TODO: fill this with some code.
    return TCL_ERROR;
}

int
GdiplusAdapter::GraphicsPageUnitCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    // TODO: fill this with some code.
    return TCL_ERROR;
}

int
GdiplusAdapter::GraphicsPixelOffsetModeCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    // TODO: fill this with some code.
    return TCL_ERROR;
}

int
GdiplusAdapter::GraphicsRenderingOriginCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    // TODO: fill this with some code.
    return TCL_ERROR;
}

int
GdiplusAdapter::GraphicsSmoothingModeCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    // TODO: fill this with some code.
    return TCL_ERROR;
}

int
GdiplusAdapter::GraphicsTextContrastCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    // TODO: fill this with some code.
    return TCL_ERROR;
}

int
GdiplusAdapter::GraphicsTextRenderingHintCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    // TODO: fill this with some code.
    return TCL_ERROR;
}

int
GdiplusAdapter::GraphicsTransformCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    // TODO: fill this with some code.
    return TCL_ERROR;
}

int
GdiplusAdapter::GraphicsGetDpiXCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    // TODO: fill this with some code.
    return TCL_ERROR;
}

int
GdiplusAdapter::GraphicsGetDpiYCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    // TODO: fill this with some code.
    return TCL_ERROR;
}

int
GdiplusAdapter::GraphicsGetNearestColorCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    // TODO: fill this with some code.
    return TCL_ERROR;
}

int
GdiplusAdapter::GraphicsGetVisibleClipBoundsCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    // TODO: fill this with some code.
    return TCL_ERROR;
}

int
GdiplusAdapter::GraphicsIntersectClipCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    // TODO: fill this with some code.
    return TCL_ERROR;
}

int
GdiplusAdapter::GraphicsIsClipEmptyCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    // TODO: fill this with some code.
    return TCL_ERROR;
}

int
GdiplusAdapter::GraphicsIsVisibleCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    // TODO: fill this with some code.
    return TCL_ERROR;
}

int
GdiplusAdapter::GraphicsIsVisibleClipEmptyCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    // TODO: fill this with some code.
    return TCL_ERROR;
}

int
GdiplusAdapter::GraphicsMeasureCharacterRangesCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    // TODO: fill this with some code.
    return TCL_ERROR;
}

int
GdiplusAdapter::GraphicsMeasureDriverStringCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    // TODO: fill this with some code.
    return TCL_ERROR;
}

int
GdiplusAdapter::GraphicsMeasureStringCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    // TODO: fill this with some code.
    return TCL_ERROR;
}

int
GdiplusAdapter::GraphicsMultiplyTransformCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    // TODO: fill this with some code.
    return TCL_ERROR;
}

int
GdiplusAdapter::GraphicsResetClipCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    // TODO: fill this with some code.
    return TCL_ERROR;
}

int
GdiplusAdapter::GraphicsResetTransformCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    // TODO: fill this with some code.
    return TCL_ERROR;
}

int
GdiplusAdapter::GraphicsRestoreCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    // TODO: fill this with some code.
    return TCL_ERROR;
}

int
GdiplusAdapter::GraphicsRotateTransformCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    // TODO: fill this with some code.
    return TCL_ERROR;
}

int
GdiplusAdapter::GraphicsSaveCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    // TODO: fill this with some code.
    return TCL_ERROR;
}

int
GdiplusAdapter::GraphicsScaleTransformCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    // TODO: fill this with some code.
    return TCL_ERROR;
}

int
GdiplusAdapter::GraphicsTransformPointsCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    // TODO: fill this with some code.
    return TCL_ERROR;
}

int
GdiplusAdapter::GraphicsTranslateClipCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    // TODO: fill this with some code.
    return TCL_ERROR;
}

int
GdiplusAdapter::GraphicsTranslateTransformCmd (int objc, struct Tcl_Obj * CONST objv[])
{
    // TODO: fill this with some code.
    return TCL_ERROR;
}
