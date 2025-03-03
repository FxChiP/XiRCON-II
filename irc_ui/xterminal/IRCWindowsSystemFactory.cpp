/*
 ------------------------------------------------------------------------------
 * IRCWindowsSystemFactory.cpp --
 *
 *   Abstract factory class defined as a singleton to create 
 *   implimentation windowing classes at run-time for the UI.
 *
 *   This code is the Xterminal flavor.
 *   
 * Copyright (c) 2002 Tomasoft Engineering
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

IRCWindowsSystemFactory::IRCWindowsSystemFactory() {
}

IRCWindowsSystemFactory *IRCWindowsSystemFactory::m_instance = NULL;

IRCWindowsSystemFactory *IRCWindowsSystemFactory::Instance() {
    if (m_instance == 0L) {
	m_instance = new IRCWindowsSystemFactory();
    }
    return m_instance;
}

StatBoxImp *IRCWindowsSystemFactory::MakeStatBoxImp () {
    return new XterminalStatBoxImp();
}

ChanBoxImp *IRCWindowsSystemFactory::MakeChanBoxImp () {
    return 0L; //new XterminalChanBoxImp();
}

QueryBoxImp *IRCWindowsSystemFactory::MakeQueryBoxImp () {
    return 0L; //new XterminalQueryBoxImp();
}

DCCChatBoxImp *IRCWindowsSystemFactory::MakeDCCChatBoxImp () {
    return 0L; //new XterminalDCCChatBoxImp();
}

