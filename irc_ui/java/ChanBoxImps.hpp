/*
 ------------------------------------------------------------------------------
 * ChanBoxImps.hpp --
 *
 *   Platform specific implimentation class of the component
 *
 * Copyright (c) 1999-2000 Tomahawk Software Group
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: ChanBoxImps.hpp,v 1.1 2003/11/14 10:57:46 davygrvy Exp $
 ------------------------------------------------------------------------------
 */
#ifndef INC_ChanBoxImps_hpp__
#define INC_ChanBoxImps_hpp__

#include <ChanBoxCommonImp.hpp>

class AllWin32ChanBoxImp : public ChanBoxCommonImp {
public:
  void Echo(char *text, int len);
  void SetTitle(char *text, int len);
};

#endif  // #ifndef INC_ChanBoxImps_hpp__

