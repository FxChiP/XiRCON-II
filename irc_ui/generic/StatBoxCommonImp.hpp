/*
 ------------------------------------------------------------------------------
 * StatBoxCommonImp.hpp --
 *
 *   Base class for the internal implimentation without any platform
 *   specifics.
 *
 * Copyright (c) 1999-2000 Tomahawk Software Group
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: StatBoxCommonImp.hpp,v 1.1 2003/11/14 10:36:20 davygrvy Exp $
 ------------------------------------------------------------------------------
 */
#ifndef INC_StatBoxCommonImp_hpp__
#define INC_StatBoxCommonImp_hpp__

#include <IRCWindows.hpp>

class StatBoxCommonImp : public StatBoxImp {
public:
  virtual ~StatBoxCommonImp() {};
  virtual void Echo(char16_t *text, size_t len) = 0;
  virtual void SetTitle(char16_t *text, size_t len) = 0;
};

#endif //#ifndef INC_StatBoxCommonImp_hpp__
