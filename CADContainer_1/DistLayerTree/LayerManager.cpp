#include "StdAfx.h"
#include "resource.h"
#include "LayerManager.h"

//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC (CLayerManager, CAcUiDockControlBar)

BEGIN_MESSAGE_MAP(CLayerManager, CAcUiDockControlBar)
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
CLayerManager::CLayerManager () : CAcUiDockControlBar(), mpChildDlg(NULL) {
}

//-----------------------------------------------------------------------------
CLayerManager::~CLayerManager () {
	//----- If the child dialog is still valid
	if ( mpChildDlg != NULL )
	{
		delete mpChildDlg ;
	}
}

//-----------------------------------------------------------------------------
#ifdef _DEBUG
//- Please uncomment the 2 following lines to avoid linker error when compiling
//- in release mode. But make sure to uncomment these lines only once per project
//- if you derive multiple times from CAdUiDockControlBar/CAcUiDockControlBar
//- classes.

//void CAdUiDockControlBar::AssertValid () const {
//}
#endif

//-----------------------------------------------------------------------------
BOOL CLayerManager::Create (CWnd *pParent, LPCTSTR lpszTitle) {
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

	CLayerManager::EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);// | CBRS_ALIGN_TOP
	CLayerManager::DockControlBar(CBRS_ALIGN_LEFT,CRect(0, 0, 200, 150));
	CLayerManager::RestoreControlBar();

	// TODO: Add your code here
	
	return (TRUE) ;
}

//-----------------------------------------------------------------------------
//----- This member function is called when an application requests the window be 
//----- created by calling the Create or CreateEx member function
int CLayerManager::OnCreate (LPCREATESTRUCT lpCreateStruct) {
	if ( CAcUiDockControlBar::OnCreate (lpCreateStruct) == -1 )
		return (-1) ;

	//----- Point to our resource
	CAcModuleResourceOverride resourceOverride; 	
	//----- Now create a new dialog with our stuff in it
	mpChildDlg =new CDlgToolTree ;
	//----- Create it and set the parent as the dockctrl bar
	mpChildDlg->Create (IDD_DLG_TOOLTREE, this) ;

	mpChildDlg->ShowWindow(SW_NORMAL);
	//----- Move the window over so we can see the control lines
	mpChildDlg->MoveWindow (0, 0, 100, 100, TRUE) ;
	return (0) ;
}

//-----------------------------------------------------------------------------
void CLayerManager::SizeChanged (CRect *lpRect, BOOL bFloating, int flags) {
	//----- If the child dialog is valid
	if ( mpChildDlg != NULL ) {
		//----- Always point to our resource to be safe
		CAcModuleResourceOverride resourceOverride ;
		//----- Then update its window size relatively
		mpChildDlg->SizeChanged(lpRect,bFloating,flags);
		//mpChildDlg->SetWindowPos (this, lpRect->left + 4, lpRect->top + 4, lpRect->Width (), lpRect->Height (), SWP_NOZORDER) ;
	}
}

//-----------------------------------------------------------------------------
//-----  Function called when user selects a command from Control menu or when user 
//----- selects the Maximize or the Minimize button.
void CLayerManager::OnSysCommand (UINT nID, LPARAM lParam) {
	CAcUiDockControlBar::OnSysCommand (nID, lParam) ;
}

//-----------------------------------------------------------------------------
//----- The framework calls this member function after the window's size has changed
void CLayerManager::OnSize (UINT nType, int cx, int cy) {
	CAcUiDockControlBar::OnSize (nType, cx, cy) ;
	//----- If the child dialog is valid
	if ( mpChildDlg != NULL ) {
		//----- Always point to our resource to be safe
		CAcModuleResourceOverride resourceOverride ;
		//----- then update its window position relatively
		mpChildDlg->MoveWindow (0, 0, cx, cy) ;
	}
}

BOOL CLayerManager::ShowDockPane(BOOL bShow /*= TRUE*/)
{
	CMDIFrameWnd *pAcadFrame = acedGetAcadFrame();

	if (pAcadFrame)
	{
		pAcadFrame->ShowControlBar(this, bShow, FALSE);
		return TRUE;
	}

	return FALSE;
}



void CLayerManager::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值


	mpChildDlg->SendMessage(WM_CLOSE);
	CAcUiDockControlBar::OnClose();
}
