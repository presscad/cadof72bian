#pragma once

//-----------------------------------------------------------------------------
#include "acui.h"

//-----------------------------------------------------------------------------
class CDlgXmSearch : public CAcUiDialog {
	DECLARE_DYNAMIC (CDlgXmSearch)

public:
	CDlgXmSearch (CWnd *pParent =NULL, HINSTANCE hInstance =NULL) ;

	void SetInfo(CString strCadLyName,CString strSdeLyName);

	enum { IDD = IDD_DLGXM_SEARCH} ;

protected:
	virtual void DoDataExchange (CDataExchange *pDX) ;
	afx_msg LRESULT OnAcadKeepFocus (WPARAM, LPARAM) ;

	DECLARE_MESSAGE_MAP()
public:
	// 根据范围选择项目信息
	CAcUiSelectButton m_btn_select;
	CListCtrl m_list_xm;
	CListCtrl m_list_xmxx;

	CString  m_strSdeLyName;
	CString  m_strCadLyName;

	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedBtnCondition();
	afx_msg void OnBnClickedBtnRect();
	afx_msg void OnNMClickListXm(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnPos();
	afx_msg void OnBnClickedBtnOut();
	afx_msg void OnBnClickedBtnExit();
private:
	BOOL SelectAllXMXX(const char* strCadLyName,const char* strSdeLyName,CStringArray& strArrayXMMC,CStringArray& strArrayObjId);
	double m_CenterX;
	double m_CenterY;
} ;
