/*
 ------------------------------------------------------------------------------
 * QueryBoxImps.hpp --
 *
 *   Platform specific implimentation class of the component.
 *
 * Copyright (c) 1999-2000 Tomahawk Software Group
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: QueryBoxImps.hpp,v 1.1 2003/11/14 10:36:21 davygrvy Exp $
 ------------------------------------------------------------------------------
 */
#ifndef INC_QueryBoxImps_hpp__
#define INC_QueryBoxImps_hpp__

#include <QueryBoxCommonImp.hpp>

class AllWin32QueryBoxImp : public QueryBoxCommonImp {
public:
  void Echo(char *text, int len);
  void SetTitle(char *text, int len);
};

#endif  // #ifndef INC_QueryBoxImps_hpp__

