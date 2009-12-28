#if !defined(AFX_GRIDLISTCTRL_H__92CA5999_9434_11D1_88D5_444553540000__INCLUDED_)
#define AFX_GRIDLISTCTRL_H__92CA5999_9434_11D1_88D5_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// GridListCtrl.h : header file
//

#include "SHInPlaceEdit.h"
#include "SHInPlaceButton.h"
#include "SHInPlaceComboBox.h"
#include "SHDataTimeCtrl.h"

#define IDC_INPLACE_EDIT		77129
#define IDC_INPLACE_COMBOBOX	78067
#define IDC_INPLACE_BUTTON		78068
#define IDC_INPLACE_DataTime	78069

/////////////////////////////////////////////////////////////////////////////
// CSHListCtrl window

class CSHListCtrl : public CListCtrl
{
	// Construction
public:
	CSHListCtrl();
	CToolTipCtrl m_tooltip;
	// Attributes
public:
	// The current subitem or column number which is order based.
	int m_CurSubItem;
	// Operations
public:
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSHListCtrl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PreSubclassWindow();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL
	
	// Implementation
public:
	int InsertColumn( int nCol, const LVCOLUMN* pColumn );	
	int CSHListCtrl::InsertColumn(int nCol, LPCTSTR lpszColumnHeading, int nFormat,
		int nWidth, int nSubItem);
	BOOL AddHeaderToolTip(int ncol, LPCTSTR stip );
	BOOL m_bKeyEvent;
	BOOL m_bClickEdit;
	CComboBox* ShowInPlaceComboBox(int nItem, int nSubitem,DWORD dwStyle);
	CSHInPlaceEdit* ShowInPlaceEdit(int nItem, int nSutitem,DWORD dwStyle);
	CSHInPlaceEdit* ShowInPlaceEditEx(int nItem, int nSutitem, DWORD dwStyle);
	CDateTimeCtrl* CSHListCtrl::ShowInPlaceCDateTimeCtrl(int nItem, int nSubitem,DWORD dwStyle);
	void MakeColumnVisible(int nCol);
	BOOL PositionControl( CWnd * pWnd, int iItem, int iSubItem );
	virtual ~CSHListCtrl();
	
	// Generated message map functions
protected:
	//{{AFX_MSG(CSHListCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	WORD m_wStyle;
	DECLARE_MESSAGE_MAP()
private:
	int IndexToOrder( int iIndex );
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRIDLISTCTRL_H__92CA5999_9434_11D1_88D5_444553540000__INCLUDED_)
