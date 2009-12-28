// SampToolBar.cpp : implementation file
//
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
#include "stdafx.h"
#include "SampToolBar.h"
#include "SampToolWnd.h"
#include "Resource.h"
//#include <rxmfcapi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSampToolBar
IMPLEMENT_DYNCREATE(CSampToolBar, CToolBar)
CSampToolBar::CSampToolBar() : CToolBar()
{
}

CSampToolBar::~CSampToolBar()
{
//	CToolBar::~CToolBar();
//	saveToolBarState();
}


BEGIN_MESSAGE_MAP(CSampToolBar, CToolBar)
	//{{AFX_MSG_MAP(CSampToolBar)
	ON_UPDATE_COMMAND_UI_RANGE(IDC_TB_STARTEDIT, IDC_TB_VIEWLOG, OnUpdateSamp)
	ON_UPDATE_COMMAND_UI(IDC_TB_VIEWLOG,OnUpdateSamp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSampToolBar message handlers

void CSampToolBar::OnUpdateSamp(CCmdUI* pCmdUI) 
{
}

