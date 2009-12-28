#pragma once

//-----------------------------------------------------------------------------
#include "acui.h"
#include "resource.h"

//-----------------------------------------------------------------------------
class CDlgGongShi3 : public CAcUiDialog {
	DECLARE_DYNAMIC (CDlgGongShi3)

public:
	CDlgGongShi3 (CWnd *pParent =NULL, HINSTANCE hInstance =NULL) ;

	enum { IDD = IDD_DLG_GONGSHI3} ;

	long m_nClass; // ��ʾ����
	CString m_strGSLX; // ��ʾ����
	long m_nGSLM; // ��ʾ��Ŀ
	CString m_strGSLM; // ��ʾ��Ŀ

public:
	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange (CDataExchange *pDX) ;
	afx_msg LRESULT OnAcadKeepFocus (WPARAM, LPARAM) ;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
} ;
