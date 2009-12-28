#pragma once

#include "StdAfx.h"
#include "resource.h"

// CDlgEditLog 对话框

class CDlgEditLog : public CDialog
{
	DECLARE_DYNAMIC(CDlgEditLog)

public:
	CDlgEditLog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgEditLog();

// 对话框数据
	enum { IDD = IDD_DLG_EDITLOG };

	CListCtrl m_wndList;

	CString m_strName;
	CString m_strDesc;

public:
	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCheckShowall();
	afx_msg void OnLvnItemchangedListLog(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	afx_msg void OnHdnItemdblclickListLog(NMHDR *pNMHDR, LRESULT *pResult);
};
