/*-----------------------------------------------------------------------
 *  IRCUtils.cpp --
 *
 *  Common classes and procedures.
 *
 *-----------------------------------------------------------------------
 * RCS: @(#) $Id: IRCUtils.cpp,v 1.13 2006/09/24 06:59:33 davygrvy Exp $
 *-----------------------------------------------------------------------
 */

#include "IRCUtils.hpp"

ObjectBuffer::ObjectBuffer()
    : space(5), objv(0L), objc(0)
{
    objv = (Tcl_Obj **) ckalloc(sizeof(Tcl_Obj *) * space);
}

ObjectBuffer::~ObjectBuffer()
{
    ckfree((char *)objv);
}

void ObjectBuffer::Add (Tcl_Obj *obj)
{
    if (objc+1 > space) {
	objv = (Tcl_Obj **) ckrealloc((char *) objv, sizeof(Tcl_Obj *) * (space *= 2));
    }
    // Don't take ownership as Tcl_NewListObj() will.
    objv[objc++] = obj;
}

Tcl_Obj *ObjectBuffer::NewList (void)
{
    return Tcl_NewListObj(objc, objv);
}

// Scandanavian special cases included!
//
static unsigned char tolowermap[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
    0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
    0x40,  'a',  'b',  'c',  'd',  'e',  'f',  'g',
     'h',  'i',  'j',  'k',  'l',  'm',  'n',  'o',
     'p',  'q',  'r',  's',  't',  'u',  'v',  'w',
     'x',  'y',  'z',  '[', '\\',  ']', 0x5E, 0x5F,
    0x60,  'a',  'b',  'c',  'd',  'e',  'f',  'g',
     'h',  'i',  'j',  'k',  'l',  'm',  'n',  'o',
     'p',  'q',  'r',  's',  't',  'u',  'v',  'w',
     'x',  'y',  'z',  '[', '\\',  ']', 0x7E, 0x7F,
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
    0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
    0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
    0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7,
    0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
    0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7,
    0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
    0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
    0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
    0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7,
    0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
    0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
    0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
    0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7,
    0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
};

static char *staticBuffer = 0L;
static size_t staticBufferSize = 0;
static Tcl_UniChar *staticUBuffer = 0L;
static size_t staticUBufferSize = 0;
Tcl_DString *ds1;
Tcl_DString *ds2;


// Scandanavian safe lowering utility.
//
const char *
IRCToUtfLowerFromObj (Tcl_Obj *bytes, int useScandanavianRule)
{
    int length, i;
    Tcl_UniChar *in;
    const char *a;

    if (ds1 == 0L) {
	ds1 = (Tcl_DString *) ckalloc(sizeof(Tcl_DString));
	ds2 = (Tcl_DString *) ckalloc(sizeof(Tcl_DString));
	Tcl_DStringInit(ds1);
	Tcl_DStringInit(ds2);
    } else {
	Tcl_DStringFree(ds1);
    }

    in = Tcl_GetUnicodeFromObj(bytes, &length);
    Tcl_DStringAppend(ds1, (const char *) in,
	    (length * sizeof(Tcl_UniChar)) + 1);
    in = (Tcl_UniChar *) Tcl_DStringValue(ds1);

    for (i = 0;  i < length; i++) {
	in[i] = Tcl_UniCharToLower(in[i]);
	if (useScandanavianRule && in[i] > 0x7A &&
		in[i] < 0x7E) in[i] -= 0x20;
    }

    a = Tcl_UniCharToUtfDString((Tcl_UniChar *)Tcl_DStringValue(ds1),
	    Tcl_DStringLength(ds1)/sizeof(Tcl_UniChar), ds2);

    return a;
}

const Tcl_UniChar *
IRCToUniLowerFromObj (Tcl_Obj *bytes)
{
    size_t newSize, i;
    const Tcl_UniChar *in;

    if (staticUBufferSize == 0) {
	staticUBufferSize = newSize = Tcl_GetCharLength(bytes);
	staticUBuffer = (Tcl_UniChar *) ckalloc(staticUBufferSize * sizeof(Tcl_UniChar));
    } else if ((newSize = (size_t) Tcl_GetCharLength(bytes)) > staticUBufferSize) {
	staticUBufferSize = newSize;
	staticUBuffer = (Tcl_UniChar *) ckrealloc((char *)staticUBuffer, newSize * sizeof(Tcl_UniChar));
    }

    for (i = 0, in = Tcl_GetUnicode(bytes); i < newSize; i++) {
	staticUBuffer[i] = Tcl_UniCharToLower(in[i]);
    }
    staticUBuffer[i] = L'\0';
    return staticUBuffer;
}

int
IRCIsEqual (Tcl_Obj *a, Tcl_Obj *b)
{
    const Tcl_UniChar *first, *second;
    wchar_t pos;

    if (!a || !b) return 0;	// not equal.

    first = Tcl_GetUnicode(a);
    second = Tcl_GetUnicode(b);

    for (pos = 0; first[pos] != 0 && second[pos] != 0; pos++)
    {
	if (tolowermap[(first[pos] & 0xFF)] !=
	    tolowermap[(second[pos] & 0xFF)])
	{
	    return 0;	// not equal.
	}
    }
    return (first[pos] == 0 && second[pos] == 0);
}

int IRCIsEqual (Tcl_Obj *a, const char16_t *second)
{
    const Tcl_UniChar *first;
    wchar_t pos;

    if (!a) return 0;	// not equal.

    first = Tcl_GetUnicode(a);

    for (pos = 0; first[pos] != 0 && second[pos] != 0; pos++)
    {
	if (tolowermap[(first[pos] & 0xFF)] !=
	    tolowermap[(second[pos] & 0xFF)])
	{
	    return 0;	// not equal.
	}
    }
    return (first[pos] == 0 && second[pos] == 0);
}

void
CTCP2UniQuote (Tcl_Obj **data, Tcl_Encoding utf8)
{
    int len = Tcl_GetCharLength(*data);
    Tcl_UniChar *string = Tcl_GetUnicode(*data);
    int position;
    Tcl_Obj *firstHalf, *secondHalf;
    int parseUniStart, parseUniEnd;

    // find a character in the unicode range
    //
    for (position = 0, parseUniStart = -1; position < len; position++) {
	// bump out when found.
	if (string[position] & 0xff00) {
	    parseUniStart = position;
	    break;
	}
    }

    // do we have any?
    //
    while (parseUniStart != -1) {
	// find the next character in the non-unicode range.
	//
        for (position = parseUniStart + 1, parseUniEnd = -1;
		position < len; position++)
	{
	    // bump out when found.
	    if (!(string[position] & 0xff00)) {
		parseUniEnd = position - 1;
		break;
	    }
	}

	Tcl_DString ds;
	Tcl_Obj *segment;
	const char *s;
	int oldlen, newlen;

	// Get the segment to be converted.
	segment = Tcl_GetRange(*data, parseUniStart, parseUniEnd);
	s = Tcl_GetStringFromObj(segment, &oldlen);
	Tcl_DStringInit(&ds);
	Tcl_UtfToExternalDString(utf8, s, oldlen, &ds);
	Tcl_DecrRefCount(segment);

	// Get the segment again after conversion (actually, just a
	// pass-thru).  This time as binary data and shoehorn it back in.
	//
	segment = Tcl_NewByteArrayObj((unsigned char *)Tcl_DStringValue(&ds),
		Tcl_DStringLength(&ds));
	Tcl_DStringFree(&ds);
	newlen = Tcl_GetCharLength(segment);
	firstHalf = Tcl_GetRange(*data, 0, parseUniStart - 1);
	// Add the CTCP/2 start marker.  This mean the text to follow is
	// encoded in utf-8, which it now is :)
	//
	Tcl_AppendToObj(firstHalf, "\6EU\6", -1);
	Tcl_AppendObjToObj(firstHalf, segment);
	Tcl_DecrRefCount(segment);

	if (parseUniEnd != -1) {
	    secondHalf = Tcl_GetRange(*data, parseUniEnd + 1, -1);
	    // Add the end marker.
	    //
	    Tcl_AppendToObj(firstHalf, "\6E\6", -1);
	    Tcl_AppendObjToObj(firstHalf, secondHalf);
	    Tcl_DecrRefCount(secondHalf);
	}
	Tcl_DecrRefCount(*data);
	Tcl_IncrRefCount(*data = firstHalf);

	// reset the length and string.
	string = Tcl_GetUnicode(*data);
	len = Tcl_GetCharLength(*data);

        for (position = (parseUniStart + newlen + 7 /* marker lengths */),
		parseUniStart = -1; position < len; position++)
	{
	    // bump out when found.
	    if (string[position] & 0xff00) {
		parseUniStart = position;
		break;
	    }
	}
    }
}


// ------------------------------------------------------------------
// Split it into a list from a seperator character.
// ------------------------------------------------------------------
void IRCSplitFromObj (Tcl_Obj *in, Tcl_Obj **out, const char16_t *splitUChars)
{
    char *element, *string, *end;
    int stringLen, len, i;
    Tcl_UniChar ch1, ch2;
    Tcl_Obj *objPtr;
    int splitCharLen;

    string = Tcl_GetStringFromObj(in, &stringLen);
    end = string + stringLen;
    splitCharLen = Tcl_UniCharLen((const Tcl_UniChar *)splitUChars);
    *out = Tcl_NewObj();

    for (element = string; string < end; string += len) {
	len = Tcl_UtfToUniChar(string, &ch1);
	for (ch2 = splitUChars[0], i = 0; ch2 != 0; ch2 = splitUChars[++i]) {
	    if (ch1 == ch2) {
		objPtr = Tcl_NewStringObj(element, string - element);
		Tcl_ListObjAppendElement(0L, *out, objPtr);
		element = string + len;
	    }
	}
    }
    objPtr = Tcl_NewStringObj(element, string - element);
    Tcl_ListObjAppendElement(0L, *out, objPtr);
}



#define EVIL_TCL_HACKER 1

// This feature MUST be added to the core.  It can not live here!
// THIS STRUCTURE IS FILE SCOPE PRIVATE FROM generic/tclEncoding.c !
// We copy it because there is just is no other way.

#if EVIL_TCL_HACKER   
typedef size_t (LengthProc)_ANSI_ARGS_((CONST char *src));
/*
 * The following data structure represents an encoding, which describes how
 * to convert between various character sets and UTF-8.
 */

typedef struct Encoding {
    char *name;			/* Name of encoding.  Malloced because (1)
				 * hash table entry that owns this encoding
				 * may be freed prior to this encoding being
				 * freed, (2) string passed in the
				 * Tcl_EncodingType structure may not be
				 * persistent. */
    Tcl_EncodingConvertProc *toUtfProc;
				/* Procedure to convert from external
				 * encoding into UTF-8. */
    Tcl_EncodingConvertProc *fromUtfProc;
				/* Procedure to convert from UTF-8 into
				 * external encoding. */
    Tcl_EncodingFreeProc *freeProc;
				/* If non-NULL, procedure to call when this
				 * encoding is deleted. */
    int nullSize;		/* Number of 0x00 bytes that signify
				 * end-of-string in this encoding.  This
				 * number is used to determine the source
				 * string length when the srcLen argument is
				 * negative.  This number can be 1 or 2. */
    ClientData clientData;	/* Arbitrary value associated with encoding
				 * type.  Passed to conversion procedures. */
    LengthProc *lengthProc;	/* Function to compute length of
				 * null-terminated strings in this encoding.
				 * If nullSize is 1, this is strlen; if
				 * nullSize is 2, this is a function that
				 * returns the number of bytes in a 0x0000
				 * terminated string. */
    int refCount;		/* Number of uses of this structure. */
    Tcl_HashEntry *hPtr;	/* Hash table entry that owns this encoding. */
} Encoding;
#endif

char *
IRC_UtfToLayeredExternalDString (
    Tcl_Encoding encoding,	/* The encoding for the converted string,
				 * or NULL for the default system encoding. */
    const char *src,		/* Source string in UTF-8. */
    int srcLen,			/* Source string length in bytes, or < 0 for
				 * strlen(). */
    Tcl_DString *dstPtr)	/* Uninitialized or free DString in which 
				 * the converted string is to be stored. */
{
    char *dst;
    Tcl_EncodingState state;
    Encoding *encodingPtr;
    int flags, dstLen, result, soFar, srcRead, dstWrote, dstChars;

    Tcl_DStringInit(dstPtr);
    dst = Tcl_DStringValue(dstPtr);
    dstLen = dstPtr->spaceAvl - 1;

//    if (encoding == NULL) {
//	encoding = systemEncoding;
//    }
    encodingPtr = (Encoding *) encoding;

    if (src == NULL) {
	srcLen = 0;
    } else if (srcLen < 0) {
	srcLen = strlen(src);
    }
    flags = TCL_ENCODING_START | TCL_ENCODING_END | TCL_ENCODING_STOPONERROR;
    while (1) {
	result = (*encodingPtr->fromUtfProc)(encodingPtr->clientData, src,
		srcLen, flags, &state, dst, dstLen, &srcRead, &dstWrote,
		&dstChars);
	soFar = dst + dstWrote - Tcl_DStringValue(dstPtr);
	if (result != TCL_CONVERT_NOSPACE) {
	    if (encodingPtr->nullSize == 2) {
	        Tcl_DStringSetLength(dstPtr, soFar + 1);
	    }
	    Tcl_DStringSetLength(dstPtr, soFar);
	}
	if (result != TCL_CONVERT_UNKNOWN) {
	    return Tcl_DStringValue(dstPtr);
	} else {
	    Tcl_UniChar uch;
	    int utf8Len;
	    /* put the unconvertible character back into the output in utf-8 */
	    utf8Len = Tcl_UtfToUniChar(src+soFar, &uch);
	    Tcl_DStringAppend(dstPtr, src+soFar, utf8Len);
	    src += srcRead + utf8Len;
	    srcLen -= srcRead + utf8Len;
	    dst = Tcl_DStringValue(dstPtr) + soFar + utf8Len;
	    dstLen = Tcl_DStringLength(dstPtr) - soFar + utf8Len - 1;
	    continue;
	}
	flags &= ~TCL_ENCODING_START;
	src += srcRead;
	srcLen -= srcRead;
	if (Tcl_DStringLength(dstPtr) == 0) {
	    Tcl_DStringSetLength(dstPtr, dstLen);
	}
	Tcl_DStringSetLength(dstPtr, 2 * Tcl_DStringLength(dstPtr) + 1);
	dst = Tcl_DStringValue(dstPtr) + soFar;
	dstLen = Tcl_DStringLength(dstPtr) - soFar - 1;
    }
}
