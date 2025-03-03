/*
 ------------------------------------------------------------------------------
 * CmdBoxCommonImp.hpp --
 *
 *   Base class for the internal implimentation without any platform
 *   specifics.
 *
 * Copyright (c) 1999-2000 Tomahawk Software Group
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: CmdBoxCommonImp.hpp,v 1.1 2003/11/14 10:36:20 davygrvy Exp $
 ------------------------------------------------------------------------------
 */
#ifndef INC_CmdBoxCommonImp_hpp__
#define INC_CmdBoxCommonImp_hpp__

#ifdef _MSC_VER
// Identifier was truncated to '255' characters...
// Like, who cares?
#pragma warning(disable:4786)
#endif

#include <string>
#include <vector>

class CmdBoxCommonImp {
public:
  CmdBoxCommonImp();
  ~CmdBoxCommonImp();
protected:
  enum direction {up, down};
  virtual void AddHist(const std::u16string *line);
  virtual const std::u16string *GetHist(direction d);
  std::u16string *sHistoryTemp;
private:
  std::vector<std::u16string *> vHistory;
  size_t iHistPos;
  const size_t iMaxHistory;
  std::u16string *empty;
};

#endif //#ifndef INC_CmdBoxCommonImp_hpp__
