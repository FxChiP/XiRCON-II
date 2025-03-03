#ifndef INC_winTclEventSystemInt_hpp__
#define INC_winTclEventSystemInt_hpp__

#include <CMcl.h>
#include "../generic/TclEventSystemInt.hpp"

class TclEventLoop;

class TclEventSystemPlatInt : public TclEventSystemInt
{
public:
    TclEventSystemPlatInt (
	const char *tclVer,
	int exact,
	Tcl_PanicProc *fatal,
	Tcl_PanicProc *nfatal,
	const char *appfile);
    ~TclEventSystemPlatInt ();
    virtual void ShutDown (void);
    virtual int  Q_Get(const TclAsyncJob *&aj);
    virtual void Q_Put(const TclAsyncJob *aj);
private:
    CMclQueue<const TclAsyncJob *> workQ;
    CMclThreadAutoPtr TclEventLoopThread;
    TclEventLoop *loop;
};

#endif  /* INC_winTclEventSystemInt_hpp__ */