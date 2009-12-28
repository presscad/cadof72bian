// SHInPlaceComboBox.cpp : implementation file
//

#include "StdAfx.h"
#include "SHInPlaceComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSHInPlaceComboBox

BEGIN_MESSAGE_MAP(CSHInPlaceComboBox, CComboBox)
	//{{AFX_MSG_MAP(CSHInPlaceComboBox)
	ON_WM_CREATE()
	ON_WM_KILLFOCUS()
	ON_WM_CHAR()
	ON_WM_NCDESTROY()
	ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCloseup)
	ON_CONTROL_REFLECT(CBN_KILLFOCUS, OnKillfocus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSHInPlaceComboBox message handlers

CSHInPlaceComboBox::CSHInPlaceComboBox(int iItem, int iSubItem)
{
	m_iItem = iItem;
	m_iSubItem = iSubItem;

	m_bESC = FALSE;
}

CSHInPlaceComboBox::~CSHInPlaceComboBox()
{
}

BOOL CSHInPlaceComboBox::PreTranslateMessage(MSG* pMsg) 
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
	// My code end here
	return CComboBox::PreTranslateMessage(pMsg);
}

int CSHInPlaceComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	CFont* font = GetParent()->GetFont();
	SetFont(font);
	if ((GetStyle() & CBS_DROPDOWNLIST) == CBS_DROPDOWNLIST)
		SetFocus();
	else
	{
		CWnd* pWnd = GetWindow(GW_CHILD);
		char className[8];
		do
		{
			::GetClassName(pWnd->m_hWnd, className, 8);
			if (!stricmp("Edit", className))
				pWnd->SetFocus();
		}
		while ((pWnd = pWnd->GetWindow(GW_HWNDNEXT)));
	}

	return 0;
}

void CSHInPlaceComboBox::OnKillFocus(CWnd* pNewWnd) 
{
	CComboBox::OnKillFocus(pNewWnd);
	
	// TODO: Add your message handler code here
	CString str;
	GetWindowText(str);

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
	PostMessage(WM_CLOSE, 0, 0);
	//注意:
	//此处曾出现过的问题
	//将PostMessage(WM_CLOSE, 0, 0)改为DestroyWindow()后
	//在下拉列表框打开后随意在列表项中晃动几下不选中任何项
	//在点击任何列表框以外的地方,程序会执行非法操作

}

void CSHInPlaceComboBox::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	if( nChar == VK_ESCAPE || nChar == VK_RETURN)
	{
		if ( nChar == VK_ESCAPE )
			m_bESC = TRUE;
		GetParent()->SetFocus();
		return;
	}

	CComboBox::OnChar(nChar, nRepCnt, nFlags);
}

void CSHInPlaceComboBox::OnNcDestroy() 
{
	CComboBox::OnNcDestroy();
	
	// TODO: Add your message handler code here
	delete this;

}

void CSHInPlaceComboBox::OnCloseup() 
{
	// TODO: Add your control notification handler code here
	if ((GetStyle() & CBS_DROPDOWNLIST) == CBS_DROPDOWNLIST)
		GetParent()->SetFocus();
	else
	{
		CString str;
		GetWindowText(str);

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

	}
}

void CSHInPlaceComboBox::OnKillfocus() 
{
	// TODO: Add your control notification handler code here
	if ((GetStyle() & CBS_DROPDOWNLIST) == CBS_DROPDOWNLIST)
		return;
	CString str;
	GetWindowText(str);

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
}
