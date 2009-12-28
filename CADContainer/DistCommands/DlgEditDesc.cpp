// DlgEditDesc.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DlgEditDesc.h"
#include "DlgEditLog.h"

#include "DataDefine.h"

#include "..\SdeData\DistExchangeInterface.h"
#include "..\SdeData\DistBaseInterface.h"

extern IDistConnection * Glb_pConnection;         // SDE���ݿ����Ӷ���ָ��
extern ST_BIZZINFO Glb_BizzInfo; // ҵ����Ϣ

// CDlgEditDesc �Ի���

IMPLEMENT_DYNAMIC(CDlgEditDesc, CDialog)
CDlgEditDesc::CDlgEditDesc(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEditDesc::IDD, pParent)
{
	m_nEditRowId = -1;
}

CDlgEditDesc::~CDlgEditDesc()
{
}

void CDlgEditDesc::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDT_NAME, m_strName);
	DDX_Text(pDX, IDC_EDT_DESC, m_strDesc);
}


BEGIN_MESSAGE_MAP(CDlgEditDesc, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_SHOWLOG, OnBnClickedBtnShowlog)
END_MESSAGE_MAP()


// CDlgEditDesc ��Ϣ�������

BOOL CDlgEditDesc::OnInitDialog() 
{
	CDialog::OnInitDialog();

	return TRUE;
}

void CDlgEditDesc::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);

	// д��־
	if (!m_strName.IsEmpty())
	{
		IDistConnection* pConnection = Glb_pConnection;

		if(pConnection == NULL)
		{
			acutPrintf("\nSDE ���ݿ� û�����ӣ�"); 
			return;
		}

		IDistCommand *pCommand = CreateSDECommandObjcet();
		if (NULL == pCommand) 
		{
			acutPrintf("\nSDE ���ݿ� û�����ӣ�"); 
			return;
		}

		pCommand->SetConnectObj(pConnection);

		// ��ȡ������ʱ��
		tm tm_server;
		pCommand->GetServerTime(tm_server);

		CString strDate;
		strDate.Format("%4d-%02d-%02d", tm_server.tm_year+1900, tm_server.tm_mon+1, tm_server.tm_mday);

		// �����Ƿ������ͬ��¼ //
		TCHAR lpSQL[256] = {0};
		sprintf(lpSQL, "F:OBJECTID,NAME,DESCRIPTION,DATE_,REF_USER_ID,W:NAME='%s' AND DESCRIPTION='%s' AND DATE_ = Date '%s' AND REF_USER_ID=%s,T:TB_EDITLOG,##", m_strName, m_strDesc, strDate, Glb_BizzInfo.strUserId);

		IDistRecordSet* pRcd = NULL;
		if ((pCommand->SelectData(lpSQL, &pRcd) == 1) && 
			(pRcd->BatchRead()==1))
		{
			char strValue[64] = {0};
			pRcd->GetValueAsString(strValue, 0);
			m_nEditRowId = atoi(strValue);

			pRcd->Release();
			pCommand->Release(); 
			OnOK();
			return;
		} 
		
		pRcd->Release(); 
		// �����Ƿ������ͬ��¼ //

		// TCHAR lpSQL[256] = {0};
		sprintf(lpSQL, "F:NAME,DESCRIPTION,DATE_,REF_USER_ID,T:TB_EDITLOG,##");

		int nRowId;
		long rc = pCommand->InsertData(&nRowId,lpSQL,m_strName.GetBuffer(0),
			m_strDesc.GetBuffer(0), strDate.GetBuffer(0), atol(Glb_BizzInfo.strUserId));

		if (rc != 1)
		{
			CString strError;
			strError.Format("��Ӽ�¼ʧ��! ԭ��:%s ",ReturnErrorCodeInfo(rc));
			MessageBox(strError);
		}

		m_nEditRowId = nRowId;
		pCommand->Release();  
	}
	else
	{
		MessageBox("����д�������ơ�");
		return;
	}

	OnOK();
}

void CDlgEditDesc::OnBnClickedBtnShowlog()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDlgEditLog dlg;
	if (IDOK == dlg.DoModal())
	{
		SetDlgItemText(IDC_EDT_NAME, dlg.m_strName);
		SetDlgItemText(IDC_EDT_DESC, dlg.m_strDesc);
	}
}
