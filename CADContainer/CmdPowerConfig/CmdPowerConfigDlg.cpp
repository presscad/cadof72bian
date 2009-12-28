// CmdPowerConfigDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CmdPowerConfig.h"
#include "CmdPowerConfigDlg.h"
#include <shlwapi.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCmdPowerConfigDlg 对话框



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


// CCmdPowerConfigDlg 消息处理程序

BOOL CCmdPowerConfigDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO：在此添加额外的初始化代码
	TCHAR exePath[MAX_PATH];
	::GetModuleFileName(NULL,exePath,MAX_PATH);
	PathRemoveFileSpec(exePath);
	PathAppend(exePath,"cmdPower.cfg");
	this->m_strConfigFile = exePath;
	m_ListCtrl.InsertColumn(0,"命令名");
	m_ListCtrl.InsertColumn(1,"权限状态");
	m_ListCtrl.InsertColumn(2,"命令描述");
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
			strVal = "［×］";
		else
			strVal = "［√］";
		m_ListCtrl.SetItemText(count,1,strVal);
		m_ListCtrl.SetItemText(count++,2,m_cmdPower.cmdDesc[it->first]);
	}
	m_ListCtrl.SetExtendedStyle(m_ListCtrl.GetExtendedStyle() | LVS_EX_GRIDLINES);
	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CCmdPowerConfigDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CCmdPowerConfigDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CCmdPowerConfigDlg::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
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
		if (oldState == "［√］")
		{
			m_ListCtrl.SetItemText(lvhti.iItem,1,"［×］");
		}
		else 
		{
			m_ListCtrl.SetItemText(lvhti.iItem,1,"［√］");
		}
		*pResult = 0;
	}
	
}

void CCmdPowerConfigDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	int n = this->m_ListCtrl.GetItemCount();
	for (int i = 0 ; i < n; i ++)
	{
		CString strKey = m_ListCtrl.GetItemText(i,0);
		CString strVal = m_ListCtrl.GetItemText(i,1);
		BOOL BoolVal;
		if (strVal == "［×］")
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
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}
