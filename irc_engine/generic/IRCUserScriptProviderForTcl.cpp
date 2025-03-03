/*-----------------------------------------------------------------------
 *  IRCUserScriptProviderForTcl.cpp --
 *
 *  This is the Tcl user script provider.
 *
 *-----------------------------------------------------------------------
 * RCS: @(#) $Id: IRCUserScriptProviderForTcl.cpp,v 1.31 2006/10/11 02:13:02 davygrvy Exp $
 *-----------------------------------------------------------------------
 */

#include "IRCUserScriptProvider.hpp"

/*
 * Fix Tcl bug #803489 the right way.  We need to always use the old Stub
 * slot positions, not the new broken ones part of TIP 127.  I do like
 * that these functions have moved to the public space (about time), but
 * the slot change is the killer and is the painful side affect.
 */

#if defined(USE_TCL_STUBS) && \
	(TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION >= 5)
#   undef Tcl_CreateNamespace
#   define Tcl_CreateNamespace \
	(tclIntStubsPtr->tcl_CreateNamespace)
#   undef Tcl_DeleteNamespace
#   define Tcl_DeleteNamespace \
	(tclIntStubsPtr->tcl_DeleteNamespace)
#   undef Tcl_AppendExportList
#   define Tcl_AppendExportList \
	(tclIntStubsPtr->tcl_AppendExportList)
#   undef Tcl_Export
#   define Tcl_Export \
	(tclIntStubsPtr->tcl_Export)
#   undef Tcl_Import
#   define Tcl_Import \
	(tclIntStubsPtr->tcl_Import)
#   undef Tcl_ForgetImport
#   define Tcl_ForgetImport \
	(tclIntStubsPtr->tcl_ForgetImport)
#   undef Tcl_GetCurrentNamespace
#   define Tcl_GetCurrentNamespace \
	(tclIntStubsPtr->tcl_GetCurrentNamespace)
#   undef Tcl_GetGlobalNamespace
#   define Tcl_GetGlobalNamespace \
	(tclIntStubsPtr->tcl_GetGlobalNamespace)
#   undef Tcl_FindNamespace
#   define Tcl_FindNamespace \
	(tclIntStubsPtr->tcl_FindNamespace)
#   undef Tcl_FindCommand
#   define Tcl_FindCommand \
	(tclIntStubsPtr->tcl_FindCommand)
#   undef Tcl_GetCommandFromObj
#   define Tcl_GetCommandFromObj \
	(tclIntStubsPtr->tcl_GetCommandFromObj)
#   undef Tcl_GetCommandFullName
#   define Tcl_GetCommandFullName \
	(tclIntStubsPtr->tcl_GetCommandFullName)
#endif


static void
EventsDeletionProc (void *element)
{
    Tcl_DecrRefCount((Tcl_Obj *)element);
}

IRCUserScriptProviderForTcl::IRCUserScriptProviderForTcl(
    const char *scriptFileName,
    const char *instanceName,
    Tcl_Interp *globalInterp,
    int useSafe)
    :
    Tcl::Adapter<IRCUserScriptProviderForTcl>(),
    Events(EventsDeletionProc),
    Aliases(EventsDeletionProc),
    Args(0L),
    RawArgs(0L),
    RawLine(0L),
    FullAddress(0L),
    Event(0L),
    Nick(0L),
    User(0L),
    Host(0L),
    Completed(IRCUserScriptProvider::NOTCOMPLETED)
{
    const char *argv[1];

    interp = Tcl_CreateInterp();

    // Don't call:
    // Tcl::Adapter<IRCUserScriptProviderForTcl>::InitBase(interp);
    // We don't want the.. umm, self-destructing behavior because
    // the top of this is already managed.

    if (Tcl_Init(interp) == TCL_ERROR) {
	status = TCL_ERROR;
	traceBack = Tcl_GetStringResult(interp);
	return;
    }

    if (useSafe) {
	Tcl_MakeSafe(interp);
    }

    // exit is not allowed is user scripts.
    TclRenameCommand(interp, "exit", "");


    // Make the local commands.
    NewTclCmd("IRC::on",	    &IRCUserScriptProviderForTcl::OnCmd);
    NewTclCmd("IRC::alias",	    &IRCUserScriptProviderForTcl::AliasCmd);
    NewTclCmd("IRC::fulladdress",   &IRCUserScriptProviderForTcl::FullAddrCmd);
    NewTclCmd("IRC::nick",	    &IRCUserScriptProviderForTcl::NickCmd);
    NewTclCmd("IRC::user",	    &IRCUserScriptProviderForTcl::UserCmd);
    NewTclCmd("IRC::host",	    &IRCUserScriptProviderForTcl::HostCmd);
    NewTclCmd("IRC::event",	    &IRCUserScriptProviderForTcl::EventCmd);
    NewTclCmd("IRC::args",	    &IRCUserScriptProviderForTcl::ArgsCmd);
    NewTclCmd("IRC::raw_args",	    &IRCUserScriptProviderForTcl::RawArgsCmd);
    NewTclCmd("IRC::raw_line",	    &IRCUserScriptProviderForTcl::RawLineCmd);
    NewTclCmd("IRC::complete",	    &IRCUserScriptProviderForTcl::CompleteCmd);

    // Make the link up to the global interp for the commands we want
    // connected.
    //
#   define NewAliasedCmd(CMD) \
	argv[0] = STRINGIFY(CMD); \
	Tcl_CreateAlias(interp, "IRC::" STRINGIFY(CMD), \
		globalInterp, instanceName, 1, argv)

    NewAliasedCmd(raw);
    NewAliasedCmd(connect);
    NewAliasedCmd(disconnect);
    NewAliasedCmd(connected);
    NewAliasedCmd(server);
    NewAliasedCmd(my_nick);
    NewAliasedCmd(my_ip);
    NewAliasedCmd(my_host);
    NewAliasedCmd(my_user);
    NewAliasedCmd(my_port);
    NewAliasedCmd(channels);
    NewAliasedCmd(nicks);
    NewAliasedCmd(modes);
    NewAliasedCmd(whois);
    NewAliasedCmd(lookup);
    NewAliasedCmd(color);
    NewAliasedCmd(topic);

    // Assume these UI commands are in the parent [Incr Tcl] object
    // through inheritence :)  This is a type of soft-linking and a
    // new type of design pattern that I have found.  I'm rather
    // pround of myself.  The power of a dynamic language in action.
    // 
    NewAliasedCmd(echo);
    NewAliasedCmd(window);
    NewAliasedCmd(menu);
    NewAliasedCmd(hotkey);
    NewAliasedCmd(channel);
    NewAliasedCmd(query);
    NewAliasedCmd(chat);
    NewAliasedCmd(queries);
    NewAliasedCmd(chats);
    NewAliasedCmd(say);
    NewAliasedCmd(input);

    // Allow links to the parent object
    NewAliasedCmd(user1);
    NewAliasedCmd(user2);
    NewAliasedCmd(user3);
    NewAliasedCmd(user4);
    NewAliasedCmd(user5);
    NewAliasedCmd(user6);

#   undef NewAliasedCmd

    // Let older XiRCON scripts know they are in the new interface.
    //
    Tcl_PkgProvide(interp, "XiRCON", "2.0");

    Tcl_ResetResult(interp);

    status = Tcl_EvalFile(interp, scriptFileName);
    if (status != TCL_OK) {
	traceBack = Tcl_GetVar(interp, "::errorInfo", 0);
    } else {
	IRCParse *line = new IRCParse(
		Tcl_NewStringObj("", -1),
		Tcl_NewStringObj("", -1),
		Tcl_NewStringObj("", -1),
		Tcl_NewStringObj("", -1),
		Tcl_NewStringObj("load", -1),
		Tcl_NewStringObj(scriptFileName, -1),
		Tcl_NewStringObj("", -1),
		Tcl_NewStringObj("", -1)
	    );
	IncomingEvent(line);
	delete line;
    }
}

IRCUserScriptProviderForTcl::~IRCUserScriptProviderForTcl()
{
    IRCParse *line = new IRCParse(
	    Tcl_NewStringObj("", -1),
	    Tcl_NewStringObj("", -1),
	    Tcl_NewStringObj("", -1),
	    Tcl_NewStringObj("", -1),
	    Tcl_NewStringObj("unload", -1),
	    Tcl_NewStringObj("", -1),
	    Tcl_NewStringObj("", -1),
	    Tcl_NewStringObj("", -1)
	);
    IncomingEvent(line);
    delete line;
    Tcl_DeleteInterp(interp);
}

IRCUserScriptProvider::CompletionStatus
IRCUserScriptProviderForTcl::IncomingEvent (const IRCParse *line) 
{
    OurEvents::LPEVENTLIST evlistPtr = 0L;
    int result = TCL_OK, i;

    FullAddress = line->GetFullAddress();
    Nick = line->GetNick();
    User = line->GetUser();
    Host = line->GetHost();
    Event = line->GetEvent();
    Args = line->GetArgs();
    RawArgs = line->GetRawArgs();
    RawLine = line->GetRawLine();
    Completed = NOTCOMPLETED;

    Tcl_Preserve(interp);

    if ( Events.Find ( line->GetEvent(), &evlistPtr ) == TCL_OK ) {

	// we run these in the ::IRC namespace
	Tcl_Namespace *nsPtr = Tcl_FindNamespace(interp, "::IRC", 0L, 0);

	// execute the event hooks in LIFO order
	for (i = 0; i < evlistPtr->count; i++) {
	    CallFrame frame;
	    Proc proc;

	    // Make it run like a proc (not global scope)
	    Tcl_PushCallFrame(interp,
		    reinterpret_cast <Tcl_CallFrame *>(&frame),
		    nsPtr, 1);
	    proc.numCompiledLocals = 0;
	    proc.firstLocalPtr = 0;
	    proc.lastLocalPtr = 0;
	    frame.procPtr = &proc;

	    // Use the precompiled bytecodes ;)
	    result = Tcl_EvalObj(interp, evlistPtr->bytecodes[i]);

	    // garbage collect local variables
	    Tcl_PopCallFrame(interp);

	    if (result == TCL_ERROR) {
		char buff[60];
		Completed = COMPLETED_WITHERROR;
		sprintf(buff, "\nError in event script \"%s\"",
			Tcl_GetStringFromObj(line->GetEvent(), 0L));
		Tcl_AddObjErrorInfo(interp, buff, -1);
		Tcl_BackgroundError(interp);
	    }
	    if (Completed != NOTCOMPLETED) break;
	}
    }

    Tcl_Release(interp);

    FullAddress = Nick = User = Host = Event = Args = RawArgs =
	    RawLine = 0L;
    return Completed;
}

int
IRCUserScriptProviderForTcl::OnCmd(int objc, struct Tcl_Obj * const objv[])
{
    Tcl_Obj *resultPtr = Tcl_GetObjResult (interp);

    if (objc != 3) {
	Tcl_WrongNumArgs (interp, 1, objv, "<event> <script>");
	return TCL_ERROR;
    }

    if (Events.Add (objv[1], objv[2]) != TCL_OK) {
	Tcl_AppendStringsToObj (resultPtr, 
		Tcl_GetStringFromObj(objv[1], 0L), " NOT Added!", 0L);
	return TCL_ERROR;
    }

    // Take ownership now that we are saving it in the hash table.
    //
    Tcl_IncrRefCount(objv[2]);

    return TCL_OK;
}

int
IRCUserScriptProviderForTcl::AliasCmd(int objc, struct Tcl_Obj* const objv[])
{
    Tcl_Obj* resultPtr = Tcl_GetObjResult(interp);

    if (objc != 3) {
	Tcl_WrongNumArgs(interp, 1, objv, "<event> <script>");
	return TCL_ERROR;
    }

    if (Aliases.Add(objv[1], objv[2]) != TCL_OK) {
	Tcl_AppendStringsToObj(resultPtr,
	    Tcl_GetStringFromObj(objv[1], 0L), " NOT Added!", 0L);
	return TCL_ERROR;
    }

    // Take ownership now that we are saving it in the hash table.
    //
    Tcl_IncrRefCount(objv[2]);

    return TCL_OK;
}


int
IRCUserScriptProviderForTcl::FullAddrCmd(int objc, struct Tcl_Obj * const objv[])
{
    if (FullAddress) Tcl_SetObjResult(interp, FullAddress);
    return TCL_OK;
}

int
IRCUserScriptProviderForTcl::NickCmd(int objc, struct Tcl_Obj * const objv[])
{
    if (Nick) Tcl_SetObjResult(interp, Nick);
    return TCL_OK;
}

int
IRCUserScriptProviderForTcl::UserCmd(int objc, struct Tcl_Obj * const objv[])
{
    if (User) Tcl_SetObjResult(interp, User);
    return TCL_OK;
}

int
IRCUserScriptProviderForTcl::HostCmd(int objc, struct Tcl_Obj * const objv[])
{
    if (Host) Tcl_SetObjResult(interp, Host);
    return TCL_OK;
}

int
IRCUserScriptProviderForTcl::EventCmd(int objc, struct Tcl_Obj * const objv[])
{
    if (Event) Tcl_SetObjResult(interp, Event);
    return TCL_OK;
}

int
IRCUserScriptProviderForTcl::ArgsCmd(int objc, struct Tcl_Obj * const objv[])
{
    if (Args) Tcl_SetObjResult(interp, Args);
    return TCL_OK;
}

int
IRCUserScriptProviderForTcl::RawArgsCmd(int objc, struct Tcl_Obj * const objv[])
{
    if (RawArgs) Tcl_SetObjResult(interp, RawArgs);
    return TCL_OK;
}

int
IRCUserScriptProviderForTcl::RawLineCmd(int objc, struct Tcl_Obj * const objv[])
{
    if (RawLine) Tcl_SetObjResult(interp, RawLine);
    return TCL_OK;
}

int
IRCUserScriptProviderForTcl::CompleteCmd(int objc, struct Tcl_Obj * const objv[])
{
    Completed = IRCUserScriptProvider::COMPLETED;
    return TCL_OK;
}
