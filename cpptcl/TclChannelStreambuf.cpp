// -*- C++ -*-

/*
## File: TclChannelStreambuf.cpp
##
## Synopsis:
##	Class to support redirection of a C++ stream to a Tcl channel
##	(particularly, routing cin/cout/cerr to the console).
*/

#include "cpptcl.hpp"
#include <memory.h>

#if 0    // THIS IS NOT TO BE USED UNTIL IT IS REPAIRED

CPPTCL_BEGIN_NAMESPACE(Tcl)

/*
## Function: TclChannelStreambuf::TclChannelStreambuf
##
## Synopsis:
##	Associate a Tcl channel with a streambuf.
*/

Streambuf::Streambuf(Tcl_Channel channel)
    : channel_(channel)
{
    Tcl_DStringInit(&iBuffer_);
    iOffset_ = 0;
}

/*
## Function: TclChannelStreambuf::~TclChannelStreambuf
##
## Synopsis:
##	Destroy a streambuf associated with a Tcl channel.
*/

Streambuf::~Streambuf()
{
    Tcl_DStringFree(&iBuffer_);
}

/*
## Function: TclChannelStreambuf::overflow
##
## Synopsis:
##	Handle a full output buffer on a streambuf writing to a Tcl channel.
*/

int
Streambuf::overflow(int c)
{
    int status;

    // Allocate reserve area if necessary

    if (base() == 0) {
	doallocate();
    }

    // If there's no output buffer, allocate one.  Place it after the
    // end of the input buffer if there's input.

    if (!pbase()) {
	if (egptr() > gptr()) {
	    setp(egptr(), ebuf());
	} else {
	    setp(base(), ebuf());
	}
    }

    // If there's stuff in the output buffer, write it to the channel.

    if (pptr() > pbase()) {
	status = Tcl_Write(channel_, pbase(), pptr() - pbase());
	if (status < (pptr() - pbase())) {
	    return EOF;
	}
	setp (pbase(), ebuf());
    }

    // Save the next character in the output buffer.  If there is none,
    // flush the channel

    if (c != EOF) {
	*(pptr()) = (char)c;
	pbump(1);
	return c;
    } else {
	setp(0, 0);
	status = Tcl_Flush(channel_);
	if (status != TCL_OK) return EOF;
	return 0;
    }
}

/*
## Function: TclChannelStreambuf::underflow
##
## Synopsis:
##	Get a fresh input buffer from a Tcl channel for a streambuf
*/

int
Streambuf::underflow()
{

    // Nothing to do if the buffer hasn't underflowed.

    if (egptr() > gptr()) {
	return *gptr();
    }

    // Make sure we have a reserve area

    if (!base()) {
	doallocate();
    }

    // Flush any pending output

    if (pptr() > pbase()) {
	if (overflow(EOF) == EOF) {
	    return EOF;
	}
    }

    // Get a fresh line of input if needed

    if (iOffset_ >= Tcl_DStringLength(&iBuffer_)) {
	if (Tcl_Gets(channel_, &iBuffer_)) {
	    return EOF;
	}
	Tcl_DStringAppend(&iBuffer_, "\n", 1);
    }

    // Determine how much input to transfer.  Don't fill the reserve
    // area more than half full

    size_t xferlen = Tcl_DStringLength(&iBuffer_);
    if ((long) xferlen > (ebuf() - base()) / 2) {
	xferlen = (ebuf() - base()) / 2;
    }

    // Copy string into the buffer, and advance pointers

    memcpy ((void *) base(), (void *) Tcl_DStringValue(&iBuffer_), xferlen);
    iOffset_ += xferlen;
    setg(base(), base(), base() + xferlen);

    // Free the input string if we're finished with it

    if (iOffset_ >= Tcl_DStringLength(&iBuffer_)) {
	Tcl_DStringFree(&iBuffer_);
	iOffset_ = 0;
    }

    // Return the first character read.

    return *gptr();
}

/*
## Function: TclChannelStreambuf::sync
##
## Synopsis:
##	Synchronize input and output on a Tcl channe associated with
##	a streambuf
*/

int
Streambuf::sync() {
  // Flush output

    if (overflow(EOF) == EOF) {
	return EOF;
    }

    // Discard input
    setg(0, 0, 0);

    return 0;
}

/*
## Function: TclConsoleStreambufSetup
##
## Synopsis:
##	Bind the standard streams to the Tcl console
*/

Streambuf::Init (Tcl_Interp *)
{
    cin = new Streambuf(Tcl_GetStdChannel(TCL_STDIN));
    cout = new Streambuf(Tcl_GetStdChannel(TCL_STDOUT));
    cerr = new Streambuf(Tcl_GetStdChannel(TCL_STDERR));
    //cerr << "C++ standard input and output now on the Tcl console" << endl;
    return TCL_OK;
}

CPPTCL_END_NAMESPACE

#endif  // #if 0