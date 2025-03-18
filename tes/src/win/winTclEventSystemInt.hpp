#ifndef INC_winTclEventSystemInt_hpp__
#define INC_winTclEventSystemInt_hpp__

#include <CMcl.h>
#include "../generic/TclEventSystemInt.hpp"

class TclEventLoop;

class TclEventSystemPlatInt : public TclEventSystemInt
{
public:
    TclEventSystemPlatInt (const char *libToLoad, Tcl_PanicProc *fatal);
    ~TclEventSystemPlatInt ();
    virtual void ShutDown (void);
    virtual int  Q_Get(TclAsyncJob *&aj);
    virtual void Q_Put(TclAsyncJob *aj);
private:
    CMclQueue<TclAsyncJob *> workQ;
    CMclThreadAutoPtr TclEventLoopThread;
    TclEventLoop *loop;
};

#endif  /* INC_winTclEventSystemInt_hpp__ */