// (C) Copyright 1990-2002 by Autodesk, Inc. 
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

//-----------------------------------------------------------------------------
//----- DlgXmYDXZ.cpp : Implementation of CDlgXmYDXZ
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "resource.h"
#include "DlgXmYDXZ.h"
#include "resource.h"

//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC (CDlgXmYDXZ, CAcUiDialog)

BEGIN_MESSAGE_MAP(CDlgXmYDXZ, CAcUiDialog)
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, OnAcadKeepFocus)
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
CDlgXmYDXZ::CDlgXmYDXZ (CWnd *pParent /*=NULL*/, HINSTANCE hInstance /*=NULL*/) : CAcUiDialog (CDlgXmYDXZ::IDD, pParent, hInstance) {
}

//-----------------------------------------------------------------------------
void CDlgXmYDXZ::DoDataExchange (CDataExchange *pDX) {
	CAcUiDialog::DoDataExchange (pDX) ;
}

//-----------------------------------------------------------------------------
//----- Needed for modeless dialogs to keep focus.
//----- Return FALSE to not keep the focus, return TRUE to keep the focus
LRESULT CDlgXmYDXZ::OnAcadKeepFocus (WPARAM, LPARAM) {
	return (TRUE) ;
}
