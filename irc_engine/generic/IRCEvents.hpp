/*-----------------------------------------------------------------------
 *  IRCEvents.hpp --
 *
 *  Templated class for storing script specific bytecode bits for
 *  IRC events.
 *
 *-----------------------------------------------------------------------
 * RCS: @(#) $Id: IRCEvents.hpp,v 1.16 2003/11/08 01:51:34 davygrvy Exp $
 *-----------------------------------------------------------------------
 */

#ifndef INC_IRCEvents_hpp__
#define INC_IRCEvents_hpp__

#include "cpptcl.hpp"


template <class T>
    class IRCEvents
{
public:
    struct EVENTLIST {
	int count;
	T bytecodes[1];  // variable length array, will grow.
    };

    typedef EVENTLIST* LPEVENTLIST;
    typedef void (*elemDeletionProc)(void *);

    IRCEvents(elemDeletionProc proc);
    ~IRCEvents();

    int Add(Tcl_Obj * const event, T const bytecode);
    int Add(const char *event, T const bytecode);
    int Find(Tcl_Obj * const event, LPEVENTLIST *pevlist);
    int Find(const char *event, LPEVENTLIST *pevlist);
    int Delete(Tcl_Obj * const event);
private:
    Tcl::Hash<LPEVENTLIST> hash;
    elemDeletionProc delproc;
};

template <class T>
    IRCEvents<T>::IRCEvents (elemDeletionProc proc) : delproc(proc) {}

template <class T>
    IRCEvents<T>::~IRCEvents ()
{
    LPEVENTLIST pevlist;
    int i, result;

    for (
	result = hash.Top(&pevlist);
	result != TCL_ERROR;
	result = hash.Next(&pevlist)
    ) {
	for (i = 0; i < pevlist->count; i++) {
	    delproc(pevlist->bytecodes[i]);
	}
	ckfree(reinterpret_cast<char *>(pevlist));
    }
}

#include <stdlib.h>
#include <string.h>

template <class T>
    int IRCEvents<T>::Find (Tcl_Obj * const event, LPEVENTLIST *ppevlist)
{
    int i,retVal,len;
    char *temp,*strkey;

    strkey = Tcl_GetStringFromObj(event, &len);
    temp = Tcl_Alloc(len + 1);
    for (i=0;i<len;i++) {
	temp[i] = tolower(strkey[i]);
    }
    temp[i] = '\0';

    retVal = hash.Find(temp, ppevlist);
    ckfree(temp);
    return retVal;
}

template <class T>
    int IRCEvents<T>::Delete (Tcl_Obj * const event)
{
    LPEVENTLIST pevlist = 0L;
    int i, result;

    result = hash.Extract(Tcl_GetString(event), pevlist);

    if (result == TCL_ERROR) {
	return TCL_ERROR;
    }

    for ( i = 0; i < pevlist->count; i++ ) {
	delproc(pevlist->bytecodes[i]);
    }
    ckfree( (char *) pevlist );
    return TCL_OK;
}

template <class T>
    int IRCEvents<T>::Add (Tcl_Obj * const event, T const bytecode)
{
    return Add(Tcl_GetString(event), bytecode);
}

template <class T>
    int IRCEvents<T>::Add (const char *event, T const bytecode)
{
    int i,len,newmemsize;
    bool firstentry = false;
    char *eventlstr;
    LPEVENTLIST pevlist = 0L;

    len = strlen(event);
    eventlstr = Tcl_Alloc(len + 1);
    for (i=0;i<len;i++) {
	eventlstr[i] = tolower(event[i]);
    }
    eventlstr[i] = '\0';


    if (hash.Find(eventlstr, &pevlist) == TCL_ERROR)
    {
	// make a new storage structure for our list of Proc pointers
	pevlist = (LPEVENTLIST) ckalloc(sizeof(EVENTLIST));
	pevlist->count = 0;
	firstentry = true;
    }
    newmemsize = sizeof(EVENTLIST) + (sizeof(T) * (pevlist->count + 1));
    pevlist = (LPEVENTLIST) Tcl_Realloc((char *) pevlist, newmemsize);
    pevlist->bytecodes[pevlist->count++] = bytecode;

    // the realloc'ing may have changed the memory location.
    // So we have to delete it and re-add it to the hash table.
    //
    if (!firstentry) hash.Delete(eventlstr);

    hash.Add(eventlstr, pevlist);
    Tcl_Free(eventlstr);

    return TCL_OK;
}


#endif // INC_IRCEvents_hpp__
