#pragma once

//-----------------------------------------------------------------------------
#include "acui.h"
#include "resource.h"

//-----------------------------------------------------------------------------
class CTKYongDiDlg : public CAcUiDialog {
	DECLARE_DYNAMIC (CTKYongDiDlg)

public:
	CTKYongDiDlg (CWnd *pParent =NULL, HINSTANCE hInstance =NULL) ;

	enum { IDD = IDD_TK_YONGDI} ;

	CString strBicode; // 项目编号
	CString strOwner; // 用地单位
	CString strOwnerProperty; // 用地性质
	CString strDate; // 日期

	CStringArray arrayTag, arrayValue;

public:
	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange (CDataExchange *pDX) ;
	afx_msg LRESULT OnAcadKeepFocus (WPARAM, LPARAM) ;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
} ;
