/*-----------------------------------------------------------------------
 *  IRCUtils.hpp --
 *
 *  Common classes and procedures.
 *
 *-----------------------------------------------------------------------
 * RCS: @(#) $Id: IRCUtils.hpp,v 1.11 2006/09/24 06:59:33 davygrvy Exp $
 *-----------------------------------------------------------------------
 */

#ifndef INC_IRCUtils_hpp_
#define INC_IRCUtils_hpp_

#include <cpptcl.hpp>

// A small utility class to manage an array of Tcl_Objs.
//
class ObjectBuffer
{
    int objc;
    int space;
    Tcl_Obj **objv;

public:
    ObjectBuffer();
    ~ObjectBuffer();
    
    void Add (Tcl_Obj *obj);
    Tcl_Obj *NewList (void);
};

// These AIN'T thread-safe!!!
const char *IRCToUtfLowerFromObj (Tcl_Obj *bytes, int useScandanavianRule);
const Tcl_UniChar *IRCToUniLowerFromObj (Tcl_Obj *bytes);

int IRCIsEqual (Tcl_Obj *a, Tcl_Obj *b);
int IRCIsEqual (Tcl_Obj *a, const char16_t *b);

void CTCP2UniQuote(Tcl_Obj **data, Tcl_Encoding utf8);
void IRCSplitFromObj (Tcl_Obj *in, Tcl_Obj **out, const char16_t *splitUChars);

char *
IRC_UtfToLayeredExternalDString (Tcl_Encoding encoding, const char *src,
			       int srcLen, Tcl_DString *dstPtr);

#endif /* #ifndef INC_IRCUtils_hpp_ */
