#pragma once

#include "StdAfx.h"
#include "resource.h"

// CDlgEditDesc �Ի���

class CDlgEditDesc : public CDialog
{
	DECLARE_DYNAMIC(CDlgEditDesc)

public:
	CDlgEditDesc(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgEditDesc();

// �Ի�������
	enum { IDD = IDD_DLG_EDITDESC };

	CString m_strName; // ���߸�������
	CString m_strDesc; // ���߸�������

	int m_nEditRowId; // �����޸���־��OBJECTID

public:
	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedBtnShowlog();
};
