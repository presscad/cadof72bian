#include "StdAfx.h"
#include "resource.h"
#include "DlgReadInfo.h"
#include "DistEntityTool.h"
#include "DistXData.h"
#include "..\SdeData\DistBaseInterface.h"

extern IDistConnection*    g_pConnection;         //SDE数据库连接对象指针

//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC (CDlgReadInfo, CAcUiDialog)

BEGIN_MESSAGE_MAP(CDlgReadInfo, CAcUiDialog)
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, OnAcadKeepFocus)
	ON_BN_CLICKED(IDC_BTN_SELECT, OnBnClickedBtnSelect)
	ON_NOTIFY(NM_CLICK, IDC_LIST_OBJ, OnNMClickListObj)
	ON_WM_ACTIVATE()
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

	DDX_Control(pDX, IDC_LIST_XDATA, m_list_xdata);
	DDX_Control(pDX, IDC_LIST_OBJ, m_list_obj);
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
	m_list_xdata.SetExtendedStyle(m_list_xdata.GetExtendedStyle()| LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_list_xdata.InsertColumn(0,"信息名称",LVCFMT_LEFT,90);
	m_list_xdata.InsertColumn(1,"信息内容",LVCFMT_LEFT,200);

	m_list_obj.SetExtendedStyle(m_list_obj.GetExtendedStyle()| LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_list_obj.InsertColumn(0,"实体",LVCFMT_LEFT,140);
	m_list_obj.InsertColumn(1,"SDE名称",LVCFMT_LEFT,0);
	m_list_obj.InsertColumn(2,"RowId",LVCFMT_LEFT,0);

	CComboBox* pCmb = (CComboBox*) GetDlgItem(IDC_COMBO_CADLY);
	pCmb->ResetContent();
	pCmb->InsertString(0,"所有图层");
	pCmb->SetCurSel(0);

	return TRUE;
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
	m_list_obj.DeleteAllItems();
	for(int i=0; i<nNum; i++)
	{
		acedSSName(ssName, i, ssTemp);
		if(Acad::eOk != acdbGetObjectId(tempObjId, ssTemp)) continue;
		strId.Format("%d",tempObjId.asOldId());

		CDistXData tempXData(tempObjId,"ROWID_OBJS");
		if(tempXData.Select("SDELYNAME",strSdeName)==FALSE) continue;
		if(tempXData.Select("OBJECTID",strRowId)==FALSE) continue;
		tempXData.Close();

		m_list_obj.InsertItem(nPos,strId);
		m_list_obj.SetItemText(nPos,1,strSdeName);
		m_list_obj.SetItemText(nPos,2,strRowId);
		nPos++;
	}
	
	acedSSFree(ssName);

	CompleteEditorCommand();

	
	if(m_list_obj.GetItemCount()>0)
	{
		m_list_obj.SetSelectionMark(0);
		ReadEntityInfo();
	}
}

void CDlgReadInfo::OnNMClickListObj(NMHDR *pNMHDR, LRESULT *pResult)
{
	
	ReadEntityInfo();
	*pResult = 0;
}


void CDlgReadInfo::FlashEntity(AcDbObjectId ObjId) 
{ 
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




void CDlgReadInfo::ReadEntityInfo()
{
	if(NULL == g_pConnection)
	{
		acutPrintf("\n空间数据库没有连接！"); return;
	}

	int nSelect = m_list_obj.GetSelectionMark();
	if(nSelect < 0) return;

	CString strId,strSdeName,strRowId;
	strId      = m_list_obj.GetItemText(nSelect,0);
	strSdeName = m_list_obj.GetItemText(nSelect,1);
	strRowId   = m_list_obj.GetItemText(nSelect,2);


	IDistParameter* pParam = NULL;
	int nParamNum = 0;
	IDistCommand* pCommand = CreateSDECommandObjcet();
	pCommand->SetConnectObj(g_pConnection);
	// 临时
	//if(1 != pCommand->GetTableInfo(&pParam,&nParamNum,strSdeName.GetBuffer(0),"XMGHSDE")){
	if(1 != pCommand->GetTableInfo(&pParam,&nParamNum,strSdeName.GetBuffer(0),"SDE")){
		pCommand->Release(); return;
	}

	CStringArray strFNameArray,strFValueArray;
	CString strName; 
	strName.Empty();
	for(int i=0; i<nParamNum; i++)
	{
		if(pParam[i].fld_nType == kShape) continue;
		if(pParam[i].fld_nRowIdType == kRowIdColumnSde) continue;
		strName = strName + pParam[i].fld_strName + ",";
		strFNameArray.Add(pParam[i].fld_strAliasName);
	}

	if(strName.IsEmpty())
	{
		pCommand->Release(); return;
	}

	char strSQL[1024]={0};
	sprintf(strSQL,"F:%sT:%s,W:OBJECTID=%s",strName.GetBuffer(0),strSdeName.GetBuffer(0),strRowId.GetBuffer(0));
	IDistRecordSet* pRcdSet = NULL;
	if(1 != pCommand->SelectData(strSQL,&pRcdSet,NULL))
	{
		if(pRcdSet != NULL) pRcdSet->Release();
		pCommand->Release(); return;
	}

	m_list_xdata.DeleteAllItems();
	int nCount = strFNameArray.GetCount();
	if(pRcdSet->BatchRead()==1)
	{
		char strValue[1024]={0};
		for(int k=0; k<nCount; k++)
		{
			m_list_xdata.InsertItem(k,strFNameArray.GetAt(k));
			memset(strValue,0,1024);
			pRcdSet->GetValueAsString(strValue,k);
			m_list_xdata.SetItemText(k,1,strValue);
		}
	}
	pRcdSet->Release();
	pCommand->Release();

	AcDbObjectId ObjId;
	ObjId.setFromOldId(atoi(strId));
	FlashEntity(ObjId);
}

void CDlgReadInfo::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CAcUiDialog::OnActivate(nState, pWndOther, bMinimized);

	//读取图层信息
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
