#pragma once

#include "stdafx.h"
#include "resource.h"

// CDlgHistory �Ի���

class CDlgHistory : public CDialog
{
	DECLARE_DYNAMIC(CDlgHistory)

public:
	CDlgHistory(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgHistory();

// �Ի�������
	enum { IDD = IDD_DLG_HISTORY };

	CTreeCtrl m_wndTree;
	CDateTimeCtrl m_wndDate;

public:
	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
