/* ----------------------------------------------------------------------
 *  IRCParse.cpp --
 *
 *  Our IRC parser.  The use of Unicode speeds the processing by
 *  using the wide 16-bit characters directly.  Once converted up to
 *  unicode initially, the Tcl_Obj stays at unicode for later use.
 *
 *  In Tcl, one char != one character because strings in char form
 *  are encoded in utf-8.
 * 
 *  Although today, it is UTF-16.
 *
 * ----------------------------------------------------------------------
 * RCS: @(#) $Id: IRCParse.cpp,v 1.16 2003/01/13 20:55:48 davygrvy Exp $
 * ----------------------------------------------------------------------
 */

#include "IRCParse.hpp"
#include "IRCUtils.hpp"


IRCParse::IRCParse (Tcl_Obj *oLine)
    : nullObj(Tcl_NewStringObj("",-1)), status(TCL_OK)
{
    Tcl_IncrRefCount(oFullAddress = nullObj);
    Tcl_IncrRefCount(oArgs = nullObj);
    Tcl_IncrRefCount(oRawArgs = nullObj);
    Tcl_IncrRefCount(oRawLine = nullObj);
    Tcl_IncrRefCount(oEvent = nullObj);
    Tcl_IncrRefCount(oNick = nullObj);
    Tcl_IncrRefCount(oUser = nullObj);
    Tcl_IncrRefCount(oHost = nullObj);

    Tcl_IncrRefCount(oRawLine = oLine);
    Parseit(oLine);
}

void IRCParse::Parseit(Tcl_Obj *oLine)
{
    Tcl_UniChar *line = Tcl_GetUnicode(oLine);
    int pos1=0, pos2=0, srcStart=0, srcEnd=0;
    ObjectBuffer params;
    int paramCount=0;


    // prevent empty strings.
    if (Tcl_GetCharLength(oLine) == 0) {
	status = TCL_ERROR;
	return;
    }

    // remove extra leading spaces, if any.
    while (line[pos1] == u' ') pos1++;
    pos2 = pos1;
    
    //get the event source, if any.
    if (line[pos1] == u':') {

	// save the start point.
	srcStart = ++pos1;

	// go to the end of the first "word"
	while (line[pos2] != u' ' && line[pos2] != u'\0') pos2++;

	// back-up one.
	srcEnd = pos2--;

	// set the WHOLE event source.
	Tcl_DecrRefCount(oFullAddress);
	Tcl_IncrRefCount(oFullAddress = Tcl_NewUnicodeObj(line + srcStart, srcEnd - srcStart));

	pos1 = srcEnd;

	// search backwards for a token.
	while (line[pos1] != u'@' && srcStart < pos1) pos1--;

	// set the host of the event
	Tcl_DecrRefCount(oHost);
	Tcl_IncrRefCount(oHost = Tcl_NewUnicodeObj(line + pos1 + 1, pos2 - pos1));

	if (pos1 == srcStart) {
	    // if we're done, nick = host, too.
	    Tcl_DecrRefCount(oNick);
	    Tcl_IncrRefCount(oNick = oHost);
	    goto out;
	}

	// reset the ending position, and keep backing-up for the next token.
	pos2 = --pos1;
	while (line[pos1] != u'!' && line[pos1] != u':') pos1--;

	// set the username of the event.
	Tcl_DecrRefCount(oUser);
	Tcl_IncrRefCount(oUser = Tcl_NewUnicodeObj(line + pos1 + 1, pos2 - pos1));

	if (line[pos1] == u':') {
	    // improbable logic..
	    goto out;
	}

	// set the nick
	Tcl_DecrRefCount(oNick);
	Tcl_IncrRefCount(oNick = Tcl_NewUnicodeObj(line + srcStart, pos1 - srcStart));
out:
	pos1 = pos2 = (line[srcEnd] != u'\0' ? srcEnd+1 : srcEnd);

	// remove extra spaces, if any.
	while (line[pos1] == u' ') pos1++;
	pos2 = pos1;
    }

    // o0pps..  The command is required!  Ditch the scene with an error.
    if (line[pos1] == u'\0') {
	status = TCL_ERROR;
	return;
    }

    // get the ending position of the event.  RFC2812 calls this the
    // command, but we call it the event.
    while (line[pos2] != u' ' && line[pos2] != u'\0') pos2++;

    // set the event.
    Tcl_DecrRefCount(oEvent);
    Tcl_IncrRefCount(oEvent = Tcl_NewUnicodeObj(line + pos1, pos2 - pos1));

    // Are we done?  params are optional -- this is not an error
    // condition.
    //
    if (line[pos2] == u'\0') {
	return;
    }

    pos1 = ++pos2;

    // remove extra spaces, if any.
    while (line[pos1] == u' ') pos1++;
    pos2 = pos1;

    // Are we done?  params are optional -- this is not an error
    // condition.
    //
    if (line[pos1] == u'\0') {
	return;
    }

    // set the remainder as our raw_args INCLUDING the 
    // trailing param's prefix ':'.
    Tcl_DecrRefCount(oRawArgs);
    Tcl_IncrRefCount(oRawArgs = Tcl_NewUnicodeObj(line+pos1, -1));

    // extract all middle params and trailing param
    //
    while (1) {
	if (++paramCount == 15) {
	    // RFC2812, section 2.3 states no more than 15 params allowed.
	    params.Add(Tcl_NewUnicodeObj(line + pos1, -1));
	    break;
	}

	// check for trailing prefix
	if (line[pos1] == u':') {
	    params.Add(Tcl_NewUnicodeObj(line + pos1 + 1, -1));
	    break;
	}

	// get the ending position of the param
	while (line[pos2] != u' ' && line[pos2] != u'\0') pos2++;

	// set the param
	params.Add(Tcl_NewUnicodeObj(line + pos1, pos2 - pos1));

	// move the start position up to the next word
	if (line[pos2] != u'\0') {
	    pos1 = ++pos2;
	} else {
	    // jump out because we're done.
	    break;
	}

	// remove extra spaces, if any.
	while (line[pos1] == u' ') pos1++;
	pos2 = pos1;

	// jump out of the loop if we're done.
	if (line[pos2] == u'\0') break;
    }

    // create the params list.
    Tcl_DecrRefCount(oArgs);
    Tcl_IncrRefCount(oArgs = params.NewList());
}

#if 0
void
IRCParse::SetNick(const char *bytes, int len)
{
    if (Tcl_IsShared(oNick)) {
	Tcl_DecrRefCount(oNick);
	// let our pointer go and just create a new one.
	oNick = Tcl_NewStringObj(bytes, len);
	Tcl_IncrRefCount(oNick);
    } else {
	// replace it.
	Tcl_SetStringObj(oNick, bytes, len);
    }
}

void
IRCParse::SetUser(const char *bytes, int len)
{
    if (Tcl_IsShared(oUser)) {
	Tcl_DecrRefCount(oUser);
	// let our pointer go and just create a new one.
	oUser = Tcl_NewStringObj(bytes, len);
	Tcl_IncrRefCount(oUser);
    } else {
	// replace it.
	Tcl_SetStringObj(oUser, bytes, len);
    }
}
#endif

IRCParse::IRCParse(Tcl_Obj *fa, Tcl_Obj *n, Tcl_Obj *u, Tcl_Obj *h,
		   Tcl_Obj *e, Tcl_Obj *a, Tcl_Obj *ra, Tcl_Obj *rl)
    : nullObj(0L)
{
    Tcl_IncrRefCount(oFullAddress = fa);
    Tcl_IncrRefCount(oNick = n);
    Tcl_IncrRefCount(oUser = u);
    Tcl_IncrRefCount(oHost = h);
    Tcl_IncrRefCount(oEvent = e);
    Tcl_IncrRefCount(oArgs = a);
    Tcl_IncrRefCount(oRawArgs = ra);
    Tcl_IncrRefCount(oRawLine = rl);
    status = TCL_OK;
}


IRCParse::IRCParse(IRCParse &ircP)
    : nullObj(0L)
{
    Tcl_IncrRefCount(oFullAddress = ircP.oFullAddress);
    Tcl_IncrRefCount(oNick = ircP.oNick);
    Tcl_IncrRefCount(oUser = ircP.oUser);
    Tcl_IncrRefCount(oHost = ircP.oHost);
    Tcl_IncrRefCount(oEvent = ircP.oEvent);
    Tcl_IncrRefCount(oArgs = ircP.oArgs);
    Tcl_IncrRefCount(oRawArgs = ircP.oRawArgs);
    Tcl_IncrRefCount(oRawLine = ircP.oRawLine);
    status = ircP.status;
}

IRCParse::~IRCParse()
{
    Tcl_DecrRefCount(oFullAddress);
    Tcl_DecrRefCount(oNick);
    Tcl_DecrRefCount(oUser);
    Tcl_DecrRefCount(oHost);
    Tcl_DecrRefCount(oEvent);
    Tcl_DecrRefCount(oArgs);  
    Tcl_DecrRefCount(oRawArgs);
    Tcl_DecrRefCount(oRawLine);

    if (nullObj != 0L) Tcl_DecrRefCount(nullObj);
};
