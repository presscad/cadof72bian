#pragma once

//-----------------------------------------------------------------------------
#include "acui.h"
#include "resource.h"
#include "SplitterBar.h"
#include ".\SortListCtrl\SortListCtrl.h"

//-----------------------------------------------------------------------------
class CDlgReadInfo : public CAcUiDialog {
	DECLARE_DYNAMIC (CDlgReadInfo)

public:
	CDlgReadInfo (CWnd *pParent =NULL, HINSTANCE hInstance =NULL) ;

	enum { IDD = IDD_DLG_READINFO} ;

	CTreeCtrl		m_wndTree;
	CSortListCtrl   m_wndList;
	CSplitterBar	m_wndSplitterBar;

//	COptionTree m_otTree;

	CAcUiSelectButton	m_btnSelect;

	CString m_strCurObjectId;

	CString m_strCadlayerName;

public:
	virtual BOOL OnInitDialog();

	void InitLayerList();

	void OnSelectChanged(HTREEITEM hItem);

	void FlashEntity(AcDbObjectId ObjId);

	void ResetListItems();

protected:
	virtual void DoDataExchange (CDataExchange *pDX) ;
	afx_msg LRESULT OnAcadKeepFocus (WPARAM, LPARAM) ;
	afx_msg void OnClickObjectTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedBtnSelect();
	afx_msg void OnBnClickedSave();
	afx_msg LRESULT OnEndEdit(WPARAM, LPARAM);

	DECLARE_MESSAGE_MAP()
public:
} ;
