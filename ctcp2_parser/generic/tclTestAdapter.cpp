/*
 ------------------------------------------------------------------------------
 * tclTestAdaptor.cpp --
 *
 *   Tcl glue code for the Test interface.  Gives access to sections of 
 *   the code so they can be tested seperately from the entire unit.
 *
 * Copyright (c) 1999-2001 TomaSoft Engineering.
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: tclTestAdapter.cpp,v 1.1 2004/12/12 19:15:40 davygrvy Exp $
 ------------------------------------------------------------------------------
 */
#include "CTCP2Parse.hpp"
#include "cpptcl.hpp"
#include <string>

//-----------------------------------------------------------------------------

// CTCP/2 color attribute parser test class.  Used for validating the parser.
//
class TestCTCP2Action : protected CTCP2Parse
{
public:
    TestCTCP2Action(std::u16string *line, Tcl_Interp *interp)
	: CTCP2Parse(CTCP2Parse::Gateway)
    {
	resultPtr = Tcl_GetObjResult(interp);

	// Crank it through
	CTCP2Parse::ParseIt(line);
    }

private:
    virtual void PushStart ()
    {
    }

    virtual void PushBold (bool doit)
    {
	Tcl_Obj *subListPtr;
	Tcl_Obj *valListPtr[2];

	valListPtr[0] = Tcl_NewStringObj("PushBold", -1);
	valListPtr[1] = Tcl_NewBooleanObj(doit);
	subListPtr = Tcl_NewListObj(2, valListPtr);
	Tcl_ListObjAppendElement(NULL, resultPtr, subListPtr);
    }

    virtual void PushReverse (bool doit)
    {
	Tcl_Obj *subListPtr;
	Tcl_Obj *valListPtr[2];

	valListPtr[0] = Tcl_NewStringObj("PushReverse", -1);
	valListPtr[1] = Tcl_NewBooleanObj(doit);
	subListPtr = Tcl_NewListObj(2, valListPtr);
	Tcl_ListObjAppendElement(NULL, resultPtr, subListPtr);
    }

    virtual void PushUnderline (bool doit)
    {
	Tcl_Obj *subListPtr;
	Tcl_Obj *valListPtr[2];

	valListPtr[0] = Tcl_NewStringObj("PushUnderline", -1);
	valListPtr[1] = Tcl_NewBooleanObj(doit);
	subListPtr = Tcl_NewListObj(2, valListPtr);
	Tcl_ListObjAppendElement(NULL, resultPtr, subListPtr);
    }

    virtual void PushOverstrike (bool doit)
    {
	Tcl_Obj *subListPtr;
	Tcl_Obj *valListPtr[2];

	valListPtr[0] = Tcl_NewStringObj("PushOverstrike", -1);
	valListPtr[1] = Tcl_NewBooleanObj(doit);
	subListPtr = Tcl_NewListObj(2, valListPtr);
	Tcl_ListObjAppendElement(NULL, resultPtr, subListPtr);
    }

    virtual void PushItalic (bool doit)
    {
	Tcl_Obj *subListPtr;
	Tcl_Obj *valListPtr[2];

	valListPtr[0] = Tcl_NewStringObj("PushItalic", -1);
	valListPtr[1] = Tcl_NewBooleanObj(doit);
	subListPtr = Tcl_NewListObj(2, valListPtr);
	Tcl_ListObjAppendElement(NULL, resultPtr, subListPtr);
    }

    virtual void PushBlink (bool doit)
    {
	Tcl_Obj *subListPtr;
	Tcl_Obj *valListPtr[2];

	valListPtr[0] = Tcl_NewStringObj("PushBlink", -1);
	valListPtr[1] = Tcl_NewBooleanObj(doit);
	subListPtr = Tcl_NewListObj(2, valListPtr);
	Tcl_ListObjAppendElement(NULL, resultPtr, subListPtr);
    }

    virtual void PushURL (bool doit)
    {
	Tcl_Obj *subListPtr;
	Tcl_Obj *valListPtr[2];

	valListPtr[0] = Tcl_NewStringObj("PushURL", -1);
	valListPtr[1] = Tcl_NewBooleanObj(doit);
	subListPtr = Tcl_NewListObj(2, valListPtr);
	Tcl_ListObjAppendElement(NULL, resultPtr, subListPtr);
    }

    virtual void PushSpacing (CharSpacingType sp)
    {
	Tcl_Obj *subListPtr;
	Tcl_Obj *valListPtr[2];

	valListPtr[0] = Tcl_NewStringObj("PushSpacing", -1);

	switch (sp)
	{
	case Fixed:
	    valListPtr[1] = Tcl_NewStringObj("Fixed", -1);
	    break;
	case Proportional:
	    valListPtr[1] = Tcl_NewStringObj("Proportional", -1);
	    break;
	default:
	    valListPtr[1] = Tcl_NewStringObj("MASSIVE_ERROR", -1);
	}

	subListPtr = Tcl_NewListObj(2, valListPtr);
	Tcl_ListObjAppendElement(NULL, resultPtr, subListPtr);
    }

    virtual void PushFontSize (int size)
    {
	Tcl_Obj *subListPtr;
	Tcl_Obj *valListPtr[2];

	valListPtr[0] = Tcl_NewStringObj("PushFontSize", -1);
	valListPtr[1] = Tcl_NewIntObj(size);
	subListPtr = Tcl_NewListObj(2, valListPtr);
	Tcl_ListObjAppendElement(NULL, resultPtr, subListPtr);
    }

    virtual void PushForegroundColor (unsigned char R, unsigned char G, unsigned char B)
    {
	Tcl_Obj *subListPtr;
	Tcl_Obj *valListPtr[4];

	valListPtr[0] = Tcl_NewStringObj("PushForegroundColor", -1);
	valListPtr[1] = Tcl_NewIntObj(R);
	valListPtr[2] = Tcl_NewIntObj(G);
	valListPtr[3] = Tcl_NewIntObj(B);
	subListPtr = Tcl_NewListObj(4, valListPtr);
	Tcl_ListObjAppendElement(NULL, resultPtr, subListPtr);
    }

    virtual void PushBackgroundColor (unsigned char R, unsigned char G, unsigned char B)
    {
	Tcl_Obj *subListPtr;
	Tcl_Obj *valListPtr[4];

	valListPtr[0] = Tcl_NewStringObj("PushBackgroundColor", -1);
	valListPtr[1] = Tcl_NewIntObj(R);
	valListPtr[2] = Tcl_NewIntObj(G);
	valListPtr[3] = Tcl_NewIntObj(B);
	subListPtr = Tcl_NewListObj(4, valListPtr);
	Tcl_ListObjAppendElement(NULL, resultPtr, subListPtr);
    }

    virtual void PushTextSegment (std::u16string *segment)
    {
	// TODO! I hope we won't have to revisit this.
	_ASSERT(sizeof(char16_t) == sizeof(Tcl_UniChar));

	Tcl_ListObjAppendElement(NULL, resultPtr,
		Tcl_NewUnicodeObj((Tcl_UniChar *)segment->c_str(), segment->length()));
    }

    Tcl_Obj *resultPtr;
};

/*
class TestCTCP2EncodingAction : protected CTCP2PreParseEncodings
{
public:
    TestCTCP2EncodingAction(std::wstring *line, Tcl_Interp *interp)
	: CTCP2PreParseEncodings()
    {
	resultPtr = Tcl_GetObjResult(interp);

	// Crank it through
	CTCP2PreParseEncodings::ParseIt(line);
    }

private:
    virtual void PushStart ()
    {
    }

    virtual void PushEncoding (EncodingType enc)
    {
	Tcl_Obj *subListPtr;
	Tcl_Obj *valListPtr[2];

	valListPtr[0] = Tcl_NewStringObj("PushEncoding", -1);

    	switch (enc)
    	{
	case iso8859_1:
	    valListPtr[1] = Tcl_NewStringObj("iso8859-1", -1);
	    break;
	case iso8859_2:
	    valListPtr[1] = Tcl_NewStringObj("iso8859-2", -1);
	    break;
	case iso8859_3:
	    valListPtr[1] = Tcl_NewStringObj("iso8859-3", -1);
	    break;
	case iso8859_4:
	    valListPtr[1] = Tcl_NewStringObj("iso8859-4", -1);
	    break;
	case iso8859_5:
	    valListPtr[1] = Tcl_NewStringObj("iso8859-5", -1);
	    break;
	case iso8859_6:
	    valListPtr[1] = Tcl_NewStringObj("iso8859-6", -1);
	    break;
	case iso8859_7:
	    valListPtr[1] = Tcl_NewStringObj("iso8859-7", -1);
	    break;
	case iso8859_8:
	    valListPtr[1] = Tcl_NewStringObj("iso8859-8", -1);
	    break;
	case iso8859_9:
	    valListPtr[1] = Tcl_NewStringObj("iso8859-9", -1);
	    break;
	case UTF_8:
	    valListPtr[1] = Tcl_NewStringObj("UTF8", -1);
	    break;
	default:
	    valListPtr[1] = Tcl_NewStringObj("MASSIVE_ERROR", -1);
	}

	subListPtr = Tcl_NewListObj(2, valListPtr);
	Tcl_ListObjAppendElement(NULL, resultPtr, subListPtr);
    }

    virtual void PushTextSegment (std::u16string *segment)
    {
	Tcl_ListObjAppendElement(NULL, resultPtr,
		Tcl_NewUnicodeObj(segment->c_str(), segment->length()));
    }

    Tcl_Obj *resultPtr;
};
*/

//-----------------------------------------------------------------------------

class tclTestAdaptor
    : private Tcl::Adapter<tclTestAdaptor>
{
    friend class Tcl::Adapter<tclTestAdaptor>;

    ansiAttrCvt ansiCvt;    // the ansi2ctcp convertor
    mircAttrCvt mircCvt;    // the mirc2ctcp convertor
    irciiAttrCvt irciiCvt;  // the ircii2ctcp convertor
    hydraAttrCvt hydraCvt;  // the hydra2ctcp convertor

public:
    tclTestAdaptor(Tcl_Interp* interp)
	: Tcl::Adapter<tclTestAdaptor>(interp)
    {
	NewTclCmd("test::ansi", AnsiCmd);
	NewTclCmd("test::mirc", MircCmd);
	NewTclCmd("test::ircii", IrciiCmd);
	NewTclCmd("test::hydra", HydraCmd);
	NewTclCmd("test::ctcp2", ctcp2Cmd);
	//	NewTclCmd("test::enc",   encCmd);

	Tcl_PkgProvide(interp, "CTCP2Test", "1.0");
    };
    
private:
    int AnsiCmd (int objc, struct Tcl_Obj * const objv[])
    {
	Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);
	int size;

	if (objc != 2) return TCL_ERROR;
    
	const Tcl_UniChar *str = Tcl_GetUnicodeFromObj(objv[1], &size);
	std::u16string s(str, size);
	ansiCvt.ansi2ctcp(&s);
	Tcl_AppendUnicodeToObj(resultPtr, s.data(), s.length());
	return TCL_OK;
    }


    int MircCmd (int objc, struct Tcl_Obj * const objv[])
    {
	Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);
	int size;

	if (objc != 2) return TCL_ERROR;

	const Tcl_UniChar *str = Tcl_GetUnicodeFromObj(objv[1], &size);
	std::u16string s(str, size);
	mircCvt.mirc2ctcp(&s);
	Tcl_AppendUnicodeToObj(resultPtr, s.data(), s.length());
	return TCL_OK;
    }


    int IrciiCmd (int objc, struct Tcl_Obj * const objv[])
    {
	Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);
	int size;

	if (objc != 2) return TCL_ERROR;
    
	const Tcl_UniChar *str = Tcl_GetUnicodeFromObj(objv[1], &size);
	std::u16string s(str, size);
	irciiCvt.ircii2ctcp(&s);
	Tcl_AppendUnicodeToObj(resultPtr, s.data(), s.length());
	return TCL_OK;
    }


    int HydraCmd (int objc, struct Tcl_Obj * const objv[])
    {
	Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);
	int size;

	if (objc != 2) return TCL_ERROR;
    
	const Tcl_UniChar *str = Tcl_GetUnicodeFromObj(objv[1], &size);
	std::u16string s(str, size);
	hydraCvt.hydra2ctcp(&s);
	Tcl_AppendUnicodeToObj(resultPtr, s.data(), s.length());
	return TCL_OK;
    }


    int ctcp2Cmd (int objc, struct Tcl_Obj * const objv[])
    {
	Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);
	int size;

	if (objc != 2) return TCL_ERROR;
    
	const Tcl_UniChar *str = Tcl_GetUnicodeFromObj(objv[1], &size);
	std::u16string s(str, size);
	TestCTCP2Action parser_test(&s, interp);
	return TCL_OK;
    }


/*    int encCmd (int objc, struct Tcl_Obj * const objv[])
    {
	Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);
	int size;

	if (objc != 2) return TCL_ERROR;
    
	const Tcl_UniChar *str = Tcl_GetUnicodeFromObj(objv[1], &size);
	std::wstring s(str, size);
	TestCTCP2EncodingAction parser_test(&s, interp);
	return TCL_OK;
    }*/
};

#undef TCL_STORAGE_CLASS
#define TCL_STORAGE_CLASS DLLEXPORT

EXTERN int
Test_Init (Tcl_Interp *interp)
{
#ifdef USE_TCL_STUBS
    if (Tcl_InitStubs(interp, "8.4", 0) == NULL) {
	return TCL_ERROR;
    }
#endif
    new tclTestAdaptor(interp);
    return TCL_OK;
};

