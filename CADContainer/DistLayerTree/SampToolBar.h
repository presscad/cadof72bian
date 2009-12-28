//
//
// (C) Copyright 1997 by Autodesk, Inc. 
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted, 
// provided that the above copyright notice appears in all copies and 
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting 
// documentation.
//
// AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC. 
// DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//
// Use, duplication, or disclosure by the U.S. Government is subject to 
// restrictions set forth in FAR 52.227-19 (Commercial Computer
// Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
// (Rights in Technical Data and Computer Software), as applicable.
//
/////////////////////////////////////////////////////////////////////////////////
// SampToolBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSampToolBar window

class CSampToolBar : public CToolBar
{
	DECLARE_DYNCREATE(CSampToolBar)
// Construction
public:
	CSampToolBar();
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSampToolBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSampToolBar();
protected:
	// Generated message map functions
public:
	// Generated message map functions
protected:
	//{{AFX_MSG(CSampToolBar)
	afx_msg void OnUpdateSamp(CCmdUI* pCmdUI);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
