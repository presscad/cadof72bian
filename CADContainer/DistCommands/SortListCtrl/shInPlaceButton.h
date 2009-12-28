#if !defined(AFX_SHINPLACEBUTTON_H__FB7A02E1_599A_11D4_88C3_000021E5CC7C__INCLUDED_)
#define AFX_SHINPLACEBUTTON_H__FB7A02E1_599A_11D4_88C3_000021E5CC7C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SHInPlaceButton.h : header file
//

#define MSG_INPLACE_BUTTON_CLICK WM_USER + 1

#include "SHInPlaceEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CSHInPlaceButton window

class CSHInPlaceButton : public CButton
{
// Construction
public:
	CSHInPlaceButton(int nItem, int nSubitem);
	CSHInPlaceButton();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSHInPlaceButton)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetEditCtrl(CSHInPlaceEdit* pEdit);
	virtual ~CSHInPlaceButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSHInPlaceButton)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnNcDestroy();
	afx_msg void OnClicked();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	int m_nItem;
	int m_nSubitem;
	BOOL m_bKillFocus;
	CSHInPlaceEdit* m_pEdit;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHINPLACEBUTTON_H__FB7A02E1_599A_11D4_88C3_000021E5CC7C__INCLUDED_)
