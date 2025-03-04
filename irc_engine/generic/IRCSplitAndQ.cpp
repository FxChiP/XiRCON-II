/*-----------------------------------------------------------------------
 *  IRCSplitAndQ.cpp --
 *
 *  One line received from the server may have more than one "irc event"
 *  within it.  This class:
 *
 *	1) removes CTCPs from within a privmsg or notice.
 *	2) translates embedded CTCP/2 encodings within a privmsg or
 *	    notice to Tcl's native unicode.
 *	3) splits modes.
 *
 *  Essentially, all post-parser special processing prior to queueing
 *  the irc event is done here.
 *
 *-----------------------------------------------------------------------
 * RCS: @(#) $Id: IRCSplitAndQ.cpp,v 1.25 2006/09/24 06:50:57 davygrvy Exp $
 *-----------------------------------------------------------------------
 */

#include "IRCSplitAndQ.hpp"
#include "IRCUtils.hpp"

IRCSplitAndQ::IRCSplitAndQ (
	IRCParse &_line, QCallbackBase *_qCB, Tcl_Encoding encs[], Tcl_Obj **_uri_RE)
    : line(_line), IRCevent(NONE), qCB(_qCB), modified(0),
    encodings(encs), PrimaryDecoding(encs[11]), SecondaryDecoding(encs[12]),
    TertiaryDecoding(encs[13]), uri_RE(_uri_RE), destination(0L)
{
    Tcl_Obj *temp1, *temp2, *trailingParam;

    // Is it bogus?
    if (line.Status() == TCL_ERROR) return;

    // fix nickname and username if we need to.
    if (PrimaryDecoding != 0L) {
	DoDecodingHacks(line.GetNickP());
	DoDecodingHacks(line.GetUserP());
    }

    // Is it a special case?
    if (IRCIsEqual(line.GetEvent(), u"privmsg"))
	    IRCevent = PRIVMSG;
    else if (IRCIsEqual(line.GetEvent(), u"notice"))
	    IRCevent = NOTICE;
    else if (IRCIsEqual(line.GetEvent(), u"mode"))
	    IRCevent = MODE;

    switch (IRCevent)
    {
	case PRIVMSG:
	case NOTICE:
	    // scan and replace CTCPs and embedded encodings within the
	    // trailing param.
	    Tcl_ListObjIndex(0L, line.GetArgs(), 0, &temp1);
	    Tcl_ListObjIndex(0L, line.GetArgs(), 1, &temp2);
	    if (temp1 == 0L || temp2 == 0L) goto out;
	    Tcl_IncrRefCount(destination = Tcl_DuplicateObj(temp1));
	    Tcl_IncrRefCount(trailingParam = Tcl_DuplicateObj(temp2));

	    check4Encodings(&trailingParam);
	    check4CTCPs(&trailingParam);
	    //check4Unquotes(&trailingParam);
	    check4URIs(&trailingParam);

	    if (modified) {
		Tcl_ListObjReplace(0L, line.GetArgs(), 1, 1, 1,
			&trailingParam);
	    }
	    Tcl_DecrRefCount(trailingParam);

	    // If a primary encoding is specified, convert destination, too.
	    if (PrimaryDecoding != 0L) {
		DoDecodingHacks(&destination);
		// Shoehorn it back in.
		Tcl_ListObjReplace(0L, line.GetArgs(), 0, 1, 1,
			&destination);
	    }
	    Tcl_DecrRefCount(destination);

	    break;

	case MODE:
	default:
	    // Translate every arg.
	    {
		int objc;
		Tcl_Obj **objv;

		Tcl_ListObjGetElements(0L, line.GetArgs(), &objc, &objv);
		while (objc--) {
		    Tcl_IncrRefCount(temp1 = Tcl_DuplicateObj(objv[objc]));
		    modified = 0;
	    	    check4Encodings(&temp1);
		    check4URIs(&temp1);
		    if (modified) {
			Tcl_ListObjReplace(0L, line.GetArgs(), objc, 1, 1,
				&temp1);
		    }
		    Tcl_DecrRefCount(temp1);
		}
	    }

	    // pull apart the modes and post each.
	    if (IRCevent == MODE) SplitModes();
	    break;
    }

out:
    // post it.
    qCB->QCallback(new IRCParse(line));
}

#define CTCPMARKER L'\001'

void IRCSplitAndQ::check4CTCPs (Tcl_Obj **trailingParam)
{
    int ctcpStartMarker = -1, ctcpEndMarker, ctcpLastEndMarker = 0;
    int len, pos;
    Tcl_UniChar *string;
    Tcl_Obj *firstHalf, *secondHalf;

    string = Tcl_GetUnicode(*trailingParam);
    len = Tcl_GetCharLength(*trailingParam);

    // look for embedded CTCPs inside this trailing param.
    for (pos = 0; pos < len; pos++) {
	// bump out when found.
	if (string[pos] == CTCPMARKER) { ctcpStartMarker = pos; break; }
    }

    // do we have any?
    while (ctcpStartMarker != -1) {

	// find the closing ctcp marker.
        for (pos = ctcpStartMarker + 1, ctcpEndMarker = -1; pos < len; pos++) {
	    // bump out when found.
	    if (string[pos] == CTCPMARKER) { ctcpEndMarker = pos; break; }
	}

	if (ctcpEndMarker == -1) {
	    // an opening without a closing is bogus.
	    break;
	}

	buildCtcpEvent(Tcl_NewUnicodeObj(string + ctcpStartMarker + 1, ctcpEndMarker - ctcpStartMarker - 1));

	// remove the ctcp message from the original.
	firstHalf = Tcl_GetRange(*trailingParam, 0, ctcpStartMarker - 1);
	secondHalf = Tcl_GetRange(*trailingParam, ctcpEndMarker + 1, len - 1);
	Tcl_AppendObjToObj(firstHalf, secondHalf);
	Tcl_DecrRefCount(secondHalf);
	Tcl_DecrRefCount(*trailingParam);
	Tcl_IncrRefCount(*trailingParam = firstHalf);

	// mark the original trailing param as modified.
	modified = 1;

	// reset the length and string.
	string = Tcl_GetUnicode(*trailingParam);
	len = Tcl_GetCharLength(*trailingParam);

	// got any more?
	for (pos = ctcpStartMarker + 1,ctcpStartMarker = -1; pos < len; pos++) {
	    // bump out when found.
	    if (string[pos] == CTCPMARKER) { ctcpStartMarker = pos; break; }
	}
    }
}

#define CTCPTAG_SEP L'\040'

void IRCSplitAndQ::buildCtcpEvent (Tcl_Obj *ctcpMsg)
{
    int pos, pos1 = 0, pos2 = -1;
    int len;
    ObjectBuffer params;
    Tcl_UniChar *string;

    string = Tcl_GetUnicode(ctcpMsg);
    len = Tcl_GetCharLength(ctcpMsg);

    if (len == 0) return;

    // look for the tag seperator which is a space.
    for (pos = 0; pos < len; pos++) {
	// bump out when found.
	if (string[pos] == CTCPTAG_SEP) { pos2 = pos; break; }
    }

    // Target of CTCP.
    params.Add(destination);

    // extract the keyword.  keyword doesn't get unquoted.
    params.Add(Tcl_GetRange(ctcpMsg, 0, (pos2 == -1 ? len - 1 : pos2 - pos1 - 1)));
    
    if (pos2 != -1) {
	Tcl_Obj *tag;

	pos1 = pos2 + 1;
	pos2 = -1;
	for (pos = pos1; pos < len; pos++) {
	    // bump out when found.
	    if (string[pos] == CTCPTAG_SEP) { pos2 = pos; break; }
	}

	while (pos2 != -1) {
	    tag = Tcl_GetRange(ctcpMsg, pos1, pos2 - 1);
	    unQuote(&tag);
	    params.Add(tag);
	    pos1 = pos2 + 1;
	    for (pos = pos1, pos2 = -1; pos < len; pos++) {
		// bump out when found.
		if (string[pos] == CTCPTAG_SEP) { pos2 = pos; break; }
	    }
	}

	tag = Tcl_GetRange(ctcpMsg, pos1, len - 1);
	unQuote(&tag);
	params.Add(tag);
    }

    qCB->QCallback(
	new IRCParse(
	    line.GetFullAddress(), 
	    line.GetNick(), 
	    line.GetUser(),
	    line.GetHost(),
	    (IRCevent == PRIVMSG ? Tcl_NewStringObj("ctcp",-1) : Tcl_NewStringObj("ctcp_reply",-1)),
	    params.NewList(),
	    ctcpMsg,
	    Tcl_NewStringObj("", -1)
	    )
	);
}

void IRCSplitAndQ::unQuote (Tcl_Obj **ctcpTag)
{
    int pos, pos1, len;
    Tcl_UniChar *string;
    Tcl_Obj *firstHalf, *secondHalf;

    string = Tcl_GetUnicode(*ctcpTag);
    len = Tcl_GetCharLength(*ctcpTag);

    //CTCP/2 spec section 3.1:
    //    
    //                      ASCII             Quoted
    // Name                  (Octal)   Quoted  (Octal)
    // --------------------  -------   ------  -------
    // NUL  (null)           000       ^P '0'  020 060
    // STX  (ctcp marker)    001       ^P '1'  020 061
    // LF   (newline)        012       ^P 'n'  020 156
    // CR   (newline)        015       ^P 'r'  020 162
    // SPC  (space)          040       ^P '@'  020 100
    // DLE  (control-P)      020       ^P ^P   020 020


    // look for a DLE.
    for (pos = 0, pos1 = -1; pos < len; pos++) {
	// bump out when found.
	if (string[pos] == u'\020') { pos1 = pos; break; }
    }

    while (pos1 != -1) {
	switch (string[pos1+1]) {

	// NUL
	case u'\060':
	    firstHalf = Tcl_GetRange(*ctcpTag, 0, pos1 - 1);
	    secondHalf = Tcl_GetRange(*ctcpTag, pos1 + 2, len - 1);
	    Tcl_AppendUnicodeToObj(firstHalf, (Tcl_UniChar*)u"\000", 1);
	    Tcl_AppendObjToObj(firstHalf, secondHalf);
	    Tcl_DecrRefCount(secondHalf);
	    Tcl_DecrRefCount(*ctcpTag);
	    Tcl_IncrRefCount(*ctcpTag = firstHalf);
	    break;

	// STX
	case u'\061':
	    firstHalf = Tcl_GetRange(*ctcpTag, 0, pos1 - 1);
	    secondHalf = Tcl_GetRange(*ctcpTag, pos1 + 2, len - 1);
	    Tcl_AppendUnicodeToObj(firstHalf, (Tcl_UniChar*)u"\001", 1);
	    Tcl_AppendObjToObj(firstHalf, secondHalf);
	    Tcl_DecrRefCount(secondHalf);
	    Tcl_DecrRefCount(*ctcpTag);
	    Tcl_IncrRefCount(*ctcpTag = firstHalf);
	    break;

	// LF
	case u'\156':
	    firstHalf = Tcl_GetRange(*ctcpTag, 0, pos1 - 1);
	    secondHalf = Tcl_GetRange(*ctcpTag, pos1 + 2, len - 1);
	    Tcl_AppendUnicodeToObj(firstHalf, (Tcl_UniChar*)u"\012", 1);
	    Tcl_AppendObjToObj(firstHalf, secondHalf);
	    Tcl_DecrRefCount(secondHalf);
	    Tcl_DecrRefCount(*ctcpTag);
	    Tcl_IncrRefCount(*ctcpTag = firstHalf);
	    break;

	// CR
	case u'\152':
	    firstHalf = Tcl_GetRange(*ctcpTag, 0, pos1 - 1);
	    secondHalf = Tcl_GetRange(*ctcpTag, pos1 + 2, len - 1);
	    Tcl_AppendUnicodeToObj(firstHalf, (Tcl_UniChar*)u"\015", 1);
	    Tcl_AppendObjToObj(firstHalf, secondHalf);
	    Tcl_DecrRefCount(secondHalf);
	    Tcl_DecrRefCount(*ctcpTag);
	    Tcl_IncrRefCount(*ctcpTag = firstHalf);
	    break;

	// SPC
	case u'\100':
	    firstHalf = Tcl_GetRange(*ctcpTag, 0, pos1 - 1);
	    secondHalf = Tcl_GetRange(*ctcpTag, pos1 + 2, len - 1);
	    Tcl_AppendUnicodeToObj(firstHalf, (Tcl_UniChar*)u"\040", 1);
	    Tcl_AppendObjToObj(firstHalf, secondHalf);
	    Tcl_DecrRefCount(secondHalf);
	    Tcl_DecrRefCount(*ctcpTag);
	    Tcl_IncrRefCount(*ctcpTag = firstHalf);
	    break;

	// DLE
	case u'\020':
	    firstHalf = Tcl_GetRange(*ctcpTag, 0, pos1 - 1);
	    secondHalf = Tcl_GetRange(*ctcpTag, pos1 + 1, len - 1);
	    Tcl_AppendObjToObj(firstHalf, secondHalf);
	    Tcl_DecrRefCount(secondHalf);
	    Tcl_DecrRefCount(*ctcpTag);
	    Tcl_IncrRefCount(*ctcpTag = firstHalf);
	    break;
	}

	// reset the string and length.
	string = Tcl_GetUnicode(*ctcpTag);
	len = Tcl_GetCharLength(*ctcpTag);

	// look for another DLE.
	for (pos = pos1+1, pos1 = -1; pos < len; pos++) {
	    // bump out when found.
	    if (string[pos] == u'\020') { pos1 = pos; break; }
	}
    }
}

#define CTCP2_FCHAR    u'\006'

void IRCSplitAndQ::check4Encodings(Tcl_Obj **trailingParam)
{
    int len = Tcl_GetCharLength(*trailingParam);
    Tcl_UniChar *ustr = Tcl_GetUnicode(*trailingParam);
    int pos;
    Tcl_Encoding enc = (Tcl_Encoding) -1;
    Tcl_Obj *firstHalf, *secondHalf;
    int parseFrmChar, parseEndChar = 0, parseStart = 0;

    // find an opening ctcp/2 format character (^F)
    for (pos = 0, parseFrmChar = -1; pos < len; pos++) {
	// bump out when found.
	if (ustr[pos] == CTCP2_FCHAR) {
	    parseFrmChar = pos;
	    break;
	}
    }

    // do we have any?
    while (parseFrmChar != -1) {
	// find the closing ctcp/2 format character (^F)
        for (pos = parseFrmChar + 1, parseEndChar = -1; pos < len; pos++) {
	    // bump out when found.
	    if (ustr[pos] == CTCP2_FCHAR) {
		parseEndChar = pos;
		break;
	    }
	}

	if (parseEndChar == -1) {
	    // an opening without a closing is bogus.
	    break;
	}

	// check for text to decode.
	if (parseFrmChar > parseStart) {
	    Tcl_DString ds;
	    Tcl_Obj *segment;
	    unsigned char *s;
	    int oldlen, newlen;

	    Tcl_DStringInit(&ds);

	    // Get the segment to be converted.
	    segment = Tcl_GetRange(*trailingParam, parseStart, parseFrmChar-1);

	    if (enc == (Tcl_Encoding)-1) {
		oldlen = Tcl_GetCharLength(segment);
		DoDecodingHacks(&segment);
	    } else {
		s = Tcl_GetByteArrayFromObj(segment, &oldlen);
		Tcl_ExternalToUtfDString(enc, (const char *)s, oldlen, &ds);
		Tcl_DecrRefCount(segment);

		// Get the segment again after conversion and shoehorn it back in.
		segment = Tcl_NewStringObj(Tcl_DStringValue(&ds), Tcl_DStringLength(&ds));
	    }
	    newlen = Tcl_GetCharLength(segment);
	    if (parseStart) {
		firstHalf = Tcl_GetRange(*trailingParam, 0, parseStart - 1);
	    } else {
		firstHalf = Tcl_NewObj();
	    }
	    secondHalf = Tcl_GetRange(*trailingParam, parseFrmChar, len - 1);
	    Tcl_AppendObjToObj(firstHalf, segment);
	    Tcl_AppendObjToObj(firstHalf, secondHalf);
	    Tcl_DecrRefCount(segment);
	    Tcl_DecrRefCount(secondHalf);
	    Tcl_DecrRefCount(*trailingParam);
	    Tcl_IncrRefCount(*trailingParam = firstHalf);
	    Tcl_DStringFree(&ds);

	    // shift the positions by the length change.
	    parseFrmChar += newlen - oldlen;
	    parseEndChar += newlen - oldlen;

	    // reset the length and string.
	    ustr = Tcl_GetUnicode(*trailingParam);
	    len = Tcl_GetCharLength(*trailingParam);

	    // mark the original trailing param as modified.
	    modified = 1;
	}

	// now, interpret this format specifier
	switch (ustr[parseFrmChar+1])	{
	case u'E':
	    // between 1 and 9 ?
	    if (ustr[parseFrmChar+2] > 48 && ustr[parseFrmChar+2] < 58) {
		int index;
		index = ustr[parseFrmChar+2]-48;
		enc = encodings[index];
	    // letter U ?  UTF-8
	    } else if (ustr[parseFrmChar+2] == u'U') {
		enc = encodings[9];
	    // letter B ?  Big5
	    } else if (ustr[parseFrmChar+2] == u'B') {
		enc = encodings[10];
	    // reset ?
	    } else if (parseFrmChar+2 == parseEndChar) {
		enc = (Tcl_Encoding)-1;
	    } else {
		// not valid.
		break;
	    }

	    // First, remove the format specifier from the original as the
	    // info has already been read.

	    firstHalf = Tcl_GetRange(*trailingParam, 0, parseFrmChar - 1);
	    secondHalf = Tcl_GetRange(*trailingParam, parseEndChar + 1, len - 1);
	    Tcl_AppendObjToObj(firstHalf, secondHalf);
	    Tcl_DecrRefCount(secondHalf);
	    Tcl_DecrRefCount(*trailingParam);
	    Tcl_IncrRefCount(*trailingParam = firstHalf);

	    // mark the original trailing param as modified.
	    modified = 1;

	    // reset the length and string.
	    ustr = Tcl_GetUnicode(*trailingParam);
	    len = Tcl_GetCharLength(*trailingParam);

	    break;

	// revert to normal
	case u'N':
	    enc = encodings[0];
	    break;

	default:
	    //ignore other CTCP/2 attributes.
	    parseStart = parseFrmChar - 1;
	    for (pos = parseEndChar + 1, parseFrmChar = -1; pos < len; pos++) {
		// bump out when found.
		if (ustr[pos] == CTCP2_FCHAR) {
		    parseFrmChar = pos;
		    break;
		}
	    }
	    continue;
	}

	// save old end position
	parseStart = parseFrmChar;
	
	// got any more?
        for (pos = parseStart, parseFrmChar = -1; pos < len; pos++) {
	    // bump out when found.
	    if (ustr[pos] == CTCP2_FCHAR) {
		parseFrmChar = pos;
		break;
	    }
	}
    }

    // Any left-over text that needs converting?
    if (parseStart < len) {
	Tcl_DString ads;
	Tcl_Obj *segment;
	unsigned char *s;
	int oldlen;

	Tcl_DStringInit(&ads);

	// Get the segment to be converted.
	segment = Tcl_GetRange(*trailingParam, parseStart, len-1);

    	if (enc == (Tcl_Encoding)-1) {
	    DoDecodingHacks(&segment);
	} else {
	    s = Tcl_GetByteArrayFromObj(segment, &oldlen);
	    Tcl_ExternalToUtfDString(enc, (const char *)s, oldlen, &ads);
	    Tcl_DecrRefCount(segment);

	    // Get the segment again after conversion.
	    segment = Tcl_NewStringObj(Tcl_DStringValue(&ads),
		    Tcl_DStringLength(&ads));
	}

	// If we converted the entire thing, no sense merging.
	if (parseStart) {
	    firstHalf = Tcl_GetRange(*trailingParam, 0, parseStart - 1);
	    Tcl_AppendObjToObj(firstHalf, segment);
	    Tcl_DecrRefCount(segment);
	} else {
	    firstHalf = segment;
	}
	Tcl_DecrRefCount(*trailingParam);
	Tcl_IncrRefCount(*trailingParam = firstHalf);
	Tcl_DStringFree(&ads);

	// mark the original trailing param as modified.
	modified = 1;
    }
}

/*
  proc filterURIs {var} {
    global uriRE1 uriRE2
    upvar 1 $var local
    # mark URIs with the CTCP/2 attribute for URIs
    regsub -all -- $uriRE1 $local "\006L+\006\\0\006L-\006" local
    # ending parens are 93% bogus (good odds!)
    regsub -all -- $uriRE2 $local "\\1\\2\006L-\006\\3" local
  }
 */
void IRCSplitAndQ::check4URIs (Tcl_Obj **param)
{
    //register int len = Tcl_GetCharLength(*param);
    //register Tcl_UniChar *string = Tcl_GetUnicode(*param);

    if (Tcl_RegExpMatchObj(0L, *param, uri_RE[1])) {
	Tcl_RegExpInfo info;
	Tcl_RegExpGetInfo(Tcl_GetRegExpFromObj(0L, uri_RE[1], 0), &info);
    }
}


void IRCSplitAndQ::SplitModes()
{
    const int argsNone = 0x0, argsPlus = 0x1, argsMinus = 0x2;
    int argsNeeded, direction;
    Tcl_UniChar *modeString, targetFirstChar;
    Tcl_Obj *temp, *modeTarget, *splitModeEvent, *modeArgs;
    int result, len, i, argIndex = 2;

    // plus is assumed until stated otherwise.
    //
    direction = argsPlus;
    argsNeeded = argsNone;

    // Get the mode target.
    //
    result = Tcl_ListObjIndex(0L, line.GetArgs(), 0, &modeTarget);
    if (modeTarget == 0L || result == TCL_ERROR) return;
    targetFirstChar = *Tcl_GetUnicode(modeTarget);

    // Get the mode string.
    //
    result = Tcl_ListObjIndex(0L, line.GetArgs(), 1, &temp);
    if (temp == 0L || result == TCL_ERROR) return;
    modeString = Tcl_GetUnicode(temp);
    len = Tcl_GetCharLength(temp);

    for (i = 0; i < len; i++)
    {
	switch (modeString[i])
	{
	    case L'+':
		direction = argsPlus; break;
	    case L'-':
		direction = argsMinus; break;

	    // Args for both + and -.
	    //
//	    case L'I':	    // invite exemption [dancer-ircd 1.0].
	    case L'b':	    // bans
//	    case L'd':	    // "real name" ban [dancer-ircd 1.0].
  //  	    case L'e':	    // exclusions [dancer-ircd 1.0].
	    //case L'f':	    // forward on uninvited [dancer-ircd 1.0].
			    // UltimateIRCd uses this for flood settings
			    // and is args on +, so we can't use it here.

	    case L'h':	    // half-op [old TS beta].
	    case L'o':	    // chanop.
//	    case L'q':	    // quiet user [dancer-ircd 1.0].
	    case L'v':	    // voice.
	    
		argsNeeded |= argsMinus;

	    // Args for only +.
	    //
	    case L'k':	    // key
	    case L'l':	    // limit

		argsNeeded |= argsPlus;

	    // No args.
//	    case L'A':	    // admin only [UltimateIRCd].
//	    case L'F':	    // forwarding features enabled [dancer-ircd 1.0].
//	    case L'O':	    // ircop only [dalnet].
//	    case L'P':	    // permanent [dancer-ircd 1.0].
//	    case L'R':	    // registered nicks only [dalnet].
//	    case L'S':	    // strip colors (same as 'c') [UltimateIRCd].
//	    case L'c':	    // color filter [dancer-ircd 1.0].
	    case L'i':	    // invite only.
	    case L'j':	    // jupe channel (closed) [dancer-ircd 1.0].
	    case L'm':	    // moderated.
	    case L'n':	    // no external messages.
	    case L'p':	    // private (obsolete).
	    case L'r':	    // registered channel [dalnet].
	    case L's':	    // secret (invisible).
	    case L't':	    // topic protect.
	    default:
	    {
		ObjectBuffer params;

		splitModeEvent = Tcl_NewUnicodeObj((Tcl_UniChar*)u"mode", -1);
		Tcl_AppendUnicodeToObj(splitModeEvent,
			(direction == argsPlus ? plus : minus), 1);
		Tcl_AppendUnicodeToObj(splitModeEvent, &modeString[i], 1);
		params.Add(modeTarget);
		if (direction & argsNeeded) {
		    if (Tcl_ListObjIndex(0L, line.GetArgs(), argIndex++,
			    &modeArgs) != TCL_OK || !modeArgs) {
			modeArgs = Tcl_NewStringObj("", -1);
		    }
		    params.Add(modeArgs);
		}
		qCB->QCallback(
		    new IRCParse(
			line.GetFullAddress(), 
			line.GetNick(), 
			line.GetUser(),
			line.GetHost(),
			splitModeEvent,
			params.NewList(),
			Tcl_NewStringObj("", -1),
			Tcl_NewStringObj("", -1)
			)
		    );
		argsNeeded = argsNone;
	    }
	}
    }
}

void
IRCSplitAndQ::DoDecodingHacks(Tcl_Obj **toBeFixed)
{
    int len;
    const char *string;
    Tcl_UniChar *ustring;
    Tcl_DString ds;
    Tcl_Obj *local = *toBeFixed;

    if (PrimaryDecoding != 0L) {
	Tcl_DStringInit(&ds);
	string = (const char *) Tcl_GetByteArrayFromObj(local, &len);
	Tcl_ExternalToUtfDString(PrimaryDecoding, string, len, &ds);
	if (Tcl_IsShared(local)) {
	    Tcl_DecrRefCount(local);
	    // let our pointer go and just create a new one.
	    local = *toBeFixed = Tcl_NewStringObj(Tcl_DStringValue(&ds),
		    Tcl_DStringLength(&ds));
	    Tcl_IncrRefCount(local);
	} else {
	    // replace it.
	    Tcl_SetStringObj(local, Tcl_DStringValue(&ds),
		    Tcl_DStringLength(&ds));
	}
	if (SecondaryDecoding != 0L) {
	    int position, newlen, oldlen;
	    Tcl_Obj *segment, *firstHalf, *secondHalf;
	    int parseUniStart, parseUniEnd;

	    // Decoding is from 8-bit to 16-bit, but we need to
	    // pass-thru everything already in unicode, and just
	    // translate the 8-bit ranges only.
	    //
	    ustring = Tcl_GetUnicode(local);
	    len = Tcl_GetCharLength(local);

	    // find a character not in the unicode range
	    //
	    for (position = 0, parseUniStart = -1; position < len; position++) {
		// bump out when found.
		if (!(ustring[position] & 0xff00)) {
		    parseUniStart = position;
		    break;
		}
	    }

	    // do we have any?
	    //
	    while (parseUniStart != -1) {
		// find the next character in the unicode range.
		//
		for (position = parseUniStart + 1, parseUniEnd = -1;
			position < len; position++)
		{
		    // bump out when found.
		    if (ustring[position] & 0xff00) {
			parseUniEnd = position - 1;
			break;
		    }
		}

		// Get the segment to be converted.
		segment = Tcl_GetRange(local, parseUniStart,
			(parseUniEnd == -1 ? len : parseUniEnd));
		// We know we won't truncate this.
		string = (const char *) Tcl_GetByteArrayFromObj(segment, &oldlen);
		Tcl_ExternalToUtfDString(SecondaryDecoding, string, oldlen-1, &ds);
		Tcl_DecrRefCount(segment);
		segment = Tcl_NewStringObj(Tcl_DStringValue(&ds), Tcl_DStringLength(&ds));
		newlen = Tcl_GetCharLength(segment);
		if (parseUniStart) {
		    firstHalf = Tcl_GetRange(local, 0, parseUniStart - 1);
		} else {
		    firstHalf = Tcl_NewObj();
		}
		Tcl_AppendObjToObj(firstHalf, segment);
		Tcl_DecrRefCount(segment);

		if (parseUniEnd != -1) {
		    secondHalf = Tcl_GetRange(local, parseUniEnd + 1, -1);
		    Tcl_AppendObjToObj(firstHalf, secondHalf);
		    Tcl_DecrRefCount(secondHalf);
		}

		// replace it.
		Tcl_SetStringObj(local, Tcl_GetString(firstHalf), -1);
		Tcl_DecrRefCount(firstHalf);

		// reset the length and string.
		ustring = Tcl_GetUnicode(local);
		len = Tcl_GetCharLength(local);

		for (position = (parseUniStart + newlen),
			parseUniStart = -1; position < len; position++)
		{
		    // bump out when found.
		    if (!(ustring[position] & 0xff00)) {
			parseUniStart = position;
			break;
		    }
		}
	    }
	    if (TertiaryDecoding != 0L) {
		int position, newlen, oldlen;
		Tcl_Obj *segment, *firstHalf, *secondHalf;
		int parseUniStart, parseUniEnd;

		// Decoding is from 8-bit to 16-bit, but we need to
		// pass-thru everything already in unicode, and just
		// translate the 8-bit ranges only.
		//
		ustring = Tcl_GetUnicode(local);
		len = Tcl_GetCharLength(local);

		// find a character not in the unicode range
		//
		for (position = 0, parseUniStart = -1; position < len; position++) {
		    // bump out when found.
		    if (!(ustring[position] & 0xff00)) {
			parseUniStart = position;
			break;
		    }
		}

		// do we have any?
		//
		while (parseUniStart != -1) {
		    // find the next character in the unicode range.
		    //
		    for (position = parseUniStart + 1, parseUniEnd = -1;
			    position < len; position++)
		    {
			// bump out when found.
			if (ustring[position] & 0xff00) {
			    parseUniEnd = position - 1;
			    break;
			}
		    }

		    // Get the segment to be converted.
		    segment = Tcl_GetRange(local, parseUniStart,
			    (parseUniEnd == -1 ? len : parseUniEnd));
		    // We know we won't truncate this.
		    string = (const char *) Tcl_GetByteArrayFromObj(segment, &oldlen);
		    Tcl_ExternalToUtfDString(TertiaryDecoding, string, oldlen-1, &ds);
		    Tcl_DecrRefCount(segment);
		    segment = Tcl_NewStringObj(Tcl_DStringValue(&ds), Tcl_DStringLength(&ds));
		    newlen = Tcl_GetCharLength(segment);
		    if (parseUniStart) {
			firstHalf = Tcl_GetRange(local, 0, parseUniStart - 1);
		    } else {
			firstHalf = Tcl_NewObj();
		    }
		    Tcl_AppendObjToObj(firstHalf, segment);
		    Tcl_DecrRefCount(segment);

		    if (parseUniEnd != -1) {
			secondHalf = Tcl_GetRange(local, parseUniEnd + 1, -1);
			Tcl_AppendObjToObj(firstHalf, secondHalf);
			Tcl_DecrRefCount(secondHalf);
		    }

		    // replace it.
		    Tcl_SetStringObj(local, Tcl_GetString(firstHalf), -1);
		    Tcl_DecrRefCount(firstHalf);

		    // reset the length and string.
		    ustring = Tcl_GetUnicode(local);
		    len = Tcl_GetCharLength(local);

		    for (position = (parseUniStart + newlen),
			    parseUniStart = -1; position < len; position++)
		    {
			// bump out when found.
			if (!(ustring[position] & 0xff00)) {
			    parseUniStart = position;
			    break;
			}
		    }
		}
	    }
	}
	Tcl_DStringFree(&ds);
    }
}

const Tcl_UniChar *IRCSplitAndQ::plus = (Tcl_UniChar*)L"+";
const Tcl_UniChar *IRCSplitAndQ::minus = (Tcl_UniChar*)L"-";
