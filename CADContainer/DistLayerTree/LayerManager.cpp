#include "StdAfx.h"
#include "resource.h"
#include "LayerManager.h"

//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC (CLayerManager, CAcUiDockControlBar)

BEGIN_MESSAGE_MAP(CLayerManager, CAcUiDockControlBar)
	ON_WM_CREATE()
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
//----- CLayerManager *pInstance = new CLayerManager;
//----- pInstance->Create (acedGetAcadFrame (), "My title bar") ;
//----- pInstance->EnableDocking (CBRS_ALIGN_ANY) ;
//----- pInstance->RestoreControlBar () ;

//-----------------------------------------------------------------------------
static CLSID clsCLayerManager = {0xeab78c04, 0x2194, 0x47ad, {0xa4, 0xf2, 0xad, 0xca, 0x3e, 0x3b, 0xb6, 0x3c}} ;

//-----------------------------------------------------------------------------
CLayerManager::CLayerManager () : CAcUiDockControlBar(), m_pLayerTreeDlg(NULL) {
}

//-----------------------------------------------------------------------------
CLayerManager::~CLayerManager () {
	//----- If the child dialog is still valid
	if ( m_pLayerTreeDlg != NULL )
	{
		delete m_pLayerTreeDlg;
		m_pLayerTreeDlg = NULL;
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
BOOL CLayerManager::Create (CWnd *pParent, LPCTSTR lpszTitle, long nType) {
	CString strWndClass ;

	strWndClass =AfxRegisterWndClass (CS_DBLCLKS, LoadCursor (NULL, IDC_ARROW)) ;

	CRect rect (0, 0, 200, 150) ;

	if (!CAcUiDockControlBar::Create (
		strWndClass, lpszTitle, WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN,
		rect, pParent, 0))
	{
		return (FALSE) ;
	}

	SetToolID (&clsCLayerManager) ;

	CLayerManager::EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);// | CBRS_ALIGN_TOP
//	CLayerManager::DockControlBar(CBRS_ALIGN_LEFT,CRect(0, 0, 200, 150));
//	CLayerManager::RestoreControlBar();

	// TODO: Add your code here

	m_nType = nType; // nType： 项目ID 六线调整为0,电子展板为-1
	
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

	m_pLayerTreeDlg = new CLayerTreeDlg;
	m_pLayerTreeDlg->Create (IDD_LAYERTREEDLG, this) ;
	m_pLayerTreeDlg->m_nType = m_nType;

	m_pLayerTreeDlg->ShowWindow(SW_NORMAL);
	m_pLayerTreeDlg->MoveWindow (0, 0, 100, 100, TRUE) ;

	return (0) ;
}

//-----------------------------------------------------------------------------
void CLayerManager::SizeChanged (CRect *lpRect, BOOL bFloating, int flags) 
{
	//----- Always point to our resource to be safe
//	CAcModuleResourceOverride resourceOverride ;

	//----- If the child dialog is valid
	if ( m_pLayerTreeDlg != NULL ) 
	{
		m_pLayerTreeDlg->SizeChanged(lpRect, bFloating, flags);
	}

//	OutputDebugString("CLayerManager::SizeChanged");
}

//-----------------------------------------------------------------------------
//-----  Function called when user selects a command from Control menu or when user 
//----- selects the Maximize or the Minimize button.
void CLayerManager::OnSysCommand (UINT nID, LPARAM lParam) 
{
	CAcUiDockControlBar::OnSysCommand (nID, lParam) ;
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

BOOL CLayerManager::AddCustomMenuItems(LPARAM hMenu)
{
	HMENU hPopMenu;
	hPopMenu=(HMENU)(hMenu&0xffff);
	CMenu* pop_menu=CMenu::FromHandle(hPopMenu);
	int nCount=pop_menu->GetMenuItemCount()+1;
	for(int i=0;i<nCount;i++)
		pop_menu->DeleteMenu(0,MF_BYPOSITION);
	return TRUE;
}