#pragma once
#include "Resource.h"

// CDlgPathSet �Ի���

class CDlgPathSet : public CDialog
{
	DECLARE_DYNAMIC(CDlgPathSet)

public:
	CDlgPathSet(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgPathSet();
	virtual BOOL OnInitDialog();

// �Ի�������
	enum { IDD = IDD_DLG_SET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

private:
	BOOL ReadCfgLyNameArray(CString& strCT,CString& strPrj);
	CString GetFilePath(CString strFileName);

public:
	afx_msg void OnBnClickedBtnCt();
	afx_msg void OnBnClickedBtnXm();
	afx_msg void OnBnClickedBtnSet();
};
