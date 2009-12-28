#pragma once

#include "StdAfx.h"
#include "resource.h"

// CDlgEditLog �Ի���

class CDlgEditLog : public CDialog
{
	DECLARE_DYNAMIC(CDlgEditLog)

public:
	CDlgEditLog(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgEditLog();

// �Ի�������
	enum { IDD = IDD_DLG_EDITLOG };

	CListCtrl m_wndList;

	CString m_strName;
	CString m_strDesc;

public:
	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCheckShowall();
	afx_msg void OnLvnItemchangedListLog(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	afx_msg void OnHdnItemdblclickListLog(NMHDR *pNMHDR, LRESULT *pResult);
};
