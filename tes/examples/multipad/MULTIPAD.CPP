// multipad.cpp : Defines the class behaviors for the Multipad application.
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1997 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"
#include "multipad.h"
#include <locale.h>

//---- Start Tcl code ----
#include "../../include/TclEventSystem.hpp"
#pragma comment (lib, "../../lib/tes.lib")
#define USE_TCL_STUBS
#include <tcl.h>

// small hack to avoid the unicode stuff in MCL.
#define LPCTSTR char const *
#include "../../Mcl/include/Cmcl.h"
#pragma comment (lib, "../../Mcl/lib/mcl.lib")

const TclEventSystem *TES;
Tcl_Interp *globalInterp;

class TclUp : public TclAsyncJob
{
public:
    TclUp ()
    {
	TclEventSystem::Instance()->QueueJob(this);
    }
    ~TclUp() {}
    bool AsyncProc() {return false;}
    void EventProc()
    {
	// This is running in Tcl's thread.
	// NOT the main application thread.
	//Tcl_SetPanicProc(MyPanic);
	globalInterp = Tcl_CreateInterp();
	if (Tcl_Init(globalInterp) != TCL_OK) {
	    MessageBeep(MB_ICONSTOP);
	    MessageBoxA(NULL, globalInterp->result, "Tcl_Init() failed",
		    MB_ICONSTOP | MB_OK | MB_TASKMODAL | MB_SETFOREGROUND);
	    return;
	}

	// Source some stuff.
        if (Tcl_EvalFile(globalInterp, "multipad.tcl") != TCL_OK) {
	    Tcl_GetStringResult(globalInterp);
	    MessageBeep(MB_ICONSTOP);
	    MessageBoxA(NULL, globalInterp->result, "Tcl_EvalFile() failed",
		    MB_ICONSTOP | MB_OK | MB_TASKMODAL | MB_SETFOREGROUND);
	}
    }
};


class TclDown : public TclAsyncJob
{
    CMclEvent &_isDown;
public:
    TclDown (CMclEvent &isDown) : _isDown(isDown)
    {
	TclEventSystem::Instance()->QueueJob(this);
    }
    ~TclDown() {}
    bool AsyncProc() {return false;}
    void EventProc()
    {
	Tcl_DeleteInterp(globalInterp);
	globalInterp = 0L;
	_isDown.Set();
    }
};
//----- End Tcl code -----

CMultiPadApp NEAR theApp;

BEGIN_MESSAGE_MAP(CMultiPadApp, CWinApp)
	//{{AFX_MSG_MAP(CMultiPadApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)     // file commands...
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

BOOL CMultiPadApp::InitInstance()
{
	_tsetlocale(LC_ALL, _T(""));

	Enable3dControls();
	LoadStdProfileSettings();

	AddDocTemplate(new CMultiDocTemplate(IDR_TEXTTYPE,
		RUNTIME_CLASS(CPadDoc), RUNTIME_CLASS(CMDIChildWnd),
		RUNTIME_CLASS(CEditView)));
	m_pMainWnd = new CMainFrame;
	((CFrameWnd*)m_pMainWnd)->LoadFrame(IDR_MAINFRAME);
	m_pMainWnd->ShowWindow(m_nCmdShow);

	// enable file manager drag/drop and DDE Execute open
	m_pMainWnd->DragAcceptFiles();
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	//---- Start Tcl code ----
	TES = TclEventSystem::Instance("8.1" /*tcl version*/, 0 /*exact*/);
	new TclUp;
	//----- End Tcl code -----

	return TRUE;
}

void CMultiPadApp::OnAppAbout()
{
	CDialog(IDD_ABOUTBOX).DoModal();
}

IMPLEMENT_DYNCREATE(CMainFrame, CMDIFrameWnd)
BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT buttons[] =
{
	ID_FILE_NEW, ID_FILE_OPEN, ID_FILE_SAVE, ID_SEPARATOR,
	ID_EDIT_CUT, ID_EDIT_COPY, ID_EDIT_PASTE, ID_SEPARATOR,
	ID_FILE_PRINT, ID_APP_ABOUT
};

static UINT indicators[] =
{
	ID_SEPARATOR, ID_INDICATOR_CAPS, ID_INDICATOR_NUM, ID_INDICATOR_SCRL
};

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	((m_ToolBar.Create(this) &&
		m_ToolBar.LoadBitmap(IDR_MAINFRAME) &&
		m_ToolBar.SetButtons(buttons, sizeof(buttons)/sizeof(UINT)) &&
		m_StatusBar.Create(this) &&
		m_StatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT)))
	  ? 0 : -1);
	return CMDIFrameWnd::OnCreate(lpCreateStruct);
}

IMPLEMENT_DYNCREATE(CPadDoc, CDocument)
BEGIN_MESSAGE_MAP(CPadDoc, CDocument)
	//{{AFX_MSG_MAP(CPadDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CPadDoc::Serialize(CArchive& ar)
{
	((CEditView*)m_viewList.GetHead())->SerializeRaw(ar);
}

/////////////////////////////////////////////////////////////////////////////

void CMainFrame::OnDestroy() 
{
	CMclEvent isDown;
	CMDIFrameWnd::OnDestroy();
	
	//---- Start Tcl code ----
	new TclDown(isDown);

	// block waiting for the job to finish.
	isDown.Wait(INFINITE);
	TES->ShutDown();
	//----- End Tcl code -----
}
