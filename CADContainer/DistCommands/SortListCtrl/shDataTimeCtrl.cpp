// SHDataTimeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "SHDataTimeCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SHDataTimeCtrl

SHDataTimeCtrl::SHDataTimeCtrl(int iItem, int iSubItem)
{
	m_iItem = iItem;
	m_iSubItem = iSubItem;
	m_bESC = FALSE;
}


SHDataTimeCtrl::~SHDataTimeCtrl()
{
}


BEGIN_MESSAGE_MAP(SHDataTimeCtrl, CDateTimeCtrl)
	//{{AFX_MSG_MAP(SHDataTimeCtrl)
	ON_NOTIFY_REFLECT(DTN_CLOSEUP, OnCloseup)
	ON_WM_KILLFOCUS()
	ON_WM_CHAR()
	ON_WM_NCDESTROY()
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(NM_KILLFOCUS, OnKillfocus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SHDataTimeCtrl message handlers

BOOL SHDataTimeCtrl::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if( pMsg->message == WM_KEYDOWN )
	{
		if (/*pMsg->wParam == VK_RETURN || */pMsg->wParam == VK_ESCAPE)
		{
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			return TRUE;				// DO NOT process further
		}
		else if (pMsg->wParam == VK_RETURN)
		{
			GetParent()->SetFocus();
		}
	}

	return CDateTimeCtrl::PreTranslateMessage(pMsg);
}

void SHDataTimeCtrl::OnCloseup(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
/*		CString str;
		GetTime(m_date);
	    str=m_date.Format("%Y-%m-%d");
		// Send Notification to parent of ListView ctrl
		LV_DISPINFO dispinfo;
		dispinfo.hdr.hwndFrom = GetParent()->m_hWnd;
		dispinfo.hdr.idFrom = GetDlgCtrlID();
		dispinfo.hdr.code = LVN_ENDLABELEDIT;
	
		dispinfo.item.mask = LVIF_TEXT;
		dispinfo.item.iItem = m_iItem;
		dispinfo.item.iSubItem = m_iSubItem;
		dispinfo.item.pszText = m_bESC ? NULL : LPTSTR((LPCTSTR)str);
		dispinfo.item.cchTextMax = str.GetLength();
		if (!m_bESC)
			GetParent()->GetParent()->SendMessage( WM_NOTIFY, GetParent()->GetDlgCtrlID(), (LPARAM)&dispinfo );
		PostMessage(WM_CLOSE, 0, 0);
*/
	*pResult = 0;
}

void SHDataTimeCtrl::OnKillFocus(CWnd* pNewWnd) 
{

	CDateTimeCtrl::OnKillFocus(pNewWnd);
    CMonthCalCtrl* pm=GetMonthCalCtrl( );
	if(pm==pNewWnd)return;
	CString str;
	GetTime(m_date);
	str=m_date.Format("%Y-%m-%d");
	// Send Notification to parent of ListView ctrl
	LV_DISPINFO dispinfo;
	dispinfo.hdr.hwndFrom = GetParent()->m_hWnd;
	dispinfo.hdr.idFrom = GetDlgCtrlID();
	dispinfo.hdr.code = LVN_ENDLABELEDIT;

	dispinfo.item.mask = LVIF_TEXT;
	dispinfo.item.iItem = m_iItem;
	dispinfo.item.iSubItem = m_iSubItem;
	dispinfo.item.pszText = m_bESC ? NULL : LPTSTR((LPCTSTR)str);
	dispinfo.item.cchTextMax = str.GetLength();
	if (!m_bESC)
		GetParent()->GetParent()->SendMessage( WM_NOTIFY, GetParent()->GetDlgCtrlID(), (LPARAM)&dispinfo );
	//DestroyWindow();
	if(pm!=NULL)
	   pm->SendMessage(WM_CLOSE, 0, 0);
	PostMessage(WM_CLOSE, 0, 0);
	
	// TODO: Add your message handler code here
	
}

void SHDataTimeCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	if( nChar == VK_ESCAPE || nChar == VK_RETURN)
	{
		if ( nChar == VK_ESCAPE )
			m_bESC = TRUE;
		GetParent()->SetFocus();
		return;
	}
	CDateTimeCtrl::OnChar(nChar, nRepCnt, nFlags);
}

void SHDataTimeCtrl::OnNcDestroy() 
{
	CDateTimeCtrl::OnNcDestroy();
	delete this;
	// TODO: Add your message handler code here
	
}

int SHDataTimeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDateTimeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	CFont* font = GetParent()->GetFont();
	SetFont(font);
    SetFocus();
	// TODO: Add your specialized creation code here
	
	return 0;
}

void SHDataTimeCtrl::OnKillfocus(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
/*	CString str;
    GetTime(m_date);
	str=m_date.Format("%Y-%m-%d");
	// Send Notification to parent of ListView ctrl
	LV_DISPINFO dispinfo;
	dispinfo.hdr.hwndFrom = GetParent()->m_hWnd;
	dispinfo.hdr.idFrom = GetDlgCtrlID();
	dispinfo.hdr.code = LVN_ENDLABELEDIT;

	dispinfo.item.mask = LVIF_TEXT;
	dispinfo.item.iItem = m_iItem;
	dispinfo.item.iSubItem = m_iSubItem;
	dispinfo.item.pszText = m_bESC ? NULL : LPTSTR((LPCTSTR)str);
	dispinfo.item.cchTextMax = str.GetLength();
	if (!m_bESC)
		GetParent()->GetParent()->SendMessage( WM_NOTIFY, GetParent()->GetDlgCtrlID(), (LPARAM)&dispinfo );
	PostMessage(WM_CLOSE, 0, 0);*/
	*pResult = 0;
}



