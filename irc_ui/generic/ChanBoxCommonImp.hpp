/*
 ------------------------------------------------------------------------------
 * ChanBoxCommonImp.hpp --
 *
 *   Base class for the internal implimentation without any platform
 *   specifics.
 *
 * Copyright (c) 1999-2000 Tomahawk Software Group
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: ChanBoxCommonImp.hpp,v 1.1 2003/11/14 10:36:20 davygrvy Exp $
 ------------------------------------------------------------------------------
 */
#ifndef INC_ChanBoxCommonImp_hpp__
#define INC_ChanBoxCommonImp_hpp__

#include <IRCWindows.hpp>

class ChanBoxCommonImp : public ChanBoxImp {
public:
  virtual void Echo(wchar_t *text, size_t len);
  virtual void SetTitle(wchar_t *text, size_t len);
};

#endif //#ifndef INC_ChanBoxCommonImp_hpp__
