#include "IRCChanInfo.hpp"
#include <string.h>
#include <stdlib.h>

IRCChanInfo::IRCChanInfo()
    : CreationTime(0L)
{
    Tcl_IncrRefCount(Topic = Tcl_NewListObj(0,0L));
}


IRCChanInfo::~IRCChanInfo()
{
    Tcl_DecrRefCount(Topic);
    if (CreationTime) Tcl_DecrRefCount(CreationTime);
}


Tcl_Obj *
IRCChanInfo::GetTopic()
{
    return Topic;
}

Tcl_Obj *
IRCChanInfo::GetAllInfo()
{
    // TODO: not implemented, yet
    return 0L;
}

void
IRCChanInfo::SetTopic(Tcl_Obj *newtopic)
{
    int len;

    if (Tcl_IsShared(Topic)) {
	Tcl_IncrRefCount(Topic = Tcl_DuplicateObj(Topic));
    }

    if (Tcl_ListObjLength(0L, Topic, &len) != TCL_ERROR
	    && len > 0) {
	Tcl_ListObjReplace(0L, Topic, 0, 1, 1, &newtopic);
    } else {
	Tcl_ListObjAppendElement(0L, Topic, newtopic);
    }
}

void
IRCChanInfo::SetTopicBy (Tcl_Obj *who, Tcl_Obj *when)
{
    Tcl_Obj *list[2];
    int len;

    if (Tcl_IsShared(Topic)) {
	Tcl_IncrRefCount(Topic = Tcl_DuplicateObj(Topic));
    }

    if (Tcl_ListObjLength(0L, Topic, &len) != TCL_ERROR
	    && len > 2) {
	list[0] = who;
	list[1] = when;
	Tcl_ListObjReplace(0L, Topic, 1, 2, 2, list);
    } else {
	Tcl_ListObjAppendElement(0L, Topic, who);
	Tcl_ListObjAppendElement(0L, Topic, when);
    }
}

void
IRCChanInfo::SetCreationTime (Tcl_Obj *when)
{
    Tcl_IncrRefCount(CreationTime = when);
}
