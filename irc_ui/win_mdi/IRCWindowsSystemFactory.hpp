/*
 ------------------------------------------------------------------------------
 * IRCWindowsSystemFactory.hpp --
 *
 *   Abstract factory class defined as a singleton to create 
 *   implimentation windowing classes at run-time for the UI.
 *
 *   This code is the Win32 GUI flavor.
 *
 * Copyright (c) 1999-2000 Tomasoft Engineering
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: IRCWindowsSystemFactory.hpp,v 1.2 2003/11/16 02:04:20 davygrvy Exp $
 ------------------------------------------------------------------------------
 */

#ifndef __INC_IRCWindowsSystemFactory__
#define __INC_IRCWindowsSystemFactory__

#include <IRCWindows.hpp>
#include <windows.h>

class IRCWindowsSystemFactory {
public:
  static IRCWindowsSystemFactory *Instance();
  StatBoxImp *MakeStatBoxImp();
  ChanBoxImp *MakeChanBoxImp();
  QueryBoxImp *MakeQueryBoxImp();
  DCCChatBoxImp *MakeDCCChatBoxImp();
  BlankBoxImp *MakeBlankBoxImp();
protected:
  IRCWindowsSystemFactory();
private:
  static IRCWindowsSystemFactory *m_instance;
  DWORD osplat;
};

#endif // #ifndef __INC_IRCWindowsSystemFactory__
