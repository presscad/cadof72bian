#pragma once
#include "Resource.h"

// CDlgPathSet 对话框

class CDlgPathSet : public CDialog
{
	DECLARE_DYNAMIC(CDlgPathSet)

public:
	CDlgPathSet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgPathSet();
	virtual BOOL OnInitDialog();

// 对话框数据
	enum { IDD = IDD_DLG_SET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	BOOL ReadCfgLyNameArray(CString& strCT,CString& strPrj);
	CString GetFilePath(CString strFileName);

public:
	afx_msg void OnBnClickedBtnCt();
	afx_msg void OnBnClickedBtnXm();
	afx_msg void OnBnClickedBtnSet();
};
