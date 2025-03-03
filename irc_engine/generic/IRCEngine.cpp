/*-----------------------------------------------------------------------
 *  IRCEngine.cpp --
 *
 *  The main class for handling an IRC connection and its scripts.
 *
 *-----------------------------------------------------------------------
 * RCS: @(#) $Id: IRCEngine.cpp,v 1.66 2006/10/11 05:43:37 davygrvy Exp $
 *-----------------------------------------------------------------------
 */

#include "IRCEngine.hpp"
#include "IRCUtils.hpp"
#include <string>


#define ClearObj(obj) if (obj != 0L) {Tcl_DecrRefCount(obj); obj = 0L;}


IRCEngine::IRCEngine(
    Tcl_Interp *_interp,
    const char *instanceName,
    Tcl_Obj *scripts,
    Tcl_Obj *_uri_RE[2])
    :
    interp(_interp),
    clientDefault(&Nub),
    uri_RE(_uri_RE),
    connected(0),
    ServerHost(0L),
    ServerPort(0L),
    ServerInfo(0L),
    ConnectServerUser(0L),
    ConnectServerPass(0L),
    MyNick(0L),
    MyUser(0L),
    Sock(0L),
    waitingForMyUserHost(false),
    scriptFileNames(0L)
{
    instanceCmd = Tcl_NewStringObj(instanceName,-1);
    scriptFileNames = Tcl_DuplicateObj(scripts);

    encodings[0] = Tcl_GetEncoding(interp, "iso8859-1");
    encodings[1] = Tcl_GetEncoding(interp, "iso8859-2");
    encodings[2] = Tcl_GetEncoding(interp, "iso8859-3");
    encodings[3] = Tcl_GetEncoding(interp, "iso8859-4");
    encodings[4] = Tcl_GetEncoding(interp, "iso8859-5");
    encodings[5] = Tcl_GetEncoding(interp, "iso8859-6");
    encodings[6] = Tcl_GetEncoding(interp, "iso8859-7");
    encodings[7] = Tcl_GetEncoding(interp, "iso8859-8");
    encodings[8] = Tcl_GetEncoding(interp, "iso8859-9");
    encodings[9] = Tcl_GetEncoding(interp, "utf-8");
    encodings[10] = Tcl_GetEncoding(interp, "big5");
    encodings[11] = 0L;  // Primary.
    encodings[12] = 0L;  // Secondary.
    encodings[13] = 0L;  // Tertiary.

    status = LoadScripts();
}

IRCEngine::~IRCEngine()
{
    IRCUserScriptProvider *scriptEngine;
    int result, i;

    if (Sock != 0L) delete Sock;
    ClearAllConnectionData();

    for (
	result = scriptEngines.Top(&scriptEngine);
	result != TCL_ERROR;
	result = scriptEngines.Next(&scriptEngine)
    ) {
	delete scriptEngine;
    }

    ClearObj(scriptFileNames);
    ClearObj(instanceCmd);
    if (clientDefault != &Nub) delete clientDefault;

    for (i = 0; i < 14; i++) {
	if (encodings[i] != 0L) Tcl_FreeEncoding(encodings[i]);
    }

//    for (
//	result = colors.Top(&color);
//	result != TCL_ERROR;
//	result = colors.Next(&color)
//    ) {
//	Tcl_DecrRefCount(color);
//    }
}

int IRCEngine::LoadScripts()
{
    int numScripts, i;
    Tcl_Obj **scriptArray;

    if (LoadDefaultBehavior() == TCL_ERROR) {
	return TCL_ERROR;
    }

    Tcl_ListObjGetElements(0L, scriptFileNames, &numScripts, &scriptArray);

    for (i = 0; i < numScripts; i++) {
	if (LoadScript(scriptArray[i]) == TCL_ERROR) {
	    return TCL_ERROR;
	}
    }
    return TCL_OK;
}

int IRCEngine::LoadScript(Tcl_Obj *scriptFileName)
{
    const char *script = Tcl_GetString(scriptFileName);
    IRCUserScriptProvider *provider;

    if (Tcl_StringCaseMatch(script, "*.tcl", 1)) {
	provider = new IRCUserScriptProviderForTcl(
		script, Tcl_GetString(instanceCmd), interp);
    }
#if IRCE_USEJAVA
    else if (Tcl_StringCaseMatch(script, "*.java", 1)) {
	provider = new IRCUserScriptProviderForJava(
		script, Tcl_GetString(instanceCmd), interp);
    }
#endif
#if IRCE_USEPYTHON
    else if (Tcl_StringCaseMatch(script, "*.py", 1)) {
	provider = new IRCUserScriptProviderForPython(
		script, Tcl_GetString(instanceCmd), interp);
    }
#endif
#if IRCE_USEPERL
    else if (Tcl_StringCaseMatch(script, "*.pl", 1)) {
	provider = new IRCUserScriptProviderForPerl(
		script, Tcl_GetString(instanceCmd), interp);
    }
#endif
    else {
	Tcl_AppendStringsToObj(Tcl_GetObjResult(interp),
		"\"", script,
		"\" is not recognized and can not be loaded.\n",
		0L);
	return TCL_ERROR;
    }

    if (provider->Status() == TCL_OK) {
	scriptEngines.Add(script, provider);
    } else {
	Tcl_AddErrorInfo(interp, provider->GetTraceBack());
	delete provider;
	return TCL_ERROR;
    }
    return TCL_OK;
}

int
IRCEngine::ConnectToIRC()
{
    if (connected) {
	DisconnectFromIRC();
    }
    Sock = new Tcl::Socket(this);
    return Sock->Connect(Tcl_GetString(ServerHost), Tcl_GetString(ServerPort));
}

int
IRCEngine::DisconnectFromIRC()
{
    if (connected) {
	Tcl_Obj *bytes = Tcl_NewStringObj("QUIT :client closing hard.", -1);
	SendToIRC(bytes);
	Tcl_DecrRefCount(bytes);
    }

    /* Don't wait, just dump */
    if (Sock != 0L) delete Sock;
    Sock = 0L;

    return TCL_OK;
}

int
IRCEngine::SetEncodingHacksFromListObj(Tcl_Obj *encList)
{
    int encCount;
    Tcl_Obj *encInfo, **encListVec;
    Tcl_Encoding enc;

    // Split into a list with & as the delimiter.
    //
    IRCSplitFromObj(encList, &encInfo, u"&");
    Tcl_ListObjGetElements(0L, encInfo, &encCount, &encListVec);

    if (encodings[11] != 0L) {Tcl_FreeEncoding(encodings[11]); encodings[11] = 0L;}
    if (encodings[12] != 0L) {Tcl_FreeEncoding(encodings[12]); encodings[11] = 0L;}
    if (encodings[13] != 0L) {Tcl_FreeEncoding(encodings[13]); encodings[11] = 0L;}

    if (encCount >= 1) {
	enc = Tcl_GetEncoding(interp, Tcl_GetString(encListVec[0]));
	if (enc == 0L) {
	    Tcl_SetObjResult(interp, Tcl_NewStringObj(
		    "The encoding requested could not be loaded.",
		    -1));
	    return TCL_ERROR;
	}
	encodings[11] = enc;
    }

    if (encCount >= 2) {
	enc = Tcl_GetEncoding(interp, Tcl_GetString(encListVec[1]));
	if (enc == 0L) {
	    Tcl_SetObjResult(interp, Tcl_NewStringObj(
		    "The encoding requested could not be loaded.",
		    -1));
	    return TCL_ERROR;
	}
	encodings[12] = enc;
    }

    if (encCount == 3) {
	enc = Tcl_GetEncoding(interp, Tcl_GetString(encListVec[2]));
	if (enc == 0L) {
	    Tcl_SetObjResult(interp, Tcl_NewStringObj(
		    "The encoding requested could not be loaded.",
		    -1));
	    return TCL_ERROR;
	}
	encodings[13] = enc;
    }
    return TCL_OK;
}

int
IRCEngine::SendToIRC(Tcl_Obj *bytes)
{
    int result, length;
    Tcl_Obj *dupe, *resultobj;

    if (Sock == 0L) {
	resultobj = Tcl_GetObjResult(interp);
	Tcl_AppendObjToObj(resultobj, Tcl_NewStringObj("No active connection.", -1));
	return TCL_ERROR;
    }

    // First make a dupe because we'll need to write to it.
    dupe = Tcl_DuplicateObj(bytes);

    // Convert any characters in the unicode range to utf-8 and mark them
    // as being utf-8 encoded using CTCP/2's ^FEU.
    CTCP2UniQuote(&dupe, encodings[9]);

    Tcl_GetStringFromObj(dupe, &length);
    if (length > 510) {
	resultobj = Tcl_GetObjResult(interp);
	Tcl_AppendObjToObj(resultobj, Tcl_NewStringObj("Byte length too long at ", -1));
	Tcl_AppendObjToObj(resultobj, Tcl_NewIntObj(length));
	Tcl_AppendObjToObj(resultobj, Tcl_NewStringObj(".  Should not be greater than 510.", -1));
	Tcl_DecrRefCount(dupe);
	return TCL_ERROR;
    }
    Tcl_AppendToObj(dupe, "\n", 1);

    // All done twiddling, go send it out over the wire.
    result = Sock->Send(dupe);

    Tcl_DecrRefCount(dupe);
    return result;
}

// Tcl::Socket entries
int
IRCEngine::socketMsgCallback (socketMsgIds Id, Tcl_Obj *msg)
{
    Tcl_Obj *objv[6];
    Tcl_Obj **old;
    Tcl_Obj *outmsg;
    int dummy;

    // Send it to the scripts
    //
    QCallback(
	new IRCParse(
	    Tcl_NewStringObj("", -1),
	    Tcl_NewStringObj("", -1),
	    Tcl_NewStringObj("", -1),
	    Tcl_NewStringObj("", -1),
	    Tcl_NewStringObj("@socket", -1),
	    msg,
	    Tcl_NewStringObj("", -1),
	    Tcl_NewStringObj("", -1)
	)
    );

    // messages come in here about socket state changes, etc.
    //
    switch(Id) {
	case SocketNotifyBase::MSG_CONNECTED:
	    // First set our server info list.
	    //
	    objv[0] = Tcl_NewStringObj("", -1);
	    Tcl_ListObjGetElements(0L, msg, &dummy, &old);
	    while (--dummy) objv[dummy] = old[dummy];
	    Tcl_IncrRefCount(ServerInfo = Tcl_NewListObj(4, objv));

	    if (ConnectServerPass) {
		objv[0] = Tcl_NewStringObj("pass", -1);
		objv[1] = ConnectServerPass;
		outmsg = Tcl_ConcatObj(2, (Tcl_Obj **)&objv);
		SendToIRC(outmsg);
		Tcl_DecrRefCount(outmsg);
		Tcl_DecrRefCount(objv[0]);
		ClearObj(ConnectServerPass);
	    }

	    objv[0] = Tcl_NewStringObj("nick", -1);
	    objv[1] = ConnectNickname;
	    outmsg = Tcl_ConcatObj(2, (Tcl_Obj **)&objv);
	    SendToIRC(outmsg);
	    Tcl_DecrRefCount(outmsg);
	    Tcl_DecrRefCount(objv[0]);
	    ClearObj(ConnectNickname);

	    objv[0] = Tcl_NewStringObj("user", -1);
	    objv[1] = ConnectUsername;
	    objv[2] = Tcl_NewStringObj("x", -1);
	    objv[3] = Tcl_NewStringObj("x", -1);
	    objv[4] = Tcl_NewStringObj(":", -1);
	    Tcl_AppendObjToObj(objv[4], ConnectRealname);
	    outmsg = Tcl_ConcatObj(5, (Tcl_Obj **)&objv);
	    SendToIRC(outmsg);
	    Tcl_DecrRefCount(outmsg);
	    Tcl_DecrRefCount(objv[0]);
	    ClearObj(ConnectUsername);
	    Tcl_DecrRefCount(objv[2]);
	    Tcl_DecrRefCount(objv[3]);
	    Tcl_DecrRefCount(objv[4]);
	    ClearObj(ConnectRealname);
	    break;

	case SocketNotifyBase::MSG_DISCONNECTED:
	    // Clear all info.
	    //
	    ClearAllConnectionData();

	    // We are down!
	    //
	    connected = 0;

	    // Send it to the scripts.
	    //
	    QCallback(
		new IRCParse(
		    Tcl_NewStringObj("", -1),
		    Tcl_NewStringObj("", -1),
		    Tcl_NewStringObj("", -1),
		    Tcl_NewStringObj("", -1),
		    Tcl_NewStringObj("disconnect", -1),
		    Tcl_NewStringObj("", -1),
		    Tcl_NewStringObj("", -1),
		    Tcl_NewStringObj("", -1)
		)
	    );
	    break;
    }

    return TCL_OK;
}

// IRC lines are parsed and Queued from here.
int
IRCEngine::socketReadCallback (Tcl_Obj *bytes)
{
    IRCParse line(bytes);
    IRCSplitAndQ(line, this, encodings, uri_RE);
    return 0;
}

// IRC events are created here from IRCSplitAndQ().
void
IRCEngine::QCallback (const IRCParse *line)
{
    IRCEVENT *event;

    event = reinterpret_cast <IRCEVENT *>(ckalloc(sizeof(IRCEVENT)));
    event->header.proc = EvalCallback;
    event->engine = this;
    event->interp = interp;
    event->line = line;

    Tcl_QueueEvent(reinterpret_cast <Tcl_Event *>(event), TCL_QUEUE_TAIL);
}

// The Tcl_EventProc
int
EvalCallback(Tcl_Event *evPtr, int flags)
{
    IRCEngine::IRCEVENT *event = (IRCEngine::IRCEVENT *) evPtr;

    // we only handle file-type events here.
    if (!(flags & TCL_FILE_EVENTS)) return 0;

    // check to make sure the interpreter is still
    // valid.  There is a remote chance that while waiting
    // in the event loop, our interpreter (and therefore, 
    // the engine pointer too) have been deleted.
    if (!Tcl_InterpDeleted(event->interp)) {
	Tcl_Preserve(event->interp);
	(event->engine)->EvalOneIRCEvent(event->line);
	Tcl_Release(event->interp);
    }
    delete event->line;

    // When proc returns 1, Tcl_ServiceEvent will remove the event
    // from the event queue and free its storage. Note that the
    // storage for an event must be allocated by the event source
    // (using Tcl_Alloc or the Tcl macro ckalloc) before calling
    // Tcl_QueueEvent, but it will be freed by Tcl_ServiceEvent,
    // not by the event source.
    return 1;
}


/* EvalOneIRCEvent
 *
 *  called from : EvalCallback
 */
void
IRCEngine::EvalOneIRCEvent(const IRCParse *line)
{
    int result;
    IRCUserScriptProvider *engine;
    IRCUserScriptProvider::CompletionStatus cstatus;

    // Do pre-processing, such as creating a channel object if joining.
    // Some events might be for internal use only, and might be completed
    // here.
    //
    cstatus = PreEvent(line);
    if (cstatus == IRCUserScriptProvider::COMPLETED) return;

    // Run this event down the list of scripts.
    // TODO: we want LIFO.. is this LIFO?
    //
    for (
	result = scriptEngines.Top(&engine);
	result != TCL_ERROR;
	result = scriptEngines.Next(&engine)
    ) {
	cstatus = engine->IncomingEvent(line);
	if (cstatus != IRCUserScriptProvider::NOTCOMPLETED) break;
    }

    // Do default behavior if not completed.
    //
    if (cstatus == IRCUserScriptProvider::NOTCOMPLETED) {
	cstatus = clientDefault->IncomingEvent(line);
    }

    // If the default script has no handler for this event and the event
    // is an original (not an extracted one), print it to the status
    // window and call it done.
    //
    if (cstatus == IRCUserScriptProvider::NOTCOMPLETED
		&& Tcl_GetCharLength(line->GetRawLine()))
    {
	Tcl_Obj *temp[3], *temp2, *temp3[2], **temp4;
	int count;

	Tcl_IncrRefCount(temp[0] = Tcl_NewStringObj("echo", -1));

	// The list is already shared, so don't bother testing.
	//
	temp2 = Tcl_DuplicateObj(line->GetArgs());

	// Unhandled ones are mark red, for now.
	//
	temp3[0] = Tcl_NewStringObj("\6CC\6***\6C\6", -1);
	temp3[1] = line->GetEvent();
	Tcl_ListObjReplace(0L, temp2, 0, 1, 2, (Tcl_Obj **)&temp3);
	Tcl_ListObjGetElements(0L, temp2, &count, &temp4);
	Tcl_IncrRefCount(temp[1] = Tcl_ConcatObj(count, temp4));
	Tcl_DecrRefCount(temp2);
	Tcl_IncrRefCount(temp[2] = Tcl_NewStringObj("status", -1));
	Tcl::FastInvoke(interp, Tcl_GetString(instanceCmd), temp[0], temp[1], temp[2], 0L);
	Tcl_DecrRefCount(temp[0]);
	Tcl_DecrRefCount(temp[1]);
	Tcl_DecrRefCount(temp[2]);
    }

    // Do post-processing, such as destroying a channel object if parting.
    //
    PostEvent(line);
}

#include "numeric.h"

IRCUserScriptProvider::CompletionStatus
IRCEngine::PreEvent(const IRCParse *line)
{
    int numeric;
    Tcl_Obj *arg0, *arg1, *arg2, *arg3, *temp, *msg;
    IRCChanInfo *chanInfo;


    // is this a numeric event?
    if (Tcl_GetIntFromObj(0L, line->GetEvent(), &numeric) == TCL_OK)
    {
	switch (numeric)
	{
	case RPL_WELCOME:
	    // We are up!
	    //
	    connected = 1;

	    // Set our registered, verified nickname.
	    //
	    ClearObj(MyNick);
	    Tcl_ListObjIndex(0L, line->GetArgs(), 0, &arg0);
	    Tcl_IncrRefCount(MyNick = arg0);

	    // Set our server name (as used in IRC messages).
	    //
	    temp = line->GetHost();
	    Tcl_ListObjReplace(0L, ServerInfo, 0, 1, 1, &temp);

	    // Request our userhost info.
	    //
	    Tcl_IncrRefCount(msg = Tcl_NewStringObj("userhost ", -1));
	    Tcl_AppendObjToObj(msg, arg0);
	    SendToIRC(msg);
	    Tcl_DecrRefCount(msg);
	    waitingForMyUserHost = true;

	    // Queue the "connect" event to the scripts.
	    //
	    QCallback(
		new IRCParse(
		    Tcl_NewStringObj("", -1),
		    Tcl_NewStringObj("", -1),
		    Tcl_NewStringObj("", -1),
		    Tcl_NewStringObj("", -1),
		    Tcl_NewStringObj("connect", -1),
		    Tcl_NewStringObj("", -1),
		    Tcl_NewStringObj("", -1),
		    Tcl_NewStringObj("", -1)
		)
	    );
	    break;

	case RPL_USERHOST:
	    if (waitingForMyUserHost) {
		Tcl_Obj *splitObj, *nickUserHosted;

		Tcl_ListObjIndex(0L, line->GetArgs(), 1, &arg1);
		IRCSplitFromObj(arg1, &splitObj, u" =@");
		Tcl_ListObjIndex(0L, splitObj, 0, &nickUserHosted);

		// Is this us?
		if (IRCIsEqual(MyNick, nickUserHosted)) {
		    // Got`cha.
		    waitingForMyUserHost = false;
		    Tcl_ListObjIndex(0L, splitObj, 1, &MyUser);
		    Tcl_IncrRefCount(MyUser);
		    Tcl_ListObjIndex(0L, splitObj, 2, &MyHost);
		    Tcl_IncrRefCount(MyHost);
		    Tcl_DecrRefCount(splitObj);
		    return IRCUserScriptProvider::COMPLETED;
		}
		Tcl_DecrRefCount(splitObj);
	    }
	    break;

	case RPL_TOPIC:
	    // Save the topic of the channel you have just joined.
	    //
	    Tcl_ListObjIndex(0L, line->GetArgs(), 1, &arg1);
	    Tcl_ListObjIndex(0L, line->GetArgs(), 2, &arg2);
	    if (channels.Find(IRCToUtfLowerFromObj(arg1, 1), &chanInfo) == TCL_OK) {
		chanInfo->SetTopic(arg2);
	    }
	    break;

	case RPL_TOPICWHOTIME:
	    // Save the topic info of the channel.
	    //
	    Tcl_ListObjIndex(0L, line->GetArgs(), 1, &arg1);
	    Tcl_ListObjIndex(0L, line->GetArgs(), 2, &arg2);
	    Tcl_ListObjIndex(0L, line->GetArgs(), 3, &arg3);
	    if (channels.Find(IRCToUtfLowerFromObj(arg1, 1), &chanInfo) == TCL_OK) {
		chanInfo->SetTopicBy(arg2, arg3);
	    }
	    break;

	case RPL_CREATIONTIME:
	    // Save the creation time of the channel.
	    //
	    Tcl_ListObjIndex(0L, line->GetArgs(), 1, &arg1);
	    Tcl_ListObjIndex(0L, line->GetArgs(), 2, &arg2);
	    if (channels.Find(IRCToUtfLowerFromObj(arg1, 1), &chanInfo) == TCL_OK) {
		chanInfo->SetCreationTime(arg2);
	    }
	    break;
	}

    } else {

	// check if the destination is from/for us.
	//
	if (!IRCIsEqual(line->GetNick(), MyNick))
	{
	    // Bail, not from/for us.
	    //
	    return IRCUserScriptProvider::NOTCOMPLETED;
	}

	if (IRCIsEqual(line->GetEvent(), u"join"))
	{
	    // We just joined a channel.
	    IRCChanInfo *chanInfo;

	    chanInfo = new IRCChanInfo;
	    Tcl_ListObjIndex(0L, line->GetArgs(), 0, &arg0);
	    channels.Add(IRCToUtfLowerFromObj(arg0, 1), chanInfo);
	}
    }
    return IRCUserScriptProvider::NOTCOMPLETED;
}

void
IRCEngine::PostEvent(const IRCParse *line)
{
//    int numeric;
    Tcl_Obj *arg0, *arg1;
    int len;

    // is this a numeric event?
//    if (Tcl_GetIntFromObj(0L, line->GetEvent(), &numeric) == TCL_OK)
//    {
//    } else {
	// check if the destination is from/for us.
	//
	if (!IRCIsEqual(line->GetNick(), MyNick))
	{
	    // Bail, not from/for us.
	    //
	    return;
	}

	if (IRCIsEqual(line->GetEvent(), u"nick"))
	{
	    if (Tcl_ListObjLength(0L, line->GetArgs(), &len) == TCL_ERROR
		    || !len) return;

	    // Our nick has changed.
	    //
	    ClearObj(MyNick);
	    Tcl_ListObjIndex(0L, line->GetArgs(), 0, &arg0);
	    Tcl_IncrRefCount(MyNick = arg0);
	}
	else if (IRCIsEqual(line->GetEvent(), u"part"))
	{
	    IRCChanInfo *chanInfo;

	    if (Tcl_ListObjLength(0L, line->GetArgs(), &len) == TCL_ERROR
		    || !len) return;

	    // We just parted a channel, delete the IRCChanInfo object
	    // for it.
	    //
	    Tcl_ListObjIndex(0L, line->GetArgs(), 0, &arg0);
	    if (channels.Extract(IRCToUtfLowerFromObj(arg0, 1), &chanInfo) == TCL_OK) {
		delete chanInfo;
	    }
	}
	else if (IRCIsEqual(line->GetEvent(), u"topic"))
	{
	    Tcl_Time now;
	    IRCChanInfo *chanInfo;

	    if (Tcl_ListObjLength(0L, line->GetArgs(), &len) == TCL_ERROR
		    || len >= 2) return;

	    // Save the topic of the channel you have just joined.
	    //
	    Tcl_ListObjIndex(0L, line->GetArgs(), 0, &arg0);
	    Tcl_ListObjIndex(0L, line->GetArgs(), 1, &arg1);
	    if (channels.Find(IRCToUtfLowerFromObj(arg0, 1), &chanInfo) == TCL_OK) {
		chanInfo->SetTopic(arg1);
		Tcl_GetTime(&now);
		chanInfo->SetTopicBy(line->GetNick(), Tcl_NewIntObj(now.sec));
	    }
	}


//    }
}

int IRCEngine::LoadDefaultBehavior ()
{
    Tcl_DString ds;
    const char *argv[2];

    Tcl_DStringInit(&ds);

    // Get the IRC_Engine library path set in the [Incr Tcl] class when
    // the package was first sourced.
    //
    argv[0] = Tcl_GetVar(interp, "irc_engine_library",
	    TCL_NAMESPACE_ONLY);
    argv[1] = "default.tcl";
    Tcl_JoinPath(2, argv, &ds);

    // Create it.
    clientDefault =
	    new IRCUserScriptProviderForTcl(Tcl_DStringValue(&ds),
		    Tcl_GetString(instanceCmd), interp);

    Tcl_DStringFree(&ds);
    status = clientDefault->Status();
    if (status == TCL_ERROR) {
	Tcl_SetObjResult(interp, Tcl_NewStringObj(
		clientDefault->GetTraceBack(), -1));
	clientDefault = &Nub;
	return TCL_ERROR;
    }

    return TCL_OK;
}

int IRCEngine::Rehash ()
{
    IRCUserScriptProvider *scriptEngine;
    int result;

    if (clientDefault != &Nub) delete clientDefault;

    for (
	result = scriptEngines.Top(&scriptEngine);
	result != TCL_ERROR;
	result = scriptEngines.Next(&scriptEngine)
    ) {
	delete scriptEngine;
    }

    return LoadScripts();
}

void IRCEngine::ClearAllConnectionData ()
{
    Tcl_Obj **listElements, *chanList = Tcl_NewObj();
    IRCChanInfo *chanInfo;
    const char *key;
    int result, count;

    ClearObj(ServerHost);
    ClearObj(ServerPort);
    ClearObj(ServerInfo);
    ClearObj(MyNick);
    ClearObj(MyUser);

    if (encodings[11] != 0L) {Tcl_FreeEncoding(encodings[11]); encodings[11] = 0L;}
    if (encodings[12] != 0L) {Tcl_FreeEncoding(encodings[12]); encodings[11] = 0L;}
    if (encodings[13] != 0L) {Tcl_FreeEncoding(encodings[13]); encodings[11] = 0L;}


    // Get all key names.  We can't delete hash entries during a hash
    // search!
    //
    for (
	result = channels.Top(0L, (const void **)&key);
	result != TCL_ERROR;
	result = channels.Next(0L, (const void **)&key)
    ) {
	Tcl_ListObjAppendElement(0L, chanList, Tcl_NewStringObj(key,-1));
    }

    Tcl_ListObjGetElements(0L, chanList, &count, &listElements);

    while (count--)
    {
	channels.Extract(Tcl_GetString(listElements[count]), &chanInfo);
	delete chanInfo;
    }

    Tcl_DecrRefCount(chanList);
}
