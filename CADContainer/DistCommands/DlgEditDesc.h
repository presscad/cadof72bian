#pragma once

#include "StdAfx.h"
#include "resource.h"

// CDlgEditDesc 对话框

class CDlgEditDesc : public CDialog
{
	DECLARE_DYNAMIC(CDlgEditDesc)

public:
	CDlgEditDesc(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgEditDesc();

// 对话框数据
	enum { IDD = IDD_DLG_EDITDESC };

	CString m_strName; // 六线更新名称
	CString m_strDesc; // 六线更新描述

	int m_nEditRowId; // 返回修改日志的OBJECTID

public:
	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedBtnShowlog();
};
