// SHInPlaceButton.cpp : implementation file
//

#include "stdafx.h"
#include "SHInPlaceButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSHInPlaceButton

CSHInPlaceButton::CSHInPlaceButton(int nItem, int nSubitem)
{
	m_nItem = nItem;
	m_nSubitem = nSubitem;

	m_bKillFocus = TRUE;
	m_pEdit = NULL;
}

CSHInPlaceButton::~CSHInPlaceButton()
{
}


BEGIN_MESSAGE_MAP(CSHInPlaceButton, CButton)
	//{{AFX_MSG_MAP(CSHInPlaceButton)
	ON_WM_KILLFOCUS()
	ON_WM_NCDESTROY()
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSHInPlaceButton message handlers

void CSHInPlaceButton::OnKillFocus(CWnd* pNewWnd) 
{
	CButton::OnKillFocus(pNewWnd);
	// TODO: Add your message handler code here
	if (pNewWnd == m_pEdit || !m_bKillFocus)
		return;
	DestroyWindow();
}

void CSHInPlaceButton::SetEditCtrl(CSHInPlaceEdit *pEdit)
{
	m_pEdit = pEdit;
}

void CSHInPlaceButton::OnNcDestroy() 
{
	CButton::OnNcDestroy();
	
	// TODO: Add your message handler code here
	delete m_pEdit;
	m_pEdit = NULL;
	delete this;	
}

void CSHInPlaceButton::OnClicked() 
{
	// TODO: Add your control notification handler code here
	m_bKillFocus = FALSE;
/*
	CString sText = ((CListCtrl*)GetParent())->GetItemText(m_nItem, m_nSubitem);
	m_pEdit->SetWindowText(sText);
	m_pEdit->SetSel(0, -1);
	m_pEdit->SetFocus();
	*/
	m_bKillFocus = TRUE;
	GetParent()->GetParent()->SendMessage(MSG_INPLACE_BUTTON_CLICK, 
		m_nItem, m_nSubitem);

	
}
