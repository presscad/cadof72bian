#if !defined(AFX_SHDATATIMECTRL_H__A5DF02C6_BC6D_11D4_8503_00E0987ECFD8__INCLUDED_)
#define AFX_SHDATATIMECTRL_H__A5DF02C6_BC6D_11D4_8503_00E0987ECFD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SHDataTimeCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SHDataTimeCtrl window

class SHDataTimeCtrl : public CDateTimeCtrl
{
// Construction
public:
	SHDataTimeCtrl(int iItem, int iSubItem);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SHDataTimeCtrl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~SHDataTimeCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(SHDataTimeCtrl)
	afx_msg void OnCloseup(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnNcDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKillfocus(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	BOOL m_bESC;
	int m_iSubItem;
	int m_iItem;
    COleDateTime  m_date;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHDATATIMECTRL_H__A5DF02C6_BC6D_11D4_8503_00E0987ECFD8__INCLUDED_)
