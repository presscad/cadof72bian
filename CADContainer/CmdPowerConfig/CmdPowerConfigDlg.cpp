// CmdPowerConfigDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CmdPowerConfig.h"
#include "CmdPowerConfigDlg.h"
#include <shlwapi.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCmdPowerConfigDlg �Ի���



CCmdPowerConfigDlg::CCmdPowerConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCmdPowerConfigDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCmdPowerConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl);
}

BEGIN_MESSAGE_MAP(CCmdPowerConfigDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnNMDblclkList1)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST1, OnNMCustomdrawList1)
END_MESSAGE_MAP()


// CCmdPowerConfigDlg ��Ϣ�������

BOOL CCmdPowerConfigDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO���ڴ���Ӷ���ĳ�ʼ������
	TCHAR exePath[MAX_PATH];
	::GetModuleFileName(NULL,exePath,MAX_PATH);
	PathRemoveFileSpec(exePath);
	PathAppend(exePath,"cmdPower.cfg");
	this->m_strConfigFile = exePath;
	m_ListCtrl.InsertColumn(0,"������");
	m_ListCtrl.InsertColumn(1,"Ȩ��״̬");
	m_ListCtrl.InsertColumn(2,"��������");
	m_ListCtrl.SetColumnWidth(0,150);
	m_ListCtrl.SetColumnWidth(1,70);
	m_ListCtrl.SetColumnWidth(2,200);
	CFile myFile(m_strConfigFile,CFile::modeRead);
	CArchive ar(&myFile,CArchive::load);
	m_cmdPower.Serialize(ar);
	ar.Close();
	myFile.Close();
	
	int count = 0;
	for (std::map<CString,BOOL>::iterator it = m_cmdPower.powerList.begin();
		it != m_cmdPower.powerList.end(); it ++)
	{
		m_ListCtrl.InsertItem(count,it->first);
		CString strVal;
		if (it->second == 0)
			strVal = "�ۡ���";
		else
			strVal = "�̣ۡ�";
		m_ListCtrl.SetItemText(count,1,strVal);
		m_ListCtrl.SetItemText(count++,2,m_cmdPower.cmdDesc[it->first]);
	}
	m_ListCtrl.SetExtendedStyle(m_ListCtrl.GetExtendedStyle() | LVS_EX_GRIDLINES);
	return TRUE;  // ���������˿ؼ��Ľ��㣬���򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CCmdPowerConfigDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ��������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
HCURSOR CCmdPowerConfigDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CCmdPowerConfigDlg::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	LPNMITEMACTIVATE pia = (LPNMITEMACTIVATE)pNMHDR;
	LVHITTESTINFO lvhti;

	// Clear the subitem text the user clicked on.
	lvhti.pt = pia->ptAction;
	m_ListCtrl.SubItemHitTest(&lvhti);

	if (lvhti.flags & LVHT_ONITEMLABEL)
	{
		int nItem = lvhti.iItem;
		int nSubItem = lvhti.iSubItem;
		if (nItem < 0 || nSubItem < 0)
			return;
		CString oldState = m_ListCtrl.GetItemText(lvhti.iItem,1);
		if (oldState == "�̣ۡ�")
		{
			m_ListCtrl.SetItemText(lvhti.iItem,1,"�ۡ���");
		}
		else 
		{
			m_ListCtrl.SetItemText(lvhti.iItem,1,"�̣ۡ�");
		}
		*pResult = 0;
	}
	
}

void CCmdPowerConfigDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int n = this->m_ListCtrl.GetItemCount();
	for (int i = 0 ; i < n; i ++)
	{
		CString strKey = m_ListCtrl.GetItemText(i,0);
		CString strVal = m_ListCtrl.GetItemText(i,1);
		BOOL BoolVal;
		if (strVal == "�ۡ���")
		{
			BoolVal = 0;
		}
		else BoolVal = 1;
		m_cmdPower.powerList[strKey] = BoolVal;
	}
	
	CFile myFile(m_strConfigFile,CFile::modeCreate | CFile::modeWrite);
	CArchive ar(&myFile,CArchive::store);
	m_cmdPower.Serialize(ar);
	ar.Close();
	myFile.Close();
	OnOK();
}

void CCmdPowerConfigDlg::OnNMCustomdrawList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;
}
