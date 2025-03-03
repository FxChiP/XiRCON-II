#include "IRCUserScriptProvider.hpp"

IRCUserScriptProviderNub Nub;

IRCUserScriptProvider::CompletionStatus
IRCUserScriptProviderNub::IncomingEvent (const IRCParse *line)
{
    return NOTCOMPLETED;
}
