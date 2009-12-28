#pragma once

#include "stdafx.h"
#include "resource.h"

// CDlgHistory 对话框

class CDlgHistory : public CDialog
{
	DECLARE_DYNAMIC(CDlgHistory)

public:
	CDlgHistory(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgHistory();

// 对话框数据
	enum { IDD = IDD_DLG_HISTORY };

	CTreeCtrl m_wndTree;
	CDateTimeCtrl m_wndDate;

public:
	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
