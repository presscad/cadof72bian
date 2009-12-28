#if !defined(AFX_SHINPLACECOMBOBOX_H__C75F4CA1_1A7E_11D4_88C2_000021E5CC7C__INCLUDED_)
#define AFX_SHINPLACECOMBOBOX_H__C75F4CA1_1A7E_11D4_88C2_000021E5CC7C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SHInPlaceComboBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSHInPlaceComboBox window

class CSHInPlaceComboBox : public CComboBox
{
// Construction
public:
	CSHInPlaceComboBox(int iItem, int iSubItem);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSHInPlaceComboBox)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSHInPlaceComboBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSHInPlaceComboBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnNcDestroy();
	afx_msg void OnCloseup();
	afx_msg void OnKillfocus();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	BOOL m_bESC;
	int m_iSubItem;
	int m_iItem;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHINPLACECOMBOBOX_H__C75F4CA1_1A7E_11D4_88C2_000021E5CC7C__INCLUDED_)
