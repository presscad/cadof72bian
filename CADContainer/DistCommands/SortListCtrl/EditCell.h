
#ifndef EditCell_H
#define EditCell_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// EditCell.h : header file
//
#ifndef SORTLISTCTRL_H
	#include "SortListCtrl.h"
#endif	// SORTLISTCTRL_H


/////////////////////////////////////////////////////////////////////////////
// CEditCell window

class CEditCell : public CEdit
{
// Construction
public:

	CEditCell();
	
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditCell)
	//}}AFX_VIRTUAL

// Implementation
public:

	virtual ~CEditCell();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEditCell)

	afx_msg void OnKillFocus(CWnd* pNewWnd);

	afx_msg void OnDestroy();

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	afx_msg UINT OnGetDlgCode();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !EditCell_H
