// CmdPowerConfigDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "ConfigReader.h"



// CCmdPowerConfigDlg 对话框
class CCmdPowerConfigDlg : public CDialog
{
private:
	CString m_strConfigFile;
	CJn_CommandPower m_cmdPower;
// 构造
public:
	CCmdPowerConfigDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_CMDPOWERCONFIG_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ListCtrl;
	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	afx_msg void OnNMCustomdrawList1(NMHDR *pNMHDR, LRESULT *pResult);
};
