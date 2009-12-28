#pragma once

#include "resource.h"

// CDlgViewLog 对话框

class CDlgViewLog : public CAcUiDialog
{
	DECLARE_DYNAMIC(CDlgViewLog)

public:
	CDlgViewLog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgViewLog();

// 对话框数据
	enum { IDD = IDD_DLG_EDITLOG2 };

	CListCtrl m_wndList;

	AcDbObjectId m_ObjId;

public:
	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
