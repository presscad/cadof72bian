#ifndef AFX_NUMEDIT_H__INCLUDED_
#define AFX_NUMEDIT_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NumEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNumEdit window

class CNumEdit : public CEdit
{
// Construction
public:

	CNumEdit();

	//�������ͣ�FALSE��long ��TRUE:double

	BOOL m_bIsDouble;
	
	//����

	BOOL m_bHasPrecision;

	int m_iPrecssion;

	//���ݷ�Χ

	BOOL m_bRange;

	double m_dMaxValue;

	double m_dMinValue;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNumEdit)
	//}}AFX_VIRTUAL

// Implementation
public:

	virtual ~CNumEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CNumEdit)

	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NUMEDIT_H__06CEFB27_19AE_4980_8690_E874F8853F9D__INCLUDED_)
