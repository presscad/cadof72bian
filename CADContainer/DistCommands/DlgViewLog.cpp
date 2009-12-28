// DlgViewLog.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DlgViewLog.h"
#include "DistXData.h"

#include "DataDefine.h"

#include "..\SdeData\DistExchangeInterface.h"
#include "..\SdeData\DistBaseInterface.h"

extern IDistConnection * Glb_pConnection;         // SDE���ݿ����Ӷ���ָ��
extern ST_BIZZINFO Glb_BizzInfo; // ҵ����Ϣ

// CDlgViewLog �Ի���

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


// CDlgViewLog ��Ϣ�������

BOOL GetEditRowIds(AcDbObjectId objId, AcDbObjectIdArray& EditRowIds)
{
	// ��ȡ��������SDEͼ�������Ѿ�OBJECTIDֵ
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
		AfxMessageBox("SDE ���ݿ� û�����ӣ�"); 
		return FALSE;
	}

	IDistCommand *pCommand = CreateSDECommandObjcet();
	if (NULL == pCommand) 
	{
		AfxMessageBox("SDE ���ݿ� û�����ӣ�"); 
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
		strError.Format("��ȡ��[%s]����ʧ��! ԭ��:%s ", strSdeName, ReturnErrorCodeInfo(rc));
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

	m_wndList.InsertColumn(0, "��������", LVCFMT_LEFT,100);
	m_wndList.InsertColumn(1, "��������", LVCFMT_LEFT,200);
	m_wndList.InsertColumn(2, "��������", LVCFMT_LEFT,80);

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
			MessageBox("SDE ���ݿ� û�����ӣ�"); 
			return FALSE;
		}

		IDistCommand *pCommand = CreateSDECommandObjcet();
		if (NULL == pCommand) 
		{
			MessageBox("SDE ���ݿ� û�����ӣ�"); 
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
			strError.Format("��ȡ����Դʧ��! ԭ��:%s ",ReturnErrorCodeInfo(rc));
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnOK();
}
