/*
 ------------------------------------------------------------------------
 *  IRCEngineItclAdapter.cpp --
 *
 *  Entry-point and front-end for the IRC_Engine extension.  We register
 *  the Itcl methods for the IRC::engine class here.
 *
 ------------------------------------------------------------------------
 * RCS: @(#) $Id: IRCEngineItclAdapter.cpp,v 1.6 2006/09/26 19:20:40 davygrvy Exp $
 ------------------------------------------------------------------------
 */

#include "cpptcl.hpp"
#include "ItclAdapter.hpp"
#include "IRCEngine.hpp"
#include "IRCUtils.hpp"
#include "CTCP2ParserAction.hpp"
#include <codecvt>


// General/simplictic form for a URI
const char *uri_RE_1_str = "(?:[[:alpha:]]?)(?:\\w){2,7}:(?://?)(?:[^[:space:]>\"]*)";
// Ending paren fix (exception #1)
const char *uri_RE_2_str = "(\\(\\006L\\+\\006)(.*?)(\\)\\006L\\-\\006)";


class IRCEngineItclAdapter
    : private Itcl::IAdapter<IRCEngineItclAdapter>
{
    friend class Tcl::Adapter<IRCEngineItclAdapter>;
    Tcl::Hash<IRCEngine *, TCL_ONE_WORD_KEYS> engines;
    Tcl::Hash<CTCP2ParserInfo *, TCL_ONE_WORD_KEYS> infos;

    // CTCP/2 extension codes we use for color tagging.
    //
    // This could be static to all IRCEngineItclAdapter instances, but
    // then we would have to concern ourselves about thread safety.  Not
    // an issue I want to bother with.  So I'll avoid it by using a
    // per-interp hash for these even though they are the same for each.
    //
    Tcl::Hash<Tcl_Obj *> Xcodes;

    struct tagset {
	const char *name;
	const char *code;
    };
    static const tagset tags[20];

    Tcl_Obj *uri_RE[2];

public:
    IRCEngineItclAdapter(Tcl_Interp *interp)
	: Itcl::IAdapter<IRCEngineItclAdapter>(interp)
    {
	Tcl_Obj *code;
	int i;

    	// Setup our special color tags.
	for (i = 0; tags[i].name != 0L; i++) {
	    Tcl_IncrRefCount(code = Tcl_NewStringObj(tags[i].code,-1));
	    Xcodes.Add(tags[i].name, code);
	}

	// Let [Incr Tcl] know we have some methods in here.
	//
	NewItclCmd("irc-engine-construct",  &IRCEngineItclAdapter::ConstructCmd);
	NewItclCmd("irc-engine-destruct",   &IRCEngineItclAdapter::DestructCmd);
	NewItclCmd("irc-engine-connect",    &IRCEngineItclAdapter::ConnectCmd);
	NewItclCmd("irc-engine-disconnect", &IRCEngineItclAdapter::DisconnectCmd);
	NewItclCmd("irc-engine-connected",  &IRCEngineItclAdapter::ConnectedCmd);
	NewItclCmd("irc-engine-raw",	    &IRCEngineItclAdapter::RawCmd);
	NewItclCmd("irc-engine-server",	    &IRCEngineItclAdapter::ServerCmd);
	NewItclCmd("irc-engine-my_nick",    &IRCEngineItclAdapter::MyNickCmd);
	NewItclCmd("irc-engine-my_ip",	    &IRCEngineItclAdapter::NotImplementedCmd);
	NewItclCmd("irc-engine-my_host",    &IRCEngineItclAdapter::MyHostCmd);
	NewItclCmd("irc-engine-my_user",    &IRCEngineItclAdapter::MyUserCmd);
	NewItclCmd("irc-engine-my_port",    &IRCEngineItclAdapter::NotImplementedCmd);
	NewItclCmd("irc-engine-channels",   &IRCEngineItclAdapter::ChannelsCmd);
	NewItclCmd("irc-engine-nicks",	    &IRCEngineItclAdapter::NotImplementedCmd);
	NewItclCmd("irc-engine-scripts",    &IRCEngineItclAdapter::ScriptsCmd);
	NewItclCmd("irc-engine-mode",	    &IRCEngineItclAdapter::NotImplementedCmd);
	NewItclCmd("irc-engine-whois",	    &IRCEngineItclAdapter::WhoisCmd);
	NewItclCmd("irc-engine-lookup",	    &IRCEngineItclAdapter::NotImplementedCmd);
	NewItclCmd("irc-engine-color",	    &IRCEngineItclAdapter::ColorCmd);
	NewItclCmd("irc-engine-topic",	    &IRCEngineItclAdapter::TopicCmd);

	NewItclCmd("CTCP2-parse-construct", &IRCEngineItclAdapter::ParseConstructCmd);
	NewItclCmd("CTCP2-parse-destruct",  &IRCEngineItclAdapter::ParseDestructCmd);
	NewItclCmd("CTCP2-parse-parse",	    &IRCEngineItclAdapter::ParseCmd);

	// Just temporary!
	NewItclCmd("irc-engine-rehash",	    &IRCEngineItclAdapter::RehashCmd);

	Tcl_IncrRefCount(uri_RE[0] = Tcl_NewStringObj(uri_RE_1_str, -1));
	Tcl_GetRegExpFromObj(0L, uri_RE[0], TCL_REG_ADVANCED);
	Tcl_IncrRefCount(uri_RE[1] = Tcl_NewStringObj(uri_RE_2_str, -1));
	Tcl_GetRegExpFromObj(0L, uri_RE[1], TCL_REG_ADVANCED);

	Tcl_PkgProvide(interp, "IRC_Engine", "0.2");
    }

    ~IRCEngineItclAdapter()
    {
	// Use of the interp, in here, is not allowed.
	IRCEngine *engine;
	Tcl_Obj *code;
	int result;

	for (
	    result = engines.Top(&engine);
	    result != TCL_ERROR;
	    result = engines.Next(&engine)
	) {
	    delete engine;
	}

	for (
	    result = Xcodes.Top(&code);
	    result != TCL_ERROR;
	    result = Xcodes.Next(&code)
	) {
	    Tcl_DecrRefCount(code);
	}
	Tcl_DecrRefCount(uri_RE[0]);
	Tcl_DecrRefCount(uri_RE[1]);
    }

private:

    int ConstructCmd (int objc, struct Tcl_Obj * const objv[])
    {
	ItclObject *ItclObj;
	IRCEngine *engine;
	Tcl_Obj *scripts;
	const char *instanceName;

	if (GetItclObj(&ItclObj, objv[0]) != TCL_OK) return TCL_ERROR;
	instanceName = Tcl_GetCommandName(interp, ItclObj->accessCmd);

	// Get scripts, if any.
	//
	Tcl_IncrRefCount(scripts = Tcl_NewListObj(objc-1, objv+1));

	// Using the new Itcl object context pointer as our key, create a
	// new IRCEngine C++ object and store the pointer in the hash
	// table.
	//
	engine = new IRCEngine(interp, instanceName, scripts, uri_RE);

	Tcl_DecrRefCount(scripts);

	if (engine->Status() != TCL_OK) {
	    /* error state already set into the interp. */
	    delete engine;
	    return TCL_ERROR;
	}

	engines.Add(ItclObj, engine);
	return TCL_OK;
    }

    int DestructCmd (int objc, struct Tcl_Obj * const objv[])
    {
	ItclObject *ItclObj;
	IRCEngine *engine;

	if (objc != 1) {
	    Tcl_WrongNumArgs(interp, 1, objv, "");
	    return TCL_ERROR;
	}

	// Retrieve the Itcl object context.
	//
	if (GetItclObj(&ItclObj, objv[0]) != TCL_OK) return TCL_ERROR;

	// Remove the C++ IRCEngine instance associated to this Itcl
	// instance.
	//
	if (engines.Extract(ItclObj, &engine) == TCL_OK) {

	    // Itcl's destructor may be called even though the Itcl class
	    // constructor returned with an error!  Thus, no IRCEngine
	    // instance exists in the hash table even though an Itcl
	    // object context exists.  Only delete what we know is there.
	    //
	    delete engine;
	}
	return TCL_OK;
    }

    int ConnectCmd (int objc, struct Tcl_Obj * const objv[])
    {
	// TODO: we need to add the ability to bind to a local
	//       interface and port as well...

	int ciParts, j;
	Tcl_Obj *connectInfo, **ciList;
	ItclObject *ItclObj;
	IRCEngine *engine;
	Tcl_Encoding enc = 0L;
	bool done = false;
	const char *arg;
	static const char *options[] = {
	    "-encoding", "-encrypt", "-proxy", "-casemap", 0L
	};
	enum options {
	    CONNECT_ENCODING, CONNECT_ENCRYPT, CONNECT_PROXY, CONNECT_CMAP
	};
	int option;


	if (objc < 5) {
	    Tcl_WrongNumArgs (interp, 1, objv,
		    "[-encoding <enc>] "
		    "[-casemap ascii|rfc1459|auto] "
		    "[-encrypt <encryptMode> <encryptData>] "
		    "[-proxy <proxyType> <proxyData>] "
		    "<hostname|IP[:<port>[:<user>[:<pass>]]]> "
		    "<nickname> <username> <realname>");
	    return TCL_ERROR;
	}

	// Get the Itcl object.
	//
	if (GetItclObj(&ItclObj, objv[0]) != TCL_OK) return TCL_ERROR;

	// Get the IRCEngine instance associated to this Itcl object.
	//
	if (engines.Find(ItclObj, &engine) != TCL_OK) {
	    Tcl_SetObjResult(interp,
		    Tcl_NewStringObj("IRCEngine instance lost!", -1));
	    return TCL_ERROR;
	}

	/* Decode args */
	for (j = 1; (j < objc) && !done; j++) {
	    arg = Tcl_GetString(objv[j]);
	    if (*arg == '-') {
		if (Tcl_GetIndexFromObj(interp, objv[j], options, "option", 0,
			&option) != TCL_OK) {
		    return TCL_ERROR;
		}
		switch ((enum options) option) {
		case CONNECT_ENCODING:
		    if (objc > j+1) {
			if (engine->SetEncodingHacksFromListObj(objv[++j])
				== TCL_ERROR) {
			    return TCL_ERROR;
			}
			break;
		    } else {
			Tcl_SetObjResult(interp, Tcl_NewStringObj(
				"The -encoding option requires a parameter.", -1));
			return TCL_ERROR;
		    }
		case CONNECT_CMAP:
		case CONNECT_ENCRYPT:
		case CONNECT_PROXY:
		    ((void)0);
		}
	    } else {
		done = true;
		/* back it up one. */
		--j;
	    }
	}


	IRCSplitFromObj(objv[j++], &connectInfo, u":");
	Tcl_ListObjGetElements(0L, connectInfo, &ciParts, &ciList);

	Tcl_IncrRefCount(engine->ServerHost = ciList[0]);

	if (ciParts >= 2) {
	    Tcl_IncrRefCount(engine->ServerPort = ciList[1]);
	} else {
	    /* default port is 6667 */
	    Tcl_IncrRefCount(engine->ServerPort = Tcl_NewIntObj(6667));
	}

	if (ciParts >= 3) {
	    Tcl_IncrRefCount(engine->ConnectServerUser = ciList[2]);
	}
	if (ciParts >= 4) {
	    Tcl_IncrRefCount(engine->ConnectServerPass = ciList[3]);
	}
	Tcl_DecrRefCount(connectInfo);

	Tcl_IncrRefCount(engine->ConnectNickname = objv[j++]);
	Tcl_IncrRefCount(engine->ConnectUsername = objv[j++]);
	Tcl_IncrRefCount(engine->ConnectRealname = objv[j]);

	return engine->ConnectToIRC();
    }


    int DisconnectCmd (int objc, struct Tcl_Obj * const objv[])
    {
	ItclObject *ItclObj;
	IRCEngine *engine;

	// Get the Itcl object.
	//
	if (GetItclObj(&ItclObj, objv[0]) != TCL_OK) return TCL_ERROR;

	// Get the IRCEngine instance associated to this Itcl object.
	//
	if (engines.Find(ItclObj, &engine) != TCL_OK) {
	    Tcl_SetObjResult(interp,
		    Tcl_NewStringObj("IRCEngine instance lost!", -1));
	    return TCL_ERROR;
	}

	engine->DisconnectFromIRC();
	return TCL_OK;
    }


    int ConnectedCmd (int objc, struct Tcl_Obj * const objv[])
    {
	ItclObject *ItclObj;
	IRCEngine *engine;

	// Get the Itcl object.
	//
	if (GetItclObj(&ItclObj, objv[0]) != TCL_OK) return TCL_ERROR;

	// Get the IRCEngine instance associated to this Itcl object.
	//
	if (engines.Find(ItclObj, &engine) != TCL_OK) {
	    Tcl_SetObjResult(interp,
		    Tcl_NewStringObj("IRCEngine instance lost!", -1));
	    return TCL_ERROR;
	}

	Tcl_SetObjResult(interp, Tcl_NewBooleanObj(engine->connected));
	return TCL_OK;
    }


    int RawCmd (int objc, struct Tcl_Obj * const objv[])
    {
	ItclObject *ItclObj;
	IRCEngine *engine;

	if (objc < 2) {
	    Tcl_WrongNumArgs (interp, 1, objv, "<bytes>");
	    return TCL_ERROR;
	}

	// Get the Itcl object.
	//
	if (GetItclObj(&ItclObj, objv[0]) != TCL_OK) return TCL_ERROR;

	// Get the IRCEngine instance associated to this Itcl object.
	//
	if (engines.Find(ItclObj, &engine) != TCL_OK) {
	    Tcl_SetObjResult(interp,
		    Tcl_NewStringObj("IRCEngine instance lost!", -1));
	    return TCL_ERROR;
	}

	return engine->SendToIRC(objv[1]);
    }


    int ServerCmd (int objc, struct Tcl_Obj * const objv[])
    {
	ItclObject *ItclObj;
	IRCEngine *engine;

	// Get the Itcl object.
	//
	if (GetItclObj(&ItclObj, objv[0]) != TCL_OK) return TCL_ERROR;

	// Get the IRCEngine instance associated to this Itcl object.
	//
	if (engines.Find(ItclObj, &engine) != TCL_OK) {
	    Tcl_SetObjResult(interp,
		    Tcl_NewStringObj("IRCEngine instance lost!", -1));
	    return TCL_ERROR;
	}

	if (engine->ServerInfo) {
	    Tcl_SetObjResult(interp, engine->ServerInfo);
	}

	return TCL_OK;
    }


    int MyNickCmd (int objc, struct Tcl_Obj * const objv[])
    {
	ItclObject *ItclObj;
	IRCEngine *engine;

	// Get the Itcl object.
	//
	if (GetItclObj(&ItclObj, objv[0]) != TCL_OK) return TCL_ERROR;

	// Get the IRCEngine instance associated to this Itcl object.
	//
	if (engines.Find(ItclObj, &engine) != TCL_OK) {
	    Tcl_SetObjResult(interp,
		    Tcl_NewStringObj("IRCEngine instance lost!", -1));
	    return TCL_ERROR;
	}

	if (engine->MyNick) {
	    Tcl_SetObjResult(interp, engine->MyNick);
	}

	return TCL_OK;
    }


    int MyUserCmd (int objc, struct Tcl_Obj * const objv[])
    {
	ItclObject *ItclObj;
	IRCEngine *engine;

	// Get the Itcl object.
	//
	if (GetItclObj(&ItclObj, objv[0]) != TCL_OK) return TCL_ERROR;

	// Get the IRCEngine instance associated to this Itcl object.
	//
	if (engines.Find(ItclObj, &engine) != TCL_OK) {
	    Tcl_SetObjResult(interp,
		    Tcl_NewStringObj("IRCEngine instance lost!", -1));
	    return TCL_ERROR;
	}

	if (engine->MyUser) {
	    Tcl_SetObjResult(interp, engine->MyUser);
	}

	return TCL_OK;
    }


    int MyHostCmd (int objc, struct Tcl_Obj * const objv[])
    {
	ItclObject *ItclObj;
	IRCEngine *engine;

	// Get the Itcl object.
	//
	if (GetItclObj(&ItclObj, objv[0]) != TCL_OK) return TCL_ERROR;

	// Get the IRCEngine instance associated to this Itcl object.
	//
	if (engines.Find(ItclObj, &engine) != TCL_OK) {
	    Tcl_SetObjResult(interp,
		    Tcl_NewStringObj("IRCEngine instance lost!", -1));
	    return TCL_ERROR;
	}

	if (engine->MyHost) {
	    Tcl_SetObjResult(interp, engine->MyHost);
	}

	return TCL_OK;
    }


    int ChannelsCmd (int objc, struct Tcl_Obj * const objv[])
    {
	ItclObject *ItclObj;
	IRCEngine *engine;
	int result;
	const char *channel;

	// Get the Itcl object.
	//
	if (GetItclObj(&ItclObj, objv[0]) != TCL_OK) return TCL_ERROR;

	// Get the IRCEngine instance associated to this Itcl object.
	//
	if (engines.Find(ItclObj, &engine) != TCL_OK) {
	    Tcl_SetObjResult(interp,
		    Tcl_NewStringObj("IRCEngine instance lost!", -1));
	    return TCL_ERROR;
	}

	for (
	    result = engine->channels.Top(0L, (const void **)&channel);
	    result != TCL_ERROR;
	    result = engine->channels.Next(0L, (const void **)&channel)
	) {
	    Tcl_ListObjAppendElement(0L, Tcl_GetObjResult(interp),
		    Tcl_NewStringObj(channel, -1));
	}

	return TCL_OK;
    }


    int ScriptsCmd (int objc, struct Tcl_Obj * const objv[])
    {
	ItclObject *ItclObj;
	IRCEngine *engine;
	int result;
	const char *script;

	// Get the Itcl object.
	//
	if (GetItclObj(&ItclObj, objv[0]) != TCL_OK) return TCL_ERROR;

	// Get the IRCEngine instance associated to this Itcl object.
	//
	if (engines.Find(ItclObj, &engine) != TCL_OK) {
	    Tcl_SetObjResult(interp,
		    Tcl_NewStringObj("IRCEngine instance lost!", -1));
	    return TCL_ERROR;
	}

	for (
	    result = engine->scriptEngines.Top(0L, (const void **)&script);
	    result != TCL_ERROR;
	    result = engine->scriptEngines.Next(0L, (const void **)&script)
	) {
	    Tcl_ListObjAppendElement(0L, Tcl_GetObjResult(interp),
		    Tcl_NewStringObj(script, -1));
	}

	return TCL_OK;
    }


    int ColorCmd (int objc, struct Tcl_Obj * const objv[])
    {
	Tcl_Obj *code;

	if (Xcodes.Find(Tcl_GetString(objv[1]), &code) == TCL_ERROR) {
	    Tcl_SetObjResult(interp,
		    Tcl_NewStringObj("color tag not found.", -1));
	    return TCL_ERROR;
	}

	Tcl_SetObjResult(interp, code);
	return TCL_OK;
    }

    
    int TopicCmd (int objc, struct Tcl_Obj * const objv[])
    {
	ItclObject *ItclObj;
	IRCEngine *engine;
	IRCChanInfo *chanInfo;
	Tcl_Obj *topic;

	// Get the Itcl object.
	//
	if (GetItclObj(&ItclObj, objv[0]) != TCL_OK) return TCL_ERROR;

	// Get the IRCEngine instance associated to this Itcl object.
	//
	if (engines.Find(ItclObj, &engine) != TCL_OK) {
	    Tcl_SetObjResult(interp,
		    Tcl_NewStringObj("IRCEngine instance lost!", -1));
	    return TCL_ERROR;
	}

	if (engine->channels.Find(IRCToUtfLowerFromObj(objv[1], 1), &chanInfo) == TCL_OK) {
	    if (topic = chanInfo->GetTopic()) Tcl_SetObjResult(interp, topic);
	}

	return TCL_OK;
    }


    // This is temporary!
    int RehashCmd (int objc, struct Tcl_Obj * const objv[])
    {
	ItclObject *ItclObj;
	IRCEngine *engine;

	// Get the Itcl object.
	//
	if (GetItclObj(&ItclObj, objv[0]) != TCL_OK) return TCL_ERROR;

	// Get the IRCEngine instance associated to this Itcl object.
	//
	if (engines.Find(ItclObj, &engine) != TCL_OK) {
	    Tcl_SetObjResult(interp,
		    Tcl_NewStringObj("IRCEngine instance lost!", -1));
	    return TCL_ERROR;
	}

	return engine->Rehash();
    }


    int WhoisCmd (int objc, struct Tcl_Obj * const objv[])
    {
	ItclObject *ItclObj;
	IRCEngine *engine;
	Tcl_Obj *line;
	int i, length, result;
	const char *bytes;

	// Get the Itcl object.
	//
	if (GetItclObj(&ItclObj, objv[0]) != TCL_OK) return TCL_ERROR;

	// Get the IRCEngine instance associated to this Itcl object.
	//
	if (engines.Find(ItclObj, &engine) != TCL_OK) {
	    Tcl_SetObjResult(interp,
		    Tcl_NewStringObj("IRCEngine instance lost!", -1));
	    return TCL_ERROR;
	}

	if (objc < 1) {
	    Tcl_WrongNumArgs(interp, 1, objv, "nick ?nick ...?");
	    return TCL_ERROR;
	}

	line = Tcl_NewStringObj("WHOIS ", -1);

	for (i = 1;  i < objc;  i++) {
	    bytes = Tcl_GetStringFromObj(objv[i], &length);
	    if (i > 1) {
		Tcl_AppendToObj(line, ",", 1);
	    }
	    Tcl_AppendToObj(line, bytes, length);
	}

	result = engine->SendToIRC(line);
	Tcl_DecrRefCount(line);
	return result;
    }


    int NotImplementedCmd (int objc, struct Tcl_Obj * const objv[])
    {
	Tcl_SetObjResult(interp, Tcl_NewStringObj(
		"This command has not been implemented yet.", -1));
	return TCL_ERROR;
    }


    int ParseConstructCmd (int objc, struct Tcl_Obj * const objv[])
    {
	ItclObject *ItclObj;
	CTCP2ParserInfo *info;
	CTCP2Parse::ParseMode mode = CTCP2Parse::Gateway;
	CTCP2Parse::CharSpacingType cspc = CTCP2Parse::Fixed;
	int fg = CTCP2Parse::Lt_Grey;	// default 24-bit foreground color
	int bg = CTCP2Parse::Black;	// default 24-bit background color
	const char *modeStr;

	if (objc < 2) {
	    Tcl_WrongNumArgs(interp, 1, objv, "callback mode ?options?");
	    return TCL_ERROR;
	}

	// Retrieve the Itcl object context.
	//
	if (GetItclObj(&ItclObj, objv[0]) != TCL_OK) return TCL_ERROR;

	modeStr = Tcl_GetString(objv[2]);
	if (!strcmp(modeStr, "render")) {
	    mode = CTCP2Parse::Render;
	} else if (!strcmp(modeStr, "gateway")) {
	    mode = CTCP2Parse::Gateway;
	} else {
	    Tcl_SetObjResult(interp, Tcl_NewStringObj(
		    "mode must be \"render\" or \"gateway\"", -1));
	    return TCL_ERROR;
	}

	// Using the new Itcl object context pointer as our key, create a
	// new ParserInfo C++ object and store the pointer in the hash
	// table.
	//
	info = new CTCP2ParserInfo(interp, objv[1], mode, cspc, fg, bg);
	infos.Add(ItclObj, info);
	return TCL_OK;
    }


    int ParseDestructCmd (int objc, struct Tcl_Obj * const objv[])
    {
	ItclObject *ItclObj;
	CTCP2ParserInfo *info;

	if (objc != 1) {
	    Tcl_WrongNumArgs(interp, 1, objv, "");
	    return TCL_ERROR;
	}

	// Retrieve the Itcl object context.
	//
	if (GetItclObj(&ItclObj, objv[0]) != TCL_OK) return TCL_ERROR;

	// Remove the C++ ParserInfo instance associated to this Itcl
	// instance.
	//
	if (infos.Extract(ItclObj, &info) == TCL_OK) {

	    // Itcl's destructor may be called even though the Itcl class
	    // constructor returned with an error!  Thus, no ParserInfo
	    // instance exists in the hash table even though an Itcl
	    // object context exists.  Only delete what we know is there.
	    //
	    delete info;
	}
	return TCL_OK;
    }


    int ParseCmd (int objc, struct Tcl_Obj * const objv[])
    {
	ItclObject *ItclObj;
	CTCP2ParserInfo *info;
	int size;
	const Tcl_UniChar *str;

	if (objc != 2) {
	    Tcl_WrongNumArgs(interp, 1, objv, "text");
	    return TCL_ERROR;
	}

	// Get the Itcl object.
	//
	if (GetItclObj(&ItclObj, objv[0]) != TCL_OK) return TCL_ERROR;

	// Get the ParserInfo instance associated to this Itcl object.
	//
	if (infos.Find(ItclObj, &info) != TCL_OK) {
	    Tcl_SetObjResult(interp,
		    Tcl_NewStringObj("ParserInfo instance lost!", -1));
	    return TCL_ERROR;
	}

	str = Tcl_GetUnicodeFromObj(objv[1], &size);
	std::u16string utf16(reinterpret_cast<const char16_t*>(str), size);

	// just to be safe in case Tcl decides to become UTF-32
	_ASSERT(sizeof(char16_t) == sizeof(Tcl_UniChar));

	TclCallbackAction action(info, &utf16);

	return action.Result();
    }
};

// Special CTCP/2 attribute extensions for "tagging" text segments
// that the display will resolve the meaning of.  This avoids the
// CTCP/2 attribute parser having to push-out literal color codes
// and allows the display to define how these tags are to be rendered
// (or rerendered).
//
const struct IRCEngineItclAdapter::tagset
IRCEngineItclAdapter::tags[20] = {
	{"default",	"\6XT0\6"},
	{"public",	"\6XT1\6"},
	{"private",	"\6XT2\6"},
	{"action",	"\6XT3\6"},
	{"notice",	"\6XT4\6"},
	{"ctcp",	"\6XT5\6"},
	{"change",	"\6XT6\6"},
	{"join",	"\6XT7\6"},
	{"part",	"\6XT8\6"},
	{"kick",	"\6XT9\6"},
	{"quit",	"\6XT10\6"},
	{"highlight",	"\6XT11\6"},
	{"error",	"\6XT12\6"},
	{"nick",	"\6XT13\6"},
	{"channel",	"\6XT14\6"},
	{"mode",	"\6XT15\6"},
	{"socket",	"\6XT16\6"},
	{0L, 0L}
};


/*
 *----------------------------------------------------------------------
 *
 * Irc_engine_Init --
 *	the callback for initializing the extension from Tcl
 *
 * Results:
 *	returns TCL_OK.
 *
 * Side effects:
 *	adds more commands to the interp.
 *
 *----------------------------------------------------------------------
 */
#undef TCL_STORAGE_CLASS
#define TCL_STORAGE_CLASS DLLEXPORT

EXTERN int
Irc_engine_Init (Tcl_Interp *interp)
{
    int result;

#ifdef USE_TCL_STUBS
    if (Tcl_InitStubs(interp, "8.4", 0) == NULL) {
	return TCL_ERROR;
    }
#endif
#ifdef USE_ITCL_STUBS
    if (Itcl_InitStubs(interp, "4.2-", 0) == NULL) {
	return TCL_ERROR;
    }
#endif
    result = Tcl::Socket::Init(interp);
    if (result != TCL_ERROR) {
	new IRCEngineItclAdapter(interp);
    }
    return result;
}

EXTERN int
Irc_engine_SafeInit (Tcl_Interp *interp)
{
    return Irc_engine_Init(interp);
}
