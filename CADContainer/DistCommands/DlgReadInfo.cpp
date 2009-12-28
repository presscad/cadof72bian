#include "StdAfx.h"
#include "resource.h"
#include "DlgReadInfo.h"
#include "DistEntityTool.h"
#include "DistXData.h"
#include "CommonFunc.h"
#include "DataDefine.h"
#include "..\SdeData\DistBaseInterface.h"

extern BOOL Glb_bMultiConnection;
extern IDistConnection * Glb_pConnection;         // SDE数据库连接对象指针
extern IDistConnection * Glb_pLocalConnection;         // 市局 SDE数据库连接对象指针

extern MapString2String Glb_mapFormItem;
extern POWER Glb_nFormPower; // 整个表单的权限

extern MapString2ContentTree Glb_mapTree; // 树结构对照表
extern MapLong2Power Glb_mapPower; // 图层权限对照表

struct ITEM_DATA 
{
	CString strID;
	CString strCadLayerName;
	CString strSdelayerName;
	CString strRowId;
};

typedef map<CString, ITEM_DATA> MapString2ItemData;

MapString2ItemData mapItemData;

struct COLUMN_VALUE
{
	CString strName;
	CString strAlias;
	CString strValue;
};

CArray<COLUMN_VALUE, COLUMN_VALUE> paramList;


//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC (CDlgReadInfo, CAcUiDialog)

BEGIN_MESSAGE_MAP(CDlgReadInfo, CAcUiDialog)
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, OnAcadKeepFocus)
	ON_MESSAGE(WM_ON_ENDEDIT, OnEndEdit)
	ON_BN_CLICKED(IDC_BTN_SELECT, OnBnClickedBtnSelect)
	ON_NOTIFY(NM_CLICK,IDC_TREE_OBJECT, OnClickObjectTree)
	ON_BN_CLICKED(IDC_SAVE, OnBnClickedSave)
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
CDlgReadInfo::CDlgReadInfo (CWnd *pParent /*=NULL*/, HINSTANCE hInstance /*=NULL*/) 
             :CAcUiDialog (CDlgReadInfo::IDD, pParent, hInstance) 
{
}

//-----------------------------------------------------------------------------
void CDlgReadInfo::DoDataExchange (CDataExchange *pDX) 
{
	CAcUiDialog::DoDataExchange (pDX) ;

	DDX_Control(pDX, IDC_BTN_SELECT,m_btnSelect);

	DDX_Control(pDX, IDC_LIST_XDATA, m_wndList);
	DDX_Control(pDX, IDC_TREE_OBJECT, m_wndTree);
}


LRESULT CDlgReadInfo::OnAcadKeepFocus (WPARAM, LPARAM) 
{
	return (TRUE) ;
}


BOOL CDlgReadInfo::OnInitDialog() 
{
	CAcUiDialog::OnInitDialog();

	m_btnSelect.AutoLoad();

	//初始化配置信息列表
	//ListCtrl初始化
	m_wndList.SetGridLines();
	m_wndList.SetFullSelectedStyle();
	m_wndList.SetExtendedStyle(LVS_EX_INFOTIP);
	m_wndList.SetHeadings( _T("名称,90;内容,120") );
	m_wndList.SetAllowEdit();
	m_wndList.SetEditTextColor(RGB(255,0,0));
	m_wndList.SetEditBkColor(RGB(255,255,255));

	//分割条初始化
	m_wndSplitterBar.Create(WS_CHILD|WS_BORDER|WS_DLGFRAME|WS_VISIBLE,
		CRect(0,0,0,0), this, IDC_SPLITTERBAR);
	m_wndSplitterBar.SetPanes(&m_wndTree, &m_wndList);

	GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);

	//读取图层信息
	InitLayerList();

	// 选择实体
	OnBnClickedBtnSelect();

	if (m_wndTree.GetCount() <= 0)
	{
		EndDialog(IDCANCEL);
		return FALSE;
	}

	HTREEITEM hItem = m_wndTree.GetFirstVisibleItem();

	// 显示属性
	OnSelectChanged(hItem);

	return TRUE;
}

void CDlgReadInfo::InitLayerList()
{
	CStringArray strArray;
	CDistEntityTool tempTool;
	tempTool.GetAllLayerName(strArray);
	CComboBox* pCmb = (CComboBox*) GetDlgItem(IDC_COMBO_CADLY);
	pCmb->ResetContent();
	int nCount = strArray.GetCount();
	for(int i=0; i<nCount; i++)
		pCmb->AddString(strArray.GetAt(i));

	pCmb->InsertString(0,"所有图层");
	pCmb->SetCurSel(0);
}

void CDlgReadInfo::OnBnClickedBtnSelect()
{
	//交互选择实体
	BeginEditorCommand();

	CString strCadName;
	CComboBox* pCmb = (CComboBox*)GetDlgItem(IDC_COMBO_CADLY);
	pCmb->GetLBText(pCmb->GetCurSel(),strCadName);

	
	ads_name ssName;
	struct resbuf *filter=NULL;
	if(strCadName.CompareNoCase("所有图层") != 0)
	{
		filter=acutNewRb(AcDb::kDxfLayerName);
		filter->restype=AcDb::kDxfLayerName;
		filter->resval.rstring = (char*) malloc((strCadName.GetLength()+1)*sizeof(char));
		strcpy(filter->resval.rstring,strCadName.GetBuffer(0));
		filter->rbnext=NULL;
	}
	
	if (acedSSGet(":S", NULL, NULL, filter, ssName) != RTNORM)
	{
		acutPrintf("\n选择实体有误!");
		acedSSFree(ssName); acutRelRb(filter);
		CompleteEditorCommand();
		return;
	}
	if(filter != NULL) acutRelRb(filter);

	AcDbObjectId tempObjId;
	ads_name ssTemp;
	long nNum = 0;
	int nPos = 0;
	acedSSLength(ssName,&nNum);
	CString strSdeName,strId,strRowId;

	m_wndTree.DeleteAllItems();

	// 清空属性列表
	ResetListItems();

	for(int i=0; i<nNum; i++)
	{
		acedSSName(ssName, i, ssTemp);
		if(Acad::eOk != acdbGetObjectId(tempObjId, ssTemp)) continue;
		strId.Format("%d",tempObjId.asOldId());
		CDistXData tempXData(tempObjId,"ROWID_OBJS");
		if(tempXData.Select("SDELYNAME",strSdeName)==FALSE) continue;
		if(tempXData.Select("OBJECTID",strRowId)==FALSE) continue;
		tempXData.Close();

		HTREEITEM hItem = m_wndTree.InsertItem(strId);

		AcDbEntity *pEnt = NULL;
		if (Acad::eOk != acdbOpenObject(pEnt, tempObjId, AcDb::kForRead))
		{
			return;
		}
		pEnt->close();

		ITEM_DATA xxItemData;
		xxItemData.strID = strId;
		xxItemData.strSdelayerName = strSdeName;
		xxItemData.strCadLayerName = pEnt->layer();
		xxItemData.strRowId = strRowId;

		mapItemData[strId] = xxItemData;

		nPos++;
	}
	
	acedSSFree(ssName);

	CompleteEditorCommand();

	if (m_wndTree.GetCount() <= 0)
	{
		return;
	}

	HTREEITEM hItem = m_wndTree.GetFirstVisibleItem();

	// 显示属性
	OnSelectChanged(hItem);
}

// 清空属性列表
void CDlgReadInfo::ResetListItems()
{
	m_wndList.DeleteAllItems();

	paramList.RemoveAll();

	GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);
}

void CDlgReadInfo::OnClickObjectTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	// Get mouse position.
	CPoint point;
	::GetCursorPos(&point);
	m_wndTree.ScreenToClient(&point);

	// Get hit item.
	HTREEITEM newItem = m_wndTree.HitTest(point);
	if (newItem == NULL)
	{
		return;
	}

	GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);

	OnSelectChanged(newItem);

	AcDbObjectId ObjId;
	ObjId.setFromOldId(atoi(m_strCurObjectId));
	FlashEntity(ObjId);
}

void CDlgReadInfo::OnSelectChanged(HTREEITEM hItem)
{
	// Quit previous field editing.
	m_wndList.EndEdit(FALSE);

	// 清空属性列表
	ResetListItems();

	CString strCadLayerName, strSdeLayerName,strRowId;

	m_strCurObjectId = m_wndTree.GetItemText(hItem);

	ITEM_DATA xxItemData = mapItemData[m_strCurObjectId];
	m_strCadlayerName = strCadLayerName = xxItemData.strCadLayerName;
	strSdeLayerName = xxItemData.strSdelayerName;
	strRowId = xxItemData.strRowId;

	STB_CONTENT_TREE xxContentTree = Glb_mapTree[strCadLayerName];

	IDistConnection* pConnection = Glb_pConnection;

	if (Glb_bMultiConnection)
	{
		pConnection = xxContentTree.bIsLocalLayer ? Glb_pLocalConnection : Glb_pConnection;
	}

	if(NULL == pConnection)
	{
		acutPrintf("\n空间数据库没有连接！"); return;
	}

	IDistParameter* pParam = NULL;
	int nParamNum = 0;
	IDistCommand* pCommand = CreateSDECommandObjcet();
	pCommand->SetConnectObj(pConnection);

	long nRet = pCommand->GetTableInfo(&pParam,&nParamNum,strSdeLayerName.GetBuffer(),"SDE");
	strSdeLayerName.ReleaseBuffer();
	if(1 != nRet)
	{
		pCommand->Release(); 
		return;
	}

	CString strRowIdColumn = "OBJECTID";
	CString strName; 
	for(int i=0; i<nParamNum; i++)
	{
		if(pParam[i].fld_nType == kShape) 
			continue;

		if(pParam[i].fld_nRowIdType == kRowIdColumnSde)
		{
			strRowIdColumn = pParam[i].fld_strName;
			continue;
		}

		strName = strName + pParam[i].fld_strName + ",";
	}

	if(strName.IsEmpty())
	{
		pCommand->Release(); 

		return;
	}

	char strSQL[1024]={0};
	sprintf(strSQL,"F:%sT:%s,W:%s=%s",strName, strSdeLayerName, strRowIdColumn, strRowId);

	IDistRecordSet* pRcdSet = NULL;
	if(1 != pCommand->SelectData(strSQL,&pRcdSet,NULL))
	{
		if(pRcdSet != NULL)
		{
			pRcdSet->Release();
		}

		pCommand->Release(); 

		return;
	}

	if(pRcdSet->BatchRead()==1)
	{
		for(int i=0; i<nParamNum; i++)
		{
			if(pParam[i].fld_nType == kShape) 
				continue;

			if(pParam[i].fld_nRowIdType == kRowIdColumnSde)
				continue;

			COLUMN_VALUE xxParam;
			xxParam.strName = pParam[i].fld_strName;
			xxParam.strAlias = pParam[i].fld_strAliasName;

			pRcdSet->GetValueAsString(xxParam.strValue.GetBuffer(128), xxParam.strName);
			xxParam.strValue.ReleaseBuffer();

			long nType = 0;
			switch(pParam[i].fld_nType)
			{
			case kInt16:
			case kInt32:
				nType = 1;
				break;
			case kFloat32:
			case kFloat64:
				nType = 2;
				break;
			case kDate:
				nType = 3;
				break;
			case kString:
			case kNString:
			case kBLOB:
			case kShape:
				break;
			default:
				break;
			}

			paramList.Add(xxParam);
			m_wndList.AddItem(nType, xxParam.strAlias, xxParam.strValue);

			/*			if (IsFieldEditable())
			{
			m_wndList.FreezeColumn(i);
			m_wndList.SetItemTextColor(i, 1, GetSysColor(COLOR_GRAYTEXT));
			}
			else
			{
			m_wndList.UnfreezeColumn(i);
			}
			*/		}
	}

	pRcdSet->Release();
	pCommand->Release();
}

void CDlgReadInfo::FlashEntity(AcDbObjectId ObjId) 
{ 
	LockDocument _Lock;

	AcDbEntity *pEnt=NULL; 
	AcTransaction* pTrans=actrTransactionManager->startTransaction();
	if(Acad::eOk != acdbOpenAcDbEntity(pEnt,ObjId,AcDb::kForWrite))
	{
		actrTransactionManager->endTransaction();
		return; 
	}
	pEnt->highlight(); 
	acedUpdateDisplay();


	DWORD dwCount=GetTickCount(); 
	while(1)
	{
		if(GetTickCount()-dwCount>=200) break;      
	}
	pEnt->unhighlight(); 
	acedUpdateDisplay();

	dwCount=GetTickCount(); 
	while(1)
	{
		if(GetTickCount()-dwCount>=200) break;      
	}
	pEnt->highlight(); 
	acedUpdateDisplay();
	dwCount=GetTickCount(); 
	while(1)
	{
		if(GetTickCount()-dwCount>=200) break;      
	}
	pEnt->unhighlight(); 
	acedUpdateDisplay();

	dwCount=GetTickCount(); 
	while(1)
	{
		if(GetTickCount()-dwCount>=200) break;      
	}
	pEnt->highlight(); 
	acedUpdateDisplay();
	dwCount=GetTickCount(); 
	while(1)
	{
		if(GetTickCount()-dwCount>=200) break;      
	}
	pEnt->unhighlight(); 
	acedUpdateDisplay();

	pEnt->close();

	actrTransactionManager->endTransaction();
}

void CDlgReadInfo::OnBnClickedSave()
{
	GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);

	if (m_strCurObjectId.IsEmpty())
		return;

	ITEM_DATA xxItemData = mapItemData[m_strCurObjectId];
	CString strSdeLayerName = xxItemData.strSdelayerName;
	CString strRowId = xxItemData.strRowId;

	STB_CONTENT_TREE xxContentTree = Glb_mapTree[xxItemData.strCadLayerName];

	IDistConnection* pConnection = Glb_pConnection;

	if (Glb_bMultiConnection)
	{
		pConnection = xxContentTree.bIsLocalLayer ? Glb_pLocalConnection : Glb_pConnection;
	}
/*
	// 检查sde表的权限
	POWER power = Glb_mapPower[xxContentTree.nId];

	// 沿用整个表单的权限
	if ((Glb_nFormPower.nPower >= 0) && (Glb_nFormPower.nPrior >= 0))
	{
		if (Glb_nFormPower.nPrior > power.nPrior)
		{
			power.nPower = Glb_nFormPower.nPower;
			power.nPrior = Glb_nFormPower.nPrior;
		}
	}

	BOOL bReadOnly = TRUE;
	if ((power.nPower >= 0) && (power.nPrior >= 0))
	{
		bReadOnly = (power.nPower != 2);
	}

	if (bReadOnly)
	{
		AfxMessageBox("只读图层不能修改!"); return;
		return;
	}
*/
	IDistCommand* pCommand = CreateSDECommandObjcet();
	pCommand->SetConnectObj(pConnection);

	int nCount = paramList.GetCount();
	char** pValues = new char*[nCount];

	CString strName; 
	for(int i=0; i<nCount; i++)
	{
		strName = strName + paramList.GetAt(i).strName + ",";

		CString strValue = m_wndList.GetItemText(i, 1);
		
		pValues[i] = new char[strValue.GetLength() + 1];
		memset(pValues[i], 0, strValue.GetLength() + 1);
		strcpy(pValues[i], strValue);
	}

	if(strName.IsEmpty())
	{
		pCommand->Release(); 
		
		return;
	}

	char strSQL[512]={0};
	sprintf(strSQL,"F:%sT:%s,W:OBJECTID=%s",strName.GetBuffer(),strSdeLayerName.GetBuffer(),strRowId.GetBuffer());

	strName.ReleaseBuffer();
	strSdeLayerName.ReleaseBuffer();
	strRowId.ReleaseBuffer();

	long rc = pCommand->UpdateDataAsString(strSQL,pValues);

	for(i=0; i<nCount; i++)
		delete[] pValues[i];
	delete[] pValues;
}

LRESULT CDlgReadInfo::OnEndEdit(WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加控件通知处理程序代码

	CString layers[] = 
	{
		"道路红线","道路中心线","道路设施线","道路注记","铁路中心线","铁路防护线","铁路设施线","铁路注记","红线辅助线",

			"河道蓝线","河道中心线","河道防护线","蓝线辅助线","河道注记",

			"城市黄线","黄线辅助线","黄线注记",

			"城市紫线","紫线辅助线","紫线注记",

			"城市绿线","绿线辅助线","绿线注记",

			"城市橙线","橙线辅助线","橙线注记",
	};

	if ((DocVars.docData().GetProjectId().CompareNoCase("0") == 0)) // 六线调整
	{
		int nCount = sizeof(layers)/sizeof(CString);

		for (int i=0; i<nCount; i++)
		{
			if (m_strCadlayerName.CompareNoCase(layers[i]) == 0)
			{
				GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
				break;
			}
		}

	}

	return S_OK;
}
