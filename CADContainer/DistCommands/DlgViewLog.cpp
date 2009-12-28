// DlgViewLog.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgViewLog.h"
#include "DistXData.h"

#include "DataDefine.h"

#include "..\SdeData\DistExchangeInterface.h"
#include "..\SdeData\DistBaseInterface.h"

extern IDistConnection * Glb_pConnection;         // SDE数据库连接对象指针
extern ST_BIZZINFO Glb_BizzInfo; // 业务信息

// CDlgViewLog 对话框

IMPLEMENT_DYNAMIC(CDlgViewLog, CAcUiDialog)
CDlgViewLog::CDlgViewLog(CWnd* pParent /*=NULL*/)
	: CAcUiDialog(CDlgViewLog::IDD, pParent)
{
}

CDlgViewLog::~CDlgViewLog()
{
}

void CDlgViewLog::DoDataExchange(CDataExchange* pDX)
{
	CAcUiDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_VIEWLOG, m_wndList);
}


BEGIN_MESSAGE_MAP(CDlgViewLog, CAcUiDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgViewLog 消息处理程序

BOOL GetEditRowIds(AcDbObjectId objId, AcDbObjectIdArray& EditRowIds)
{
	// 获取对象所在SDE图层名称已经OBJECTID值
	CString strSdeName, strRowId;

	CDistXData tempXData(objId,"ROWID_OBJS");
	tempXData.Select("SDELYNAME",strSdeName);
	tempXData.Select("OBJECTID",strRowId);
	tempXData.Close();

	if (strSdeName.IsEmpty() || strRowId.IsEmpty())
		return FALSE;

	strSdeName += "_LS";

	IDistConnection* pConnection = Glb_pConnection;

	if(pConnection == NULL)
	{
		AfxMessageBox("SDE 数据库 没有连接！"); 
		return FALSE;
	}

	IDistCommand *pCommand = CreateSDECommandObjcet();
	if (NULL == pCommand) 
	{
		AfxMessageBox("SDE 数据库 没有连接！"); 
		return FALSE;
	}

	pCommand->SetConnectObj(pConnection);

	TCHAR lpSQL[256] = {0};
	sprintf(lpSQL, "F:EDITROWID,T:%s,W:OLDID=%s,##", strSdeName, strRowId);

	long rc = -1;
	IDistRecordSet* pRcd = NULL;
	if ((rc=pCommand->SelectData(lpSQL, &pRcd)) != 1) 
	{
		CString strError;
		strError.Format("读取表[%s]数据失败! 原因:%s ", strSdeName, ReturnErrorCodeInfo(rc));
		AfxMessageBox(strError);
		if(NULL != pRcd)
			pRcd->Release();
		pCommand->Release(); 
		return FALSE;
	} 

	while(pRcd->BatchRead()==1)
	{ 
		char strValue[255]={0};

		pRcd->GetValueAsString(strValue,0);

		AcDbObjectId ObjId;
		ObjId.setFromOldId(atoi(strValue));

		EditRowIds.append(ObjId);
	}

	pRcd->Release(); 
	pCommand->Release(); 

	return TRUE;
}

BOOL CDlgViewLog::OnInitDialog() 
{
	CAcUiDialog::OnInitDialog();

	m_wndList.InsertColumn(0, "更新名称", LVCFMT_LEFT,100);
	m_wndList.InsertColumn(1, "更新描述", LVCFMT_LEFT,200);
	m_wndList.InsertColumn(2, "更新日期", LVCFMT_LEFT,80);

	m_wndList.SetExtendedStyle(m_wndList.GetExtendedStyle()|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);


	AcDbObjectIdArray EditRowIds;
	if (!GetEditRowIds(m_ObjId, EditRowIds))
	{
		EndDialog(IDCANCEL);
		return FALSE;
	}

	for (int i=0; i<EditRowIds.logicalLength(); i++) 
	{
		IDistConnection* pConnection = Glb_pConnection;

		if(pConnection == NULL)
		{
			MessageBox("SDE 数据库 没有连接！"); 
			return FALSE;
		}

		IDistCommand *pCommand = CreateSDECommandObjcet();
		if (NULL == pCommand) 
		{
			MessageBox("SDE 数据库 没有连接！"); 
			return FALSE;
		}

		pCommand->SetConnectObj(pConnection);

		TCHAR lpSQL[256] = {0};
		sprintf(lpSQL, "F:NAME,DESCRIPTION,DATE_,REF_USER_ID,T:TB_EDITLOG,W:OBJECTID=%d,##", EditRowIds.at(i).asOldId());

		long rc = -1;
		IDistRecordSet* pRcd = NULL;
		if ((rc=pCommand->SelectData(lpSQL, &pRcd)) != 1) 
		{
			CString strError;
			strError.Format("读取数据源失败! 原因:%s ",ReturnErrorCodeInfo(rc));
			MessageBox(strError);
			if(NULL != pRcd)
				pRcd->Release();
			pCommand->Release(); 
			return FALSE;
		} 

		if (pRcd->BatchRead()==1)
		{ 
			CString strName, strDesc, strDate;

			char strValue[255]={0};

			pRcd->GetValueAsString(strValue,0);
			strName = strValue;
			pRcd->GetValueAsString(strValue,1);
			strDesc = strValue;
			pRcd->GetValueAsString(strValue,2);
			strDate = strValue;

			int nIndex = m_wndList.InsertItem(m_wndList.GetItemCount(), strName);
			m_wndList.SetItemText(nIndex, 1, strDesc);
			m_wndList.SetItemText(nIndex, 2, strDate);
		}

		pRcd->Release(); 
		pCommand->Release(); 
	}

	if (m_wndList.GetItemCount() > 0)
		m_wndList.SetSelectionMark(0);

	return TRUE;
}

void CDlgViewLog::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	OnOK();
}
