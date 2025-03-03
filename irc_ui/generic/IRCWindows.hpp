/*
 ------------------------------------------------------------------------------
 * IRCWindows.hpp --
 *
 *   Public header for the platform independent client interface for all 
 *   the UI window types.
 *
 * Copyright (c) 1999-2003 Tomasoft Engineering.
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: IRCWindows.hpp,v 1.2 2003/11/16 02:04:20 davygrvy Exp $
 ------------------------------------------------------------------------------
 */

#ifndef INC_IRCWindows_hpp__
#define INC_IRCWindows_hpp__

// for char16_t and size_t
#include <stddef.h>


// Underlying implimentation base of StatBox.
//
class StatBoxImp {
public:
  virtual ~StatBoxImp() {};
  virtual void Echo(char16_t *text, size_t len) = 0;
  virtual void SetTitle(char16_t *text, size_t len) = 0;
};


// StatBox client abstraction interface.
//
class StatBox {
public:
  StatBox();
  ~StatBox();
  virtual void Echo(char16_t *text, size_t len);
  virtual void SetTitle(char16_t *text, size_t len);
private:
  StatBoxImp *m_imp;
};


// Underlying implimentation base of ChanBox.
//
class ChanBoxImp {
public:
  virtual ~ChanBoxImp() {};
  virtual void Echo(char16_t *text, size_t len) = 0;
  virtual void SetTitle(char16_t *text, size_t len) = 0;
};


// ChanBox client abstraction interface.
//
class ChanBox {
public:
  ChanBox();
  ~ChanBox();
  virtual void Echo(char16_t *text, size_t len);
  virtual void SetTitle(char16_t *text, size_t len);
private:
  ChanBoxImp *m_imp;
};


// Underlying implimentation base of QueryBox.
//
class QueryBoxImp {
public:
  virtual ~QueryBoxImp() {};
  virtual void Echo(char16_t *text, size_t len) = 0;
  virtual void SetTitle(char16_t *text, size_t len) = 0;
};


// QueryBox client abstraction interface.
//
class QueryBox {
public:
  QueryBox();
  ~QueryBox();
  virtual void Echo(char16_t *text, size_t len);
  virtual void SetTitle(char16_t *text, size_t len);
private:
  QueryBoxImp *m_imp;
};


// Underlying implimentation base of DCCChatBox.
//
class DCCChatBoxImp {
public:
  virtual ~DCCChatBoxImp() {};
  virtual void Echo(char16_t *text, size_t len) = 0;
  virtual void SetTitle(char16_t *text, size_t len) = 0;
};


// DCCChatBox client abstraction interface.
//
class DCCChatBox {
public:
  DCCChatBox();
  ~DCCChatBox();
  virtual void Echo(char16_t *text, size_t len);
  virtual void SetTitle(char16_t *text, size_t len);
private:
  DCCChatBoxImp *m_imp;
};

// Underlying implimentation base of BlankBox.
// This is used for Tk.  See [toplevel -uses ..] in the docs.
//
class BlankBoxImp {
public:
  virtual ~BlankBoxImp() {};
  virtual void *GetWindowId() = 0;
};


// BlankBox client abstraction interface.
//
class BlankBox {
public:
  BlankBox();
  ~BlankBox();
  virtual void *GetWindowId();
private:
  BlankBoxImp *m_imp;
};

#endif  // #ifndef INC_IRCWindows_hpp__

