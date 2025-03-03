/*
 ------------------------------------------------------------------------------
 * IRCWindowsSystemFactory.hpp --
 *
 *   Abstract factory class defined as a singleton to create 
 *   implimentation windowing classes at run-time for the UI.
 *
 *   This code is the Xterminal flavor.
 *   
 * Copyright (c) 1999-2000 TomaSoft Engineering
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: IRCWindowsSystemFactory.hpp,v 1.1 2003/11/14 10:36:21 davygrvy Exp $
 ------------------------------------------------------------------------------
 */

#ifndef __INC_IRCWindowsSystemFactory__
#define __INC_IRCWindowsSystemFactory__

#include "IRCWindows.hpp"

class IRCWindowsSystemFactory {
public:
  static IRCWindowsSystemFactory *Instance();
  StatBoxImp *MakeStatBoxImp();
  ChanBoxImp *MakeChanBoxImp();
  QueryBoxImp *MakeQueryBoxImp();
  DCCChatBoxImp *MakeDCCChatBoxImp();
protected:
  IRCWindowsSystemFactory();
private:
  static IRCWindowsSystemFactory *m_instance;
};

#endif // #ifndef __INC_IRCWindowsSystemFactory__
