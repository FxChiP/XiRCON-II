#include "IRCUserScriptProvider.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>


IRCUserScriptProviderForDLL::IRCUserScriptProviderForDLL (
    const char *dllFileName,
    const char *instanceName,
    Tcl_Interp *globalInterp)
    :
    Events(0L),   // <- wrong!!!!
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
    //::LoadLibrary();
    //::GetProcAddress();
}

IRCUserScriptProviderForDLL::~IRCUserScriptProviderForDLL()
{
    //::FreeLibrary();
}

IRCUserScriptProvider::CompletionStatus
IRCUserScriptProviderForDLL::IncomingEvent (const IRCParse *line)
{
    return NOTCOMPLETED;
}

IRCUserScriptProvider::CompletionStatus
IRCUserScriptProviderForDLL::RunAlias (const IRCParse* line)
{
    return NOTCOMPLETED;
}

