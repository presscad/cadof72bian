// DlgEditLog.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DlgEditLog.h"

#include "DataDefine.h"

#include "..\SdeData\DistExchangeInterface.h"
#include "..\SdeData\DistBaseInterface.h"

extern IDistConnection * Glb_pConnection;         // SDE���ݿ����Ӷ���ָ��
extern ST_BIZZINFO Glb_BizzInfo; // ҵ����Ϣ

// CDlgEditLog �Ի���

IMPLEMENT_DYNAMIC(CDlgEditLog, CDialog)
CDlgEditLog::CDlgEditLog(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEditLog::IDD, pParent)
{
}

CDlgEditLog::~CDlgEditLog()
{
}

void CDlgEditLog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_LOG, m_wndList);
}


BEGIN_MESSAGE_MAP(CDlgEditLog, CDialog)
	ON_BN_CLICKED(IDC_CHECK_SHOWALL, OnBnClickedCheckShowall)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_LOG, OnLvnItemchangedListLog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_NOTIFY(HDN_ITEMDBLCLICK, IDC_LIST_LOG, OnHdnItemdblclickListLog)
END_MESSAGE_MAP()


// CDlgEditLog ��Ϣ�������

BOOL CDlgEditLog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_wndList.InsertColumn(0, "��������", LVCFMT_LEFT,100);
	m_wndList.InsertColumn(1, "��������", LVCFMT_LEFT,200);
	m_wndList.InsertColumn(2, "��������", LVCFMT_LEFT,80);

	m_wndList.SetExtendedStyle(m_wndList.GetExtendedStyle()|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

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

		// ��ȡ������ʱ��
		tm tm_server;
		pCommand->GetServerTime(tm_server);

		CString strStartDate, strEndDate;
		// ������³���ʼ
		strStartDate.Format("%4d-%02d-%02d", tm_server.tm_year+1900, tm_server.tm_mon+1, 1);
		// ���¸��³�����
		if (tm_server.tm_mon == 11)
			strEndDate.Format("%4d-%02d-%02d", tm_server.tm_year+1900+1, 1, 1);
		else
			strEndDate.Format("%4d-%02d-%02d", tm_server.tm_year+1900, tm_server.tm_mon+2, 1);

		TCHAR lpSQL[256] = {0};
		sprintf(lpSQL, "F:NAME,DESCRIPTION,DATE_,REF_USER_ID,T:TB_EDITLOG,W:REF_USER_ID=%s and DATE_ >= Date '%s' and DATE_ < Date '%s',##", Glb_BizzInfo.strUserId, strStartDate, strEndDate);

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

		while(pRcd->BatchRead()==1)
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

void CDlgEditLog::OnBnClickedCheckShowall()
{
	m_wndList.DeleteAllItems();
	IDistConnection* pConnection = Glb_pConnection;

	if(pConnection == NULL)
	{
		MessageBox("SDE ���ݿ� û�����ӣ�"); 
		return;
	}

	IDistCommand *pCommand = CreateSDECommandObjcet();
	if (NULL == pCommand) 
	{
		MessageBox("SDE ���ݿ� û�����ӣ�"); 
		return;
	}

	pCommand->SetConnectObj(pConnection);

	// ��ȡ������ʱ��
	tm tm_server;
	pCommand->GetServerTime(tm_server);
	CTime time_server(mktime(&tm_server));

	CString strStartDate, strEndDate;
	// ������³���ʼ
	strStartDate.Format("%4d-%02d-%02d", tm_server.tm_year+1900, tm_server.tm_mon+1, 1);
	// ���¸��³�����
	if (tm_server.tm_mon == 11)
		strEndDate.Format("%4d-%02d-%02d", tm_server.tm_year+1900+1, 1, 1);
	else
		strEndDate.Format("%4d-%02d-%02d", tm_server.tm_year+1900, tm_server.tm_mon+2, 1);

	TCHAR lpSQL[256] = {0};
	if (BST_CHECKED == ((CButton*)GetDlgItem(IDC_CHECK_SHOWALL))->GetCheck())
	{
		sprintf(lpSQL, "F:NAME,DESCRIPTION,DATE_,REF_USER_ID,T:TB_EDITLOG,W:REF_USER_ID=%s,##", Glb_BizzInfo.strUserId);
	}
	else
	{
		sprintf(lpSQL, "F:NAME,DESCRIPTION,DATE_,REF_USER_ID,T:TB_EDITLOG,W:REF_USER_ID=%s and DATE_ >= Date '%s' and DATE_ < Date '%s',##", Glb_BizzInfo.strUserId, strStartDate, strEndDate);
	}

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
		return;
	} 

	while(pRcd->BatchRead()==1)
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

	if (m_wndList.GetItemCount() > 0)
		m_wndList.SetSelectionMark(0);
}

void CDlgEditLog::OnLvnItemchangedListLog(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;
}

void CDlgEditLog::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	POSITION pos = m_wndList.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		int nItem = m_wndList.GetNextSelectedItem(pos);
		m_strName = m_wndList.GetItemText(nItem,0);
		m_strDesc = m_wndList.GetItemText(nItem,1);
	}

	OnOK();
}

void CDlgEditLog::OnHdnItemdblclickListLog(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;

	OnBnClickedOk();
}
