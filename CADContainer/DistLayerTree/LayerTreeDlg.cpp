#include "StdAfx.h"
#include "resource.h"
#include "LayerTreeDlg.h"



// 向CAD窗口发送命令
void  SendMessageToCad(CString& strCommand, CStringArray& strParamArray)
{
	MSXML2::IXMLDOMDocumentPtr lpDocument;
	long hr = lpDocument.CreateInstance(__uuidof(DOMDocument));

	if ( FAILED(hr) ) 
		_com_raise_error(hr);

	MSXML2::IXMLDOMProcessingInstructionPtr lpInstruction = lpDocument->createProcessingInstruction("xml","version='1.0' encoding='GB2312'");
	lpDocument->appendChild(lpInstruction);

	MSXML2::IXMLDOMElementPtr lpRoot = lpDocument->createElement(LPCTSTR("root"));

	lpDocument->appendChild(lpRoot);

	// 数据类型：命令或数据
	MSXML2::IXMLDOMElementPtr lpCommand = lpDocument->createElement("command");

	lpCommand->setAttribute("name", _variant_t(strCommand));

	lpRoot->appendChild(lpCommand);

	// 参数列表
	for (int i=0; i<strParamArray.GetSize(); i++)
	{
		CString strParam = strParamArray.GetAt(i);
		CString strName = strParam.Left(strParam.Find('='));
		CString strValue = strParam.Right(strParam.GetLength()-strParam.Find('=')-1);

		MSXML2::IXMLDOMElementPtr lpParam = lpDocument->createElement("param");
		lpParam->setAttribute("name", _variant_t(strName));
		lpParam->setAttribute("value", _variant_t(strValue));

		lpCommand->appendChild(lpParam);
	}

	CString strXML = (LPCTSTR)(lpDocument->xml);
	//	OutputDebugString(lpDocument->xml);

	COPYDATASTRUCT cds;
	cds.dwData = 0;
	cds.cbData = strXML.GetLength()+1;
	cds.lpData = (void*)strXML.GetBuffer(0);
	strXML.ReleaseBuffer();
	::SendMessage(acedGetAcadFrame()->m_hWnd, WM_COPYDATA, (WPARAM)NULL, (LPARAM)&cds);
}

//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC (CLayerTreeDlg, CAcUiDialog)

BEGIN_MESSAGE_MAP(CLayerTreeDlg, CAcUiDialog)
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, OnAcadKeepFocus)
	ON_WM_DESTROY()
	ON_NOTIFY(NM_CLICK, IDC_TREE_LAYER, OnNMClickLayerTree)
	ON_NOTIFY_EX( TTN_NEEDTEXT, 0, NotifyFunction )
	ON_BN_CLICKED(IDC_TB_SAVE, OnBnClickedBtnSave)
END_MESSAGE_MAP()


//-----------------------------------------------------------------------------
CLayerTreeDlg::CLayerTreeDlg (CWnd *pParent /*=NULL*/, HINSTANCE hInstance /*=NULL*/) 
             :CAcUiDialog (CLayerTreeDlg::IDD, pParent, hInstance) 
{
}


void CLayerTreeDlg::OnDestroy()
{
	CAcUiDialog::OnDestroy();
}


BOOL CLayerTreeDlg::OnInitDialog()
{
	CAcUiDialog::OnInitDialog();

	CAcModuleResourceOverride resourceOverride;

	// 添加树型控件
	m_tree.ModifyStyleEx(0,WS_EX_STATICEDGE);
	m_tree.SetItemHeight(17);
	m_tree.ModifyStyle(TVS_CHECKBOXES,0);
	m_tree.ModifyStyle(0,TVS_CHECKBOXES);

	m_imagelist.Create(IDB_LAYERTREE, 16, 1, RGB(255,0,0));
	m_tree.SetImageList(&m_imagelist,TVSIL_NORMAL);
/*
	if (m_nType == 0)
	{
		m_wndToolBar.Create(this);
		m_wndToolBar.LoadToolBar(IDR_TOOLBAR);

		m_wndToolBar.SetBarStyle(CBRS_ALIGN_TOP | CBRS_TOOLTIPS | CBRS_GRIPPER);

		m_wndToolBar.ModifyStyleEx(0,WS_EX_TRANSPARENT);
		m_wndToolBar.ModifyStyle(0, TBSTYLE_FLAT);

		RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
	}
*/
	return TRUE;  
}

BOOL CLayerTreeDlg::NotifyFunction( UINT id, NMHDR * pTTTStruct, LRESULT * pResult )
{
	pResult = NULL ;	// Not Used 
	id = 0 ;	// Not used 

	TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pTTTStruct;
	UINT nID = pTTTStruct->idFrom;
	switch ( nID)
	{
	case IDC_TB_STARTEDIT:
		pTTT->lpszText = TEXT("开始编辑");//MAKEINTRESOURCE((LPCTSTR)(ID_BTN_AUTOREAD));
		break;
	case IDC_TB_ENDEDIT:
		pTTT->lpszText = TEXT("结束编辑");//MAKEINTRESOURCE((LPCTSTR)(ID_BTN_AUTOREAD));
		break;
	case IDC_TB_CANCELEDIT:
		pTTT->lpszText = TEXT("取消编辑");//MAKEINTRESOURCE((LPCTSTR)(ID_BTN_AUTOREAD));
		break;	
	}
	return(TRUE);
}

//-----------------------------------------------------------------------------
void CLayerTreeDlg::DoDataExchange (CDataExchange *pDX) 
{
	CAcUiDialog::DoDataExchange (pDX) ;
	DDX_Control(pDX, IDC_TREE_LAYER, m_tree);
}

BOOL CLayerTreeDlg::OnCommand (WPARAM wParam, LPARAM lParam) 
{
	switch ( wParam )
	{
		case IDOK:
		case IDCANCEL:
		return (FALSE) ;
	}	
	return (CAcUiDialog::OnCommand (wParam, lParam)) ;
}

//-----------------------------------------------------------------------------
LRESULT CLayerTreeDlg::OnAcadKeepFocus (WPARAM, LPARAM) 
{
	return (TRUE) ;
}

void CLayerTreeDlg::OnSize(UINT nType, int cx, int cy) 
{
	CAcUiDialog::OnSize (nType, cx, cy) ;

	OutputDebugString("CLayerTreeDlg::OnSize");
}

//-----------------------------------------------------------------------------
void CLayerTreeDlg::SizeChanged (CRect *lpRect, BOOL bFloating, int flags) 
{
//	CAcModuleResourceOverride resourceOverride;

	MoveWindow(lpRect->left + 2, lpRect->top + 2, lpRect->right - 2, lpRect->bottom - 2);

	//m_tree.MoveWindow(lpRect->left + 2, lpRect->top+20, lpRect->right - 2, lpRect->bottom - 30-20);
/*	if (m_nType == 0)
		m_tree.MoveWindow(lpRect->left + 2, lpRect->top+35, lpRect->right - 2, lpRect->bottom - 30);
	else
*/		m_tree.MoveWindow(lpRect->left + 2, lpRect->top, lpRect->right - 2, lpRect->bottom - 30);
}

void CLayerTreeDlg::GetCheckedLayers(CStringArray& arrayLayers)
{
	// 递归展开所有子项
	HTREEITEM hRoot = m_tree.GetRootItem();
	while (hRoot != NULL)
	{
		EnumCheckItems(hRoot, arrayLayers);
		hRoot = m_tree.GetNextItem(hRoot, TVGN_NEXT);
	}
}
void CLayerTreeDlg::SetCheckedLayers(CStringArray& arrayLayers)
{
	// 递归展开所有子项
	HTREEITEM hRoot = m_tree.GetRootItem();
	while (hRoot != NULL)
	{
		SetCheckItems(hRoot, arrayLayers);
		hRoot = m_tree.GetNextItem(hRoot, TVGN_NEXT);
	}
}

void CLayerTreeDlg::SetCheckItems(HTREEITEM hItem, CStringArray& arrayLayers)
{
	if (NULL != hItem)
	{
		for (int  i=0; i<arrayLayers.GetCount(); i++)
		{
			if (arrayLayers.GetAt(i).CompareNoCase(m_tree.GetItemText(hItem)) == 0)
			{
				m_tree.SetCheck(hItem);
			}
		}

		HTREEITEM hChildItem = m_tree.GetChildItem(hItem);

		if (hChildItem != NULL)
		{
			// 递归搜索所有子项
			while (hChildItem != NULL)
			{
				SetCheckItems(hChildItem, arrayLayers);
				hChildItem = m_tree.GetNextItem(hChildItem, TVGN_NEXT);
			}
		}
	}
}

void CLayerTreeDlg::EnumCheckItems(HTREEITEM hItem, CStringArray& arrayLayers)
{
	if (NULL != hItem)
	{
		HTREEITEM hChildItem = m_tree.GetChildItem(hItem);

		if (NULL == hChildItem)
		{
			if (m_tree.GetCheck(hItem))
			{
				arrayLayers.Add(m_tree.GetItemText(hItem));
			}
		}
		else
		{
			if (m_tree.GetCheck(hItem))
			{
				arrayLayers.Add(m_tree.GetItemText(hItem));
			}

			// 递归搜索所有子项
			while (hChildItem != NULL)
			{
				EnumCheckItems(hChildItem, arrayLayers);
				hChildItem = m_tree.GetNextItem(hChildItem, TVGN_NEXT);
			}
		}
	}
}

BOOL CLayerTreeDlg::LoadXmlFromFile(LPCTSTR lpFile)
{
	m_xmlDocument.DeleteContents();

	if (!m_xmlDocument.Load(lpFile))
	{
		return FALSE;
	}
	m_tree.DeleteAllItems();

	LoadXmlSubTree(m_xmlDocument.GetRootElement(), TVI_ROOT);

	// 递归展开所有子项
	HTREEITEM hRoot = m_tree.GetRootItem();
	while (hRoot != NULL)
	{
		m_tree.Expand(hRoot, TVE_EXPAND);
//		ExpandTreeItem(hRoot);
		hRoot = m_tree.GetNextItem(hRoot, TVGN_NEXT);
	}

	m_tree.SetScrollPos(SB_VERT, 0);

	return TRUE;
}

BOOL CLayerTreeDlg::LoadXmlFromBuffer(LPCTSTR lpBuffer)
{
	m_xmlDocument.DeleteContents();

	if (!m_xmlDocument.LoadXml(lpBuffer))
	{
		return FALSE;
	}
	m_tree.DeleteAllItems();

	LoadXmlSubTree(m_xmlDocument.GetRootElement(), TVI_ROOT);

	// 递归展开所有子项
	HTREEITEM hRoot = m_tree.GetRootItem();
	while (hRoot != NULL)
	{
		m_tree.Expand(hRoot, TVE_EXPAND);
//		ExpandTreeItem(hRoot);
		hRoot = m_tree.GetNextItem(hRoot, TVGN_NEXT);
	}

	m_tree.SetScrollPos(SB_VERT, 0);
/*
#ifdef XHTMLXML

	CXHtmlTree::ConvertAction eConvertAction = CXHtmlTree::NoConvertAction;

#ifdef UNICODE
	eConvertAction = CXHtmlTree::ConvertToUnicode;
#endif

#endif

//	m_tree.DeleteAllItems();

	m_tree.LoadXmlFromBuffer((BYTE*)lpBuffer, 1000, eConvertAction);
*/
	return TRUE;
}

BOOL CLayerTreeDlg::LoadXmlSubTree(CXmlElement *pElement, HTREEITEM hItem, BOOL bChecked /*= FALSE*/)
{
	if ((NULL == pElement) || (NULL == hItem))
		return FALSE;

	CXmlElement *pChild = m_xmlDocument.GetFirstChild(pElement);

	while (pChild != NULL)
	{
		CString strName = pChild->GetValue("name");
		if (strName.IsEmpty())
			strName = pChild->m_strData;
		if (strName.IsEmpty())
			strName = pChild->m_strName;

		CString strChecked = pChild->GetValue("checked");

		int image = (pChild->m_ChildElements.GetCount()!=0)?0:2;

		if (strName.Find("六线历史") >= 0)
		{
			pChild = m_xmlDocument.GetNextSibling(pElement);
			continue;
		}

		HTREEITEM hChildItem = m_tree.InsertItem(strName, image, image+1, hItem);

		BOOL check = bChecked | atoi(strChecked);
		m_tree.SetCheck(hChildItem, check);

		if (NULL == hChildItem)
		{
			return FALSE;
		}

		// save data
		m_tree.SetItemData(hChildItem, (DWORD)pChild);

		LoadXmlSubTree(pChild, hChildItem, check);

		pChild = m_xmlDocument.GetNextSibling(pElement);
	}

	return TRUE;
}

void CLayerTreeDlg::ExpandTreeItem(HTREEITEM hItem)
{
	if (NULL != hItem)
	{
		m_tree.Expand(hItem, TVE_EXPAND);

		hItem = m_tree.GetChildItem(hItem);

		// 递归展开所有子项
		while (hItem != NULL)
		{
			ExpandTreeItem(hItem);
			hItem = m_tree.GetNextItem(hItem, TVGN_NEXT);
		}
	}
}

//-----------------------------------------------------------------------------
void CLayerTreeDlg::SetTreeCheck(HTREEITEM hTI /*= TVI_ROOT*/, BOOL bCheck /*= TRUE*/)
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
void CLayerTreeDlg::OnNMClickLayerTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	UINT uFlag;
	CPoint pt;
	CString sSelLayer;
	HTREEITEM SelRoot,ParRoot,NexRoot;

	pt = CPoint(this->GetCurrentMessage()->pt);
	m_tree.ScreenToClient(&pt);
	SelRoot = m_tree.HitTest(pt,&uFlag);

	if (NULL == SelRoot)
		return;

	if (uFlag & TVHT_ONITEMSTATEICON)
	{
		BOOL bChecked = m_tree.GetCheck(SelRoot);
		//置反选中状态
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

		m_tree.SetCheck(SelRoot, bChecked);

		ShowLayer(SelRoot, !bChecked);

//		sendCommandToAutoCAD("redraw ");
	}

	else if(!m_tree.ItemHasChildren(SelRoot)) 
	{
		CString strLayerName = m_tree.GetItemText(SelRoot);

		Acad::ErrorStatus es = Acad::eOk;

		AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();

		if (NULL == pDB)
			return;

		AcDbLayerTable *pLyTable = NULL;
		AcDbLayerTableRecord *pLyRcd = NULL;

		es = pDB->getSymbolTable(pLyTable,AcDb::kForRead);            //获得当前数据库中层表指针
		if ( es != Acad::eOk ) 
		{
			acutPrintf("\n获取LayerTable的时候发生错误，请稍后再试。\n");
			return;
		}

		if ( !pLyTable->has(strLayerName) )
		{
			pLyTable->close();
			return;
		}

		pLyTable->close();

		SetCurrentLayer(strLayerName);
	}
}

void CLayerTreeDlg::ShowLayer(HTREEITEM hItem, BOOL bShow)
{
	CString sLayerName = m_tree.GetItemText(hItem);

	HTREEITEM hChild = m_tree.GetNextItem(hItem,TVGN_CHILD);

	// 只有树叶才有对应的图层
	if (hChild == NULL)
	{
		ShowLayer(sLayerName, bShow);
		//如果是面层，还要把它的填充图层打开
		CString sHatchLayer;
		sHatchLayer.Format("%s_填充",sLayerName);
		bool hasHatch = false;
		acDocManager->lockDocument(acDocManager->curDocument());
		AcDbLayerTable * pLayerTbl = NULL;
		Acad::ErrorStatus es =
		acdbHostApplicationServices()->workingDatabase()->getSymbolTable(pLayerTbl,AcDb::kForRead);
		if (pLayerTbl->has(sHatchLayer))
		{
			AcDbLayerTableRecord * pLayerTbRcd = NULL;
			es = pLayerTbl->getAt(sHatchLayer,pLayerTbRcd,AcDb::kForWrite);
			pLayerTbRcd->setIsOff(bShow == 0);
			pLayerTbRcd->close();
			hasHatch = true;
		}
		pLayerTbl->close();
		acDocManager->unlockDocument(acDocManager->curDocument());
	}

	while(hChild != NULL)
	{
		ShowLayer(hChild, bShow);
		hChild = m_tree.GetNextItem(hChild,TVGN_NEXT);
	}
}

void CLayerTreeDlg::ShowLayer(CString strLayerName, BOOL bShow)
{
	CString strCommand = "ShowLayer";
	CStringArray strParamArray;

	CString strParam;
	strParam.Format("layer=%s", strLayerName);
	strParamArray.Add(strParam);
	strParam.Format("show=%d", bShow);
	strParamArray.Add(strParam);

	SendMessageToCad(strCommand, strParamArray);
}

LRESULT CLayerTreeDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch (message)
	{
	case WM_COMMAND:
		{
			switch( LOWORD(wParam) )
			{
			//case IDR_ADD_NODE: // 添加目录
			//	OnAddNode();
			//	break;
			}
		}
		break;
	case WM_NOTIFY:
		{
			LPNMHDR  pNMHDR = (LPNMHDR)lParam;

			if (m_tree.m_hWnd == pNMHDR->hwndFrom)
			{
				switch (pNMHDR->code)
				{
				case TVN_SELCHANGED:
//					OnTvSelChanged((NMHDR*)lParam);
					return 0;
				case NM_DBLCLK:
					OnTvDblClk((NMHDR*)lParam);
					return 0;
				case NM_RCLICK:
					OnTvRClick((NMHDR*)lParam);
					return 0;
				default:
					break;
				}
			}
		}
		break;
	default:
		break;
	}
	return CAcUiDialog::WindowProc(message, wParam, lParam);
}

BOOL CLayerTreeDlg::SetCurrentLayer(CString strCADLayer)
{
	acDocManager->lockDocument(curDoc());

	struct resbuf *Value = acutBuildList(RTSTR,strCADLayer,0); 

	int err = acedSetVar("clayer", Value);

	acutRelRb(Value);

	if( err != RTNORM)
	{
		acDocManager->unlockDocument(curDoc());

		acutPrintf("\n切换当前图层至 %s 的时候发生错误，请稍后再试。\n",strCADLayer);

		return FALSE;
	}

	acDocManager->unlockDocument(curDoc());

	acutPrintf("\n当前图层切换为 %s\n", strCADLayer);

	return TRUE;
}

void CLayerTreeDlg::OnTvSelChanged(NMHDR* pNMHDR)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here

	TV_ITEM tvi = pNMTreeView->itemNew;
	HTREEITEM hItem = tvi.hItem;

	if(m_tree.ItemHasChildren(hItem)) 
	{
		return;
	}

	CString strLayerName = m_tree.GetItemText(hItem);

	Acad::ErrorStatus es = Acad::eOk;

	AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();

	if (NULL == pDB)
		return;

	AcDbLayerTable *pLyTable = NULL;
	AcDbLayerTableRecord *pLyRcd = NULL;

	es = pDB->getSymbolTable(pLyTable,AcDb::kForRead);            //获得当前数据库中层表指针
	if ( es != Acad::eOk ) 
	{
		acutPrintf("\n获取LayerTable的时候发生错误，请稍后再试。\n");
		return;
	}

	if ( !pLyTable->has(strLayerName) )
	{
		pLyTable->close();
		return;
	}

	pLyTable->close();

	SetCurrentLayer(strLayerName);
}

void CLayerTreeDlg::OnTvDblClk(NMHDR* pNMHDR)
{

}

void CLayerTreeDlg::OnTvRClick(NMHDR* pNMHDR)
{
	CAcModuleResourceOverride resourceOverride; 	

	//得到鼠标位置
	POINT pos;
	::GetCursorPos(&pos);

	POINT myPoint = pos;
	m_tree.ScreenToClient(&myPoint);

	UINT uFlags;
	HTREEITEM hItem = m_tree.HitTest(myPoint, &uFlags);

	if (hItem == NULL || m_tree.ItemHasChildren(hItem) || !(TVHT_ONITEM & uFlags))
		return;

	CMenu menu;
	menu.LoadMenu(IDR_MENU1);

	menu.GetSubMenu(0)->TrackPopupMenu(TPM_RIGHTBUTTON,pos.x,pos.y,this);

}

//-----------------------------------------------------------------------------
void CLayerTreeDlg::OnBnClickedBtnSave() //保存图层选择状态
{
	CString strCommand = "SaveLayerState";
	CStringArray arrayParams;
	SendMessageToCad(strCommand, arrayParams);
}
