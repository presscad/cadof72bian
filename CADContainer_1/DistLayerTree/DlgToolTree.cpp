#include "StdAfx.h"
#include "resource.h"
#include "DlgToolTree.h"
#include "DistEntityTool.h"
#include  <string.h>
#include  <stdio.h>
#include "DlgLayerSort.h"
//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC (CDlgToolTree, CAcUiDialog)

BEGIN_MESSAGE_MAP(CDlgToolTree, CAcUiDialog)
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, OnAcadKeepFocus)
	ON_WM_DESTROY()
	ON_NOTIFY_EX( TTN_NEEDTEXT, 0, NotifyFunction )

	ON_BN_CLICKED(ID_BTN_OPEN, OnBnClickedBtnOpen)
	ON_BN_CLICKED(ID_BTN_CLOSE, OnBnClickedBtnClose)
	ON_BN_CLICKED(ID_BTN_DELETE, OnBnClickedBtnDelete)
	ON_BN_CLICKED(ID_BTN_CHECKALL, OnBnClickedBtnCheckAll)
	ON_BN_CLICKED(ID_BTN_NOCHECK, OnBnClickedBtnNoCheck)
	ON_BN_CLICKED(ID_BTN_SORT, OnBnClickedBtnSort)


	ON_NOTIFY(NM_CLICK, IDC_TOOLTREE, OnNMClickTooltree)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TOOLTREE, OnTvnSelchangedTooltree)
	ON_WM_CLOSE()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
CDlgToolTree::CDlgToolTree (CWnd *pParent /*=NULL*/, HINSTANCE hInstance /*=NULL*/) 
             :CAcUiDialog (CDlgToolTree::IDD, pParent, hInstance) 
{
	m_InfoList.RemoveAll();

	m_strSdeLyNameArray = NULL;
	m_pPowerArray = NULL;
}


void CDlgToolTree::OnDestroy()
{
	CAcUiDialog::OnDestroy();

	int nCount = m_InfoList.GetSize();
	if(NULL != m_strSdeLyNameArray)
	{
		for(int i=0; i<nCount; i++)
			delete m_strSdeLyNameArray[i];
		delete m_strSdeLyNameArray ; m_strSdeLyNameArray = NULL;
		delete m_pPowerArray; m_pPowerArray = NULL;
	}
	m_InfoList.RemoveAll();
}


BOOL CDlgToolTree::OnInitDialog()
{
	CAcUiDialog::OnInitDialog();

	CAcModuleResourceOverride resourceOverride ;
	// 添加菜单栏
	//ARX 2005
	//LPTBBUTTON btn = new TBBUTTON[6];
	LPTBBUTTON btn = new TBBUTTON[5];

	btn[0].iBitmap = 0; btn[0].idCommand = ID_BTN_OPEN; btn[0].fsState = TBSTATE_ENABLED;
	btn[0].fsStyle = TBSTYLE_BUTTON; btn[0].dwData = 0; btn[0].iString = 1;

	btn[1].iBitmap = 1;btn[1].idCommand = ID_BTN_CLOSE;btn[1].fsState = TBSTATE_ENABLED;
	btn[1].fsStyle = TBSTYLE_BUTTON;btn[1].dwData = 0;btn[1].iString = 1;

	btn[2].iBitmap = 2;btn[2].idCommand = ID_BTN_DELETE;btn[2].fsState = TBSTATE_ENABLED;
	btn[2].fsStyle = TBSTYLE_BUTTON;btn[2].dwData = 0;btn[2].iString = 1;

	btn[3].iBitmap = 3;btn[3].idCommand = ID_BTN_CHECKALL;btn[3].fsState = TBSTATE_ENABLED;
	btn[3].fsStyle = TBSTYLE_BUTTON;btn[3].dwData = 0;btn[3].iString = 1;

	btn[4].iBitmap = 4;btn[4].idCommand = ID_BTN_NOCHECK;btn[4].fsState = TBSTATE_ENABLED;
	btn[4].fsStyle = TBSTYLE_BUTTON;btn[4].dwData = 0;btn[4].iString = 1;

	//ARX 2005
	//btn[5].iBitmap = 5;btn[5].idCommand = ID_BTN_SORT;btn[5].fsState = TBSTATE_ENABLED;
	//btn[5].fsStyle = TBSTYLE_BUTTON;btn[5].dwData = 0;btn[5].iString = 1;

	DWORD dwStyle = TBSTYLE_FLAT | TBSTYLE_TOOLTIPS | WS_CHILD
		            | CCS_NORESIZE | WS_VISIBLE | CCS_NODIVIDER;

	m_wndtoolbarctrl.Create( dwStyle, CRect(4,4,360,32) ,this , IDR_TOOLBAR );

	// 由于TBSTYLE_FLAT属性中包含了TBSTYLE_TRANSPARENT，需要在此去掉
	m_wndtoolbarctrl.ModifyStyle(TBSTYLE_TRANSPARENT, 0, 0);
	//ARX 2005
	//m_wndtoolbarctrl.AddButtons(6, btn);
	//m_wndtoolbarctrl.AddBitmap(6, IDR_TOOLBAR);
	m_wndtoolbarctrl.AddButtons(5, btn);
	m_wndtoolbarctrl.AddBitmap(5, IDR_TOOLBAR);

	CSize sz_btn(16,15);
	m_wndtoolbarctrl.SetButtonSize(sz_btn);

	CSize sz_img(16,15);
	m_wndtoolbarctrl.SetBitmapSize(sz_img);

	m_wndtoolbarctrl.AutoSize();

	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);


	m_img.Create(16,16,ILC_COLOR,0,4);
	m_img.Add(AfxGetApp()->LoadIcon(IDI_ICON1));
	m_img.Add(AfxGetApp()->LoadIcon(IDI_ICON2));
	m_img.Add(AfxGetApp()->LoadIcon(IDI_ICON3));
	m_img.Add(AfxGetApp()->LoadIcon(IDI_ICON4));
	m_img.Add(AfxGetApp()->LoadIcon(IDI_ICON5));
	m_img.Add(AfxGetApp()->LoadIcon(IDI_ICON6));
	m_img.Add(AfxGetApp()->LoadIcon(IDI_ICON7));

	// 添加树型控件
	m_tree.ModifyStyleEx(0,WS_EX_STATICEDGE);
	m_tree.ModifyStyle(TVS_CHECKBOXES,0);
	m_tree.ModifyStyle(0,TVS_CHECKBOXES);
	m_tree.SetItemHeight(17);
	m_tree.SetImageList(&m_img,TVSIL_NORMAL);

	return TRUE;  
}

BOOL CDlgToolTree::NotifyFunction( UINT id, NMHDR * pTTTStruct, LRESULT * pResult )
{
	pResult = NULL ;	// Not Used 
	id = 0 ;	// Not used 

	TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pTTTStruct;
	UINT nID = pTTTStruct->idFrom;
	switch ( nID)
	{
	case ID_BTN_OPEN:
		pTTT->lpszText =_T("显示所有CAD图层");
		break;
	case ID_BTN_CLOSE:
		pTTT->lpszText = _T("隐藏选中实体图层");
		break;
	case ID_BTN_DELETE:
		pTTT->lpszText = _T("清除所有从空间库中读取的CAD图层");
		break;
	case ID_BTN_CHECKALL:
		pTTT->lpszText = _T("选中所有树节点");
		break;
	case ID_BTN_NOCHECK:
		pTTT->lpszText = _T("所有树节点不选中");
		break;
	case ID_BTN_SORT:
		pTTT->lpszText = _T("排列图层显示次序");
		break;
	}
	return(TRUE);
}

//-----------------------------------------------------------------------------
void CDlgToolTree::DoDataExchange (CDataExchange *pDX) 
{
	CAcUiDialog::DoDataExchange (pDX) ;
	DDX_Control(pDX, IDC_TOOLTREE, m_tree);
}

BOOL CDlgToolTree::OnCommand (WPARAM wParam, LPARAM lParam) 
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
LRESULT CDlgToolTree::OnAcadKeepFocus (WPARAM, LPARAM) 
{
	return (TRUE) ;
}
void CDlgToolTree::OnSize (UINT nType, int cx, int cy) 
{
	CAcUiDialog::OnSize (nType, cx, cy) ;
	MoveWindow (0, 0, cx, cy) ;
}

//-----------------------------------------------------------------------------
void CDlgToolTree::SizeChanged (CRect *lpRect, BOOL bFloating, int flags) 
{
	CAcModuleResourceOverride resourceOverride ;
	SetWindowPos (this, lpRect->left + 4, lpRect->top + 4, lpRect->Width() - 4, lpRect->Height() - 4, SWP_NOZORDER) ;

	m_tree.SetWindowPos( this,lpRect->left + 4, 32,lpRect->Width() - 6, lpRect->bottom - 48,SWP_NOZORDER);

	/*
	CStringArray strArrayCheck;
	int nCount = m_InfoList.GetCount();
	for(int j=0; j<nCount; j++)
	{
		LAYERTREECFG temp = m_InfoList.GetAt(j);
		if(m_tree.GetCheck(temp.hItem)==TRUE)
		{
			CString strTemp;
			strTemp.Format("%d",temp.nId);
			strArrayCheck.Add(strTemp);
		}
	}
	if(strArrayCheck.GetCount()>0)
		ReadAndWriteToFile(strArrayCheck,1);
		*/
}

int CDlgToolTree::ReadTreeInfoFromDB(IDistConnection* pConnect)
{
	m_InfoList.RemoveAll();
	if(pConnect == NULL) return -1;

	IDistCommand *pCommand = CreateSDECommandObjcet();
	if(NULL == pCommand) return -1;
	pCommand->SetConnectObj(pConnect);

	char strSQL[300]={0};
	sprintf(strSQL,"F:SELFID,OWNERID,NODENAME,SDELYNAME,POSORDER,T:TB_CONTENT_TREE,##");
	IDistRecordSet *pRcdSet = NULL;

	int rc =0;
	rc = pCommand->SelectData(strSQL,&pRcdSet);
	if(1 != rc)
	{
		pCommand->Release(); 
		if(NULL != pRcdSet) pRcdSet->Release();
		return rc;
	}

	while(pRcdSet->BatchRead() == 1)
	{
		LAYERTREECFG tempData;
		tempData.nId          = *(int*)(*pRcdSet)[0];
		tempData.nOwnerId     = *(int*)(*pRcdSet)[1];
		tempData.strNodeName  = (char*)(*pRcdSet)[2];
		tempData.strSdeLyName = (char*)(*pRcdSet)[3];
		tempData.strPosOrder  = (char*)(*pRcdSet)[4];
		tempData.nPower = 1;

		int nNum = m_InfoList.GetSize();
		int nPos = 0;
		BOOL bFound = FALSE;
		for(int i=0; i<nNum; i++)  //将结果按照次序编号从小到大的次序排列
		{
			LAYERTREECFG tempInfo = m_InfoList.GetAt(i);
			if(tempInfo.strPosOrder > tempData.strPosOrder)  
			{
				nPos = i; bFound = TRUE;
				break;
			}
		}

		if((bFound==FALSE)&&(nNum>0))
		{
			nPos = nNum;
		}

		m_InfoList.InsertAt(nPos,tempData);	
	}
	pRcdSet->Release();

	if(m_InfoList.GetSize()==0)
	{
		pCommand->Release(); return 0;
	}

	//读所有SDE层对应的CAD
	sprintf(strSQL,"F:SDELYNAME,CADLYNAME,T:CADMAPTOSDE,##");
	rc = pCommand->SelectData(strSQL,&pRcdSet);
	if(1 != rc)
	{
		pCommand->Release(); 
		if(NULL != pRcdSet) pRcdSet->Release();
		return rc;
	}
	int nCount = m_InfoList.GetSize();
	CString strSdeLyName,strCadLyName;
	while(pRcdSet->BatchRead() == 1)
	{
		strSdeLyName = (char*)(*pRcdSet)[0];
		strCadLyName = (char*)(*pRcdSet)[1];

		for(int j=0; j<nCount; j++)
		{
			LAYERTREECFG tempData = m_InfoList.GetAt(j);
			if(0 == strSdeLyName.CompareNoCase(tempData.strSdeLyName))
			{
				tempData.strCadLyName = strCadLyName;
				m_InfoList.SetAt(j,tempData); break;
			}
		}
	}
	pCommand->Release();

	return 1;
}

int CDlgToolTree::SetCheckState(CStringArray* strArrayCheck)
{
	int nCount = m_InfoList.GetSize();
	if(nCount == 0) return 0;

	int nCheck = strArrayCheck->GetCount();
	if(nCheck == 0) return 0;

	int nTempId = 0;
	int nPos = 1;
	for(int i=0; i<nCount; i++)
	{
		LAYERTREECFG tempDataSub = m_InfoList.GetAt(i);

		for(int q=0; q<nCheck; q++)
		{
			CString strCheck = strArrayCheck->GetAt(q);
			nTempId = atoi(strCheck);
			if(nTempId == tempDataSub.nId)
			{
				if(tempDataSub.nPower != 3)
				{
					HTREEITEM hh = m_tree.GetParentItem(tempDataSub.hItem);
					if(NULL != hh)
						m_tree.SetCheck(hh,TRUE);
					m_tree.SetCheck(tempDataSub.hItem,TRUE); break;

				}
			}
		}
	}

	return 1;
}

int CDlgToolTree::CreateTreeByData()
{
	m_tree.DeleteAllItems();
	int nCount = m_InfoList.GetSize();
	if(nCount == 0) return 0;

	///临时
	//CStringArray strArrayCheck;
	//ReadAndWriteToFile(strArrayCheck,0);

	//int nCheck = strArrayCheck.GetCount();

	int nTempId = 0;
	int nPos = 1;
	for(int i=0; i<nCount; i++)
	{
		LAYERTREECFG tempDataSub = m_InfoList.GetAt(i);
		CString strOrder = tempDataSub.strPosOrder;
		if(tempDataSub.nPower == 3) 
		{
			tempDataSub.hItem = NULL;
			m_InfoList.SetAt(i,tempDataSub);
			continue;  //不可见,就继续
		}
		if(strOrder.Find('A')>=0)
		{
			HTREEITEM hSub = m_tree.InsertItem(tempDataSub.strNodeName,1,1,TVI_ROOT);
			tempDataSub.hItem = hSub;
			//for(int q=0; q<nCheck; q++)
			//{
			//	CString strCheck = strArrayCheck.GetAt(q);
			//	nTempId = atoi(strCheck);
			//	if(nTempId == tempDataSub.nId)
			//	{
			//		if(tempDataSub.nPower != 3)
			//		{
			//			m_tree.SetCheck(hSub,TRUE); break;
			//		}
			//	}
			//}
			
			m_InfoList.SetAt(i,tempDataSub);  //保存树节点句柄
		}
		else
		{
			for(int k=0; k<nCount; k++)
			{
				LAYERTREECFG tempDataMain = m_InfoList.GetAt(k);
				if(tempDataMain.nId == tempDataSub.nOwnerId) //比较句柄
				{
					HTREEITEM hMain = tempDataMain.hItem;
					if(hMain==NULL)
					{
						tempDataSub.hItem = NULL;
						tempDataSub.nPower = 3;
						m_InfoList.SetAt(i,tempDataSub);
						break;
					}
					int nImgIndex = 2;
					if(tempDataSub.strSdeLyName.IsEmpty() || tempDataSub.strSdeLyName.Find("SDE图层")>=0) nImgIndex = 1;

					HTREEITEM hSub = m_tree.InsertItem(tempDataSub.strNodeName,nImgIndex,nImgIndex,hMain);

					//for(int q=0; q<nCheck; q++)
					//{
					//	CString strCheck = strArrayCheck.GetAt(q);
					//	nTempId = atoi(strCheck);
					//	if(nTempId == tempDataSub.nId)
					//	{
					//		if(tempDataSub.nPower != 3)
					//		{
					//			HTREEITEM hh = m_tree.GetParentItem(hSub);
					//			if(NULL != hh)
					//				m_tree.SetCheck(hh,TRUE);
					//			m_tree.SetCheck(hSub,TRUE); break;

					//		}
					//	}
					//}
					tempDataSub.hItem = hSub;
					m_InfoList.SetAt(i,tempDataSub);
					//m_tree.Expand(hMain,TVE_EXPAND);
					break;
				}
			}
	
		}
	}

	//将没有子节点的根节点,不可见
	for(int jj=nCount-1; jj>=0; jj--)
	{
		LAYERTREECFG tempMain = m_InfoList.GetAt(jj);
		CString strTemp = tempMain.strSdeLyName;
		strTemp = strTemp.Trim();
		if(strTemp.IsEmpty() || tempMain.strSdeLyName.Find("SDE图层")>=0)
		{
			BOOL bFound = FALSE;
			for(int qq=nCount-1; qq>=0; qq--)
			{
				LAYERTREECFG tempSub =  m_InfoList.GetAt(qq);
				if(tempMain.nId == tempSub.nOwnerId)
				{
					bFound = TRUE; break;
				}
			}
			if(bFound==FALSE)
				m_tree.DeleteItem(tempMain.hItem);
		}
	}
	return 1;
}



//创建图层树
int CDlgToolTree::CreateTree(char* strServerName,char* strInstanceName,
				              char* strDBName,char* strUserName,char* strPassword)
{
	//创建SDE连接
	IDistConnection* pConnect = CreateSDEConnectObjcet();
	if(pConnect == NULL) return -1;
	int rc = 0;
	rc = pConnect->Connect(strServerName,strInstanceName,strDBName,strUserName,strPassword);
	if(rc != 1)
	{
		pConnect->Release();
		return rc;
	}
	rc = ReadTreeInfoFromDB(pConnect);
	pConnect->Release();
	if(rc != 1) return rc;

	return CreateTreeByData();
}

//设置权限
int CDlgToolTree::SetTreeLyPowerAndReCreate(int* pIdArray,int* pPowerArray,int* pNum)
{
	if(NULL == pIdArray || NULL == pPowerArray || NULL == pNum) return -1;
	int nNum = *pNum;
	int nCount = m_InfoList.GetCount();
	for(int i=0; i<nNum; i++)
	{
		int nId = pIdArray[i];
		int nPower = pPowerArray[i];
		for(int j=0; j<nCount; j++)
		{
			LAYERTREECFG tempData = m_InfoList.GetAt(j);
			if(tempData.nId == nId)
			{
				tempData.nPower = nPower;
				m_InfoList.SetAt(j,tempData);
				break;
			}
		}
	}
	
	return CreateTreeByData();
}

//获取SDE图层名称和权限值(外部调用,无需开辟空间)
int CDlgToolTree::GetTreeLyPower(char*** strSdeLyNameArray,int** pPowerArray,int* pNum)
{
	int nCount = m_InfoList.GetCount();
	if(nCount == 0) return 0;
	///临时
	//SaveTreeCfgToFile();//##############################################################
	if(NULL == m_strSdeLyNameArray)  //只创建一次 
	{
		m_pPowerArray = new int[nCount];
		m_strSdeLyNameArray = new char*[nCount];
		for(int i=0; i<nCount; i++)
		{
			m_strSdeLyNameArray[i] = new char[255];
			memset(m_strSdeLyNameArray[i],0,sizeof(char)*255);
		}
	}
	*pNum = 0;

	int nNum = 0;
	CString strSdeName;
	int nPower =0;
	for(int j=0; j<nCount; j++)
	{
		LAYERTREECFG tempData = m_InfoList.GetAt(j);
		strSdeName = tempData.strSdeLyName;
		nPower = tempData.nPower;
		if(nPower == 3) continue;
		int k=strSdeName.GetLength()-1;
	
		strSdeName = strSdeName.Trim();
		if(strSdeName.IsEmpty() || strSdeName.Find("SDE图层")>=0) continue;  //根节点,没有对应SDE图层

		if(m_tree.GetCheck(tempData.hItem)==FALSE) continue;

		strcpy(m_strSdeLyNameArray[nNum],strSdeName.GetBuffer(0));
		//m_pPowerArray[nNum] = nPower;
		///临时
		m_pPowerArray[nNum] = 1;
		nNum++;
	}
	*pNum = nNum;
	*strSdeLyNameArray = m_strSdeLyNameArray;
	*pPowerArray = m_pPowerArray;

	return 1;
}

void CDlgToolTree::OnBnClickedBtnOpen()
{
	CStringArray strLyNameArray;
	CDistEntityTool tempTool;
	tempTool.GetAllLayerName(strLyNameArray);
	int nNum = strLyNameArray.GetCount();
	for(int i=0; i<nNum; i++)
	{
		Acad::ErrorStatus es ;
		CString strTemp = strLyNameArray.GetAt(i);
		AcApDocument* pDoc = acDocManager->curDocument();
		if(acDocManager->lockDocument(pDoc)==Acad::eOk)
		{
			AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();
			AcDbLayerTable *pLyTable = NULL;
			AcDbLayerTableRecord *pLyRcd = NULL;
			pDB->getSymbolTable(pLyTable,AcDb::kForRead);            //获得当前数据库中层表指针
			if(Acad::eOk != pLyTable->getAt(strTemp.GetBuffer(0), pLyRcd, AcDb::kForWrite))//在pLyRcd打开strLyName指定的图层并且返回指针到被打开的pLyRcd图层
			{
				pLyTable->close(); break;
			}
			pLyTable->close();

			if (pLyRcd->isOff() == Adesk::kTrue)                    //查询图层开关状态，kFalse为开
			{
				pLyRcd->setIsOff(Adesk::kFalse);                      //打开层
			}
			pLyRcd->close();

			acedUpdateDisplay();
			es = acDocManager->unlockDocument(pDoc);
			acedUpdateDisplay();
			acedGetAcadFrame()->SetFocus();
		}
	}
}


void CDlgToolTree::OnCloseAllLayer()
{
	CStringArray strLyNameArray;
	CDistEntityTool tempTool;
	tempTool.GetAllLayerName(strLyNameArray);
	int nNum = strLyNameArray.GetCount();
	for(int i=0; i<nNum; i++)
	{
		Acad::ErrorStatus es ;
		CString strTemp = strLyNameArray.GetAt(i);
		AcApDocument* pDoc = acDocManager->curDocument();
		if(acDocManager->lockDocument(pDoc)==Acad::eOk)
		{
			AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();
			AcDbLayerTable *pLyTable = NULL;
			AcDbLayerTableRecord *pLyRcd = NULL;
			pDB->getSymbolTable(pLyTable,AcDb::kForRead);            //获得当前数据库中层表指针
			if(Acad::eOk != pLyTable->getAt(strTemp.GetBuffer(0), pLyRcd, AcDb::kForWrite))//在pLyRcd打开strLyName指定的图层并且返回指针到被打开的pLyRcd图层
			{
				pLyTable->close(); break;
			}
			pLyTable->close();

			if (pLyRcd->isOff() == Adesk::kFalse)                    //查询图层开关状态，kFalse为开
			{
				pLyRcd->setIsOff(Adesk::kTrue);                      //打开层
			}
			pLyRcd->close();

			acedUpdateDisplay();
			es = acDocManager->unlockDocument(pDoc);
			acedUpdateDisplay();
			acedGetAcadFrame()->SetFocus();
		}
	}
}


void CDlgToolTree::OnBnClickedBtnClose()
{
	ads_name ssName;
	while(acedSSGet(":S", NULL, NULL, NULL, ssName) == RTNORM)
	{
		ads_name ssTemp;
		AcDbObjectId tempObjId;
		acedSSName(ssName, 0, ssTemp);
		acdbGetObjectId(tempObjId, ssTemp);
		acedSSFree(ssName);

		AcDbEntity* pEnt = NULL;
		if(Acad::eOk == acdbOpenAcDbEntity(pEnt,tempObjId,AcDb::kForRead)) 
		{
			char* pLyName = pEnt->layer();
			pEnt->close();

			AcApDocument* pDoc = acDocManager->curDocument();
			if(acDocManager->lockDocument(pDoc)==Acad::eOk)
			{
				AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();
				AcDbLayerTable *pLyTable = NULL;
				AcDbLayerTableRecord *pLyRcd = NULL;
				Acad::ErrorStatus es = pDB->getSymbolTable(pLyTable,AcDb::kForRead);//获得当前数据库中层表指针
				if(Acad::eOk != es) {
					acutPrintf("\n\n"); return;
				}
				es= pLyTable->getAt(pLyName, pLyRcd, AcDb::kForWrite);//在pLyRcd打开strLyName指定的图层并且返回指针到被打开的pLyRcd图层
				pLyTable->close();
				if(Acad::eOk != es)  {
					acutPrintf("\n\n"); return;
				}

				if (pLyRcd->isOff() == Adesk::kFalse)                    //查询图层开关状态，kFalse为开
				{
					pLyRcd->setIsOff(Adesk::kTrue);                      //关闭层
				}
				pLyRcd->close();
				es = acDocManager->unlockDocument(pDoc);
				acedGetAcadFrame()->SetFocus();
				acedUpdateDisplay();
			}			
		}
	}
	acutPrintf("\n\n\n");
}


void CDlgToolTree::OnBnClickedBtnDelete()
{
	CStringArray strLyNameArray;
	CDistEntityTool tempTool;
	tempTool.GetAllLayerName(strLyNameArray);
	int nNum = strLyNameArray.GetCount();


	CString strTemp;
	for(int i=0; i<nNum; i++)
	{
		strTemp = strLyNameArray.GetAt(i);
		strTemp.MakeUpper();
		if(strTemp.Find("SDE_")>=0)
		{
			AcApDocument* pDoc = acDocManager->curDocument();
			if(acDocManager->lockDocument(pDoc)==Acad::eOk)
			{
				tempTool.DeleteAllEntityInLayer(strTemp.GetBuffer()); 
				acedUpdateDisplay();
				acDocManager->unlockDocument(pDoc);
			}
		}
	}

	/*
	int nCount = m_InfoList.GetCount();
	CString strTemp,strCadName;
	for(int i=0; i<nNum; i++)
	{
		strCadName = strLyNameArray.GetAt(i);
		for(int j=0; j<nCount; j++)
		{
			LAYERTREECFG temp = m_InfoList.GetAt(j);
			strTemp = temp.strCadLyName;

			if((strTemp.CompareNoCase(strCadName)==0) ||(strTemp.Find("SDE_")>=0))
			{
				AcApDocument* pDoc = acDocManager->curDocument();
				if(acDocManager->lockDocument(pDoc)==Acad::eOk)
				{
					tempTool.DeleteAllEntityInLayer(strTemp.GetBuffer()); 
					acedUpdateDisplay();
					acDocManager->unlockDocument(pDoc);
					break;
				}
			}
		}
	}
	*/
}


void CDlgToolTree::OnBnClickedBtnCheckAll()
{
	int nCount = m_InfoList.GetCount();
	for(int i=0; i<nCount; i++)
	{
		LAYERTREECFG temp = m_InfoList.GetAt(i);
		HTREEITEM hItem = temp.hItem;
		if(hItem != NULL)
			m_tree.SetCheck(hItem,TRUE);
	}
}


void CDlgToolTree::OnBnClickedBtnNoCheck()
{
	int nCount = m_InfoList.GetCount();
	for(int i=0; i<nCount; i++)
	{
		LAYERTREECFG temp = m_InfoList.GetAt(i);
		HTREEITEM hItem = temp.hItem;
		if(hItem != NULL)
			m_tree.SetCheck(hItem,FALSE);
	}
}

void CDlgToolTree::OnBnClickedBtnSort()
{
	//CAcModuleResourceOverride _UseThisRes;
	//CDlgLayerSort dlg(CWnd::FromHandle(adsw_acadMainWnd()));
	//dlg.DoModal();

	CDistEntityTool tempTool;
	tempTool.SendCommandToAutoCAD("SORTLAYER ");
}


void CDlgToolTree::OnNMClickTooltree(NMHDR *pNMHDR, LRESULT *pResult)
{
	UINT uFlag;
	CPoint pt;
	HTREEITEM SelRoot,ParRoot,NexRoot;
	GetCursorPos(&pt);
	m_tree.ScreenToClient(&pt);
	SelRoot = m_tree.HitTest(pt,&uFlag);  
	if ((NULL!=SelRoot) && (uFlag&TVHT_ONITEMSTATEICON)) //得到当前选择节点
	{

		BOOL bChecked = m_tree.GetCheck(SelRoot);
		SetParentCheck(SelRoot,!bChecked);
		SetChildCheck(SelRoot,!bChecked);
		/*
		BOOL bChecked = m_tree.GetCheck(SelRoot);

		//设置选择节点和其子节点
		SetTreeCheck(SelRoot, !bChecked);

		//分析是否有必要对树枝进行调整
		ParRoot = m_tree.GetParentItem(SelRoot);
		while (ParRoot != NULL)
		{
			//if (bChecked)
				//m_tree.SetCheck(ParRoot,TRUE);//当前处在被选择状态即将被取消选择
			//else
			if(!bChecked)
			{
				m_tree.SetCheck(ParRoot,TRUE);
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
	*/
	}

}

void CDlgToolTree::OnTvnSelchangedTooltree(NMHDR *pNMHDR, LRESULT *pResult)
{

	*pResult = 0;
}


CString CDlgToolTree::GetCurArxAppPath(CString strArxName)
{
	TCHAR exeFullPath[MAX_PATH];
	CString strPath; 
	GetModuleFileName(GetModuleHandle(strArxName),exeFullPath,MAX_PATH); 
	strPath=(CString)exeFullPath; 
	int position=strPath.ReverseFind('\\'); 
	strPath=strPath.Left(position+1); 
	return strPath;
}



void CDlgToolTree::SetTreeCheck(HTREEITEM hTI /*= TVI_ROOT*/, BOOL bCheck /*= TRUE*/)
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

void CDlgToolTree::OnClose()
{
	/*
	CStringArray strArrayCheck;
	int nCount = m_InfoList.GetCount();
	for(int j=0; j<nCount; j++)
	{
		LAYERTREECFG temp = m_InfoList.GetAt(j);
		if(m_tree.GetCheck(temp.hItem)==TRUE)
		{
			CString strTemp;
			strTemp.Format("%d",temp.nId);
			strArrayCheck.Add(strTemp);
		}
	}
	if(strArrayCheck.GetCount()>0)
	{
		ReadAndWriteToFile(strArrayCheck,1);
	}
	*/
	CAcUiDialog::OnClose();
}

void CDlgToolTree::OnKillFocus(CWnd* pNewWnd)
{
	CAcUiDialog::OnKillFocus(pNewWnd);

}

void CDlgToolTree::SetParentCheck(HTREEITEM hItem,BOOL bCheck)
{
	if(NULL == hItem) return;
	if(bCheck==TRUE)
	{
		HTREEITEM hParent = m_tree.GetParentItem(hItem);
		while(hParent != NULL)
		{
			m_tree.SetCheck(hParent,TRUE);
			hParent = m_tree.GetParentItem(hParent);
		}
	}
	else
	{
		HTREEITEM hParent = m_tree.GetParentItem(hItem);
		while(hParent != NULL)
		{
			HTREEITEM hNext = m_tree.GetChildItem(hParent);
			while(hNext != NULL)
			{
				if(hNext == hItem)
				{
					hNext = m_tree.GetNextItem(hNext,TVGN_NEXT);
					continue;
				}
				if(m_tree.GetCheck(hNext)) return;
				hNext = m_tree.GetNextItem(hNext,TVGN_NEXT);
			}
			m_tree.SetCheck(hParent,FALSE);
			hParent = m_tree.GetParentItem(hParent);
		}
	}
}


void CDlgToolTree::SetChildCheck(HTREEITEM hItem,BOOL bCheck)
{
	if(NULL == hItem) return;
	HTREEITEM hChild = m_tree.GetChildItem(hItem);
	while(hChild != NULL)
	{
		m_tree.SetCheck(hChild,bCheck);
		SetChildCheck(hChild,bCheck);
		hChild = m_tree.GetNextItem(hChild,TVGN_NEXT);
	}	
}

void CDlgToolTree::SaveTreeCfgToFile()
{
	CStringArray strArrayCheck;
	int nCount = m_InfoList.GetCount();
	for(int j=0; j<nCount; j++)
	{
		LAYERTREECFG temp = m_InfoList.GetAt(j);
		if(m_tree.GetCheck(temp.hItem)==TRUE)
		{
			CString strTemp;
			strTemp.Format("%d",temp.nId);
			strArrayCheck.Add(strTemp);
		}
	}
	if(strArrayCheck.GetCount()>0)
	{
		ReadAndWriteToFile(strArrayCheck,1);
	}
}

//获取图层树钩选状态
CString CDlgToolTree::GetCheckState()
{
	CString strData = "";
	
	CStringArray strArrayCheck;
	int nCount = m_InfoList.GetCount();
	for(int j=0; j<nCount; j++)
	{
		LAYERTREECFG temp = m_InfoList.GetAt(j);
		if(m_tree.GetCheck(temp.hItem)==TRUE)
		{
			CString strTemp;
			strTemp.Format("%d",temp.nId);
			strArrayCheck.Add(strTemp);
		}
	}
	if(strArrayCheck.GetCount()>0)
	{
		int nCount = 0;
		char strBuf[10] = {0};
		nCount = strArrayCheck.GetCount();
		if(nCount>0)
		{
			for(int i=0; i<nCount; i++)
			{
				CString strTemp = strArrayCheck.GetAt(i);
				strTemp+=",";
				strData+=strTemp;
			}
			if (strData.GetLength() > 0)
				strData = strData.Left(strData.GetLength()-1);
			//Write(strDataBuf,nCount*10);
			//delete[] strDataBuf; strDataBuf = NULL;
		}
	}
	return strData;
}

void CDlgToolTree::ReadAndWriteToFile(CStringArray& strArrayCheck,int nTag)
{
	CString strFName;
	strFName.Format("%sUserCheck.dat",GetCurArxAppPath("shDistLayerTree.arx"));
	
	int nCount = 0;
	char* strDataBuf = NULL;
	char strBuf[10] = {0};

	if(nTag == 0) //读
	{
		strArrayCheck.RemoveAll();
		CFile file;
		int nLen = 0;
		if(file.Open(strFName,CFile::modeRead))
		{
			nLen = file.GetLength();
			if(nLen < 10){
				file.Close(); return;
			}
			strDataBuf = new char[nLen];
			memset(strDataBuf,0,sizeof(char)*nLen);
			file.Read(strDataBuf,nLen);
			file.Close();
		}
		else
		{
			acutPrintf("\n file [%s] not found!",strFName);
			return ;
		}

		nCount = nLen/10;
		for(int i=0;i<nCount; i++)
		{
			memset(strBuf,0,sizeof(char)*10);
			memcpy(strBuf,strDataBuf+10*i,10);
			strArrayCheck.Add(strBuf);
		}
		delete[] strDataBuf; strDataBuf = NULL;
	}
	else
	{
		nCount = strArrayCheck.GetCount();
		if(nCount==0) return;
		strDataBuf = new char[nCount*10];
		for(int i=0; i<nCount; i++)
		{
			CString strTemp = strArrayCheck.GetAt(i);
			memset(strBuf,0,sizeof(char)*10);
			strcpy(strBuf,strTemp.GetBuffer(0));
			memcpy(strDataBuf+10*i,strBuf,10);
		}
		CFile file;
		if(file.Open(strFName,CFile::modeCreate | CFile::modeWrite))
		{
			file.Write(strDataBuf,nCount*10);
			file.Close();
		}
		delete[] strDataBuf; strDataBuf = NULL;
	}
}

/*
void CDlgToolTree::ReadAndWriteToFile(CStringArray& strArrayCheck,int nTag)
{
CString strFName;
strFName.Format("%sUserCheck.dat",GetCurArxAppPath("shDistLayerTree.arx"));
FILE* fp=NULL;
CString strTemp;
char str[10]={0};
try
{
if(nTag == 0) //读
{
strArrayCheck.RemoveAll();
fp = fopen(strFName.GetBuffer(0),"a+");//strFName.GetBuffer(0
if(fp == NULL){
acutPrintf("\nOpen file error!"); return;
}

while(!feof(fp))
{
memset(str,0,sizeof(char)*10);
fread(str,1,10,fp);
strArrayCheck.Add(str);
}
fclose(fp);
}
else
{
fp = fopen(strFName.GetBuffer(0),"w+");
if(fp == NULL)
{
acutPrintf("\nOpen file error!"); return;
}
int nNum = strArrayCheck.GetCount();
for(int i=0; i<nNum; i++)
{
memset(str,0,10*sizeof(char));
strTemp = strArrayCheck.GetAt(i);
strcpy(str,strTemp.GetBuffer(0));
fwrite(str,1,10,fp);
}
fclose(fp);
}
}
catch (...)
{
if(fp != NULL)
fclose(fp);
}

}

*/
