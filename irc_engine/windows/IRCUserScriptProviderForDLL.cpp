#include "IRCUserScriptProvider.hpp"


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
}

IRCUserScriptProviderForDLL::~IRCUserScriptProviderForDLL()
{
}

IRCUserScriptProvider::CompletionStatus
IRCUserScriptProviderForDLL::IncomingEvent (const IRCParse *line)
{
    return NOTCOMPLETED;
}
