#include "cpptcl.hpp"

class IRCChanInfo
{
public:
    IRCChanInfo();
    ~IRCChanInfo();
    Tcl_Obj *GetTopic();
    Tcl_Obj *GetAllInfo();
    void SetTopic (Tcl_Obj *newtopic);
    void SetTopicBy (Tcl_Obj *who, Tcl_Obj *when);
    void SetCreationTime (Tcl_Obj *when);

private:
    Tcl_Obj *Topic, *CreationTime;
};
