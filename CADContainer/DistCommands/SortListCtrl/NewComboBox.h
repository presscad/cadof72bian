#if !defined(AFX_NEWCOMBOBOX_H__40B57222_DE2C_415D_9B1F_ED58EC77E513__INCLUDED_)
#define AFX_NEWCOMBOBOX_H__40B57222_DE2C_415D_9B1F_ED58EC77E513__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewComboBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNewComboBox window

//##ModelId=41C1514F018A
class CNewComboBox : public CComboBox
{
// Construction
public:
	//##ModelId=41C1514F0197
	CNewComboBox();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewComboBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	//##ModelId=41C1514F0198
	virtual ~CNewComboBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CNewComboBox)
	//##ModelId=41C1514F019A
	afx_msg void OnKillfocus();
	afx_msg void OnCloseup();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWCOMBOBOX_H__40B57222_DE2C_415D_9B1F_ED58EC77E513__INCLUDED_)
