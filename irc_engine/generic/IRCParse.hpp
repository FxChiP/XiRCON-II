#ifndef __IRCPARSE_H_
#define __IRCPARSE_H_

#include <cpptcl.hpp>

class IRCParse
{
public:
    IRCParse(Tcl_Obj *line);
    IRCParse(Tcl_Obj *fa, Tcl_Obj *n, Tcl_Obj *u, Tcl_Obj *h, Tcl_Obj *e,
	    Tcl_Obj *a, Tcl_Obj *ra, Tcl_Obj *rl);
    IRCParse(IRCParse &ircP);
    ~IRCParse();
    
    inline Tcl_Obj* GetFullAddress() const { return oFullAddress; }
    inline Tcl_Obj* GetNick() const { return oNick; }
    inline Tcl_Obj** GetNickP() { return &oNick; }
    inline Tcl_Obj* GetUser() const { return oUser; }
    inline Tcl_Obj** GetUserP() { return &oUser; }
    inline Tcl_Obj* GetHost() const { return oHost; }
    inline Tcl_Obj* GetEvent() const { return oEvent; }
    inline Tcl_Obj* GetArgs() const { return oArgs; }
    inline Tcl_Obj* GetRawArgs() const { return oRawArgs; }
    inline Tcl_Obj* GetRawLine() const { return oRawLine; }
    inline int Status() const { return status; }

private:

    void Parseit(Tcl_Obj *oLine);

    Tcl_Obj *nullObj;
    Tcl_Obj *oFullAddress;
    Tcl_Obj *oNick;
    Tcl_Obj *oUser;
    Tcl_Obj *oHost;
    Tcl_Obj *oEvent;
    Tcl_Obj *oArgs;
    Tcl_Obj *oRawArgs;
    Tcl_Obj *oRawLine;
    int status;
};

#endif //#ifndef __IRCPARSE_H_

