/*-----------------------------------------------------------------------
 *  IRCSplitAndQ.hpp --
 *
 *  One line received from the server may have more than one "irc event"
 *  within it.  This class:
 *
 *	1) removes CTCPs from within a privmsg or notice.
 *	2) translates embedded CTCP/2 encodings to Tcl's native unicode.
 *	3) splits modes.
 *
 *  Essentially, all post-parser special processing prior to queueing
 *  the irc event is done here.
 *
 *-----------------------------------------------------------------------
 * RCS: @(#) $Id: IRCSplitAndQ.hpp,v 1.15 2006/09/24 06:50:57 davygrvy Exp $
 *-----------------------------------------------------------------------
 */

#ifndef INC_IRCSplitAndQ_hpp__
#define INC_IRCSplitAndQ_hpp__

#include "IRCParse.hpp"

class QCallbackBase
{
    friend class IRCSplitAndQ;
private:
    virtual void QCallback (const IRCParse *_line) = 0;
};


class IRCSplitAndQ
{
public:
    IRCSplitAndQ(IRCParse &_line, QCallbackBase *_qCB, Tcl_Encoding encs[], Tcl_Obj **uri_RE);
private:
    void check4CTCPs (Tcl_Obj **trailingParam);
    void buildCtcpEvent (Tcl_Obj *ctcpMsg);
    void unQuote (Tcl_Obj **ctcpTag);
    void check4Encodings (Tcl_Obj **trailingParam);
    void check4URIs (Tcl_Obj **trailingParam);
    void SplitModes();
    void DoDecodingHacks(Tcl_Obj **toBeFixed);

    IRCParse &line;
    QCallbackBase *qCB;
    int modified;
    Tcl_Encoding *encodings;
    enum {NONE, MODE, PRIVMSG, NOTICE} IRCevent;
    static const Tcl_UniChar *plus, *minus;
    Tcl_Obj *destination;

    // Some networks don't use ISO-8859-1.  Store the conversion settings
    // here.
    //
    Tcl_Encoding PrimaryDecoding;
    Tcl_Encoding SecondaryDecoding;
    Tcl_Encoding TertiaryDecoding;

    Tcl_Obj **uri_RE;
};

#endif	// INC_IRCSplitAndQ_hpp__
