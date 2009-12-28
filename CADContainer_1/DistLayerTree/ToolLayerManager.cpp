// (C) Copyright 2002-2003 by Autodesk, Inc. 
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted, 
// provided that the above copyright notice appears in all copies and 
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting 
// documentation.
//
#include "StdAfx.h"
#include "resource.h"
#include "ToolLayerManager.h"

//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC (CToolLayerManager, CAcUiDockControlBar)

BEGIN_MESSAGE_MAP(CToolLayerManager, CAcUiDockControlBar)
	ON_WM_CREATE()
	ON_WM_SYSCOMMAND()
	ON_WM_SIZE()
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
//----- CLayerManager *pInstance = new CLayerManager;
//----- pInstance->Create (acedGetAcadFrame (), "My title bar") ;
//----- pInstance->EnableDocking (CBRS_ALIGN_ANY) ;
//----- pInstance->RestoreControlBar () ;

//-----------------------------------------------------------------------------
static CLSID clsCLayerManager = {0xeab78c04, 0x2194, 0x47ad, {0xa4, 0xf2, 0xad, 0xca, 0x3e, 0x3b, 0xb6, 0x3c}} ;

//-----------------------------------------------------------------------------
CToolLayerManager::CToolLayerManager () : CAcUiDockControlBar()
{
}

//-----------------------------------------------------------------------------
CToolLayerManager::~CToolLayerManager () 
{
}

//-----------------------------------------------------------------------------
#ifdef _DEBUG
//- Please uncomment the 2 following lines to avoid linker error when compiling
//- in release mode. But make sure to uncomment these lines only once per project
//- if you derive multiple times from CAdUiDockControlBar/CAcUiDockControlBar
//- classes.

void CAdUiDockControlBar::AssertValid () const {
}
#endif

//-----------------------------------------------------------------------------
BOOL CToolLayerManager::Create (CWnd *pParent, LPCTSTR lpszTitle) {
	CString strWndClass ;
	strWndClass =AfxRegisterWndClass (CS_DBLCLKS, LoadCursor (NULL, IDC_ARROW)) ;
	CRect rect (0, 0, 200, 150) ;
	if (!CAcUiDockControlBar::Create (
		strWndClass, lpszTitle, WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN,
		rect, pParent, 0
		)
		)
		return (FALSE) ;

	SetToolID (&clsCLayerManager) ;

	CToolLayerManager::EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);// | CBRS_ALIGN_TOP
	CToolLayerManager::DockControlBar(CBRS_ALIGN_LEFT,CRect(0, 0, 200, 150));
	CToolLayerManager::RestoreControlBar();

	
	return (TRUE) ;
}

//-----------------------------------------------------------------------------
//----- This member function is called when an application requests the window be 
//----- created by calling the Create or CreateEx member function
int CToolLayerManager::OnCreate (LPCREATESTRUCT lpCreateStruct) {
	if ( CAcUiDockControlBar::OnCreate (lpCreateStruct) == -1 )
		return (-1) ;

	//----- Point to our resource
	CAcModuleResourceOverride resourceOverride; 	
	//----- Now create a new dialog with our stuff in it
	//mpChildDlg =new CLayerManagerChildDlg ;
	//----- Create it and set the parent as the dockctrl bar
	//mpChildDlg->Create (IDD_LAYERMANAGER, this) ;
	//----- Move the window over so we can see the control lines
	//mpChildDlg->MoveWindow (0, 0, 100, 100, TRUE) ;
	return (0) ;
}

//-----------------------------------------------------------------------------
void CToolLayerManager::SizeChanged (CRect *lpRect, BOOL bFloating, int flags) {
	//----- If the child dialog is valid
	//if ( mpChildDlg != NULL ) {
		//----- Always point to our resource to be safe
		CAcModuleResourceOverride resourceOverride ;
		//----- Then update its window size relatively
		//mpChildDlg->SizeChanged(lpRect,bFloating,flags);
		//mpChildDlg->SetWindowPos (this, lpRect->left + 4, lpRect->top + 4, lpRect->Width (), lpRect->Height (), SWP_NOZORDER) ;
	//}
}

//-----------------------------------------------------------------------------
//-----  Function called when user selects a command from Control menu or when user 
//----- selects the Maximize or the Minimize button.
void CToolLayerManager::OnSysCommand (UINT nID, LPARAM lParam) 
{
	CAcUiDockControlBar::OnSysCommand (nID, lParam) ;
}

//-----------------------------------------------------------------------------
//----- The framework calls this member function after the window's size has changed
void CToolLayerManager::OnSize (UINT nType, int cx, int cy) 
{
	CAcUiDockControlBar::OnSize (nType, cx, cy) ;
	//----- If the child dialog is valid
	/*
	if ( mpChildDlg != NULL ) {
		//----- Always point to our resource to be safe
		CAcModuleResourceOverride resourceOverride ;
		//----- then update its window position relatively
		mpChildDlg->MoveWindow (0, 0, cx, cy) ;
	}
	*/
}

BOOL CToolLayerManager::ShowDockPane(BOOL bShow /*= TRUE*/)
{
	/*
	CMDIFrameWnd *pAcadFrame = acedGetAcadFrame();

	if (pAcadFrame)
	{
		pAcadFrame->ShowControlBar(this, bShow, FALSE);
		return TRUE;
	}*/

	return FALSE;
}

void CToolLayerManager::GetSdeLayerName()
{
	/*
	m_SdeLayer.RemoveAll();
	if (mpChildDlg != NULL)
	{
		mpChildDlg->GetCurSdeLayerName();
		m_SdeLayer.Append(mpChildDlg->m_CurSelSdeLayer);
	}
	*/
}

void CToolLayerManager::RedrawTreeWindow()
{
	/*
	if (mpChildDlg != NULL)
	{
		mpChildDlg->InitializeTreeControl();
	}
	*/
}
