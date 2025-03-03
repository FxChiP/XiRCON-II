// http://msdn.microsoft.com/library/en-us/gdicpp/GDIPlus/GDIPlus.asp

#include "GdiplusAdapter.hpp"
#pragma comment (lib, "gdiplus.lib")


GdiplusAdapter::GdiplusAdapter(Tcl_Interp *interp)
    : Itcl::IAdapter<GdiplusAdapter>(interp)
{
    // Let [Incr Tcl] know we have some (lots of?) methods in here.

    // Gdiplus::Graphics
    NewItclCmd ("Gdip-Graphics-construct",
	    &GdiplusAdapter::GraphicsConstructCmd);
    
    NewItclCmd ("Gdip-Graphics-destruct",
	    &GdiplusAdapter::GraphicsDestructCmd);

    NewItclCmd ("Gdip-Graphics-AddMetafileComment",
	    &GdiplusAdapter::GraphicsAddMetafileCommentCmd);

    NewItclCmd ("Gdip-Graphics-BeginContainer",
	    &GdiplusAdapter::GraphicsBeginContainerCmd);

    NewItclCmd ("Gdip-Graphics-Clear",
	    &GdiplusAdapter::GraphicsClearCmd);

    NewItclCmd ("Gdip-Graphics-DrawArc",
	    &GdiplusAdapter::GraphicsDrawArcCmd);

    NewItclCmd ("Gdip-Graphics-DrawBezier",
	    &GdiplusAdapter::GraphicsDrawBezierCmd);

    NewItclCmd ("Gdip-Graphics-DrawCachedBitmap",
	    &GdiplusAdapter::GraphicsDrawCachedBitmapCmd);

    NewItclCmd ("Gdip-Graphics-DrawClosedCurve",
	    &GdiplusAdapter::GraphicsDrawClosedCurveCmd);

    NewItclCmd ("Gdip-Graphics-DrawCurve",
	    &GdiplusAdapter::GraphicsDrawCurveCmd);

    NewItclCmd ("Gdip-Graphics-DrawDriverString",
	    &GdiplusAdapter::GraphicsDrawDriverStringCmd);

    NewItclCmd ("Gdip-Graphics-DrawEllipse",
	    &GdiplusAdapter::GraphicsDrawEllipseCmd);

    NewItclCmd ("Gdip-Graphics-DrawImage",
	    &GdiplusAdapter::GraphicsDrawImageCmd);

    NewItclCmd ("Gdip-Graphics-DrawLine",
	    &GdiplusAdapter::GraphicsDrawLineCmd);

    NewItclCmd ("Gdip-Graphics-DrawPath",
	    &GdiplusAdapter::GraphicsDrawPathCmd);

    NewItclCmd ("Gdip-Graphics-DrawPie",
	    &GdiplusAdapter::GraphicsDrawPieCmd);

    NewItclCmd ("Gdip-Graphics-DrawPolygon",
	    &GdiplusAdapter::GraphicsDrawPolygonCmd);

    NewItclCmd ("Gdip-Graphics-DrawPolygonList",
	    &GdiplusAdapter::GraphicsDrawPolygonListCmd);

    NewItclCmd ("Gdip-Graphics-DrawRectangle",
	    &GdiplusAdapter::GraphicsDrawRectangleCmd);

    NewItclCmd ("Gdip-Graphics-DrawString",
	    &GdiplusAdapter::GraphicsDrawStringCmd);

    NewItclCmd ("Gdip-Graphics-EndContainer",
	    &GdiplusAdapter::GraphicsEndContainerCmd);

    NewItclCmd ("Gdip-Graphics-EnumerateMetafile",
	    &GdiplusAdapter::GraphicsEnumerateMetafileCmd);

    NewItclCmd ("Gdip-Graphics-ExcludeClip",
	    &GdiplusAdapter::GraphicsExcludeClipCmd);

    NewItclCmd ("Gdip-Graphics-FillClosedCurve",
	    &GdiplusAdapter::GraphicsFillClosedCurveCmd);

    NewItclCmd ("Gdip-Graphics-FillEllipse",
	    &GdiplusAdapter::GraphicsFillEllipseCmd);

    NewItclCmd ("Gdip-Graphics-FillPath",
	    &GdiplusAdapter::GraphicsFillPathCmd);

    NewItclCmd ("Gdip-Graphics-FillPie",
	    &GdiplusAdapter::GraphicsFillPieCmd);

    NewItclCmd ("Gdip-Graphics-FillPolygon",
	    &GdiplusAdapter::GraphicsFillPolygonCmd);

    NewItclCmd ("Gdip-Graphics-FillRectangle",
	    &GdiplusAdapter::GraphicsFillRectangleCmd);

    NewItclCmd ("Gdip-Graphics-FillRegion",
	    &GdiplusAdapter::GraphicsFillRegionCmd);

    NewItclCmd ("Gdip-Graphics-Flush",
	    &GdiplusAdapter::GraphicsFlushCmd);

    NewItclCmd ("Gdip-Graphics-ClipRegion",
	    &GdiplusAdapter::GraphicsClipRegionCmd);

    NewItclCmd ("Gdip-Graphics-CompositingMode",
	    &GdiplusAdapter::GraphicsCompositingModeCmd);

    NewItclCmd ("Gdip-Graphics-CompositingQuality",
	    &GdiplusAdapter::GraphicsCompositingQualityCmd);

    NewItclCmd ("Gdip-Graphics-InterpolationMode",
	    &GdiplusAdapter::GraphicsInterpolationModeCmd);

    NewItclCmd ("Gdip-Graphics-PageScale",
	    &GdiplusAdapter::GraphicsPageScaleCmd);

    NewItclCmd ("Gdip-Graphics-PageUnit",
	    &GdiplusAdapter::GraphicsPageUnitCmd);

    NewItclCmd ("Gdip-Graphics-PixelOffsetMode",
	    &GdiplusAdapter::GraphicsPixelOffsetModeCmd);

    NewItclCmd ("Gdip-Graphics-RenderingOrigin",
	    &GdiplusAdapter::GraphicsRenderingOriginCmd);

    NewItclCmd ("Gdip-Graphics-SmoothingMode",
	    &GdiplusAdapter::GraphicsSmoothingModeCmd);

    NewItclCmd ("Gdip-Graphics-TextContrast",
	    &GdiplusAdapter::GraphicsTextContrastCmd);

    NewItclCmd ("Gdip-Graphics-TextRenderingHint",
	    &GdiplusAdapter::GraphicsTextRenderingHintCmd);

    NewItclCmd ("Gdip-Graphics-Transform",
	    &GdiplusAdapter::GraphicsTransformCmd);

    NewItclCmd ("Gdip-Graphics-GetDpiX",
	    &GdiplusAdapter::GraphicsGetDpiXCmd);

    NewItclCmd ("Gdip-Graphics-GetDpiY",
	    &GdiplusAdapter::GraphicsGetDpiYCmd);

    NewItclCmd ("Gdip-Graphics-GetNearestColor",
	    &GdiplusAdapter::GraphicsGetNearestColorCmd);

    NewItclCmd ("Gdip-Graphics-GetVisibleClipBounds",
	    &GdiplusAdapter::GraphicsGetVisibleClipBoundsCmd);

    NewItclCmd ("Gdip-Graphics-IntersectClip",
	    &GdiplusAdapter::GraphicsIntersectClipCmd);

    NewItclCmd ("Gdip-Graphics-IsClipEmpty",
	    &GdiplusAdapter::GraphicsIsClipEmptyCmd);

    NewItclCmd ("Gdip-Graphics-IsVisible",
	    &GdiplusAdapter::GraphicsIsVisibleCmd);

    NewItclCmd ("Gdip-Graphics-IsVisibleClipEmpty",
	    &GdiplusAdapter::GraphicsIsVisibleClipEmptyCmd);

    NewItclCmd ("Gdip-Graphics-MeasureCharacterRanges",
	    &GdiplusAdapter::GraphicsMeasureCharacterRangesCmd);

    NewItclCmd ("Gdip-Graphics-MeasureDriverString",
	    &GdiplusAdapter::GraphicsMeasureDriverStringCmd);

    NewItclCmd ("Gdip-Graphics-MeasureString",
	    &GdiplusAdapter::GraphicsMeasureStringCmd);

    NewItclCmd ("Gdip-Graphics-MultiplyTransform",
	    &GdiplusAdapter::GraphicsMultiplyTransformCmd);

    NewItclCmd ("Gdip-Graphics-ResetClip",
	    &GdiplusAdapter::GraphicsResetClipCmd);

    NewItclCmd ("Gdip-Graphics-ResetTransform",
	    &GdiplusAdapter::GraphicsResetTransformCmd);

    NewItclCmd ("Gdip-Graphics-Restore",
	    &GdiplusAdapter::GraphicsRestoreCmd);

    NewItclCmd ("Gdip-Graphics-RotateTransform",
	    &GdiplusAdapter::GraphicsRotateTransformCmd);

    NewItclCmd ("Gdip-Graphics-Save",
	    &GdiplusAdapter::GraphicsSaveCmd);

    NewItclCmd ("Gdip-Graphics-ScaleTransform",
	    &GdiplusAdapter::GraphicsScaleTransformCmd);

    NewItclCmd ("Gdip-Graphics-TransformPoints",
	    &GdiplusAdapter::GraphicsTransformPointsCmd);

    NewItclCmd ("Gdip-Graphics-TranslateClip",
	    &GdiplusAdapter::GraphicsTranslateClipCmd);

    NewItclCmd ("Gdip-Graphics-TranslateTransform",
	    &GdiplusAdapter::GraphicsTranslateTransformCmd);


    // Gdiplus::Pen
    NewItclCmd ("Gdip-Pen-construct",
	    &GdiplusAdapter::PenConstructCmd);

    NewItclCmd ("Gdip-Pen-destruct",
	    &GdiplusAdapter::PenDestructCmd);

    NewItclCmd ("Gdip-Pen-MultiplyTransform",
	    &GdiplusAdapter::PenMultiplyTransformCmd);

    NewItclCmd ("Gdip-Pen-ResetTransform",
	    &GdiplusAdapter::PenResetTransformCmd);

    NewItclCmd ("Gdip-Pen-RotateTransform",
	    &GdiplusAdapter::PenRotateTransformCmd);

    NewItclCmd ("Gdip-Pen-ScaleTransform",
	    &GdiplusAdapter::PenScaleTransformCmd);

    NewItclCmd ("Gdip-Pen-Alignment",
	    &GdiplusAdapter::PenAlignmentCmd);

    NewItclCmd ("Gdip-Pen-Brush",
	    &GdiplusAdapter::PenBrushCmd);

    NewItclCmd ("Gdip-Pen-Color",
	    &GdiplusAdapter::PenColorCmd);

    NewItclCmd ("Gdip-Pen-CompoundArray",
	    &GdiplusAdapter::PenCompoundArrayCmd);

    NewItclCmd ("Gdip-Pen-CustomEndCap",
	    &GdiplusAdapter::PenCustomEndCapCmd);

    NewItclCmd ("Gdip-Pen-CustomStartCap",
	    &GdiplusAdapter::PenCustomStartCapCmd);

    NewItclCmd ("Gdip-Pen-DashCap",
	    &GdiplusAdapter::PenDashCapCmd);

    NewItclCmd ("Gdip-Pen-DashOffset",
	    &GdiplusAdapter::PenDashOffsetCmd);

    NewItclCmd ("Gdip-Pen-DashPattern",
	    &GdiplusAdapter::PenDashPatternCmd);

    NewItclCmd ("Gdip-Pen-DashStyle",
	    &GdiplusAdapter::PenDashStyleCmd);

    NewItclCmd ("Gdip-Pen-EndCap",
	    &GdiplusAdapter::PenEndCapCmd);

    NewItclCmd ("Gdip-Pen-LineJoin",
	    &GdiplusAdapter::PenLineJoinCmd);

    NewItclCmd ("Gdip-Pen-MiterLimit",
	    &GdiplusAdapter::PenMiterLimitCmd);

    NewItclCmd ("Gdip-Pen-StartCap",
	    &GdiplusAdapter::PenStartCapCmd);

    NewItclCmd ("Gdip-Pen-Transform",
	    &GdiplusAdapter::PenTransformCmd);

    NewItclCmd ("Gdip-Pen-Width",
	    &GdiplusAdapter::PenWidthCmd);

    // Gdiplus::SolidBrush
    NewItclCmd ("Gdip-SolidBrush-construct",
	    &GdiplusAdapter::SolidBrushConstructCmd);

    NewItclCmd ("Gdip-SolidBrush-destruct",
	    &GdiplusAdapter::SolidBrushDestructCmd);

    // Gdiplus::TextureBrush
    NewItclCmd ("Gdip-TextureBrush-construct",
	    &GdiplusAdapter::TextureBrushConstructCmd);

    NewItclCmd ("Gdip-TextureBrush-destruct",
	    &GdiplusAdapter::TextureBrushDestructCmd);

    // Gdiplus::LinearGradientBrush
    NewItclCmd ("Gdip-LinearGradientBrush-construct",
	    &GdiplusAdapter::LinearGradientBrushConstructCmd);

    NewItclCmd ("Gdip-LinearGradientBrush-destruct",
	    &GdiplusAdapter::LinearGradientBrushDestructCmd);

    // Gdiplus::HatchBrush
    NewItclCmd ("Gdip-HatchBrush-construct",
	    &GdiplusAdapter::HatchBrushConstructCmd);

    NewItclCmd ("Gdip-HatchBrush-destruct",
	    &GdiplusAdapter::HatchBrushDestructCmd);

    // Gdiplus::Font
    NewItclCmd ("Gdip-Font-construct",
	    &GdiplusAdapter::FontConstructCmd);

    NewItclCmd ("Gdip-Font-destruct",
	    &GdiplusAdapter::FontDestructCmd);

    NewItclCmd ("Gdip-Font-GetHeight",
	    &GdiplusAdapter::FontGetHeightCmd);

    NewItclCmd ("Gdip-Font-GetUnit",
	    &GdiplusAdapter::FontGetUnitCmd);

    // Gdiplus::Matrix
    NewItclCmd ("Gdip-Matrix-construct",
	    &GdiplusAdapter::MatrixConstructCmd);
    
    NewItclCmd ("Gdip-Matrix-destruct",
	    &GdiplusAdapter::MatrixDestructCmd);

    NewItclCmd ("Gdip-Matrix-Equals",
	    &GdiplusAdapter::MatrixEqualsCmd);

    NewItclCmd ("Gdip-Matrix-Invert",
	    &GdiplusAdapter::MatrixInvertCmd);

    NewItclCmd ("Gdip-Matrix-IsIdentity",
	    &GdiplusAdapter::MatrixIsIdentityCmd);

    NewItclCmd ("Gdip-Matrix-IsInvertible",
	    &GdiplusAdapter::MatrixIsInvertibleCmd);

    NewItclCmd ("Gdip-Matrix-Multiply",
	    &GdiplusAdapter::MatrixMultiplyCmd);

    NewItclCmd ("Gdip-Matrix-OffsetX",
	    &GdiplusAdapter::MatrixOffsetXCmd);

    NewItclCmd ("Gdip-Matrix-OffsetY",
	    &GdiplusAdapter::MatrixOffsetYCmd);

    NewItclCmd ("Gdip-Matrix-Rotate",
	    &GdiplusAdapter::MatrixRotateCmd);

    NewItclCmd ("Gdip-Matrix-RotateAt",
	    &GdiplusAdapter::MatrixRotateAtCmd);

    NewItclCmd ("Gdip-Matrix-Scale",
	    &GdiplusAdapter::MatrixScaleCmd);

    NewItclCmd ("Gdip-Matrix-Shear",
	    &GdiplusAdapter::MatrixShearCmd);

    NewItclCmd ("Gdip-Matrix-Translate",
	    &GdiplusAdapter::MatrixTranslateCmd);

    // Gdiplus::StringFormat
    NewItclCmd ("Gdip-StringFormat-construct",
	    &GdiplusAdapter::StringFormatConstructCmd);

    NewItclCmd ("Gdip-StringFormat-destruct",
	    &GdiplusAdapter::StringFormatDestructCmd);

    NewItclCmd ("Gdip-StringFormat-Alignment",
	    &GdiplusAdapter::StringFormatAlignmentCmd);

    NewItclCmd ("Gdip-StringFormat-DigitSubstitutionLanguage",
	    &GdiplusAdapter::StringFormatDigitSubstitutionLanguageCmd);

    NewItclCmd ("Gdip-StringFormat-FormatFlags",
	    &GdiplusAdapter::StringFormatFormatFlagsCmd);

    NewItclCmd ("Gdip-StringFormat-HotkeyPrefix",
	    &GdiplusAdapter::StringFormatHotkeyPrefixCmd);

    NewItclCmd ("Gdip-StringFormat-LineAlignment",
	    &GdiplusAdapter::StringFormatLineAlignmentCmd);

    NewItclCmd ("Gdip-StringFormat-MeasurableCharacterRangeCount",
	    &GdiplusAdapter::StringFormatMeasurableCharacterRangeCountCmd);

    NewItclCmd ("Gdip-StringFormat-TabStops",
	    &GdiplusAdapter::StringFormatTabStopsCmd);

    NewItclCmd ("Gdip-StringFormat-Trimming",
	    &GdiplusAdapter::StringFormatTrimmingCmd);

    // Gdiplus::Region
    NewItclCmd ("Gdip-Region-construct",
	    &GdiplusAdapter::RegionConstructCmd);

    NewItclCmd ("Gdip-Region-destruct",
	    &GdiplusAdapter::RegionDestructCmd);

    NewItclCmd ("Gdip-Region-Transform",
	    &GdiplusAdapter::RegionTransformCmd);

    NewItclCmd ("Gdip-Region-Translate",
	    &GdiplusAdapter::RegionTranslateCmd);

    NewItclCmd ("Gdip-Region-Union",
	    &GdiplusAdapter::RegionUnionCmd);

    NewItclCmd ("Gdip-Region-Xor",
	    &GdiplusAdapter::RegionXorCmd);
}


int
GdiplusAdapter::ObjToCombineMode (Tcl_Obj *strMode, Gdiplus::CombineMode *mode)
{
    static char *cmodes[] = {
	"Replace", "Intersect", "Union", "Xor", "Exclude",
	"Complement", 0L
    };
    enum cmodes {
	Replace, Intersect, Union, Xor, Exclude, Complement 
    };
    int index;

    if (Tcl_GetIndexFromObj(interp, strMode, (const char **)cmodes,
	    "CombineMode", 0, &index) != TCL_OK) {
	return TCL_ERROR;
    }

    switch((enum cmodes) index) {
    case Replace:
	*mode = Gdiplus::CombineModeReplace; break;
    case Intersect:
	*mode = Gdiplus::CombineModeIntersect; break;
    case Union:
	*mode = Gdiplus::CombineModeUnion; break;
    case Xor:
	*mode = Gdiplus::CombineModeXor; break;
    case Exclude:
	*mode = Gdiplus::CombineModeExclude; break;
    case Complement:
	*mode = Gdiplus::CombineModeComplement; break;
    default:
	return TCL_ERROR;
    }

    return TCL_OK;
}

void
GdiplusAdapter::SetTclErrorMsg (Gdiplus::Status err)
{
    const char *msg;

    switch (err) {
    case Gdiplus::GenericError:
	msg = "GDI+ Error: generic error.";
	break;
    case Gdiplus::InvalidParameter:
	msg = "GDI+ Error: invalid parameter.";
	break;
    case Gdiplus::OutOfMemory:
	msg = "GDI+ Error: out of memory.";
	break;
    case Gdiplus::ObjectBusy:
	msg = "GDI+ Error: object busy.";
	break;
    case Gdiplus::InsufficientBuffer:
	msg = "GDI+ Error: insufficient buffer.";
	break;
    case Gdiplus::NotImplemented:
	msg = "GDI+ Error: not implimented.";
	break;
    case Gdiplus::Win32Error:
	msg = "GDI+ Error: win32 error.";
	break;
    case Gdiplus::WrongState:
	msg = "GDI+ Error: wrong state.";
	break;
    case Gdiplus::Aborted:
	msg = "GDI+ Error: aborted.";
	break;
    case Gdiplus::FileNotFound:
	msg = "GDI+ Error: file not found.";
	break;
    case Gdiplus::ValueOverflow:
	msg = "GDI+ Error: value overflow.";
	break;
    case Gdiplus::AccessDenied:
	msg = "GDI+ Error: access denied.";
	break;
    case Gdiplus::UnknownImageFormat:
	msg = "GDI+ Error: unknown image format.";
	break;
    case Gdiplus::FontFamilyNotFound:
	msg = "GDI+ Error: font family not found.";
	break;
    case Gdiplus::FontStyleNotFound:
	msg = "GDI+ Error: font style not found.";
	break;
    case Gdiplus::NotTrueTypeFont:
	msg = "GDI+ Error: not a truetype font.";
	break;
    case Gdiplus::UnsupportedGdiplusVersion:
	msg = "GDI+ Error: unsupported version.";
	break;
    case Gdiplus::GdiplusNotInitialized:
	msg = "GDI+ Error: not initialized.";
    }
    Tcl_SetObjResult(interp, Tcl_NewStringObj(msg,-1));
}

int
GdiplusAdapter::MakeARGB (Tcl_Obj *c, Gdiplus::ARGB *argb)
{
    static char *colors[] = {
	"Black", "Silver", "Gray", "White", "Maroon", "Red", "Purple",
	"Fuchsia", "Green", "Lime", "Olive", "Yellow", "Navy", "Blue",
	"Teal", "Aqua", 0L
    };
    enum colors {
	Black, Silver, Gray, White, Maroon, Red, Purple, Fuchsia, Green,
	Lime, Olive, Yellow, Navy, Blue, Teal, Aqua
    };
    int index;

    // don't specify the interp* because we don't want an error message
    if (Tcl_GetLongFromObj(0L, c, reinterpret_cast<long *>(argb))
	    != TCL_OK) {
	// Try the shortcuts, because it wasn't a number.
	if (Tcl_GetIndexFromObj(interp, c, (const char **)colors, "ARGBcolor", 0,
		&index) != TCL_OK) {
	    Tcl_AppendObjToObj(Tcl_GetObjResult(interp),
		Tcl_NewStringObj(". Or specify a 32-bit ARGB color value.", -1));
	    return TCL_ERROR;
	}
	switch (static_cast<enum colors>(index)) {
	case Black:
	    *argb = Gdiplus::Color::Black; break;
	case Silver:
	    *argb = Gdiplus::Color::Silver; break;
	case Gray:
	    *argb = Gdiplus::Color::Gray; break;
	case White:
	    *argb = Gdiplus::Color::White; break;
	case Maroon:
	    *argb = Gdiplus::Color::Maroon; break;
	case Red:
	    *argb = Gdiplus::Color::Red; break;
	case Purple:
	    *argb = Gdiplus::Color::Purple; break;
	case Fuchsia:
	    *argb = Gdiplus::Color::Fuchsia; break;
	case Green:
	    *argb = Gdiplus::Color::Green; break;
	case Lime:
	    *argb = Gdiplus::Color::Lime; break;
	case Olive:
	    *argb = Gdiplus::Color::Olive; break;
	case Yellow:
	    *argb = Gdiplus::Color::Yellow; break;
	case Navy:
	    *argb = Gdiplus::Color::Navy; break;
	case Blue:
	    *argb = Gdiplus::Color::Blue; break;
	case Teal:
	    *argb = Gdiplus::Color::Teal; break;
	case Aqua:
	    *argb = Gdiplus::Color::Aqua;
	}
    }
    return TCL_OK;
}

int
GdiplusAdapter::GetHatchStyle(Tcl_Obj *h, Gdiplus::HatchStyle *hs)
{
    static char *hatches[] = {
	"Horizontal", "Vertical", "ForwardDiagonal", "BackwardDiagonal",
	"Cross", "DiagonalCross", "5%", "10%", "20%", "25%", "30%", "40%",
	"50%", "60%", "70%", "75%", "80%", "90%", "LightDownwardDiagonal",
	"LightUpwardDiagonal", "DarkDownwardDiagonal",
	"DarkUpwardDiagonal", "WideDownwardDiagonal", "WideUpwardDiagonal",
	"LightVertical", "LightHorizontal", "NarrowVertical",
	"NarrowHorizontal", "DarkVertical", "DarkHorizontal",
	"DashedDownwardDiagonal", "DashedUpwardDiagonal",
	"DashedHorizontal", "DashedVertical", "SmallConfetti",
	"LargeConfetti", "ZigZag", "Wave", "DiagonalBrick",
	"HorizontalBrick", "Weave", "Plaid", "Divot", "DottedGrid",
	"DottedDiamond", "Shingle", "Trellis", "Sphere", "SmallGrid",
	"SmallCheckerBoard", "LargeCheckerBoard", "OutlinedDiamond",
	"SolidDiamond", "LargeGrid", 0L
    };
    enum hatches {
	Horizontal, Vertical, ForwardDiagonal, BackwardDiagonal, Cross,
	DiagonalCross, o05Percent, o10Percent, o20Percent, o25Percent,
	o30Percent, o40Percent, o50Percent, o60Percent, o70Percent,
	o75Percent, o80Percent, o90Percent, LightDownwardDiagonal,
	LightUpwardDiagonal, DarkDownwardDiagonal, DarkUpwardDiagonal,
	WideDownwardDiagonal, WideUpwardDiagonal, LightVertical,
	LightHorizontal, NarrowVertical, NarrowHorizontal, DarkVertical,
	DarkHorizontal, DashedDownwardDiagonal, DashedUpwardDiagonal,
	DashedHorizontal, DashedVertical, SmallConfetti, LargeConfetti,
	ZigZag, Wave, DiagonalBrick, HorizontalBrick, Weave, Plaid, Divot,
	DottedGrid, DottedDiamond, Shingle, Trellis, Sphere, SmallGrid,
	SmallCheckerBoard, LargeCheckerBoard, OutlinedDiamond,
	SolidDiamond, LargeGrid 
    };
    int index;

    if (Tcl_GetIndexFromObj(interp, h, (const char **)hatches, "HatchStyle", 0, &index)
	    != TCL_OK) {
	return TCL_ERROR;
    }

    switch((enum hatches) index) {
    case Horizontal:
	*hs = Gdiplus::HatchStyleHorizontal; break;
    case Vertical:
	*hs = Gdiplus::HatchStyleVertical; break;
    case ForwardDiagonal:
	*hs = Gdiplus::HatchStyleForwardDiagonal; break;
    case BackwardDiagonal:
	*hs = Gdiplus::HatchStyleBackwardDiagonal; break;
    case Cross:
	*hs = Gdiplus::HatchStyleCross; break;
    case DiagonalCross:
	*hs = Gdiplus::HatchStyleDiagonalCross; break;
    case o05Percent:
	*hs = Gdiplus::HatchStyle05Percent; break;
    case o10Percent:
	*hs = Gdiplus::HatchStyle10Percent; break;
    case o20Percent:
	*hs = Gdiplus::HatchStyle20Percent; break;
    case o25Percent:
	*hs = Gdiplus::HatchStyle25Percent; break;
    case o30Percent:
	*hs = Gdiplus::HatchStyle30Percent; break;
    case o40Percent:
	*hs = Gdiplus::HatchStyle40Percent; break;
    case o50Percent:
	*hs = Gdiplus::HatchStyle50Percent; break;
    case o60Percent:
	*hs = Gdiplus::HatchStyle60Percent; break;
    case o70Percent:
	*hs = Gdiplus::HatchStyle70Percent; break;
    case o75Percent:
	*hs = Gdiplus::HatchStyle75Percent; break;
    case o80Percent:
	*hs = Gdiplus::HatchStyle80Percent; break;
    case o90Percent:
	*hs = Gdiplus::HatchStyle90Percent; break;
    case LightDownwardDiagonal:
	*hs = Gdiplus::HatchStyleLightDownwardDiagonal; break;
    case LightUpwardDiagonal:
	*hs = Gdiplus::HatchStyleLightUpwardDiagonal; break;
    case DarkDownwardDiagonal:
	*hs = Gdiplus::HatchStyleDarkDownwardDiagonal; break;
    case DarkUpwardDiagonal:
	*hs = Gdiplus::HatchStyleDarkUpwardDiagonal; break;
    case WideDownwardDiagonal:
	*hs = Gdiplus::HatchStyleWideDownwardDiagonal; break;
    case WideUpwardDiagonal:
	*hs = Gdiplus::HatchStyleWideUpwardDiagonal; break;
    case LightVertical:
	*hs = Gdiplus::HatchStyleLightVertical; break;
    case LightHorizontal:
	*hs = Gdiplus::HatchStyleLightHorizontal; break;
    case NarrowVertical:
	*hs = Gdiplus::HatchStyleNarrowVertical; break;
    case NarrowHorizontal:
	*hs = Gdiplus::HatchStyleNarrowHorizontal; break;
    case DarkVertical:
	*hs = Gdiplus::HatchStyleDarkVertical; break;
    case DarkHorizontal:
	*hs = Gdiplus::HatchStyleDarkHorizontal; break;
    case DashedDownwardDiagonal:
	*hs = Gdiplus::HatchStyleDashedDownwardDiagonal; break;
    case DashedUpwardDiagonal:
	*hs = Gdiplus::HatchStyleDashedUpwardDiagonal; break;
    case DashedHorizontal:
	*hs = Gdiplus::HatchStyleDashedHorizontal; break;
    case DashedVertical:
	*hs = Gdiplus::HatchStyleDashedVertical; break;
    case SmallConfetti:
	*hs = Gdiplus::HatchStyleSmallConfetti; break;
    case LargeConfetti:
	*hs = Gdiplus::HatchStyleLargeConfetti; break;
    case ZigZag:
	*hs = Gdiplus::HatchStyleZigZag; break;
    case Wave:
	*hs = Gdiplus::HatchStyleWave; break;
    case DiagonalBrick:
	*hs = Gdiplus::HatchStyleDiagonalBrick; break;
    case HorizontalBrick:
	*hs = Gdiplus::HatchStyleHorizontalBrick; break;
    case Weave:
	*hs = Gdiplus::HatchStyleWeave; break;
    case Plaid:
	*hs = Gdiplus::HatchStylePlaid; break;
    case Divot:
	*hs = Gdiplus::HatchStyleDivot; break;
    case DottedGrid:
	*hs = Gdiplus::HatchStyleDottedGrid; break;
    case DottedDiamond:
	*hs = Gdiplus::HatchStyleDottedDiamond; break;
    case Shingle:
	*hs = Gdiplus::HatchStyleShingle; break;
    case Trellis:
	*hs = Gdiplus::HatchStyleTrellis; break;
    case Sphere:
	*hs = Gdiplus::HatchStyleSphere; break;
    case SmallGrid:
	*hs = Gdiplus::HatchStyleSmallGrid; break;
    case SmallCheckerBoard:
	*hs = Gdiplus::HatchStyleSmallCheckerBoard; break;
    case LargeCheckerBoard:
	*hs = Gdiplus::HatchStyleLargeCheckerBoard; break;
    case OutlinedDiamond:
	*hs = Gdiplus::HatchStyleOutlinedDiamond; break;
    case SolidDiamond:
	*hs = Gdiplus::HatchStyleSolidDiamond; break;
    case LargeGrid:
	*hs = Gdiplus::HatchStyleLargeGrid;
    }
    return TCL_OK;
}



int
GdiplusAdapter::UnitToResult (Gdiplus::Unit unit)
{
    const char *msg;

    switch (unit) {
    case Gdiplus::UnitWorld:
	msg = "World"; break;	
    case Gdiplus::UnitDisplay:
	msg = "Display"; break;
    case Gdiplus::UnitPixel:
	msg = "Pixel"; break;
    case Gdiplus::UnitPoint:
	msg = "Point"; break;
    case Gdiplus::UnitInch:
	msg = "Inch"; break;
    case Gdiplus::UnitDocument:
	msg = "Document"; break;
    case Gdiplus::UnitMillimeter:
	msg = "Millimeter"; break;
    default:
	Tcl_SetObjResult(interp, Tcl_NewStringObj("unknown Unit!", -1));
	return TCL_ERROR;
    }
    Tcl_SetObjResult(interp, Tcl_NewStringObj(msg, -1));
    return TCL_OK;
}

int
GdiplusAdapter::PenAlignmentToResult (Gdiplus::PenAlignment align)
{
    const char *msg;

    switch (align) {
    case Gdiplus::PenAlignmentCenter:
	msg = "Center";	break;
    case Gdiplus::PenAlignmentInset:
	msg = "Inset"; break;
    default:
	Tcl_SetObjResult(interp, Tcl_NewStringObj("unknown PenAlignment!", -1));
	return TCL_ERROR;
    }
    Tcl_SetObjResult(interp, Tcl_NewStringObj(msg, -1));
    return TCL_OK;
}

int
GdiplusAdapter::GetPenAlignmentFromObj (Tcl_Obj *p, Gdiplus::PenAlignment *align)
{
    static char *alignments[] = {
	"Center", "Inset", 0L
    };
    enum alignments {
	Center, Inset
    };
    int index;

    if (Tcl_GetIndexFromObj(interp, p, (const char **)alignments, "PenAlignment", 0, &index)
	    != TCL_OK) {
	return TCL_ERROR;
    }

    switch((enum alignments) index) {
    case Center:
	*align = Gdiplus::PenAlignmentCenter; break;
    case Inset:
	*align = Gdiplus::PenAlignmentInset; break;
    }
    return TCL_OK;
}
