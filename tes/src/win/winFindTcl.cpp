/* 
 * winFindTcl.cpp --
 *
 *	Discovers the installed path to a certain tclXX(d).dll
 *
 * AUTHOR:	David Gravereaux <davygrvy@pobox.com>
 *
 * Copyright (c) 2000 by David Gravereaux and relinquished
 *   to Scriptics Corporation.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: winFindTcl.cpp,v 1.4 2003/12/23 21:38:02 davygrvy Exp $
 */


#ifdef _MSC_VER
    // I don't care to know about name truncation in the debug data
#   pragma warning(disable:4786)
    // mark this .obj as needing advapi32.lib and user32.lib at link-time
#   pragma comment(lib,"advapi32.lib")
#   pragma comment(lib,"user32.lib")
#endif

#include <string>
#include <deque>
#include <algorithm>
#include <windows.h>


// locally shared variables restricted to this module
static const char *_minVer;
static int _exact;
static int _dbgOnly;

// local protos
static void buildTail (char *, const char * = _minVer);
static void buildWildTail (char *);
static bool verRemove (std::string &);
static bool verLess (std::string &, std::string &);
static bool fileRemove (std::string &);
static bool fileLess (std::string &, std::string &);


/*
 *-------------------------------------------------------------------------
 *  findTcl --
 *
 *	Discovers the location of a Tcl core through a combination of current
 *	path followed by a registry search.  Called in a similar manner to
 *	Tcl_PkgRequire().
 *
 *	minVer  -- minimum acceptable version, ex: "8.1".
 *	exact   -- when not zero, indicates that minVer is the only 
 *		   acceptable core wanted.
 *	dbgOnly -- only load the symbol core when not zero.
 *
 *  returns:
 *	a char * that gives the location of the proper tclXX.dll requested.
 *	This char * does not need to be freed by the caller.
 *
 *  bad stuff:
 *	might over-ride development environment and use the installed one
 *	instead.  might have to 'install' tclXXd.dll to debug this extension.
 *
 *-------------------------------------------------------------------------
 */

char *
findTcl (const char *minVer, int exact, int dbgOnly)
{
    static char tclLibFullPath[MAX_PATH];
    char *savedPos;
    char *pos1, *pos2;
    char *prefix = "\\bin\\tcl";
    WIN32_FIND_DATAA hFFData;
    HANDLE hFind;
    int i = 0;
    std::deque <std::string> vers;
    std::deque <std::string>::iterator versNewLast;


    _minVer = minVer;
    _exact = exact;
    _dbgOnly = dbgOnly;


    // Look in the current path on the assumption if it's there, you want to
    // use it instead of any others if it passes the version test.


    // Append the beginning
    pos1 = tclLibFullPath;
    pos2 = ".\\tcl";
    while (*pos2 != '\0')
	*pos1++ = *pos2++;

    savedPos = pos1;

    if (exact != 0)
    {
	// build the end of the filename from the parameters given
	buildTail (savedPos);

	// See if it *really* exists
	hFind = ::FindFirstFileA (tclLibFullPath, &hFFData);

	if (hFind != INVALID_HANDLE_VALUE)
	{
	    // gotcha.
	    ::FindClose (hFind);
	    return tclLibFullPath;
	}

	// no success..  fall through for a registry search
    }
    else
    {
	// Try a range of filenames
	buildWildTail (savedPos);

	// See if we got some
	hFind = ::FindFirstFileA (tclLibFullPath, &hFFData);

	if (hFind != INVALID_HANDLE_VALUE)
	{
	    BOOL rtn;

	    do
	    {
		vers.push_back (std::string (hFFData.cFileName));
		rtn = ::FindNextFileA (hFind, &hFFData);
	    }
	    while (rtn != 0);         // got more?

	    ::FindClose (hFind);

	    // sort filename strings
	    std::sort (vers.begin (), vers.end (), fileLess);

	    // remove all invalid entries based on flags.
	    versNewLast =
		std::remove_if (vers.begin (), vers.end (), fileRemove);

	    if (vers.begin () != versNewLast)
	    {
		// using copy itself doesn't terminate the string...  how odd..
		tclLibFullPath[(versNewLast - 1)->
			copy (tclLibFullPath, MAX_PATH)] = '\0';
		return tclLibFullPath;
	    }
	}
	// no success..  fall through for a registry search
    }


    // Do a registry search.
    HKEY topTclKey, subTclVer;
    DWORD max_path = MAX_PATH;
    LONG result;
    char sSubKey[MAX_PATH];
    const char *verTarget;

    result = RegOpenKeyExA (HKEY_LOCAL_MACHINE, "SOFTWARE\\Scriptics\\Tcl", 0,
			   KEY_ENUMERATE_SUB_KEYS, &topTclKey);

    if (result != ERROR_SUCCESS)
    {
	std::string err ("This application requires Tcl");
	err.append (minVer);
	if (exact != 0)
	{
	    err.append (".\n\n");
	}
	else
	{
	    err.append (" or greater.\n\n");
	}
	err.append ("Please download it from ftp://ftp.scriptics.com/pub/tcl/.  ");
	err.append ("Or see http://dev.scriptics.com/ for details.  ");

	throw err;
    }

    // Read all SubKey names
enumAgain:
    result = ::RegEnumKeyA (topTclKey, i, sSubKey, MAX_PATH);
    if (result != ERROR_NO_MORE_ITEMS)
    {
	vers.push_back (std::string (sSubKey));
	i++;
	goto enumAgain;
    }
    else if (i == 0)
    {
	std::string err
	    ("The registry path (HKEY_LOCAL_MACHINE\\SOFTWARE\\Scriptics\\Tcl) has no SubKeys.  ");
	err.append
	    ("Reinstall Tcl to fix this or hand edit your registry.");

	throw err;
    }

    // sort version strings
    std::sort (vers.begin (), vers.end (), verLess);

    // remove all invalid entries based on flags.
    versNewLast = std::remove_if (vers.begin (), vers.end (), verRemove);

    if (vers.begin () != versNewLast)
	 verTarget = (versNewLast - 1)->c_str ();
    else
    {
	std::string err
	    ("Unable to locate a proper Tcl core in the registry for version ");
	err.append (minVer);
	if (exact != 0)
	    err.append (".");
	else
	    err.append (" or greater.");

	throw err;
    }

    ::RegOpenKeyExA (topTclKey, verTarget, 0, KEY_READ, &subTclVer);

    result = ::RegQueryValueExA (subTclVer, "Root", NULL, NULL,
                              reinterpret_cast <PUCHAR> (tclLibFullPath),
                              &max_path);

    if (result != ERROR_SUCCESS)
    {
	std::string err
	    ("The registry path (HKEY_LOCAL_MACHINE\\SOFTWARE\\Scriptics\\Tcl\\");
	err.append (verTarget);
	err.append (") did not have a \"Root\" SubKey.");

	throw err;
    }

    pos1 = tclLibFullPath;
    while (*pos1 != '\0')
	pos1++;

    // Append the bin directory and prefix
    pos2 = prefix;
    while (*pos2 != '\0')
	*pos1++ = *pos2++;
    savedPos = pos1;

    // Build the end of the filename
    buildTail (savedPos, verTarget);

    // See if it *really* exists
    hFind = ::FindFirstFileA (tclLibFullPath, &hFFData);

    if (hFind != INVALID_HANDLE_VALUE)
    {
	// gotcha.
	::FindClose (hFind);
	return tclLibFullPath;
    }
    else
    {
	::FindClose (hFind);
	std::string err
	    ("The registry key (HKEY_LOCAL_MACHINE\\SOFTWARE\\Scriptics\\Tcl\\");
	err.append (minVer);
	err.append ("\\Root) said a core was located at (");
	err.append (tclLibFullPath);
	err.append (").  This was found to be false.");

	throw err;
    }

    throw std::string("A proper Tcl core could not be found anywhere.");
}

static void
buildTail (char *pos, const char *ver)
{
    const char *pos2;
    char *shLibExt = ".dll";

    // append version w/o decimal
    pos2 = ver;
    while (*pos2 != '\0')
    {
	if (*pos2 != '.')
	    *pos++ = *pos2++;
	else
	    pos2++;
    }

    // if asked, add the debug suffix
    if (_dbgOnly != 0)
	*pos++ = 'g';

    // append the extension
    pos2 = shLibExt;
    while (*pos2 != '\0')
	*pos++ = *pos2++;

    // terminate it
    *pos = '\0';
}


static void
buildWildTail (char *pos)
{
    char *pos2;
    char *shLibExt = ".dll";

    // just append the major version
    *pos++ = *_minVer;
    *pos++ = '?';

    // if asked, add the debug suffix
    if (_dbgOnly != 0)
	*pos++ = 'd';

    // append the extension
    pos2 = shLibExt;
    while (*pos2 != '\0')
	*pos++ = *pos2++;

    // terminate it
    *pos = '\0';
}

static bool
verRemove (std::string & s)
{
    double fa, fb;
    fa = atof (s.c_str ());
    fb = atof (_minVer);

    if (_exact == 0)
    {
	// remove when major version is different.
	if ((int) fa != (int) fb)
	    return true;
	// remove when less than minor version
	return (fa - (int) fa < fb - (int) fb);
    }
    else
    {
	// remove when not identical for an exact match
	return (fa != fb);
    }
}

static bool
verLess (std::string & a, std::string & b)
{
    double fa, fb;
    fa = atof (a.c_str ());
    fb = atof (b.c_str ());
    return (fa < fb);
}

static bool
fileRemove (std::string & f)
{
    // strip off 'tcl' and '.dll'
    return verRemove (f.substr (3, 1) + "." + f.substr (4, 1));
}

static bool
fileLess (std::string & a, std::string & b)
{
    return verLess (a.substr (3, 1) + "." + a.substr (4, 1),
		    b.substr (3, 1) + "." + b.substr (4, 1));
}
