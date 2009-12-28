#include "StdAfx.h"
#include "resource.h"
#include "DlgLayerTree.h"
#include "DistEntityTool.h"

#include "..\SdeData\DistBaseInterface.h"
#include "..\SdeData\DistExchangeInterface.h"

#include "..\WSAccessor\WsAccessor_export.h"
#include "..\XMLData\DistBaseInterface.h"

#include "AccessWebServiceInfo.h"
#include "SdeDataOperate.h"

//-----------------------------------------------------------------------------

LAYERTREEINFOARRAY CLayerTreeDef::m_LayerTreeArray;

IMPLEMENT_DYNAMIC (CDlgLayerTree, CAcUiDialog)

BEGIN_MESSAGE_MAP(CDlgLayerTree, CAcUiDialog)
	//{{AFX_MSG_MAP(CDlgLayerTree)
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, OnAcadKeepFocus)    // Needed for modeless dialog.
	ON_NOTIFY_EX( TTN_NEEDTEXT, 0, NotifyFunction )
	ON_BN_CLICKED(ID_BTN_AUTOREAD, OnBnClickedBtnAutoread)
	ON_BN_CLICKED(ID_BTN_OPENALL, OnBnClickedBtnOpenall)
	ON_BN_CLICKED(ID_BTN_CLOSEALL, OnBnClickedBtnCloseall)
	ON_BN_CLICKED(ID_BTN_REFURBISH, OnBnClickedBtnRefurbish)
	//}}AFX_MSG_MAP
	ON_NOTIFY(NM_CLICK, IDC_LAYER_TREE, OnNMClickLayerTree)
	ON_NOTIFY(NM_RCLICK, IDC_LAYER_TREE, OnNMRclickLayerTree)
	ON_NOTIFY(TVN_SELCHANGED, IDC_LAYER_TREE, OnTvnSelchangedLayerTree)

END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
CDlgLayerTree::CDlgLayerTree (CWnd *pParent /*=NULL*/, HINSTANCE hInstance /*=NULL*/) : CAcUiDialog (CDlgLayerTree::IDD, pParent, hInstance)
{
	m_OldSelSdeLayer.RemoveAll();
	m_CurSelSdeLayer.RemoveAll();
}

//-----------------------------------------------------------------------------
void CDlgLayerTree::DoDataExchange (CDataExchange *pDX) 
{
	CAcUiDialog::DoDataExchange (pDX) ;
	DDX_Control(pDX, IDC_LAYER_TREE, m_tree);
}

//-----------------------------------------------------------------------------
//----- Needed for modeless dialogs to keep focus.
//----- Return FALSE to not keep the focus, return TRUE to keep the focus
LRESULT CDlgLayerTree::OnAcadKeepFocus (WPARAM, LPARAM) {
	return (TRUE) ;
}




//-----------------------------------------------------------------------------
//- As this dialog is a child dialog we need to disable ok and cancel
BOOL CDlgLayerTree::OnCommand (WPARAM wParam, LPARAM lParam) {
	switch ( wParam ) {
		case IDOK:
		case IDCANCEL:
			return (FALSE) ;
	}	
	return (CDialog::OnCommand (wParam, lParam)) ;
}

//-----------------------------------------------------------------------------
void CDlgLayerTree::OnSize (UINT nType, int cx, int cy) {
	CAcUiDialog::OnSize (nType, cx, cy) ;
	//- now update the dialog
	MoveWindow (0, 0, cx, cy) ;
}

//-----------------------------------------------------------------------------
void CDlgLayerTree::SizeChanged (CRect *lpRect, BOOL bFloating, int flags) {
	//----- If the child dialog is valid
	//----- Always point to our resource to be safe
	CAcModuleResourceOverride resourceOverride ;
	//----- Then update its window size relatively
	CDlgLayerTree::SetWindowPos (this, lpRect->left + 4, lpRect->top + 4, lpRect->Width() - 4, lpRect->Height() - 4, SWP_NOZORDER) ;

	m_tree.SetWindowPos( this,
		lpRect->left + 4, 32,
		lpRect->Width() - 6, lpRect->bottom - 48,
		SWP_NOZORDER);
}

BOOL CDlgLayerTree::OnInitDialog()
{
	CAcUiDialog::OnInitDialog();
	CAcModuleResourceOverride resourceOverride ;

	// 添加工具条按钮
	LPTBBUTTON btn = new TBBUTTON[4];

	btn[0].iBitmap = 0;
	btn[0].idCommand = ID_BTN_AUTOREAD;
	btn[0].fsState = TBSTATE_ENABLED;
	btn[0].fsStyle = TBSTYLE_BUTTON;
	btn[0].dwData = 0;
	btn[0].iString = 1;

	btn[1].iBitmap = 1;
	btn[1].idCommand = ID_BTN_OPENALL;
	btn[1].fsState = TBSTATE_ENABLED;
	btn[1].fsStyle = TBSTYLE_BUTTON;
	btn[1].dwData = 0;
	btn[1].iString = 1;

	btn[2].iBitmap = 2;
	btn[2].idCommand = ID_BTN_CLOSEALL;
	btn[2].fsState = TBSTATE_ENABLED;
	btn[2].fsStyle = TBSTYLE_BUTTON;
	btn[2].dwData = 0;
	btn[2].iString = 1;

	btn[3].iBitmap = 3;
	btn[3].idCommand = ID_BTN_REFURBISH;
	btn[3].fsState = TBSTATE_ENABLED;
	btn[3].fsStyle = TBSTYLE_BUTTON;
	btn[3].dwData = 0;
	btn[3].iString = 1;

	//创建工具条（并建立关联按钮，图标）
	DWORD dwStyle = TBSTYLE_FLAT | TBSTYLE_TOOLTIPS | WS_CHILD | CCS_NORESIZE | WS_VISIBLE | CCS_NODIVIDER;
	m_wndtoolbarctrl.Create( dwStyle, CRect(4,4,150,32) ,this , IDR_TOOLBAR );
	m_wndtoolbarctrl.ModifyStyle(TBSTYLE_TRANSPARENT, 0, 0);// 由于TBSTYLE_FLAT属性中包含了TBSTYLE_TRANSPARENT，需要在此去掉
	m_wndtoolbarctrl.AddButtons(4, btn);
	m_wndtoolbarctrl.AddBitmap(1, IDR_TOOLBAR);
	m_wndtoolbarctrl.SetButtonSize(CSize(16,15));
	m_wndtoolbarctrl.SetBitmapSize(CSize(16,15));
	m_wndtoolbarctrl.AutoSize();
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

	// 添加树型控件
	m_tree.ModifyStyleEx(0,WS_EX_STATICEDGE);
	m_tree.ModifyStyle(TVS_CHECKBOXES,0);
	m_tree.ModifyStyle(0,TVS_CHECKBOXES);
	m_tree.SetItemHeight(17);

	//初始化控件
	//InitializeTreeControl();

	return TRUE; 
}

//-----------------------------------------------------------------------------
BOOL CDlgLayerTree::NotifyFunction( UINT id, NMHDR * pTTTStruct, LRESULT * pResult )
{
	pResult = NULL ;	// Not Used 
	id = 0 ;	// Not used 

	TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pTTTStruct;
	UINT nID = pTTTStruct->idFrom;
	switch ( nID)
	{
	case ID_BTN_AUTOREAD:
		pTTT->lpszText = TEXT("读取选中SDE图层");//MAKEINTRESOURCE((LPCTSTR)(ID_BTN_AUTOREAD));
		break;
	case ID_BTN_OPENALL:
		pTTT->lpszText = TEXT("打开所有SDE图层");//MAKEINTRESOURCE((LPCTSTR)(ID_BTN_OPENALL));
		break;
	case ID_BTN_CLOSEALL:
		pTTT->lpszText = TEXT("关闭所有SDE图层");//MAKEINTRESOURCE((LPCTSTR)(ID_BTN_CLOSEALL));
		break;
	case ID_BTN_REFURBISH:
		pTTT->lpszText = TEXT("刷新图层列表");//MAKEINTRESOURCE((LPCTSTR)(ID_BTN_REFURBISH));
		break;
	}
	return(TRUE);
}

//-----------------------------------------------------------------------------
void CDlgLayerTree::InitializeTreeControl()
{

	m_tree.DeleteAllItems();
	CStringArray strNameArray,strNeedArray;

	if (CLayerTreeDef::GetChildrenAliasName(strNameArray, strNeedArray)==FALSE)  return;

	for (int i = strNameArray.GetSize() - 1; i >= 0; i--)
	{
		CString strRight;
		CPowerConfig::GetRightByResource(strNameArray.GetAt(i), strRight);
		int nRight = atoi(strRight);
		if (!(nRight & POWER_SCAN))// 对比查看权限
		{
			//strNameArray.RemoveAt(i);  ////////////////////////######################
		}
	}
	for (int i = 0; i < strNameArray.GetSize(); i++)
	{
		HTREEITEM hTI = CreateTree(strNameArray.GetAt(i));
		CString sTmp = strNeedArray.GetAt(i);
		if (!sTmp.IsEmpty() && sTmp.CollateNoCase("0") != 0 && hTI != NULL)
		{
			//必选项
			//m_tree.SetCheck(hTI);
			SetTreeCheck(hTI);
			m_SdeAliasNames.Add(strNameArray.GetAt(i));
		}
	}

	// 读取用户机器上配置的图层名称，显示为选中
	if (m_tree.GetRootItem() == NULL)
	{
		return;
	}

	ReadSelLayer();
}

//-----------------------------------------------------------------------------
HTREEITEM CDlgLayerTree::CreateTree(CString sName, HTREEITEM hParent /*= TVI_ROOT*/)
{
	HTREEITEM hReTI = m_tree.InsertItem(sName, hParent);

	CStringArray strChildNameArray,strNeedArray;
	if (CLayerTreeDef::GetChildrenAliasName(strChildNameArray, strNeedArray, sName))
	{
		for (int i = strChildNameArray.GetSize() - 1; i >= 0; i--)
		{
			CString Right;
			CPowerConfig::GetRightByResource(strChildNameArray.GetAt(i), Right);
			int nRight = atoi(Right);
			if (!(nRight & POWER_SCAN))// 对比查看权限
			{
				//strChildNameArray.RemoveAt(i);///////////////////////////////##############################
			}
		}
		for (int i = 0; i < strChildNameArray.GetSize(); i++)
		{
			HTREEITEM hTI = CreateTree(strChildNameArray.GetAt(i), hReTI);
			CString sTmp = strNeedArray.GetAt(i);
			if (!sTmp.IsEmpty() && sTmp.CollateNoCase("0") != 0 && hTI != NULL)
			{
				SetTreeCheck(hTI);
				m_SdeAliasNames.Add(strChildNameArray.GetAt(i));
			}
		}
	}

	return hReTI;
}

//-----------------------------------------------------------------------------
void CDlgLayerTree::OnBnClickedBtnAutoread() //读取选中SDE图层
{

	//LockDocument _Lock;
	IDistExchangeData *pExchange =  CreateExchangeObjcet();
	pExchange->SetDbReactorStatues(false);

	//打开所有图层
	CStringArray strCadLyNameArray;
	CDistEntityTool tempTool;
	tempTool.GetAllLayerName(strCadLyNameArray);       //获取CAD图层列表
	tempTool.OpenOrCloseLayer(TRUE,strCadLyNameArray); //打开所有图层

	//删除以前选择的SDE图层数据	m_OldSelSdeLayer
	CStringArray strCadArray;
	CSdeDataOperate tempSdeOp;
	tempSdeOp.GetCadLayerBySdeLayer(strCadArray,m_OldSelSdeLayer);
	int nNum = strCadArray.GetSize();
	for (int i = 0; i < nNum; i++)
	{
		CString strTemp = strCadArray.GetAt(i);
		tempTool.DeleteAllEntityInLayer(strTemp.GetBuffer());
	}
	m_OldSelSdeLayer.Append(m_CurSelSdeLayer);

	pExchange->SetDbReactorStatues(true);
	pExchange->Release();

	//打开当前选择的SDE图层数据 m_CurSelSdeLayer
	tempTool.SendCommandToAutoCAD("AutoReadSDE\n");
}

//-----------------------------------------------------------------------------
void CDlgLayerTree::OnBnClickedBtnOpenall() //打开所有SDE图层
{
	HTREEITEM hTI = m_tree.GetRootItem();

	while (hTI != NULL)
	{
		SetTreeCheck(hTI);

		hTI = m_tree.GetNextItem(hTI, TVGN_NEXT);
	}

	SaveSelLayer();
}

//-----------------------------------------------------------------------------
void CDlgLayerTree::OnBnClickedBtnCloseall() //关闭所有SDE图层
{
	HTREEITEM hTI = m_tree.GetRootItem();

	while (hTI != NULL)
	{
		SetTreeCheck(hTI, FALSE);

		hTI = m_tree.GetNextItem(hTI, TVGN_NEXT);
	}

	SaveSelLayer();
}

//-----------------------------------------------------------------------------
void CDlgLayerTree::OnBnClickedBtnRefurbish() //刷新图层列表
{
	InitializeTreeControl();
}

//-----------------------------------------------------------------------------
void CDlgLayerTree::SetTreeCheck(HTREEITEM hTI /*= TVI_ROOT*/, BOOL bCheck /*= TRUE*/)
{
	if (hTI == NULL)
	{
		return;
	}

	if (m_tree.ItemHasChildren(hTI))
	{
		HTREEITEM ChiItem = m_tree.GetNextItem(hTI, TVGN_CHILD);
		SetTreeCheck(ChiItem, bCheck);

		HTREEITEM NexItem = m_tree.GetNextItem(ChiItem, TVGN_NEXT);
		while (NexItem != NULL)
		{
			SetTreeCheck(NexItem, bCheck);
			NexItem = m_tree.GetNextItem(NexItem, TVGN_NEXT);
		}
	}

	m_tree.SetCheck(hTI, bCheck);
}

//-----------------------------------------------------------------------------
void CDlgLayerTree::OnNMClickLayerTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	UINT uFlag;
	CPoint pt;
	CString sSelLayer;
	HTREEITEM SelRoot,ParRoot,NexRoot;
	GetCursorPos(&pt);
	m_tree.ScreenToClient(&pt);
	SelRoot = m_tree.HitTest(pt,&uFlag);
	if ((NULL!=SelRoot) && (uFlag&TVHT_ONITEMSTATEICON))
	{
		BOOL bChecked = m_tree.GetCheck(SelRoot);

		SetTreeCheck(SelRoot, !bChecked);

		//分析是否有必要对树枝进行调整
		ParRoot = m_tree.GetParentItem(SelRoot);
		while (ParRoot != NULL)
		{
			if (bChecked)
				m_tree.SetCheck(ParRoot,FALSE);//当前处在被选择状态即将被取消选择
			else
			{
				int k=0;//当前处在非选择状态即将有可能处于选择状态(标志位)
				NexRoot=m_tree.GetNextItem(ParRoot,TVGN_CHILD);
				if(NexRoot!=SelRoot)
				{
					if(!m_tree.GetCheck(NexRoot)) k=1;
				}
				if(k==0)
				{
					while(TRUE)
					{
						NexRoot=m_tree.GetNextItem(NexRoot,TVGN_NEXT);
						if(NexRoot==NULL) break;
						if(NexRoot==SelRoot) continue;
						if(!m_tree.GetCheck(NexRoot))
						{
							k=1;
							break;
						}
					}
				}
				if(k==0) m_tree.SetCheck(ParRoot);
			}
			ParRoot = m_tree.GetParentItem(ParRoot);
		}
		//if(acedCommand(0)!=RTNORM) sendCommandToAutoCAD("\x1B");

		SaveSelLayer();

		m_tree.SetCheck(SelRoot, bChecked);
	}

	*pResult = 0;
}

//-----------------------------------------------------------------------------
void CDlgLayerTree::OnNMRclickLayerTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}

//-----------------------------------------------------------------------------
void CDlgLayerTree::OnTvnSelchangedLayerTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}

void CDlgLayerTree::GetCurSelItemName(HTREEITEM hTI/* = TVI_ROOT*/)
{
	if (hTI == NULL)
	{
		return;
	}

	if (m_tree.GetCheck(hTI))
	{
		m_SdeAliasNames.Add(m_tree.GetItemText(hTI));
	}

	if (m_tree.ItemHasChildren(hTI))
	{
		HTREEITEM ChiItem = m_tree.GetNextItem(hTI, TVGN_CHILD);
		GetCurSelItemName(ChiItem);
	}

	hTI = m_tree.GetNextItem(hTI, TVGN_NEXT);
	while (hTI != NULL)
	{
		if (m_tree.GetCheck(hTI))
		{
			m_SdeAliasNames.Add(m_tree.GetItemText(hTI));
		}

		if (m_tree.ItemHasChildren(hTI))
		{
			HTREEITEM ChiItem = m_tree.GetNextItem(hTI, TVGN_CHILD);
			GetCurSelItemName(ChiItem);
		}

		hTI = m_tree.GetNextItem(hTI, TVGN_NEXT);
	}
}

void CDlgLayerTree::SetCurSelItemName(HTREEITEM hTI /*= TVI_ROOT*/)
{
	CString sName = m_tree.GetItemText(hTI);

	BOOL bFind = FALSE;
	for (int i = 0; (i < m_SdeAliasNames.GetSize()) && !bFind; i++)
	{
		if (m_SdeAliasNames.GetAt(i).CompareNoCase(sName) == 0)
		{
			SetTreeCheck(hTI);
			bFind = TRUE;
		}
	}

	if (!bFind && (m_tree.ItemHasChildren(hTI)))
	{
		HTREEITEM ChiItem = m_tree.GetNextItem(hTI, TVGN_CHILD);
		SetCurSelItemName(ChiItem);
	}

	HTREEITEM NexItem = m_tree.GetNextItem(hTI, TVGN_NEXT);
	if (NexItem != NULL)
	{
		SetCurSelItemName(NexItem);
	}
}

void CDlgLayerTree::SaveSelLayer()
{
	HTREEITEM hTI = m_tree.GetRootItem();
	if (hTI == NULL)
	{
		return;
	}
	m_SdeAliasNames.RemoveAll();
	GetCurSelItemName(hTI);

	CString sData;
	for (int i = 0; i < m_SdeAliasNames.GetSize(); i++)
	{
		sData += m_SdeAliasNames.GetAt(i);
		sData += "，";
	}

	TCHAR szConfigFile[256] = {0};
	DWORD dwSize = GetModuleFileName(GetModuleHandle("shYDsys.arx"), szConfigFile, MAX_PATH);
	while (dwSize > 0) 
	{
		if (szConfigFile[dwSize - 1] == '\\') 
		{
			lstrcpy(szConfigFile + dwSize, "UserData.dat");
			break;
		}
		dwSize --;
	}

	HANDLE hFile = CreateFile(szConfigFile, GENERIC_WRITE, 0,
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	DWORD temp;
	WriteFile(hFile, (LPCVOID)sData, sData.GetLength(), &temp, NULL);

	CloseHandle(hFile);
}

void CDlgLayerTree::ReadSelLayer()
{
	TCHAR szConfigFile[256] = {0};
	DWORD dwSize = GetModuleFileName(GetModuleHandle("shYDsys.arx"), szConfigFile, MAX_PATH);
	while (dwSize > 0) 
	{
		if (szConfigFile[dwSize - 1] == '\\') 
		{
			lstrcpy(szConfigFile + dwSize, "UserData.dat");
			break;
		}
		dwSize --;
	}

	HANDLE hFile = CreateFile(szConfigFile, GENERIC_READ, 0,
		NULL, OPEN_EXISTING, 0, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return;
	}

	DWORD  dwTemp, dwByte;
	dwTemp = GetFileSize(hFile, &dwByte);
	char * szBuffer = new char[dwTemp+1];
	memset(szBuffer, 0, sizeof(szBuffer));
	ReadFile(hFile, szBuffer, dwTemp, &dwSize, NULL);

	CloseHandle(hFile);

	CString sData = szBuffer;

	int pos = sData.Find("，");
	while (pos != -1)
	{
		CString sTmp = sData.Left(pos);
		sData = sData.Right(sData.GetLength() - pos - 2);
		HTREEITEM hTI = m_tree.GetRootItem();
		if (CheckTreeItemName(sTmp, hTI))
		{
			m_SdeAliasNames.Add(sTmp);
		}
		pos = sData.Find("，");
	}
	delete[] szBuffer;

	GetCurSdeLayerName();

	m_OldSelSdeLayer.RemoveAll();
	m_OldSelSdeLayer.Append(m_CurSelSdeLayer);
}

void CDlgLayerTree::GetCurSdeLayerName()
{
	m_CurSelSdeLayer.RemoveAll();
	for (int i = 0; i < m_SdeAliasNames.GetSize(); i++)
	{
		CString sAlias = m_SdeAliasNames.GetAt(i);
		CString sName;
		CLayerTreeDef::GetSDELayerName(sName, sAlias);
		if (!sName.IsEmpty())
		{
			m_CurSelSdeLayer.Add(sName);
		}
	}
}

BOOL CDlgLayerTree::CheckTreeItemName(CString sName, HTREEITEM hTI /* = TVI_ROOT */)
{
	if (hTI == NULL || sName.IsEmpty())
	{
		return FALSE;
	}

	CString sItemName = m_tree.GetItemText(hTI);
	if (sItemName.CompareNoCase(sName) == 0)
	{
		SetTreeCheck(hTI);
		return TRUE;
	}

	if (m_tree.ItemHasChildren(hTI))
	{
		HTREEITEM ChiItem = m_tree.GetNextItem(hTI, TVGN_CHILD);
		BOOL bFind = CheckTreeItemName(sName, ChiItem);
		if (bFind)
		{
			return TRUE;
		}
	}

	hTI = m_tree.GetNextItem(hTI, TVGN_NEXT);
	while (hTI != NULL)
	{
		sItemName = m_tree.GetItemText(hTI);
		if (sItemName.CompareNoCase(sName) == 0)
		{
			SetTreeCheck(hTI);
			return TRUE;
		}

		if (m_tree.ItemHasChildren(hTI))
		{
			HTREEITEM ChiItem = m_tree.GetNextItem(hTI, TVGN_CHILD);
			BOOL bFind = CheckTreeItemName(sName, ChiItem);
			if (bFind)
			{
				return TRUE;
			}
		}

		hTI = m_tree.GetNextItem(hTI, TVGN_NEXT);
	}

	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//====================================================================================================//
//====================================================================================================//

IMPLEMENT_DYNAMIC (CLayerManager, CAcUiDockControlBar)

BEGIN_MESSAGE_MAP(CLayerManager, CAcUiDockControlBar)
	ON_WM_CREATE()
	ON_WM_SYSCOMMAND()
	ON_WM_SIZE()
END_MESSAGE_MAP()


//-----------------------------------------------------------------------------
static CLSID clsCLayerManager = {0xeab78c04, 0x2194, 0x47ad, {0xa4, 0xf2, 0xad, 0xca, 0x3e, 0x3b, 0xb6, 0x3c}} ;

//-----------------------------------------------------------------------------
CLayerManager::CLayerManager ()
{
	m_pChildDlg = NULL;
}

//-----------------------------------------------------------------------------
CLayerManager::~CLayerManager () {
	//----- If the child dialog is still valid
	if (NULL != m_pChildDlg  )
	{
		delete m_pChildDlg ; m_pChildDlg = NULL;
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
	if (!CAcUiDockControlBar::Create (strWndClass, lpszTitle, WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN,rect, pParent, 0))
		return (FALSE) ;

	SetToolID (&clsCLayerManager) ;

	CLayerManager::EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);// | CBRS_ALIGN_TOP
	CLayerManager::DockControlBar(CBRS_ALIGN_LEFT,CRect(0, 0, 200, 150));
	CLayerManager::RestoreControlBar();

	return (TRUE) ;
}

//-----------------------------------------------------------------------------
//----- This member function is called when an application requests the window be 
//----- created by calling the Create or CreateEx member function
int CLayerManager::OnCreate (LPCREATESTRUCT lpCreateStruct) 
{
	if ( CAcUiDockControlBar::OnCreate (lpCreateStruct) == -1 )
		return (-1) ;

	//----- Point to our resource
	CAcModuleResourceOverride resourceOverride; 	
	//----- Now create a new dialog with our stuff in it
	if(NULL == m_pChildDlg)
		m_pChildDlg =new CDlgLayerTree ;

	m_pChildDlg->Create (IDD_DLG_LAYERTREE, this) ;
	m_pChildDlg->SetParent(this);
	m_pChildDlg->MoveWindow (0, 0, 100, 100, TRUE) ;

	//m_pChildDlg->ShowWindow(SW_SHOWNORMAL);
	return (0) ;
}

//-----------------------------------------------------------------------------
void CLayerManager::SizeChanged (CRect *lpRect, BOOL bFloating, int flags)
{
	if ( m_pChildDlg != NULL ) {
		CAcModuleResourceOverride resourceOverride ;
		m_pChildDlg->SizeChanged(lpRect,bFloating,flags);
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
	if ( m_pChildDlg != NULL ) {
		CAcModuleResourceOverride resourceOverride ;
		m_pChildDlg->MoveWindow (0, 0, cx, cy) ;
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

void CLayerManager::GetSdeLayerName()
{
	m_SdeLayer.RemoveAll();
	if (m_pChildDlg != NULL)
	{
		m_pChildDlg->GetCurSdeLayerName();
		m_SdeLayer.Append(m_pChildDlg->m_CurSelSdeLayer);
	}
}

void CLayerManager::RedrawTreeWindow()
{
	if (m_pChildDlg != NULL)
	{
		m_pChildDlg->InitializeTreeControl();
	}
}



////////////////////////////////////////////////////////////////////////////////////////////////////////
//====================================================================================================//
//====================================================================================================//

CLayerTreeDef::CLayerTreeDef(void)
{
}

CLayerTreeDef::~CLayerTreeDef(void)
{
}

BOOL CLayerTreeDef::GetLayerTreeInf(CString strURL)
{
	m_LayerTreeArray.RemoveAll();
	IWSAccessor* pWsAccessor = XSingletonFactory::Instance()->CreateVarWSAccessorObject();

	if (pWsAccessor == NULL)		
	{
		return FALSE;
	}

	HRESULT hr = pWsAccessor->Connect(strURL.GetBuffer());

	TCHAR lpMsg[512];

	// 如果连接出错！
	if( hr != S_OK )
	{
		pWsAccessor->Answer(lpMsg);

		MessageBox(0, lpMsg ,"",  0);

		return FALSE;
	}


	CString sSql;
	sSql = "select * from TB_LAYERTREEDEF";

	// 注意，参数数组长度一定要多声明一些
	_variant_t varParams[5];
	varParams[0] = sSql.GetBuffer();

	_variant_t varResult;

	// 调用方法
	hr = pWsAccessor->CallMethod("ExecSqlQuery",varParams, &varResult);

	// 如果出错，获得出错信息
	if (hr != S_OK )
	{
		pWsAccessor->Answer(lpMsg);

		MessageBox(0, lpMsg ,"",  0);

		return FALSE;
	}

	IDistConnectionXML* lpConn = CreateConnectObjcet();
	if (S_OK != lpConn->Connect(NULL, (_bstr_t)varResult, NULL, NULL, NULL))
	{
		AfxMessageBox("Connect to file failed!\n");
		return FALSE;
	}

	IDistCommandXML* lpCmd = NULL;
	lpConn->QueryCommand(&lpCmd);

	char strXmlPath[] = "/NewDataSet/Table";
	IDistRecordSetXML* lpRst = NULL;
	lpCmd->SelectData(strXmlPath, &lpRst);

	if (NULL == lpRst)
	{
#ifdef _DEBUG
		AfxMessageBox("TB_LAYERTREEDEF表中未找到数据");
#endif
		lpCmd->Release();
		lpConn->Release();
		return FALSE;
	}

	BOOL bOK = lpRst->MoveFirst();
	while (bOK)
	{
		LAYERTREECONFIG temp;
		CString tmp = (char*)((*lpRst)["ID"]);
		temp.Id = tmp;
		tmp = (char*)((*lpRst)["SDELAYER"]);
		temp.SDELayer = tmp;
		tmp = (char*)((*lpRst)["ALIASNAME"]);
		if (tmp.IsEmpty())
		{
			tmp = temp.SDELayer;
		}
		temp.AliasName = tmp;
		tmp = (char*)((*lpRst)["FATHERID"]);
		temp.FatherId = tmp;
		tmp = (char*)((*lpRst)["NEEDINF"]);
		temp.NeedInf = tmp;
		tmp = (char*)((*lpRst)["ORDERNUM"]);
		if (tmp.IsEmpty())
		{
			tmp = temp.Id;
		}
		temp.OrderNum = tmp;

		m_LayerTreeArray.Add(temp);

		bOK = lpRst->MoveNext();
	}

	lpRst->Release();
	lpCmd->Release();
	lpConn->Release();

	return TRUE;
}

void CLayerTreeDef::Release()
{
	m_LayerTreeArray.RemoveAll();
}

// 根据 AliasName 别名获取其 ID
BOOL CLayerTreeDef::GetIdByAliasName(CString& strId, const CString strAliasName)
{
	strId.Empty();
	int nCount =m_LayerTreeArray.GetSize();  
	if (nCount < 1) return FALSE;

	//如果别名为空，取最小FatherID
	if (strAliasName.IsEmpty())
	{
		int nId = atoi(m_LayerTreeArray.GetAt(0).FatherId);

		for (int i = 1; i < nCount; i++)
		{
			int iTmp = atoi(m_LayerTreeArray.GetAt(i).FatherId);
			if (nId > iTmp) nId = iTmp;
		}
		strId.Format("%d", nId);
		return TRUE;
	}
	else
	{
		for (int i = 0; i < nCount; i++)
		{
			if (m_LayerTreeArray.GetAt(i).AliasName.CollateNoCase(strAliasName) == 0)
			{
				strId = m_LayerTreeArray.GetAt(i).Id;
				return TRUE;
			}
		}
	}
	return FALSE;
}


// 根据指定的父节点获取其对应的子节点数组
BOOL CLayerTreeDef::GetChildrenAliasName(CStringArray& ChildrenAliasNames, CStringArray& saNeed, const CString FatherAliasName)
{
	ChildrenAliasNames.RemoveAll(); saNeed.RemoveAll();

	CString sFatherId;
	if (GetIdByAliasName(sFatherId, FatherAliasName)==FALSE)  return FALSE;

	CStringArray saOrder;//辅助数组（排序）
	int nCount =  m_LayerTreeArray.GetSize();
	for (int i = 0; i <nCount; i++)
	{
		if (m_LayerTreeArray.GetAt(i).FatherId.CollateNoCase(sFatherId) == 0)  //判断ID是否相等
		{
			//根据排序确定插入位置
			int nOrder1 = atoi(m_LayerTreeArray.GetAt(i).OrderNum);
			int nNum = saOrder.GetSize();
			int nPos = 0;
			for (int j =nNum-1; j >= 0; j--){
				int nOrder2 = atoi(saOrder.GetAt(j));
				if (nOrder1 >= nOrder2)
				{
					nPos = j; break;  //记录插入位置
				}
			}

			//插入数据
			ChildrenAliasNames.InsertAt(nPos, m_LayerTreeArray.GetAt(i).AliasName);
			saNeed.InsertAt(nPos, m_LayerTreeArray.GetAt(i).NeedInf);
			saOrder.InsertAt(nPos, m_LayerTreeArray.GetAt(i).OrderNum);
		}
	}

	return (ChildrenAliasNames.GetSize() > 0);
}

// 根据AliasName 别名，获取 SDE 图层名称
BOOL CLayerTreeDef::GetSDELayerName(CString& strSdeLyName, const CString strAliasName)
{
	strSdeLyName.Empty();
	for (int i = 0; i < m_LayerTreeArray.GetSize(); i++)
	{
		if (m_LayerTreeArray.GetAt(i).AliasName.CollateNoCase(strAliasName) == 0)
		{
			strSdeLyName = m_LayerTreeArray.GetAt(i).SDELayer;
			return TRUE;
		}
	}
	return FALSE;
}

// 根据 SDE 图层名称，获取 AliasName 别名
BOOL CLayerTreeDef::GetAliasName(CString& strAliasName, const CString strSdeLyName)
{
	strAliasName.Empty();
	for (int i = 0; i < m_LayerTreeArray.GetSize(); i++)
	{
		if (m_LayerTreeArray.GetAt(i).SDELayer.CollateNoCase(strSdeLyName) == 0)
		{
			strAliasName = m_LayerTreeArray.GetAt(i).AliasName;
			return TRUE;
		}
	}
	return FALSE;
}


