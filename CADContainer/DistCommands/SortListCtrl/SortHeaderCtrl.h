
#ifndef SORTHEADERCTRL_H
#define SORTHEADERCTRL_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WM_ON_POPMENU	(WM_APP + 10002)

class CSortHeaderCtrl : public CHeaderCtrl
{
	// Construction
public:

	CSortHeaderCtrl();

	virtual ~CSortHeaderCtrl();

public:
	// Operations

	void SetSortArrow( const int iColumn, const BOOL bAscending );

public:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSortHeaderCtrl)
	//}}AFX_VIRTUAL
	
protected:

	void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct );
	

	int m_iSortColumn;

	BOOL m_bSortAscending;
	
	//{{AFX_MSG(CSortHeaderCtrl)
	// NOTE - the ClassWizard will add and remove member functions here.

	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);	
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // SORTHEADERCTRL_H
