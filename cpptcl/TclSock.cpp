/*
 ------------------------------------------------------------------------------
 * TclSock.cpp --
 *
 *  Direct API implementation of sockets with Tcl.  This ASSUMES an active
 *  event loop for notification.
 *
 *
 * Copyright (c) 1999-2002 Tomasoft Engineering
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: TclSock.cpp,v 1.31 2006/09/26 19:19:04 davygrvy Exp $
 ------------------------------------------------------------------------------
 */
#include "TclSock.hpp"
#include "tclInt.h"




#define UNUSED(c)	(c) = (c)

namespace Tcl {

    /* local protos */
    static Tcl_FileProc gotConnectWrite;
    static Tcl_FileProc gotReadable;

//--------------------------- Behavioral classes ------------------------------

// forward reference
//
class StateManager;

// pure-virtual base class for all state specific behaviors.
//
class SockState
{
public:
    virtual int Send (StateManager *cSMPtr, Tcl_Obj *bytes) = 0;
    virtual int Connect	(StateManager *cSMPtr, const char *host, const char *port,
			 const char *myhost, const char *myport) = 0;
    virtual int ListenForOne (StateManager *cSMPtr) = 0;
    virtual int Reserve (StateManager *cSMPtr, const char *port) = 0;
    virtual int Close (StateManager *cSMPtr) = 0;
    virtual const char *state () = 0;
};

//  These global classes only define behaviors for the StateManager

class SockClosed : public SockState
{
public:
    virtual int Send (StateManager *cSMPtr, Tcl_Obj *bytes)
    {
	UNUSED(cSMPtr);
	UNUSED(bytes);
	return TCL_ERROR;
    }
    virtual int Connect (StateManager *cSMPtr, const char *host, const char *port,
		 const char *myhost, const char *myport);
    virtual int ListenForOne (StateManager *cSMPtr)
    {
	UNUSED(cSMPtr);
	return TCL_ERROR;
    }
    virtual int Reserve (StateManager *cSMPtr, const char *port)
    {
	UNUSED(cSMPtr);
	UNUSED(port);
	return TCL_ERROR;
    }
    virtual int Close (StateManager *cSMPtr)
    {
	UNUSED(cSMPtr);
	return TCL_ERROR;
    }
    virtual const char *state () { return "closed"; }
} SockClosedBehavior;


class SockConnecting : public SockState
{
public:
    virtual int Send (StateManager *cSMPtr, Tcl_Obj *bytes)
    {
	UNUSED(cSMPtr);
	UNUSED(bytes);
	return TCL_ERROR;
    }
    virtual int Connect (StateManager *cSMPtr, const char *host, const char *port,
		 const char *myhost, const char *myport)
    {
	UNUSED(cSMPtr);
	UNUSED(host);
	UNUSED(port);
	UNUSED(myhost);
	UNUSED(myport);
	return TCL_ERROR;
    }
    virtual int ListenForOne (StateManager *cSMPtr)
    {
	UNUSED(cSMPtr);
	return TCL_ERROR;
    }
    virtual int Reserve (StateManager *cSMPtr, const char *port)
    {
	UNUSED(cSMPtr);
	UNUSED(port);
	return TCL_ERROR;
    }
    virtual int Close (StateManager *cSMPtr)
    {
	UNUSED(cSMPtr);
	return TCL_ERROR;
    }
    virtual const char *state () { return "connecting"; }
} SockConnectingBehavior;


class SockConnected : public SockState
{
public:
    virtual int Send (StateManager *cSMPtr, Tcl_Obj *bytes);
    virtual int Connect (StateManager *cSMPtr, const char *host, const char *port,
		 const char *myhost, const char *myport)
    {
	UNUSED(cSMPtr);
	UNUSED(host);
	UNUSED(port);
	UNUSED(myhost);
	UNUSED(myport);
	return TCL_ERROR;
    }
    virtual int ListenForOne (StateManager *cSMPtr)
    {
	UNUSED(cSMPtr);
	return TCL_ERROR;
    }
    virtual int Reserve (StateManager *cSMPtr, const char *port)
    {
	UNUSED(cSMPtr);
	UNUSED(port);
	return TCL_ERROR;
    }
    virtual int Close (StateManager *cSMPtr);
    virtual const char *state() { return "connected"; }
} SockConnectedBehavior;


class SockListening : public SockState
{
public:
    virtual int Send (StateManager *cSMPtr, Tcl_Obj *bytes)
    {
	UNUSED(cSMPtr);
	UNUSED(bytes);
	return TCL_ERROR;
    }
    virtual int Connect (StateManager *cSMPtr, const char *host, const char *port,
		 const char *myhost, const char *myport)
    {
	UNUSED(cSMPtr);
	UNUSED(host);
	UNUSED(port);
	UNUSED(myhost);
	UNUSED(myport);
	return TCL_ERROR;
    }
    virtual int ListenForOne (StateManager *cSMPtr)
    {
	UNUSED(cSMPtr);
	return TCL_ERROR;
    }
    virtual int Reserve (StateManager *cSMPtr, const char *port)
    {
	UNUSED(cSMPtr);
	UNUSED(port);
	return TCL_ERROR;
    }
    virtual int Close (StateManager *cSMPtr)
    {
	UNUSED(cSMPtr);
	return TCL_ERROR;
    }
    virtual const char *state () { return "listening"; }
} SockListeningBehavior;


class SockListeningForOne : public SockState
{
public:
    virtual int Send (StateManager *cSMPtr, Tcl_Obj *bytes)
    {
	UNUSED(cSMPtr);
	UNUSED(bytes);
	return TCL_ERROR;
    }
    virtual int Connect (StateManager *cSMPtr, const char *host, const char *port,
		 const char *myhost, const char *myport)
    {
	UNUSED(cSMPtr);
	UNUSED(host);
	UNUSED(port);
	UNUSED(myhost);
	UNUSED(myport);
	return TCL_ERROR;
    }
    virtual int ListenForOne (StateManager *cSMPtr)
    {
	UNUSED(cSMPtr);
	return TCL_ERROR;
    }
    virtual int Reserve (StateManager *cSMPtr, const char *port)
    {
	UNUSED(cSMPtr);
	UNUSED(port);
	return TCL_ERROR;
    }
    virtual int Close (StateManager *cSMPtr)
    {
	UNUSED(cSMPtr);
	return TCL_ERROR;
    }
    virtual const char *state () { return "listening"; }
} SockListeningForOneBehavior;


//------------------------ Encryption/Proxy classes ---------------------------


// base class for added responsibilities.
//
class SockDecorator
{
public:
    SockDecorator (SockDecorator *decor_) : _decor(decor_) {}
    ~SockDecorator () { delete _decor; }
    virtual int Send (Tcl_Obj *bytes) = 0;
    virtual int Connect (const char *host, const char *port, const char *myhost,
			 const char *myport) = 0;
    virtual int ListenForOne () = 0;
    virtual int Reserve (const char *port) = 0;
    virtual int Close () = 0;
    virtual int SetOption (const StreamOptions *opts) = 0;
    virtual const char *state () = 0;
protected:
    SockDecorator *_decor;
};


class StateManager : public SockDecorator
{
    // All behaviors have full access.
    friend class SockClosed;
    friend class SockConnecting;
    friend class SockConnected;
    friend class SockListening;

    // All callbacks have full access.
    friend Tcl_FileProc gotConnectWrite;
    friend Tcl_FileProc gotReadable;

public:
    StateManager (SocketNotifyBase *notify)
	: _notify(notify), _state(&SockClosedBehavior),
	SockDecorator(0L), _sockChannel(0), savedOptions(0L)
    {}
    ~StateManager ()
    {
	if (_state != &SockClosedBehavior)
	    _state->Close(this);
    }

    virtual int Send (Tcl_Obj *bytes)
    {
	return _state->Send(this, bytes);
    }

    virtual int Connect (const char *host, const char *port, const char *myhost,
		 const char *myport)
    {
	return _state->Connect(this, host, port, myhost, myport);
    }

    virtual int ListenForOne ()
    {
	return _state->ListenForOne(this);
    }

    virtual int Reserve (const char *port)
    {
	return _state->Reserve(this, port);
    }

    virtual int Close ()
    {
	return _state->Close(this);
    }

    virtual int SetOption (const StreamOptions *opts)
    {
	Tcl_Obj **optPair;
	int count, i;

	if (_sockChannel == 0L) {
	    savedOptions = opts;
	} else {
	    Tcl_ListObjGetElements(0L, opts->optPairs, &count, &optPair);
	    for (i = 0; i < count; i += 2) {
		Tcl_SetChannelOption(0L, _sockChannel,
			Tcl_GetString(optPair[i]), Tcl_GetString(optPair[i+1]));
	    }
	}
	return TCL_OK;
    }

    virtual const char *state ()
    {
	return _state->state();
    }

private:
    SockState *_state;
    Tcl_Channel _sockChannel;
    SocketNotifyBase *_notify;
    const StreamOptions *savedOptions;
};


class Socks4 : public SockDecorator
{
public:
    Socks4 (SockDecorator *decor_) : SockDecorator(decor_) {}
    ~Socks4 () {}
    virtual int Send (Tcl_Obj *bytes) { return _decor->Send(bytes); }
    virtual int Connect (const char *host, const char *port, const char *myhost,
			 const char *myport)
    {
	return _decor->Connect(host, port, myhost, myport);
    }

    virtual int ListenForOne ()
    {
	return _decor->ListenForOne();
    }

    virtual int Reserve (const char *port)
    {
	return _decor->Reserve(port);
    }

    virtual int Close ()
    {
	return _decor->Close();
    }

    virtual int SetOption (const StreamOptions *opts)
    {
	return _decor->SetOption(opts);
    }

    virtual const char *state ()
    {
	return _decor->state();
    }
};


class Socks5 : public SockDecorator
{
public:
    Socks5 (SockDecorator *decor_) : SockDecorator(decor_) {}
    ~Socks5 () {}
    virtual int Send (Tcl_Obj *bytes) { return _decor->Send(bytes); }
    virtual int Connect (const char *host, const char *port, const char *myhost,
			 const char *myport)
    {
	return _decor->Connect(host, port, myhost, myport);
    }

    virtual int ListenForOne ()
    {
	return _decor->ListenForOne();
    }

    virtual int Reserve (const char *port)
    {
	return _decor->Reserve(port);
    }

    virtual int Close ()
    {
	return _decor->Close();
    }

    virtual int SetOption (const StreamOptions *opts)
    {
	return _decor->SetOption(opts);
    }

    virtual const char *state ()
    {
	return _decor->state();
    }
};


class TLS : public SockDecorator
{
public:
    TLS (SockDecorator *decor_) : SockDecorator(decor_) {}
    ~TLS () {}
    virtual int Send (Tcl_Obj *bytes) { return _decor->Send(bytes); }
    virtual int Connect (const char *host, const char *port, const char *myhost,
			 const char *myport)
    {
	return _decor->Connect(host, port, myhost, myport);
    }

    virtual int ListenForOne ()
    {
	return _decor->ListenForOne();
    }

    virtual int Reserve (const char *port)
    {
	return _decor->Reserve(port);
    }

    virtual int Close ()
    {
	return _decor->Close();
    }

    virtual int SetOption (const StreamOptions *opts)
    {
	return _decor->SetOption(opts);
    }

    virtual const char *state ()
    {
	return _decor->state();
    }
};


class Kerberos5 : public SockDecorator
{
public:
    Kerberos5 (SockDecorator *decor_) : SockDecorator(decor_) {}
    ~Kerberos5 () {}
    virtual int Send (Tcl_Obj *bytes) { return _decor->Send(bytes); }
    virtual int Connect (const char *host, const char *port, const char *myhost,
			 const char *myport)
    {
	return _decor->Connect(host, port, myhost, myport);
    }

    virtual int ListenForOne ()
    {
	return _decor->ListenForOne();
    }

    virtual int Reserve (const char *port)
    {
	return _decor->Reserve(port);
    }

    virtual int Close ()
    {
	return _decor->Close();
    }

    virtual int SetOption (const StreamOptions *opts)
    {
	return _decor->SetOption(opts);
    }

    virtual const char *state ()
    {
	return _decor->state();
    }
};


//------------------------------ Private guts ---------------------------------


class SockPrivateImp : public SockPrivateImpBase
{
public:
    SockPrivateImp(SocketNotifyBase *notify,
	    const StreamOptions *opts,
	    Socket::Proxy pFlags,
	    Socket::Encryption eFlags)
	: _imp(0)
    {
	_imp = new StateManager(notify);  // end of the line in the chain.

	switch (pFlags)
	{
	    case Socket::PROXY_SOCKS4:
		_imp = new Socks4(_imp);
		break;
	    case Socket::PROXY_SOCKS5:
		_imp = new Socks5(_imp);
	}

	switch (eFlags)
	{
	    case Socket::ENCRYPT_TLS:
		_imp = new TLS(_imp);
		break;
	    case Socket::ENCRYPT_KRB5:
		_imp = new Kerberos5(_imp);
	}

	_imp->SetOption(opts);
    }

    ~SockPrivateImp()
    {
	delete _imp;
    }

    int Send (Tcl_Obj *bytes)
    {
	return _imp->Send(bytes);
    }

    int Connect (const char *host, const char *port, const char *myhost,
		 const char *myport)
    {
	return _imp->Connect(host, port, myhost, myport);
    }

    int ListenForOne ()
    {
	return _imp->ListenForOne();
    }

    int Reserve (const char *port)
    {
	return _imp->Reserve(port);
    }

    int Close ()
    {
	return _imp->Close();
    }

    int SetOption (const StreamOptions *newOptions)
    {
	return _imp->SetOption(newOptions);
    }

    const char *state ()
    {
	return _imp->state();
    }

private:
    SockDecorator *_imp;
};

int
SockClosed::Connect (StateManager *cSMPtr, const char *host, const char *port,
		     const char *myhost, const char *myport)
{
    unsigned short Uport, Umyport;

    // This call will block for "an amount of time" when it is given a hostname
    // instead of an IP.
    //
#ifdef IOCPSOCK_VERSION
    cSMPtr->_sockChannel =
	    Iocp_OpenTcpClient(0L, port, host, myhost, myport, 1);
#else
    Socket::GetPort(0L, port, &Uport);
    Socket::GetPort(0L, myport, &Umyport);
    cSMPtr->_sockChannel =
	    Tcl_OpenTcpClient(0L, Uport, host, myhost, Umyport, 1);
#endif

    if (cSMPtr->_sockChannel == 0L)
    {
	cSMPtr->_state = &SockClosedBehavior;
	// send an error notification
#ifdef IOCPSOCK_VERSION
	Tcl_Obj *msg = Tcl_NewStringObj(Tcl_Win32ErrMsg(), -1);
#else
	Tcl_Obj *msg = Tcl_NewStringObj(Tcl_ErrnoMsg(Tcl_GetErrno()), -1);
#endif
	Tcl_IncrRefCount(msg);
	cSMPtr->_notify->socketMsgCallback(SocketNotifyBase::MSG_CANNOTCONNECT, msg);
	Tcl_DecrRefCount(msg);
	return TCL_ERROR;
    }
    if (cSMPtr->savedOptions != 0L) cSMPtr->SetOption(cSMPtr->savedOptions);
    cSMPtr->_state = &SockConnectingBehavior;
    Tcl_CreateChannelHandler(cSMPtr->_sockChannel, TCL_WRITABLE | TCL_EXCEPTION,
	    gotConnectWrite, cSMPtr);

    // Send a connecting notification
    Tcl_Obj *msg = Tcl_NewStringObj("connecting", -1);
    Tcl_ListObjAppendElement(0L, msg, Tcl_NewStringObj(host, -1));
    Tcl_ListObjAppendElement(0L, msg, Tcl_NewStringObj(port, -1));
    Tcl_IncrRefCount(msg);
    cSMPtr->_notify->socketMsgCallback(SocketNotifyBase::MSG_CONNECTING, msg);
    Tcl_DecrRefCount(msg);

    return TCL_OK;
}


int
SockConnected::Close (StateManager *cSMPtr)
{
    Tcl_Close(0L, cSMPtr->_sockChannel);
    cSMPtr->_sockChannel = 0L;
    cSMPtr->_state = &SockClosedBehavior;
    // send a disconnect notification
    Tcl_Obj *msg = Tcl_NewStringObj("disconnected", -1);
    Tcl_IncrRefCount(msg);
    cSMPtr->_notify->socketMsgCallback(SocketNotifyBase::MSG_DISCONNECTED, msg);
    Tcl_DecrRefCount(msg);
    return TCL_OK;
}


int
SockConnected::Send (StateManager *cSMPtr, Tcl_Obj *bytes)
{
    if (Tcl_WriteObj(cSMPtr->_sockChannel, bytes) < 0)
    {
	return TCL_ERROR;
    }
    return TCL_OK;
}


//---------------------------- Static callbacks -------------------------------


static void
gotConnectWrite (ClientData cData, int mask)
{
    StateManager *cSMPtr = static_cast<StateManager *>(cData);
    Tcl_DString ds;

    Tcl_DStringInit(&ds);

    // remove this FD event as we're done with it now.
    Tcl_DeleteChannelHandler(cSMPtr->_sockChannel, gotConnectWrite, cData);

    if (Tcl_GetChannelOption(NULL, cSMPtr->_sockChannel, "-peername", &ds)
	    == TCL_OK)
    {
	cSMPtr->_state = &SockConnectedBehavior;
	// Send a connect notice.
	Tcl_Obj *msg = Tcl_NewStringObj("connected", -1);
	Tcl_ListObjAppendList(0L, msg,
		Tcl_NewStringObj(Tcl_DStringValue(&ds), -1));
	Tcl_IncrRefCount(msg);
	cSMPtr->_notify->socketMsgCallback(
		SocketNotifyBase::MSG_CONNECTED, msg);
	Tcl_DecrRefCount(msg);

	Tcl_CreateChannelHandler(cSMPtr->_sockChannel, TCL_READABLE,
		gotReadable, cSMPtr);

    } else {
	cSMPtr->_state = &SockClosedBehavior;
	Tcl_GetChannelOption(0L, cSMPtr->_sockChannel, "-error", &ds);
	Tcl_Close(0L, cSMPtr->_sockChannel);
	cSMPtr->_sockChannel = 0L;
	// Send an error notification.
	Tcl_Obj *msg = Tcl_NewStringObj("error", -1);
	Tcl_ListObjAppendList(0L, msg,
		Tcl_NewStringObj(Tcl_DStringValue(&ds), -1));
	Tcl_IncrRefCount(msg);
	cSMPtr->_notify->socketMsgCallback(
		SocketNotifyBase::MSG_DISCONNECTED, msg);
	Tcl_DecrRefCount(msg);
    }
    Tcl_DStringFree(&ds);

    UNUSED(mask);
}

static void
gotReadable (ClientData cData, int mask)
{
    StateManager *cSMPtr = static_cast<StateManager *>(cData);
    Tcl_Obj *oneLine;

    Tcl_IncrRefCount(oneLine = Tcl_NewObj());
    if (Tcl_GetsObj(cSMPtr->_sockChannel, oneLine) != -1) {
	cSMPtr->_notify->socketReadCallback(oneLine);
    } else {
	if (Tcl_Eof(cSMPtr->_sockChannel)) {
	    cSMPtr->_state = &SockClosedBehavior;
	    Tcl_Close(0L, cSMPtr->_sockChannel);
	    cSMPtr->_sockChannel = 0L;
	    Tcl_Obj *msg = Tcl_NewStringObj("disconnected", -1);
	    Tcl_ListObjAppendList(0L, msg,
		Tcl_NewStringObj(Tcl_ErrnoMsg(Tcl_GetErrno()), -1));
	    Tcl_IncrRefCount(msg);
	    cSMPtr->_notify->socketMsgCallback(SocketNotifyBase::MSG_DISCONNECTED, msg);
	    Tcl_DecrRefCount(msg);
	}
	/*else {
	    cSMPtr->_state = &SockClosedBehavior;
	    Tcl_Close(0L, cSMPtr->_sockChannel);
	    Tcl_Obj *msg = Tcl_NewStringObj("disconnected", -1);
	    Tcl_ListObjAppendList(0L, msg,
		Tcl_NewStringObj(Tcl_ErrnoMsg(Tcl_GetErrno()), -1));
	    Tcl_IncrRefCount(msg);
	    cSMPtr->_notify->socketMsgCallback(SocketNotifyBase::MSG_DISCONNECTED, msg);
	    Tcl_DecrRefCount(msg);
	}*/
    }
    Tcl_DecrRefCount(oneLine);

    UNUSED(mask);
}

//--------------- Public class constructor and destructor ---------------


Socket::Socket (SocketNotifyBase *notify, const StreamOptions *opts, Proxy pFlags, Encryption eFlags)
    : _imp(0)
{
    _imp = new SockPrivateImp(notify, opts, pFlags, eFlags);
}

Socket::~Socket ()
{
    delete _imp;
}


//------------------------- Public static members ------------------------


StreamOptions *DefaultStreamOptions = 0L;

int
Socket::Init(Tcl_Interp *interp)
{
    if (!DefaultStreamOptions) {
	DefaultStreamOptions = new StreamOptions (
	    "-blocking", "no",		    // never!!
	    "-encoding", "iso8859-1",	    // doesn't mangle stuff.
	    "-buffering", "line",	    // but, of course...
	    "-translation", "{auto crlf}",  // except any, send only crlf
#ifdef IOCPSOCK_VERSION			    // If using the finest sockets library known!
	    "-recvmode", "flow-controlled", // Max speed, medium resource hog
	    "-sendcap", "30",		    // Max speed
#endif
	    0L);
    }
    return TclpHasSockets(interp);
}

// Return the port in system byte order, not network byte order!
int
Socket::GetPort(Tcl_Interp *interp, const char *string, unsigned short *port)
{
    int temp, result;
    Tcl_DString ds;

    Tcl_DStringInit(&ds);

    if (string == NULL) {
	*port = 0;
	return TCL_OK;
    }

    // We use a Tcl_DString to get around the missing constant-ness
    // of TclSockGetPort().  I don't want my code to *need* write
    // permission.
    //
    Tcl_DStringAppend(&ds, string, -1);
    result = TclSockGetPort(interp, Tcl_DStringValue(&ds), "tcp", &temp);
    Tcl_DStringFree(&ds);
    if (result == TCL_ERROR) return TCL_ERROR;

    // Fix what should have been an unsigned short, but wasn't.
    //
    *port = static_cast <unsigned short> (temp);
    return TCL_OK;
}


} // namespace Tcl
