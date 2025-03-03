/*
 ------------------------------------------------------------------------------
 * TclSock.hpp --
 *
 *  Direct API implementation of sockets with Tcl.  This ASSUMES an active
 *  event loop for notification.  Based on some of the ideas found in
 *  Mo Dejong's EasySocket library.  The purpose of this class is to simplify
 *  the complexities of socket I/O as much as is possible.
 *
 * Copyright (c) 1999-2000 Tomasoft Engineering
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: TclSock.hpp,v 1.24 2006/09/26 19:19:04 davygrvy Exp $
 ------------------------------------------------------------------------------
 */
#ifndef INC_TclSock_hpp_
#define INC_TclSock_hpp_

#include "tcl.h"

namespace Tcl {

//  Minimalist internal pure-virtual representation made public to the
//  Socket class as a bridge pattern to enable us to hide our true guts.
//  Dependent code using this interface won't have to be recompiled whenever
//  the internal code is changed; just re-linked.
//
class SockPrivateImpBase
{
public:
    virtual int Send    (Tcl_Obj *bytes) = 0;
    virtual int Connect (const char *host, const char *port, const char *myhost,
			 const char *myport) = 0;
    virtual int ListenForOne  () = 0;
    virtual int Reserve (const char *port) = 0;
    virtual int Close   () = 0;
};


class StreamOptions
{
    // the StateManager is allowed to look inside us.
    friend class StateManager;

public:
    StreamOptions(const char *opt, ...) {
	va_list va;
	const char *temp;

	va_start(va, opt);
	Tcl_IncrRefCount(optPairs = Tcl_NewObj());
	Tcl_ListObjAppendElement(0L, optPairs, Tcl_NewStringObj(opt, -1));
	for (temp = va_arg(va, const char *); temp; temp = va_arg(va, const char *)) {
	    Tcl_ListObjAppendElement(0L, optPairs, Tcl_NewStringObj(temp, -1));
	}
	va_end(va);
    }
    ~StreamOptions() {
	Tcl_DecrRefCount(optPairs);
    }

private:
    Tcl_Obj *optPairs;
};

extern StreamOptions *DefaultStreamOptions;


// class used as a base for the callbacks
//
class SocketNotifyBase
{
public:
    enum socketMsgIds {
	MSG_CANNOTCONNECT,
	MSG_CONNECTING,
	MSG_CONNECTED,
	MSG_DISCONNECTED
    };

    virtual int socketMsgCallback (socketMsgIds Id, Tcl_Obj *msg) = 0;
    virtual int socketReadCallback (Tcl_Obj *bytes) = 0;
};


//  Public interface
//
class Socket
{
public:

    enum Proxy { PROXY_NONE, PROXY_SOCKS4, PROXY_SOCKS5 };
    enum Encryption { ENCRYPT_NONE, ENCRYPT_TLS, ENCRYPT_KRB5 };

    Socket (
	SocketNotifyBase *notify,
	const StreamOptions *opts = DefaultStreamOptions,
	Proxy pFlags = PROXY_NONE,
	Encryption eFlags = ENCRYPT_NONE);

    Socket (Socket &copy);  // We declare a copy constructor, but don't define one
			    // to make sure it errors when a copy is made by
			    // accident.
    ~Socket ();

    int Send (Tcl_Obj *bytes) {
	return _imp->Send(bytes);
    }
    int Connect	(
	const char *host,
	const char *port,
	const char *myhost = 0L,
	const char *myport = 0L)
    {
	return _imp->Connect(host, port, myhost, myport);
    }

    int ListenForOne ()
    {
	return _imp->ListenForOne();
    }

    int Reserve	(const char *port)
    {
	return _imp->Reserve(port);
    }
    
    int Close ()
    {
	return _imp->Close();
    }
    
    int AddOptions (StreamOptions *opts)
    {
	opts = opts;
    }
    
    Tcl_Obj *GetOption(const char *opt)
    {
	opt = opt;
    }

    static int Init (Tcl_Interp *interp);

    // Not in network byte order!
    static int GetPort(Tcl_Interp *interp, const char *string, unsigned short *port);

private:
    SockPrivateImpBase *_imp;
};

} // namespace Tcl

#endif  //#ifndef INC_TclSock_hpp_
