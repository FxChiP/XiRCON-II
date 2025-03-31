#include "IRCUserScriptProvider.hpp"

IRCUserScriptProviderNub Nub;

IRCUserScriptProvider::CompletionStatus
IRCUserScriptProviderNub::IncomingEvent (const IRCParse *line)
{
    return NOTCOMPLETED;
}

IRCUserScriptProvider::CompletionStatus
IRCUserScriptProviderNub::RunAlias (const IRCParse* line)
{
    return NOTCOMPLETED;
}
