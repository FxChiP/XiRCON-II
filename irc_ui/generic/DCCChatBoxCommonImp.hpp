/*
 ------------------------------------------------------------------------------
 * DCCChatBoxCommonImp.hpp --
 *
 *   Base class for the internal implimentation without any platform
 *   specifics.
 *
 * Copyright (c) 1999-2000 Tomahawk Software Group
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: DCCChatBoxCommonImp.hpp,v 1.1 2003/11/14 10:36:20 davygrvy Exp $
 ------------------------------------------------------------------------------
 */
#ifndef INC_DCCChatBoxCommonImp_hpp__
#define INC_DCCChatBoxCommonImp_hpp__

#include <IRCWindows.hpp>

class DCCChatBoxCommonImp : public DCCChatBoxImp {
public:
  virtual void Echo(wchar_t *text, int len);
  virtual void SetTitle(wchar_t *text, int len);
};

#endif //#ifndef INC_QueryBoxCommonImp_hpp__

