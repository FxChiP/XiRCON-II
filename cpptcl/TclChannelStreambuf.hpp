// -*- C++ -*-

/*
## File: TclChannelStreambuf.hpp
##
## Synopsis:
##	Class to support redirection of a C++ stream to a Tcl channel
##	(particularly, routing cin/cout/cerr to the console).
#@CDef
*/

#ifndef INC_TclChannelStreambuf_hpp__
#define INC_TclChannelStreambuf_hpp__

#if 0    // THIS IS NOT TO BE USED UNTIL IT IS REPAIRED

#include <iostream.h>
#ifdef _MSC_VER
#   include <streamb.h>
#else
#   include <streambuf.h>
#endif

namespace Tcl {

class Streambuf : public streambuf {
private:
    Tcl_Channel channel_;
    Tcl_DString iBuffer_;
    int iOffset_;
public:
    Streambuf(Tcl_Channel);
    ~Streambuf();
    static int Init(Tcl_Interp *interp);
    virtual int overflow(int);
    virtual int underflow();
    virtual int sync();
};

} // namespace Tcl

#endif  // #if 0

#endif /* INC_TclChannelStreambuf_hpp__ */
