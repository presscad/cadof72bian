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

	long m_nClass; // 公示类型
	CString m_strGSLX; // 公示类型
	long m_nGSLM; // 公示栏目
	CString m_strGSLM; // 公示栏目

public:
	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange (CDataExchange *pDX) ;
	afx_msg LRESULT OnAcadKeepFocus (WPARAM, LPARAM) ;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
} ;
