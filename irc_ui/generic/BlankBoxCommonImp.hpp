/*
 ------------------------------------------------------------------------------
 * BlankBoxCommonImp.hpp --
 *
 *   Base class for the internal implimentation without any platform
 *   specifics.
 *
 * Copyright (c) 1999-2000 Tomahawk Software Group
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: BlankBoxCommonImp.hpp,v 1.1 2003/11/16 02:04:20 davygrvy Exp $
 ------------------------------------------------------------------------------
 */
#ifndef INC_BlankBoxCommonImp_hpp__
#define INC_BlankBoxCommonImp_hpp__

#include <IRCWindows.hpp>

class BlankBoxCommonImp : public BlankBoxImp {
public:
    virtual void *GetWindowId() = 0;
};

#endif //#ifndef INC_BlankBoxCommonImp_hpp__
