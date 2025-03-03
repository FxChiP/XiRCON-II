#include "CTCP2Parse.hpp"
#include "cpptcl.hpp"

//-----------------------------------------------------------------------------

class CTCP2ParserInfo
{
public:
    CTCP2ParserInfo(
	Tcl_Interp *_interp,
	Tcl_Obj *_callback,
	CTCP2Parse::ParseMode _mode,
	CTCP2Parse::CharSpacingType _cspc,
	int fg,		// default 24-bit foreground color
	int bg)		// default 24-bit background color
	:
	interp(_interp), mode(_mode), callback(_callback)
    {
	Tcl_IncrRefCount(callback);
	data.fgR = (fg & 0xFF0000) >> 16;
	data.fgG = (fg & 0x00FF00) >> 8;
	data.fgB =  fg & 0x0000FF;

	data.bgR = (bg & 0xFF0000) >> 16;
	data.bgG = (bg & 0x00FF00) >> 8;
	data.bgB =  bg & 0x0000FF;
	data.cspc = _cspc;
    }

    ~CTCP2ParserInfo()
    {
	Tcl_DecrRefCount(callback);
    }

    Tcl_Interp *interp;
    Tcl_Obj *callback;
    CTCP2Parse::ParseMode mode;
    CTCP2Parse::DefaultData data;
};

//-----------------------------------------------------------------------------

// CTCP/2 text attribute action class inheriting the parser class.
//
class TclCallbackAction : protected CTCP2Parse
{
    CTCP2ParserInfo *info;
    Tcl_Obj *objv[5];
    Tcl_Obj *tagObj;
    int result;

public:
    TclCallbackAction(CTCP2ParserInfo *_info, std::u16string*line)
	: info(_info), CTCP2Parse(_info->mode), tagObj(0L), result(TCL_OK)
    {
	if (info->mode == Render) {
	    CTCP2Parse::SetDefaults(info->data);
	}

	objv[0] = info->callback;

	// Crank it through
	CTCP2Parse::ParseIt(line);
    }

    inline int Result () {return result;}

    ~TclCallbackAction()
    {
    }

private:
    void DoErrorReport (const char *msg)
    {
	Tcl_AddObjErrorInfo(info->interp, "\n", -1);
	Tcl_AddObjErrorInfo(info->interp, msg, -1);
	result = TCL_ERROR;
    }

    virtual void PushStart ()
    {
	int code;

	if (tagObj != 0L) {
	    Tcl_IncrRefCount(objv[1] = Tcl_NewStringObj("tag", -1));
	    Tcl_IncrRefCount(objv[2] = tagObj);
	    code = Tcl_EvalObjv(info->interp, 3, objv, TCL_EVAL_DIRECT);
	    Tcl_DecrRefCount(objv[1]);
	    Tcl_DecrRefCount(objv[2]);

	    if (code == TCL_ERROR) {
		DoErrorReport("Error calling tag.");
	    } else {
		Tcl_Obj **objv;
		int objc;

		code = Tcl_ListObjGetElements(info->interp,
			Tcl_GetObjResult(info->interp), &objc, &objv);
		if (code == TCL_ERROR || objc != 2) {
		    DoErrorReport("Error getting colors back from tag.  Must be a 2 element list.");
		} else if (Tcl_GetIntFromObj(info->interp, objv[0], fgCache) == TCL_ERROR) {
		    DoErrorReport("Error getting colors back from tag.  fg was bad.");
		} else if (Tcl_GetIntFromObj(info->interp, objv[1], bgCache) == TCL_ERROR) {
		    DoErrorReport("Error getting colors back from tag.  bg was bad.");
		}
	    }
	    tagObj = 0L;
	}
    }

    virtual void PushBold (bool doit)
    {
	Tcl_IncrRefCount(objv[1] = Tcl_NewStringObj("bold", -1));
	Tcl_IncrRefCount(objv[2] = Tcl_NewBooleanObj(doit));
	if (Tcl_EvalObjv(info->interp, 3, objv, TCL_EVAL_DIRECT) != TCL_OK) {
	    DoErrorReport("Error calling calling callback.");
	}
	Tcl_DecrRefCount(objv[1]);
	Tcl_DecrRefCount(objv[2]);
    }

    virtual void PushReverse (bool doit)
    {
	Tcl_IncrRefCount(objv[1] = Tcl_NewStringObj("reverse", -1));
	Tcl_IncrRefCount(objv[2] = Tcl_NewBooleanObj(doit));
	if (Tcl_EvalObjv(info->interp, 3, objv, TCL_EVAL_DIRECT) != TCL_OK) {
	    DoErrorReport("Error calling calling callback.");
	}
	Tcl_DecrRefCount(objv[1]);
	Tcl_DecrRefCount(objv[2]);
    }

    virtual void PushUnderline (bool doit)
    {
	Tcl_IncrRefCount(objv[1] = Tcl_NewStringObj("underline", -1));
	Tcl_IncrRefCount(objv[2] = Tcl_NewBooleanObj(doit));
	if (Tcl_EvalObjv(info->interp, 3, objv, TCL_EVAL_DIRECT) != TCL_OK) {
	    DoErrorReport("Error calling calling callback.");
	}
	Tcl_DecrRefCount(objv[1]);
	Tcl_DecrRefCount(objv[2]);
    }

    virtual void PushOverstrike (bool doit)
    {
	Tcl_IncrRefCount(objv[1] = Tcl_NewStringObj("overstrike", -1));
	Tcl_IncrRefCount(objv[2] = Tcl_NewBooleanObj(doit));
	if (Tcl_EvalObjv(info->interp, 3, objv, TCL_EVAL_DIRECT) != TCL_OK) {
	    DoErrorReport("Error calling calling callback.");
	}
	Tcl_DecrRefCount(objv[1]);
	Tcl_DecrRefCount(objv[2]);
    }

    virtual void PushItalic (bool doit)
    {
	Tcl_IncrRefCount(objv[1] = Tcl_NewStringObj("italic", -1));
	Tcl_IncrRefCount(objv[2] = Tcl_NewBooleanObj(doit));
	if (Tcl_EvalObjv(info->interp, 3, objv, TCL_EVAL_DIRECT) != TCL_OK) {
	    DoErrorReport("Error calling calling callback.");
	}
	Tcl_DecrRefCount(objv[1]);
	Tcl_DecrRefCount(objv[2]);
    }

    virtual void PushBlink (bool doit)
    {
	Tcl_IncrRefCount(objv[1] = Tcl_NewStringObj("blink", -1));
	Tcl_IncrRefCount(objv[2] = Tcl_NewBooleanObj(doit));
	if (Tcl_EvalObjv(info->interp, 3, objv, TCL_EVAL_DIRECT) != TCL_OK) {
	    DoErrorReport("Error calling calling callback.");
	}
	Tcl_DecrRefCount(objv[1]);
	Tcl_DecrRefCount(objv[2]);
    }

    virtual void PushURL (bool doit)
    {
	Tcl_IncrRefCount(objv[1] = Tcl_NewStringObj("url", -1));
	Tcl_IncrRefCount(objv[2] = Tcl_NewBooleanObj(doit));
	if (Tcl_EvalObjv(info->interp, 3, objv, TCL_EVAL_DIRECT) != TCL_OK) {
	    DoErrorReport("Error calling calling callback.");
	}
	Tcl_DecrRefCount(objv[1]);
	Tcl_DecrRefCount(objv[2]);
    }

    virtual void PushSpacing (CharSpacingType sp)
    {
	Tcl_IncrRefCount(objv[1] = Tcl_NewStringObj("spacing", -1));
	switch (sp) {
	case Fixed:
	    Tcl_IncrRefCount(objv[2] = Tcl_NewStringObj("fixed", -1));
	case Proportional:
	    Tcl_IncrRefCount(objv[2] = Tcl_NewStringObj("proportional", -1));
	}
	if (Tcl_EvalObjv(info->interp, 3, objv, TCL_EVAL_DIRECT) != TCL_OK) {
	    DoErrorReport("Error calling calling callback.");
	}
	Tcl_DecrRefCount(objv[1]);
	Tcl_DecrRefCount(objv[2]);
    }

    virtual void PushFontSize (int size)
    {
	Tcl_IncrRefCount(objv[1] = Tcl_NewStringObj("fontsize", -1));
	Tcl_IncrRefCount(objv[2] = Tcl_NewIntObj(size));
	if (Tcl_EvalObjv(info->interp, 3, objv, TCL_EVAL_DIRECT) != TCL_OK) {
	    DoErrorReport("Error calling calling callback.");
	}
	Tcl_DecrRefCount(objv[1]);
	Tcl_DecrRefCount(objv[2]);
    }

    virtual void PushForegroundColor (byte R, byte G, byte B)
    {
	Tcl_IncrRefCount(objv[1] = Tcl_NewStringObj("forecolor", -1));
	Tcl_IncrRefCount(objv[2] = Tcl_NewIntObj(R));
	Tcl_IncrRefCount(objv[3] = Tcl_NewIntObj(G));
	Tcl_IncrRefCount(objv[4] = Tcl_NewIntObj(B));
	if (Tcl_EvalObjv(info->interp, 5, objv, TCL_EVAL_DIRECT) != TCL_OK) {
	    DoErrorReport("Error calling calling callback.");
	}
	Tcl_DecrRefCount(objv[1]);
	Tcl_DecrRefCount(objv[2]);
	Tcl_DecrRefCount(objv[3]);
	Tcl_DecrRefCount(objv[4]);
    }

    virtual void PushForegroundColorReset ()
    {
	Tcl_IncrRefCount(objv[1] = Tcl_NewStringObj("forecolor", -1));
	Tcl_IncrRefCount(objv[2] = Tcl_NewStringObj("reset", -1));
	if (Tcl_EvalObjv(info->interp, 3, objv, TCL_EVAL_DIRECT) != TCL_OK) {
	    DoErrorReport("Error calling calling callback.");
	}
	Tcl_DecrRefCount(objv[1]);
	Tcl_DecrRefCount(objv[2]);
    }

    virtual void PushBackgroundColor (byte R, byte G, byte B)
    {
	Tcl_IncrRefCount(objv[1] = Tcl_NewStringObj("backcolor", -1));
	Tcl_IncrRefCount(objv[2] = Tcl_NewIntObj(R));
	Tcl_IncrRefCount(objv[3] = Tcl_NewIntObj(G));
	Tcl_IncrRefCount(objv[4] = Tcl_NewIntObj(B));
	if (Tcl_EvalObjv(info->interp, 5, objv, TCL_EVAL_DIRECT) != TCL_OK) {
	    DoErrorReport("Error calling calling callback.");
	}
	Tcl_DecrRefCount(objv[1]);
	Tcl_DecrRefCount(objv[2]);
	Tcl_DecrRefCount(objv[3]);
	Tcl_DecrRefCount(objv[4]);
    }

    virtual void PushBackgroundColorReset ()
    {
	Tcl_IncrRefCount(objv[1] = Tcl_NewStringObj("backcolor", -1));
	Tcl_IncrRefCount(objv[2] = Tcl_NewStringObj("reset", -1));
	if (Tcl_EvalObjv(info->interp, 3, objv, TCL_EVAL_DIRECT) != TCL_OK) {
	    DoErrorReport("Error calling calling callback.");
	}
	Tcl_DecrRefCount(objv[1]);
	Tcl_DecrRefCount(objv[2]);
    }

    virtual void PushTextSegment (std::u16string *s)
    {
	Tcl_IncrRefCount(objv[1] = Tcl_NewStringObj("segment", -1));
	Tcl_IncrRefCount(objv[2] = Tcl_NewUnicodeObj((Tcl_UniChar*)s->c_str(), s->length()));
	if (Tcl_EvalObjv(info->interp, 3, objv, TCL_EVAL_DIRECT) != TCL_OK) {
	    DoErrorReport("Error calling calling callback.");
	}
	Tcl_DecrRefCount(objv[1]);
	Tcl_DecrRefCount(objv[2]);
    }

    virtual int ParseExtension (std::u16string *string, int *offset, int *endoffset)
    {
	int index, start;;
	char16_t ch;

	if (*offset < *endoffset) {
	    switch (string->at(*offset)) {
		// Tags (dynamic color settings)
		case 'T':
		    start = (*offset)++;
		    ch = string->at(*offset);
		    if (string->at(*offset) >= '0' && string->at(*offset) <= '9') {
			if (string->at((*offset)+1) >= '0' && string->at((*offset)+1) <= '9') {
			    // two digit
			    index = (((string->at(*offset) - '0') * 10) + (string->at((*offset)+1) - '0'));
			    *offset += 2;
			} else {
			    // one digit.
			    index = string->at(*offset) - '0';
			    (*offset)++;
			}
			if (index > 16) {
			    // malformed.
			    *offset = start;
			    return 1;
			}

			fg = RGBColor::Indeterminate;
			bg = RGBColor::Indeterminate;

			if (tagObj != 0L) {
			    Tcl_DecrRefCount(tagObj);
			}
			switch (index) {
			case 0:	    // default.
			    tagObj = Tcl_NewStringObj("default", -1); break;
			case 1:	    // public.
			    tagObj = Tcl_NewStringObj("public", -1); break;
			case 2:	    // private.
			    tagObj = Tcl_NewStringObj("private", -1); break;
			case 3:	    // action.
			    tagObj = Tcl_NewStringObj("action", -1); break;
			case 4:	    // notice.
			    tagObj = Tcl_NewStringObj("notice", -1); break;
			case 5:	    // ctcp.
			    tagObj = Tcl_NewStringObj("ctcp", -1); break;
			case 6:	    // change.
			    tagObj = Tcl_NewStringObj("change", -1); break;
			case 7:	    // join.
			    tagObj = Tcl_NewStringObj("join", -1); break;
			case 8:	    // part.
			    tagObj = Tcl_NewStringObj("part", -1); break;
			case 9:	    // kick.
			    tagObj = Tcl_NewStringObj("kick", -1); break;
			case 10:    // quit.
			    tagObj = Tcl_NewStringObj("quit", -1); break;
			case 11:    // highlight.
			    tagObj = Tcl_NewStringObj("highlight", -1); break;
			case 12:    // error.
			    tagObj = Tcl_NewStringObj("error", -1); break;
			case 13:    // nick.
			    tagObj = Tcl_NewStringObj("nick", -1); break;
			case 14:    // channel.
			    tagObj = Tcl_NewStringObj("channel", -1); break;
			case 15:    // mode.
			    tagObj = Tcl_NewStringObj("mode", -1); break;
			case 16:    // socket.
			    tagObj = Tcl_NewStringObj("socket", -1); break;
			}
		    } else {
			// malformed.
			*offset = start;
			return 1;
		    }
		    break;
		default:
		    // error.
		    return 1;
	    }
	    return 0;
	} else {
	    // malformed.
	    return 1;
	}
    }

    virtual void ClearExtensions ()
    {
	if (tagObj != 0L) {
	    Tcl_DecrRefCount(tagObj);
	}
    }
};
