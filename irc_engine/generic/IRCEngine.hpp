/*-----------------------------------------------------------------------
 *  IRCEngine.hpp --
 *
 *  The main class for handling an IRC connection and its scripts.
 *
 *-----------------------------------------------------------------------
 * RCS: @(#) $Id: IRCEngine.hpp,v 1.39 2006/09/26 19:20:40 davygrvy Exp $
 *-----------------------------------------------------------------------
 */

#ifndef INC_IRCEngine_hpp__
#define INC_IRCEngine_hpp__

#include "cpptcl.hpp"
#include "IRCParse.hpp"
#include "IRCSplitAndQ.hpp"
#include "IRCUserScriptProvider.hpp"
#include "IRCChanInfo.hpp"

class IRCEngine
    : public Tcl::SocketNotifyBase, QCallbackBase
{
    friend Tcl_EventProc EvalCallback;

public:
    IRCEngine(
	Tcl_Interp *interp,
	const char *instanceName,
	Tcl_Obj *scripts,
	Tcl_Obj *uri_RE[2]);

    ~IRCEngine();

    Tcl::Socket *Sock;
    Tcl_Obj *ServerHost;
    Tcl_Obj *ServerPort;
    int connected;

    Tcl_Obj *ServerInfo;	  // The server list info.
    Tcl_Obj *ConnectServerUser;   // user name used during connection phase.
    Tcl_Obj *ConnectServerPass;   // password used during connection phase.

    Tcl_Obj *ConnectNickname;
    Tcl_Obj *ConnectUsername;
    Tcl_Obj *ConnectRealname;
    Tcl_Obj *MyUser, *MyNick, *MyHost;

    inline int Status() { return status; }

    int ConnectToIRC();
    int DisconnectFromIRC();
    int SendToIRC(Tcl_Obj *bytes);
    int SetEncodingHacksFromListObj(Tcl_Obj *encList);

    Tcl::Hash<IRCChanInfo *> channels;

    int Rehash();

    Tcl::Hash<IRCUserScriptProvider *> scriptEngines;

private:

    struct IRCEVENT {
	Tcl_Event header;
	const IRCParse *line;
	IRCEngine *engine;
	Tcl_Interp *interp;
    };

    int LoadDefaultBehavior();
    void ClearAllConnectionData();

    void QueueAllPendingIRCLines();
    void EvalOneIRCEvent(const IRCParse *line);

    virtual int socketMsgCallback(socketMsgIds Id, Tcl_Obj *msg);
    virtual int socketReadCallback(Tcl_Obj *bytes);

    virtual void QCallback(const IRCParse *line);

    IRCUserScriptProvider::CompletionStatus PreEvent(const IRCParse *line);
    void PostEvent(const IRCParse *line);

    int LoadScripts();
    void ClearScripts();
    int LoadScript(Tcl_Obj *script);
    int UnloadScript(Tcl_Obj *script);
    Tcl_Obj *GetScripts();

    Tcl_Encoding encodings[14];
    Tcl_Obj **uri_RE;
    IRCUserScriptProvider *clientDefault;
    int status;
    Tcl_Interp *interp;
    Tcl_Obj *instanceCmd;
    bool waitingForMyUserHost;
    Tcl_Obj *scriptFileNames;
};

Tcl_EventProc EvalCallback;

#endif  // INC_IRCEngine_hpp__


