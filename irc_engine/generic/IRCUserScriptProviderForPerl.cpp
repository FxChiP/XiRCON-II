/*-----------------------------------------------------------------------
 *  IRCUserScriptProviderForPerl.cpp --
 *
 *  Makes the jump to Perl.  Both Tcl and Perl calls are used here.
 *
 *-----------------------------------------------------------------------
 * RCS: @(#) $Id: IRCUserScriptProviderForPerl.cpp,v 1.9 2004/12/12 19:34:03 davygrvy Exp $
 *-----------------------------------------------------------------------
 */

#include "IRCUserScriptProvider.hpp"

#if IRCE_USEPERL

#undef STRINGIFY
#undef JOIN
#include "PerlCore.hpp"

class IRCUserScriptProviderForPerlPrivateImp
    : public IRCUserScriptProviderPrvImpBase
{
    static void
    WriteStdOut (const char *text)
    {
	Tcl_Channel stdOut = Tcl_GetStdChannel(TCL_STDOUT);
	Tcl_Write(stdOut, text);
	Tcl_Flush(stdOut);
    }

    static void
    WriteStdErr (const char *text)
    {
	Tcl_Channel stdErr = Tcl_GetStdChannel(TCL_STDERR);
	Tcl_Write(stdErr, text);
	Tcl_Flush(stdErr);
    }

    Perl::Interp *perlPtr;
    int *status;

public:

    IRCUserScriptProviderForPerlPrivateImp(
	const char *scriptFileName,
	const char *instanceName,
	Tcl_Interp *globalInterp,
	int useSafe,
	int *_status)
	:
	status(_status)
    {
	//char *result;

	try {
    	    perlPtr = new Perl::Interp(WriteStdOut, WriteStdErr);
	}
	catch (char *err) {
	    Tcl_SetObjResult(globalInterp, Tcl_NewStringObj(err, -1));
	    *status =  TCL_ERROR;
	    return;
	}
    
	//while (objc--) perlPtr->???(Tcl_GetUnicode(objv[objc]));

	//if (perlPtr->Load(scriptFileName, &result) == 1) {
	//    Tcl_SetObjResult(globalInterp, Tcl_NewStringObj(result,-1));
	//    *status = TCL_ERROR;
	//}
    }

    ~IRCUserScriptProviderForPerlPrivateImp()
    {
	delete perlPtr;
    }

    IRCUserScriptProvider::CompletionStatus
    IncomingEvent(const IRCParse *line)
    {
	// TODO: finish me!
	return IRCUserScriptProvider::NOTCOMPLETED;
    }
};

IRCUserScriptProviderForPerl::IRCUserScriptProviderForPerl(
    const char *scriptFileName,
    const char *instanceName,
    Tcl_Interp *globalInterp,
    int useSafe)
    :
    IRCUserScriptProvider()
{
    _imp = new IRCUserScriptProviderForPerlPrivateImp(scriptFileName,
	    instanceName, globalInterp, useSafe, &status);
}

#endif
