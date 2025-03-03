/*
 ------------------------------------------------------------------------------
 * IRCWindowsItclAdaptor.cpp --
 *
 *  The [Incr Tcl] glue code for the IRC windowing system's top-level classes.
 *  Through inheritence, it is assumed to be combined with the IRC_Engine.
 *
 * Copyright (c) 1999-2000 Tomahawk Software Group
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: IRCWindowsItclAdapter.cpp,v 1.2 2003/11/16 02:04:20 davygrvy Exp $
 ------------------------------------------------------------------------------
 */

#include "IRCWindows.hpp"
#include "cpptcl.hpp"
#include "ItclAdapter.hpp"


// Collection class for the IRCUi stuff.
//
class IRCUi
{

public:
    IRCUi() {
	sb = new StatBox();
    }
    ~IRCUi() {
	delete sb;
    }
    StatBox *sb;			    // Only one Status window.
    Tcl::Hash<ChanBox *> ChanBoxes;	    // Channel windows.
    Tcl::Hash<QueryBox *> QueryBoxes;	    // Query windows.
};


// This is the interface to Tcl for acting upon IRCUi instances.
//
class IRCWindowsItclAdapter
    : Itcl::IAdapter<IRCWindowsItclAdapter>
{
    friend class Tcl::Adapter<IRCWindowsItclAdapter>;
    Tcl::Hash<IRCUi *, TCL_ONE_WORD_KEYS> IRCUis;

    // DCC Chat windows are separate from an IRC connection.
    // Once established, they are disconnected from the
    // IRC connection association.
    //
    Tcl::Hash<DCCChatBox *> DCCChatBoxes;   // DCC Chat windows.


public:
    IRCWindowsItclAdapter(Tcl_Interp *interp)
	: Itcl::IAdapter<IRCWindowsItclAdapter>(interp)
    {
	NewItclCmd("irc-ui-construct",	&IRCWindowsItclAdapter::ConstructCmd);
	NewItclCmd("irc-ui-destruct",	&IRCWindowsItclAdapter::DestructCmd);
	NewItclCmd("irc-ui-echo",	&IRCWindowsItclAdapter::EchoCmd);
//	NewItclCmd("irc-ui-window",	&IRCWindowsItclAdapter::WindowCmd);
	NewItclCmd("irc-ui-menu",	&IRCWindowsItclAdapter::NotImplementedCmd);
	NewItclCmd("irc-ui-nicklist",	&IRCWindowsItclAdapter::NotImplementedCmd);
	NewItclCmd("irc-ui-hotkey",	&IRCWindowsItclAdapter::NotImplementedCmd);
	NewItclCmd("irc-ui-alias",	&IRCWindowsItclAdapter::NotImplementedCmd);
	NewItclCmd("irc-ui-channel",	&IRCWindowsItclAdapter::NotImplementedCmd);
	NewItclCmd("irc-ui-query",	&IRCWindowsItclAdapter::NotImplementedCmd);
	NewItclCmd("irc-ui-chat",	&IRCWindowsItclAdapter::NotImplementedCmd);
	NewItclCmd("irc-ui-queries",	&IRCWindowsItclAdapter::NotImplementedCmd);
	NewItclCmd("irc-ui-chats",	&IRCWindowsItclAdapter::NotImplementedCmd);
	NewItclCmd("irc-ui-say",	&IRCWindowsItclAdapter::NotImplementedCmd);
	NewItclCmd("irc-ui-input",	&IRCWindowsItclAdapter::NotImplementedCmd);

	// For use with Tk, so we can embed into.
	NewTclCmd("IRC::makeblank",	&IRCWindowsItclAdapter::MakeBlankCmd);

	Tcl_PkgProvide(interp, "IRC_UserInterface", "0.0.1");
    }

    ~IRCWindowsItclAdapter()
    {
	IRCUi *ui;
	int result;

	for (
	    result = IRCUis.Top(&ui);
	    result != TCL_ERROR;
	    result = IRCUis.Next(&ui)
	) {
	    delete ui;
	}
    }
private:

    int ConstructCmd (int objc, struct Tcl_Obj * const objv[])
    {
	ItclObject *ItclObj;
	IRCUi *ui;

	if (objc != 1) {
	    Tcl_WrongNumArgs(interp, 1, objv, "");
	    return TCL_ERROR;
	}

	// Retrieve the Itcl object context.
	//
	if (GetItclObj(&ItclObj, objv[0]) != TCL_OK) return TCL_ERROR;

	// Using the new Itcl object context pointer as our key, create a
	// new IRCUi C++ object and store the pointer in the hash
	// table.
	//
	ui = new IRCUi();
	IRCUis.Add(ItclObj, ui);
	return TCL_OK;
    }


    int DestructCmd (int objc, struct Tcl_Obj * const objv[])
    {
	ItclObject *ItclObj;
	IRCUi *ui;

	if (objc != 1) {
	    Tcl_WrongNumArgs(interp, 1, objv, "");
	    return TCL_ERROR;
	}

	// Retrieve the Itcl object context.
	//
	if (GetItclObj(&ItclObj, objv[0]) != TCL_OK) return TCL_ERROR;

	// Remove the C++ IRCUi instance associated to this Itcl
	// instance.
	//
	if (IRCUis.Extract(ItclObj, &ui) == TCL_OK) {

	    // Itcl's destructor may be called even though the Itcl class
	    // constructor returned with an error!  Thus, no IRCUi
	    // instance exists in the hash table even though an Itcl
	    // object context exists.  Only delete what we know is there.
	    //
	    delete ui;
	}
	return TCL_OK;
    }


    int EchoCmd (int objc, struct Tcl_Obj * const objv[])
    {
	ItclObject *ItclObj;
	IRCUi *ui;
	char16_t *txt;
	size_t len;

	if (objc < 2 || objc > 4) {
	    Tcl_WrongNumArgs(interp, 1, objv, "text ?type? ?which?");
	    return TCL_ERROR;
	}

	// Get the Itcl object.
	//
	if (GetItclObj(&ItclObj, objv[0]) != TCL_OK) return TCL_ERROR;

	// Get the IRCUi instance associated to this Itcl object.
	//
	if (IRCUis.Find(ItclObj, &ui) != TCL_OK) {
	    Tcl_SetObjResult(interp,
		    Tcl_NewStringObj("IRCUi instance lost!", -1));
	    return TCL_ERROR;
	}

	txt = (char16_t *) Tcl_GetUnicode(objv[1]);
	len = (size_t) Tcl_GetCharLength(objv[1]);

	if (objc == 2 || objc == 3) {
	    ui->sb->Echo(txt, len);
	} else {
	    const char *type = Tcl_GetString(objv[2]);
	    const char *which = Tcl_GetString(objv[3]);
	    if (!stricmp(type, "channel")) {
		ChanBox *chanbox;
		if (ui->ChanBoxes.Find(which, &chanbox) != TCL_ERROR) {
		    chanbox->Echo(txt, len);
		} else {
		    ui->sb->Echo(txt, len);
		}
	    } else if (!stricmp(type, "query")) {
		QueryBox *qbox;
		if (ui->QueryBoxes.Find(which, &qbox) != TCL_ERROR) {
		    qbox->Echo(txt, len);
		} else {
		    ui->sb->Echo(txt, len);
		}
	    }
	}
	return TCL_OK;
    }

#if 0
    int WindowCmd (int objc, struct Tcl_Obj * const objv[])
    {
	Tcl_Obj *resultPtr;
	int index;
	static const char *subcommands[] = {
	    "create", 0L
	};
	enum subcommands {
	    WND_CREATE
	};

	if (objc < 2) {
	    Tcl_WrongNumArgs(interp, 1, objv, "subcommand arg ?arg ...?");
	    return TCL_ERROR;
	}
    
	if (Tcl_GetIndexFromObj(interp, objv[1], subcommands, "subcommand", 0,
		&index) != TCL_OK) {
	    return TCL_ERROR;
	}

	resultPtr = Tcl_GetObjResult(interp);
	switch ((enum subcommands) index)
	{
	case WND_CREATE:
	    {
		int index;
		static const char *type[] = {
		    "status", "channel", "query",
		    "chat", 0L
		};
		enum type {
		    WND_CREATE_STATUS, WND_CREATE_CHANNEL, WND_CREATE_QUERY,
		    WND_CREATE_CHAT
		};
		
		if (objc < 3) {
		    //wnd_create_args:
		    Tcl_WrongNumArgs(interp, 2, objv, "type ?-title titleBarText? ?-associate associateName?");
		    return TCL_ERROR;
		}

		if (Tcl_GetIndexFromObj(interp, objv[2], type, "type", 0,
			&index) != TCL_OK) {
		    return TCL_ERROR;
		}

		switch ((enum type) index)
		{
		case WND_CREATE_STATUS:
		    if (!sb) {
			sb = new StatBox();
		    } else {
			Tcl_SetObjResult(interp,
				Tcl_NewStringObj("A status window already exists.", -1));
			return TCL_ERROR;
		    }

		    // check for options
		    if (objc > 3) {
			int index;
			static const char *options[] = {"-title", "-associate", 0L};
			enum options {WND_CREATE_STATUS_TITLE, WND_CREATE_STATUS_ASSOC};
			if (Tcl_GetIndexFromObj(interp, objv[3], options, "options", 0,
				&index) != TCL_OK) {
			    return TCL_ERROR;
			}
			switch ((enum options) index)
			{
			case WND_CREATE_STATUS_TITLE:
			    {
				Tcl_UniChar *title;
				int len;

				title = Tcl_GetUnicodeFromObj(objv[4], &len);
				sb->SetTitle(title, len);
			    }
			    break;
			case WND_CREATE_STATUS_ASSOC:
			    break;
			}
		    }
		    break;
		case WND_CREATE_CHANNEL:
		case WND_CREATE_QUERY:
		case WND_CREATE_CHAT:
		    Tcl_SetObjResult(interp,
			    Tcl_NewStringObj("nothing to do here", -1));
		    return TCL_ERROR;
		    break;
		}
	    }
	}
	return TCL_OK;
    }
#endif

    
    int MenuCmd (int objc, struct Tcl_Obj * const objv[])
    {
	ItclObject *ItclObj;
	IRCUi *ui;

	// Get the Itcl object.
	//
	if (GetItclObj(&ItclObj, objv[0]) != TCL_OK) return TCL_ERROR;

	// Get the IRCUi instance associated to this Itcl object.
	//
	if (IRCUis.Find(ItclObj, &ui) != TCL_OK) {
	    Tcl_SetObjResult(interp,
		    Tcl_NewStringObj("IRCUi instance lost!", -1));
	    return TCL_ERROR;
	}

	//ui->???

	return TCL_ERROR;
    }

    int HotkeyCmd (int objc, struct Tcl_Obj * const objv[])
    {
	ItclObject *ItclObj;
	IRCUi *ui;

	// Get the Itcl object.
	//
	if (GetItclObj(&ItclObj, objv[0]) != TCL_OK) return TCL_ERROR;

	// Get the IRCUi instance associated to this Itcl object.
	//
	if (IRCUis.Find(ItclObj, &ui) != TCL_OK) {
	    Tcl_SetObjResult(interp,
		    Tcl_NewStringObj("IRCUi instance lost!", -1));
	    return TCL_ERROR;
	}

	//ui->???

	return TCL_ERROR;
    }

    int AliasCmd (int objc, struct Tcl_Obj * const objv[])
    {
	ItclObject *ItclObj;
	IRCUi *ui;

	// Get the Itcl object.
	//
	if (GetItclObj(&ItclObj, objv[0]) != TCL_OK) return TCL_ERROR;

	// Get the IRCUi instance associated to this Itcl object.
	//
	if (IRCUis.Find(ItclObj, &ui) != TCL_OK) {
	    Tcl_SetObjResult(interp,
		    Tcl_NewStringObj("IRCUi instance lost!", -1));
	    return TCL_ERROR;
	}

	//ui->???

	return TCL_ERROR;
    }

    int ChannelCmd (int objc, struct Tcl_Obj * const objv[])
    {
	ItclObject *ItclObj;
	IRCUi *ui;

	// Get the Itcl object.
	//
	if (GetItclObj(&ItclObj, objv[0]) != TCL_OK) return TCL_ERROR;

	// Get the IRCUi instance associated to this Itcl object.
	//
	if (IRCUis.Find(ItclObj, &ui) != TCL_OK) {
	    Tcl_SetObjResult(interp,
		    Tcl_NewStringObj("IRCUi instance lost!", -1));
	    return TCL_ERROR;
	}

	//ui->???

	return TCL_ERROR;
    }

    int QueryCmd (int objc, struct Tcl_Obj * const objv[])
    {
	ItclObject *ItclObj;
	IRCUi *ui;

	// Get the Itcl object.
	//
	if (GetItclObj(&ItclObj, objv[0]) != TCL_OK) return TCL_ERROR;

	// Get the IRCUi instance associated to this Itcl object.
	//
	if (IRCUis.Find(ItclObj, &ui) != TCL_OK) {
	    Tcl_SetObjResult(interp,
		    Tcl_NewStringObj("IRCUi instance lost!", -1));
	    return TCL_ERROR;
	}

	//ui->???

	return TCL_ERROR;
    }

    int ChatCmd (int objc, struct Tcl_Obj * const objv[])
    {
	ItclObject *ItclObj;
	IRCUi *ui;

	// Get the Itcl object.
	//
	if (GetItclObj(&ItclObj, objv[0]) != TCL_OK) return TCL_ERROR;

	// Get the IRCUi instance associated to this Itcl object.
	//
	if (IRCUis.Find(ItclObj, &ui) != TCL_OK) {
	    Tcl_SetObjResult(interp,
		    Tcl_NewStringObj("IRCUi instance lost!", -1));
	    return TCL_ERROR;
	}

	//ui->???

	return TCL_ERROR;
    }

    int ChannelsCmd (int objc, struct Tcl_Obj * const objv[])
    {
	ItclObject *ItclObj;
	IRCUi *ui;

	// Get the Itcl object.
	//
	if (GetItclObj(&ItclObj, objv[0]) != TCL_OK) return TCL_ERROR;

	// Get the IRCUi instance associated to this Itcl object.
	//
	if (IRCUis.Find(ItclObj, &ui) != TCL_OK) {
	    Tcl_SetObjResult(interp,
		    Tcl_NewStringObj("IRCUi instance lost!", -1));
	    return TCL_ERROR;
	}

	//ui->???

	return TCL_ERROR;
    }

    int QueriesCmd (int objc, struct Tcl_Obj * const objv[])
    {
	ItclObject *ItclObj;
	IRCUi *ui;

	// Get the Itcl object.
	//
	if (GetItclObj(&ItclObj, objv[0]) != TCL_OK) return TCL_ERROR;

	// Get the IRCUi instance associated to this Itcl object.
	//
	if (IRCUis.Find(ItclObj, &ui) != TCL_OK) {
	    Tcl_SetObjResult(interp,
		    Tcl_NewStringObj("IRCUi instance lost!", -1));
	    return TCL_ERROR;
	}

	//ui->???

	return TCL_ERROR;
    }

    int ChatsCmd (int objc, struct Tcl_Obj * const objv[])
    {
	ItclObject *ItclObj;
	IRCUi *ui;

	// Get the Itcl object.
	//
	if (GetItclObj(&ItclObj, objv[0]) != TCL_OK) return TCL_ERROR;

	// Get the IRCUi instance associated to this Itcl object.
	//
	if (IRCUis.Find(ItclObj, &ui) != TCL_OK) {
	    Tcl_SetObjResult(interp,
		    Tcl_NewStringObj("IRCUi instance lost!", -1));
	    return TCL_ERROR;
	}

	//ui->???

	return TCL_ERROR;
    }

    int NicksCmd (int objc, struct Tcl_Obj * const objv[])
    {
	ItclObject *ItclObj;
	IRCUi *ui;

	// Get the Itcl object.
	//
	if (GetItclObj(&ItclObj, objv[0]) != TCL_OK) return TCL_ERROR;

	// Get the IRCUi instance associated to this Itcl object.
	//
	if (IRCUis.Find(ItclObj, &ui) != TCL_OK) {
	    Tcl_SetObjResult(interp,
		    Tcl_NewStringObj("IRCUi instance lost!", -1));
	    return TCL_ERROR;
	}

	//ui->???

	return TCL_ERROR;
    }

    int SayCmd (int objc, struct Tcl_Obj * const objv[])
    {
	ItclObject *ItclObj;
	IRCUi *ui;

	// Get the Itcl object.
	//
	if (GetItclObj(&ItclObj, objv[0]) != TCL_OK) return TCL_ERROR;

	// Get the IRCUi instance associated to this Itcl object.
	//
	if (IRCUis.Find(ItclObj, &ui) != TCL_OK) {
	    Tcl_SetObjResult(interp,
		    Tcl_NewStringObj("IRCUi instance lost!", -1));
	    return TCL_ERROR;
	}

	//ui->???

	return TCL_ERROR;
    }

    int InputCmd (int objc, struct Tcl_Obj * const objv[])
    {
	ItclObject *ItclObj;
	IRCUi *ui;

	// Get the Itcl object.
	//
	if (GetItclObj(&ItclObj, objv[0]) != TCL_OK) return TCL_ERROR;

	// Get the IRCUi instance associated to this Itcl object.
	//
	if (IRCUis.Find(ItclObj, &ui) != TCL_OK) {
	    Tcl_SetObjResult(interp,
		    Tcl_NewStringObj("IRCUi instance lost!", -1));
	    return TCL_ERROR;
	}

	//ui->???

	return TCL_ERROR;
    }

    int TopicCmd (int objc, struct Tcl_Obj * const objv[])
    {
	ItclObject *ItclObj;
	IRCUi *ui;

	// Get the Itcl object.
	//
	if (GetItclObj(&ItclObj, objv[0]) != TCL_OK) return TCL_ERROR;

	// Get the IRCUi instance associated to this Itcl object.
	//
	if (IRCUis.Find(ItclObj, &ui) != TCL_OK) {
	    Tcl_SetObjResult(interp,
		    Tcl_NewStringObj("IRCUi instance lost!", -1));
	    return TCL_ERROR;
	}

	//ui->???

	return TCL_ERROR;
    }

    int MakeBlankCmd (int objc, struct Tcl_Obj * const objv[])
    {
	BlankBox* pBlank = new BlankBox();
	Tcl_SetObjResult(interp,
		Tcl_NewLongObj(reinterpret_cast <LONG_PTR>(pBlank->GetWindowId())));
	return TCL_OK;
    }

    int NotImplementedCmd (int objc, struct Tcl_Obj * const objv[])
    {
	Tcl_SetObjResult(interp, Tcl_NewStringObj(
		"This command has not been implemented yet.", -1));
	return TCL_ERROR;
    }
};


#undef TCL_STORAGE_CLASS
#define TCL_STORAGE_CLASS DLLEXPORT

EXTERN int
Gui_irc_Init (Tcl_Interp *interp)
{
#ifdef USE_TCL_STUBS
    if (Tcl_InitStubs(interp, "8.3", 0) == NULL) {
	return TCL_ERROR;
    }
#endif
#ifdef USE_ITCL_STUBS
    if (Itcl_InitStubs(interp, "3.1", 0) == NULL) {
	return TCL_ERROR;
    }
#endif
    new IRCWindowsItclAdapter(interp);
    return TCL_OK;
}
