#pragma once

#include "resource.h"

// CDlgViewLog �Ի���

class CDlgViewLog : public CAcUiDialog
{
	DECLARE_DYNAMIC(CDlgViewLog)

public:
	CDlgViewLog(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgViewLog();

// �Ի�������
	enum { IDD = IDD_DLG_EDITLOG2 };

	CListCtrl m_wndList;

	AcDbObjectId m_ObjId;

public:
	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
