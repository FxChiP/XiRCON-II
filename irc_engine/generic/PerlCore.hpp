/*-----------------------------------------------------------------------
 *  PerlCore.hpp --
 *
 *  .
 *
 *-----------------------------------------------------------------------
 * RCS: @(#) $Id: PerlCore.hpp,v 1.5 2003/01/07 22:56:00 davygrvy Exp $
 *-----------------------------------------------------------------------
 */

#ifndef INC_PerlCore_hpp__
#define INC_PerlCore_hpp__

//#define PERL_OBJECT
//#define PERL_IMPLICIT_CONTEXT
#include "EXTERN.h"
#include "perl.h"

namespace Perl {

    class Std
    {
    public:
	typedef void (*WriteFunc) (const char *);
	Std (WriteFunc _stdOut, WriteFunc _stdErr);
	~Std ();
    protected:
	void Init();
    private:
	WriteFunc stdOut;
	WriteFunc stdErr;
	static int initialized;
    };

    class Interp : protected Std
    {
	PerlInterpreter *my_perl;
	const char perl_definitions[];

    public:
	Interp(Std::WriteFunc stdOut, Std::WriteFunc stdErr);
	~Interp();
	//int Load (const char *scriptFileName, char **errorString);
	int Eval ();
    };
}

#endif	// INC_PerlCore_hpp__
