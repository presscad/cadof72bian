#pragma once

//-----------------------------------------------------------------------------
#include "acui.h"
#include "resource.h"

//-----------------------------------------------------------------------------
class CTKGongChengDlg : public CAcUiDialog {
	DECLARE_DYNAMIC (CTKGongChengDlg)

public:
	CTKGongChengDlg (CWnd *pParent =NULL, HINSTANCE hInstance =NULL) ;

	enum { IDD = IDD_TK_GONGCHENG} ;

	CString strBicode; // 项目编号
	CString strCityZone; // 城区
	CString strUser; // 经办人
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
