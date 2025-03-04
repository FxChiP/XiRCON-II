/*
 ------------------------------------------------------------------------------
 * IRCWindowsSystemFactory.cpp --
 *
 *   Abstract factory class defined as a singleton to create 
 *   implimentation windowing classes at run-time for the UI.
 *
 *   This code is the Win32 flavor.
 *   
 * Copyright (c) 1999-2000 Tomahawk Software Group
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: IRCWindowsSystemFactory.cpp,v 1.1 2003/11/14 10:36:21 davygrvy Exp $
 ------------------------------------------------------------------------------
 */
#include "IRCWindowsSystemFactory.hpp"
#include "StatBoxImps.hpp"
#include "ChanBoxImps.hpp"
#include "QueryBoxImps.hpp"
#include "DCCChatBoxImps.hpp"

// Global App Instance
extern const HINSTANCE hAppInst = static_cast<HINSTANCE>(GetModuleHandle(NULL));

IRCWindowsSystemFactory::IRCWindowsSystemFactory() {
  OSVERSIONINFO ovi;

  ovi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  GetVersionEx(&ovi);

  // handle at run-time what implimentation we should use.
  // add more logic here, if needed.
  osplat = ovi.dwPlatformId;
};

IRCWindowsSystemFactory *IRCWindowsSystemFactory::m_instance = NULL;

IRCWindowsSystemFactory *IRCWindowsSystemFactory::Instance() {
  if (m_instance == NULL) {
    m_instance = new IRCWindowsSystemFactory();
  }
  return m_instance;
}

StatBoxImp *IRCWindowsSystemFactory::MakeStatBoxImp () {

  // change the logic here when we find platform differences.
  switch (osplat) {
  case VER_PLATFORM_WIN32s:
    throw "Win32s not supported";
  case VER_PLATFORM_WIN32_WINDOWS:
  case VER_PLATFORM_WIN32_NT:
    // adding a unicode one to use TextOutExW would be fun :)
  default:
    return new AllWin32StatBoxImp();
  }
};

ChanBoxImp *IRCWindowsSystemFactory::MakeChanBoxImp () {

  // change the logic here when we find platform differences.
  switch (osplat) {
  case VER_PLATFORM_WIN32s:
    throw "Win32s not supported";
  case VER_PLATFORM_WIN32_WINDOWS:
  case VER_PLATFORM_WIN32_NT:
    // adding a unicode one to use TextOutExW would be fun :)
  default:
    return NULL; //new AllWin32ChanBoxImp();
  }
};

QueryBoxImp *IRCWindowsSystemFactory::MakeQueryBoxImp () {

  // change the logic here when we find platform differences.
  switch (osplat) {
  case VER_PLATFORM_WIN32s:
    throw "Win32s not supported";
  case VER_PLATFORM_WIN32_WINDOWS:
  case VER_PLATFORM_WIN32_NT:
    // adding a unicode one to use TextOutExW would be fun :)
  default:
    return NULL; //new AllWin32QueryBoxImp();
  }
};

DCCChatBoxImp *IRCWindowsSystemFactory::MakeDCCChatBoxImp () {

  // change the logic here when we find platform differences.
  switch (osplat) {
  case VER_PLATFORM_WIN32s:
    throw "Win32s not supported";
  case VER_PLATFORM_WIN32_WINDOWS:
  case VER_PLATFORM_WIN32_NT:
    // adding a unicode one to use TextOutExW would be fun :)
  default:
    return NULL; //new AllWin32DCCChatBoxImp();
  }
};

