#pragma once

//-----------------------------------------------------------------------------
#include "acui.h"
#include "resource.h"

//-----------------------------------------------------------------------------
class CDlgReadInfo : public CAcUiDialog {
	DECLARE_DYNAMIC (CDlgReadInfo)

public:
	CDlgReadInfo (CWnd *pParent =NULL, HINSTANCE hInstance =NULL) ;

	enum { IDD = IDD_DLG_READINFO} ;

	CListCtrl           m_list_xdata;
	CListCtrl            m_list_obj;
	CAcUiSelectButton	m_btnSelect;

public:
	virtual BOOL OnInitDialog();
	void ReadEntityInfo();

protected:
	virtual void DoDataExchange (CDataExchange *pDX) ;
	afx_msg LRESULT OnAcadKeepFocus (WPARAM, LPARAM) ;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnSelect();
	afx_msg void OnNMClickListObj(NMHDR *pNMHDR, LRESULT *pResult);

private:
	void FlashEntity(AcDbObjectId ObjId);
	
public:
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
} ;
