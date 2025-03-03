/*
 ------------------------------------------------------------------------------
 * IRCTextLine.hpp --
 *
 *   Virtual base class for what a line of text represents to the ViewBox
 *
 * Copyright (c) 1999-2000 Tomahawk Software Group
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: IRCTextLine.hpp,v 1.1 2003/11/14 10:36:20 davygrvy Exp $
 ------------------------------------------------------------------------------
 */
#ifndef INC_IRCTextLine_hpp__
#define INC_IRCTextLine_hpp__

#ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#  undef WIN32_LEAN_AND_MEAN
  typedef HDC DRAWCONTEXT;
#else
#  error "OS not defined yet"
#endif

#include <string>

class IRCTextLine {
protected:
  virtual ~IRCTextLine() = 0;
  virtual void Draw(DRAWCONTEXT dc) = 0;
};

#endif  // #ifndef INC_IRCTextLine_hpp__

