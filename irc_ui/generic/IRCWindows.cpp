/*
 ------------------------------------------------------------------------------
 * IRCWindows.cpp --
 *
 *   Platform independent client interface for all the UI window types.
 *
 * Copyright (c) 1999-2000 Tomahawk Software Group
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: IRCWindows.cpp,v 1.2 2003/11/16 02:04:20 davygrvy Exp $
 ------------------------------------------------------------------------------
 */
#include <IRCWindows.hpp>

#if defined(_WIN32)
#   if GUI_BUILD
#	include "../win_mdi/IRCWindowsSystemFactory.hpp"
#   elif TUI_BUILD
#	include "../win_tui/IRCWindowsSystemFactory.hpp"
#   else
#	error "Bad macro logic!"
#   endif
#elif defined(FINALCUT)
#   include "../unix_finalcut/IRCWindowsSystemFactory.hpp"
#elif defined(JNI_TRUE)  // jni.h present
#   include "../jni/IRCWindowsSystemFactory.hpp"
#else
#   error "windowing system not defined yet"
#endif

StatBox::StatBox() : m_imp(NULL) {
    m_imp = IRCWindowsSystemFactory::Instance()->MakeStatBoxImp();
}
StatBox::~StatBox() { delete m_imp; }
void StatBox::Echo(char16_t *text, size_t len) { m_imp->Echo(text, len); }
void StatBox::SetTitle(char16_t *text, size_t len) { m_imp->SetTitle(text, len); }


ChanBox::ChanBox() : m_imp(NULL) {
    m_imp = IRCWindowsSystemFactory::Instance()->MakeChanBoxImp();
}
ChanBox::~ChanBox() { delete m_imp; }
void ChanBox::Echo(char16_t *text, size_t len) { m_imp->Echo(text, len); }
void ChanBox::SetTitle(char16_t *text, size_t len) { m_imp->SetTitle(text, len); }


QueryBox::QueryBox() : m_imp(NULL) {
    m_imp = IRCWindowsSystemFactory::Instance()->MakeQueryBoxImp();
}
QueryBox::~QueryBox() { delete m_imp; }
void QueryBox::Echo(char16_t *text, size_t len) { m_imp->Echo(text, len); }
void QueryBox::SetTitle(char16_t *text, size_t len) { m_imp->SetTitle(text, len); }


DCCChatBox::DCCChatBox() : m_imp(NULL) {
    m_imp = IRCWindowsSystemFactory::Instance()->MakeDCCChatBoxImp();
}
DCCChatBox::~DCCChatBox() { delete m_imp; }
void DCCChatBox::Echo(char16_t *text, size_t len) { m_imp->Echo(text, len); }
void DCCChatBox::SetTitle(char16_t *text, size_t len) { m_imp->SetTitle(text, len); }

BlankBox::BlankBox() : m_imp(NULL) {
    m_imp = IRCWindowsSystemFactory::Instance()->MakeBlankBoxImp();
}
BlankBox::~BlankBox() { delete m_imp; }
void *BlankBox::GetWindowId(void) { return m_imp->GetWindowId(); }

