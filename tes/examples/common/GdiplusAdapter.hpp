#ifndef INC_GdiplusAdapter_hpp__
#define INC_GdiplusAdapter_hpp__

// M$ broke how winsock(2) is included from windows.h, again!
#define _WINSOCKAPI_
#include <windows.h>
#include <gdiplus.h>

#include "ItclAdapter.hpp"
#include "TclHash.hpp"

class GdiplusAdapter : protected Itcl::IAdapter<GdiplusAdapter>
{
public:
    GdiplusAdapter(Tcl_Interp *interp);

protected:

    virtual int GraphicsConstructCmd (int objc, struct Tcl_Obj * CONST objv[]) = 0;
    virtual int GraphicsDestructCmd (int objc, struct Tcl_Obj * CONST objv[]) = 0;

    // Used to associate the C++ class instances to their [Incr Tcl]
    // object context.
    Tcl::Hash<Gdiplus::Bitmap *,    TCL_ONE_WORD_KEYS> BitmapHash;
    Tcl::Hash<Gdiplus::Brush *,	    TCL_ONE_WORD_KEYS> BrushHash;
    Tcl::Hash<Gdiplus::CachedBitmap *, TCL_ONE_WORD_KEYS> CachedBitmapHash;
    Tcl::Hash<Gdiplus::Font *,	    TCL_ONE_WORD_KEYS> FontHash;
    Tcl::Hash<Gdiplus::Graphics *,  TCL_ONE_WORD_KEYS> GraphicsHash;
    Tcl::Hash<Gdiplus::Image *,	    TCL_ONE_WORD_KEYS> ImageHash;
    Tcl::Hash<Gdiplus::Matrix *,    TCL_ONE_WORD_KEYS> MatrixHash;
    Tcl::Hash<Gdiplus::Pen *,	    TCL_ONE_WORD_KEYS> PenHash;
    Tcl::Hash<Gdiplus::Region *,    TCL_ONE_WORD_KEYS> RegionHash;
    Tcl::Hash<Gdiplus::StringFormat *, TCL_ONE_WORD_KEYS> StringFormatHash;

    void SetTclErrorMsg (Gdiplus::Status err);

private:
    int MakeARGB (Tcl_Obj *c, Gdiplus::ARGB *argb);
    int ObjToCombineMode (Tcl_Obj *strMode, Gdiplus::CombineMode *mode);
    int GetHatchStyle (Tcl_Obj *h, Gdiplus::HatchStyle *hs);
    int UnitToResult (Gdiplus::Unit unit);
    int PenAlignmentToResult (Gdiplus::PenAlignment align);
    int GetPenAlignmentFromObj (Tcl_Obj *p, Gdiplus::PenAlignment *align);

    // Gdiplus::Graphics
    int GraphicsAddMetafileCommentCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsBeginContainerCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsClearCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsDrawArcCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsDrawBezierCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsDrawCachedBitmapCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsDrawClosedCurveCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsDrawCurveCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsDrawDriverStringCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsDrawEllipseCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsDrawImageCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsDrawLineCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsDrawPathCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsDrawPieCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsDrawPolygonCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsDrawPolygonListCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsDrawRectangleCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsDrawStringCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsEndContainerCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsEnumerateMetafileCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsExcludeClipCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsFillClosedCurveCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsFillEllipseCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsFillPathCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsFillPieCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsFillPolygonCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsFillRectangleCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsFillRegionCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsFlushCmd (int objc, struct Tcl_Obj * CONST objv[]);

    // Get/Set pairs
    int GraphicsClipRegionCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsCompositingModeCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsCompositingQualityCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsInterpolationModeCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsPageScaleCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsPageUnitCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsPixelOffsetModeCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsRenderingOriginCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsSmoothingModeCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsTextContrastCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsTextRenderingHintCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsTransformCmd (int objc, struct Tcl_Obj * CONST objv[]);

    int GraphicsGetDpiXCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsGetDpiYCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsGetNearestColorCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsGetVisibleClipBoundsCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsIntersectClipCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsIsClipEmptyCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsIsVisibleCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsIsVisibleClipEmptyCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsMeasureCharacterRangesCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsMeasureDriverStringCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsMeasureStringCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsMultiplyTransformCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsResetClipCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsResetTransformCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsRestoreCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsRotateTransformCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsSaveCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsScaleTransformCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsTransformPointsCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsTranslateClipCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int GraphicsTranslateTransformCmd (int objc, struct Tcl_Obj * CONST objv[]);


    // Gdiplus::Pen
    int PenConstructCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int PenDestructCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int PenMultiplyTransformCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int PenResetTransformCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int PenRotateTransformCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int PenScaleTransformCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int PenAlignmentCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int PenBrushCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int PenColorCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int PenCompoundArrayCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int PenCustomEndCapCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int PenCustomStartCapCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int PenDashCapCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int PenDashOffsetCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int PenDashPatternCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int PenDashStyleCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int PenEndCapCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int PenLineJoinCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int PenMiterLimitCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int PenStartCapCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int PenTransformCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int PenWidthCmd (int objc, struct Tcl_Obj * CONST objv[]);

    // Gdiplus::(*)Brush
    int SolidBrushConstructCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int SolidBrushDestructCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int TextureBrushConstructCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int TextureBrushDestructCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int LinearGradientBrushConstructCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int LinearGradientBrushDestructCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int HatchBrushConstructCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int HatchBrushDestructCmd (int objc, struct Tcl_Obj * CONST objv[]);

    // Gdiplus::Font
    int FontConstructCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int FontDestructCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int FontGetHeightCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int FontGetUnitCmd (int objc, struct Tcl_Obj * CONST objv[]);

    // Gdiplus::Matrix
    int MatrixConstructCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int MatrixDestructCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int MatrixEqualsCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int MatrixInvertCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int MatrixIsIdentityCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int MatrixIsInvertibleCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int MatrixMultiplyCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int MatrixOffsetXCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int MatrixOffsetYCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int MatrixRotateCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int MatrixRotateAtCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int MatrixScaleCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int MatrixShearCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int MatrixTranslateCmd (int objc, struct Tcl_Obj * CONST objv[]);

    // Gdiplus::StringFormat
    int StringFormatConstructCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int StringFormatDestructCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int StringFormatAlignmentCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int StringFormatDigitSubstitutionLanguageCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int StringFormatFormatFlagsCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int StringFormatHotkeyPrefixCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int StringFormatLineAlignmentCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int StringFormatMeasurableCharacterRangeCountCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int StringFormatTabStopsCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int StringFormatTrimmingCmd (int objc, struct Tcl_Obj * CONST objv[]);

    // Gdiplus::Region
    int RegionConstructCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int RegionDestructCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int RegionTransformCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int RegionTranslateCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int RegionUnionCmd (int objc, struct Tcl_Obj * CONST objv[]);
    int RegionXorCmd (int objc, struct Tcl_Obj * CONST objv[]);
};

#endif