// SHInPlaceEdit.cpp : implementation file
//

#include "stdAfx.h"
#include "SHInPlaceEdit.h"
#include "Float.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSHInPlaceEdit

CSHInPlaceEdit::CSHInPlaceEdit(int iItem, int iSubItem)
{
	m_iItem = iItem;
	m_iSubItem = iSubItem;
	m_bESC = FALSE;
	m_bEnterKeyPress = FALSE;
	m_bNum = FALSE;
	m_NumberOfNumberAfterPoint = 0;
	m_Verbose = FALSE;
	m_MinValue = -FLT_MAX;
	m_MaxValue = FLT_MAX;
	m_Delta = FLT_ROUNDS;
	m_pBtn = NULL;
}

CSHInPlaceEdit::~CSHInPlaceEdit()
{
}


BEGIN_MESSAGE_MAP(CSHInPlaceEdit, CEdit)
//{{AFX_MSG_MAP(CSHInPlaceEdit)
	ON_WM_KILLFOCUS()
	ON_WM_CHAR()
	ON_WM_CREATE()
	ON_WM_NCDESTROY()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSHInPlaceEdit message handlers

BOOL CSHInPlaceEdit::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message == WM_KEYDOWN )
	{
		SHORT sKey = GetKeyState(VK_CONTROL);
		if(pMsg->wParam == VK_RETURN 
			|| pMsg->wParam == VK_DELETE 
			|| pMsg->wParam == VK_ESCAPE 
			|| pMsg->wParam == VK_TAB 
			|| sKey
			)
		{
			if (pMsg->wParam == VK_RETURN)
				m_bEnterKeyPress = TRUE;
			::TranslateMessage(pMsg);
			/* Strange but true:
			If the edit control has ES_MULTILINE and ESC
			is pressed the parent is destroyed if the 
			message is dispatched.  In this 
			case the parent is the list control. */
			if( !(GetStyle() & ES_MULTILINE) || pMsg->wParam != VK_ESCAPE )
			{
				::DispatchMessage(pMsg);
			}
			return TRUE;                    // DO NOT process further
		}
	}
	
	return CEdit::PreTranslateMessage(pMsg);
}


void CSHInPlaceEdit::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);

	if (pNewWnd == m_pBtn)
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
	dispinfo.item.cchTextMax = m_bESC ? 0 : str.GetLength();
	
	if (!m_bESC)
		GetParent()->GetParent()->SendMessage(WM_NOTIFY, GetParent()->GetDlgCtrlID(), (LPARAM)&dispinfo);
	int n= GetParent()->GetDlgCtrlID();
	DestroyWindow();
}

void CSHInPlaceEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{


     CEdit::OnChar(nChar, nRepCnt, nFlags);

/*		float oldValue;
    if( nChar == VK_ESCAPE || nChar == VK_RETURN)
	{
		if( nChar == VK_ESCAPE )
			m_bESC = TRUE;
		GetParent()->SetFocus();
		return;
	}
	
	if (nChar == ' ')
		return;
	oldValue = GetValue();
	CEdit::OnChar(nChar, nRepCnt, nFlags);

	int val = IsValid();
	CString s;
	if (m_bNum)
	{
		switch (val)
		{
			case VALID:
				ConstructFormat(s, GetValue());
				break;
			case MINUS_PLUS:
				break;
			case DOT:
				break;
			default:
				SetValue(oldValue);
				SetSel(0, -1);
				MSG msg;
				while (::PeekMessage(&msg, m_hWnd, WM_CHAR, WM_CHAR, PM_REMOVE));
				break;
		}
	}
	if (m_pBtn == NULL)
		CalculateSize();
		*/
}

int CSHInPlaceEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// Set the proper font
	CFont* font = GetParent()->GetFont();
	SetFont(font);
	SetFocus();

	return 0;
}

void CSHInPlaceEdit::CalculateSize()
{
	// Get text extent
	CString sText;
	
	GetWindowText( sText );
	if (sText == "")
		sText=" ";
	CWindowDC dc(this);
	CFont *pFont = GetParent()->GetFont();
	CFont *pFontDC = dc.SelectObject( pFont );
	
	// Get client rect
	CRect rect, rcParent;
	GetClientRect(&rect);
	GetParent()->GetClientRect(&rcParent);
	
	// Transform rect to parent coordinates
	ClientToScreen( &rect );
	GetParent()->ScreenToClient( &rect );
	
	CSize size;

	/*if (!(GetStyle() & ES_MULTILINE))
	{
		size = dc.GetTextExtent(sText);
		dc.SelectObject( pFontDC );
		size.cx += 5;                           // add some extra buffer
	}
	else
	{*/
		CRect thinrect( rect );  // To measure the skinniest text box
		CRect widerect( rect );  // To measure the wides text box
		widerect.right = rcParent.right;
		// Use the shortest of the two box sizes.
		int thinheight = dc.DrawText( sText, &thinrect, DT_CALCRECT|DT_NOPREFIX|DT_LEFT|DT_EXPANDTABS|DT_WORDBREAK );
		int wideheight = dc.DrawText( sText, &widerect, DT_CALCRECT|DT_NOPREFIX|DT_LEFT|DT_EXPANDTABS|DT_WORDBREAK );
		if( thinheight >= wideheight )
		{
			size.cy = wideheight + 5;
			size.cx = widerect.right - widerect.left + 5;
		}
		else
		{
			size.cy = thinheight + 5;
			size.cx = thinrect.right - thinrect.left + 5;
		}
	//}
	
	// Check whether control needs to be resized
	// and whether there is space to grow
	int bChanged = 0;
	if( size.cx > rect.Width() )
	{
		if( size.cx + rect.left < rcParent.right - 2 )
			rect.right = rect.left + size.cx;
		else
			rect.right = rcParent.right - 2;
		bChanged = 1;
	}
	if( size.cy > rect.Height() )
	{
		if( size.cy + rect.top < rcParent.bottom - 2 )
			rect.bottom = rect.top + size.cy;
		else
		{
			rect.bottom = rcParent.bottom - 2;
			//ShowScrollBar( SB_VERT );
		}
		bChanged = 1;
	}
	// If the size became larger rposition the window.
	if ( bChanged )
	{
		if (m_pBtn != NULL)
			rect.right -= rect.Height();
		MoveWindow( &rect );
	}
}

void CSHInPlaceEdit::OnNcDestroy() 
{
	CEdit::OnNcDestroy();

	// TODO: Add your message handler code here
	delete m_pBtn;
	m_pBtn = NULL;
	delete this;
}

void CSHInPlaceEdit::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	GetParent()->SetFocus();
	
	CEdit::OnClose();
}

float CSHInPlaceEdit::GetValue() const
{
	float f = m_MinValue;
	if (IsValid() == VALID)
	{
		CString str;
		GetWindowText(str);
		sscanf(str, "%f", &f);
	}
	return f;
}

BOOL CSHInPlaceEdit::SetValue(float val)
{
	if (val > m_MaxValue || val < m_MinValue) return FALSE;
	CString str, s;
	str.Format(ConstructFormat(s, val), val);
	SetWindowText(str);
	return TRUE;
}

int CSHInPlaceEdit::IsValid(const CString &str) const
{
	int res = VALID;
	float f;
	char lp[10];
	if ((str.GetLength() == 1) && ((str[0] == '+') || (str[0] == '-')))
		res = MINUS_PLUS;
	else if (sscanf(str, "%f%s", &f, lp) != 1)
		res = INVALID_CHAR;
	else if (f > m_MaxValue || f < m_MinValue)
		res = OUT_OF_RANGE;
	if (m_Verbose && (res != VALID) && (res != MINUS_PLUS))
	{
		CString msg;
		msg.Empty();
		if (res & OUT_OF_RANGE) msg += _T("Given value is out of range.\n");
		if (res & INVALID_CHAR) msg += _T("Characters must be a number.\n");
		AfxMessageBox(str, MB_OK | MB_ICONSTOP);
	}
	return res;
}

int CSHInPlaceEdit::IsValid() const
{
	CString str;
	GetWindowText(str);
	int res = VALID;
	float f;
	char lp[10];
	if ((str.GetLength() == 1) && ((str[0] == '+') || (str[0] == '-')))
		res = MINUS_PLUS;
	else if (((str.GetLength() == 1) && (str[0] == '.')) || (str.GetLength() == 2) && (str[0] == '-') && (str[1] == '.'))
		res = DOT;
	else if (sscanf(str, "%f%s", &f, lp) != 1)
		res = INVALID_CHAR;
	else if (f > m_MaxValue || f < m_MinValue)
		res = OUT_OF_RANGE;
	if (m_Verbose && (res != VALID) && (res != MINUS_PLUS))
	{
		str.Empty();
		if (res & OUT_OF_RANGE) str += _T("Given value is out of range.\n");
		if (res & INVALID_CHAR) str += _T("Characters must be a number.\n");
		AfxMessageBox(str, MB_OK | MB_ICONSTOP);
	}
	return res;
}

BOOL CSHInPlaceEdit::Verbose() const
{
	return m_Verbose;
}

void CSHInPlaceEdit::Verbose(BOOL v)
{
	m_Verbose = v;
}

void CSHInPlaceEdit::SetRange(float min, float max)
{
	m_MinValue = min;
	m_MaxValue = max;
}

void CSHInPlaceEdit::GetRange(float &min, float &max) const
{
	min = m_MinValue;
	max = m_MaxValue;
}

void CSHInPlaceEdit::SetDelta(float delta)
{
	m_Delta = delta;
}

float CSHInPlaceEdit::GetDelta()
{
	return m_Delta;
}

void CSHInPlaceEdit::ChangeAmount(int step)
{
	float f = GetValue() + step * m_Delta;
	if (f > m_MaxValue) f = m_MaxValue;
	else if (f < m_MinValue) f = m_MinValue;
	SetValue(f);
}

CString& CSHInPlaceEdit::ConstructFormat(CString &str, float num)
{
	str.Format("%G", num);
	int n = str.Find('.');
	if (n >= 0)
	{
		n = str.GetLength() - n - 1;
		if (n > m_NumberOfNumberAfterPoint) m_NumberOfNumberAfterPoint = n;
	}
	str.Format("%%.%df", m_NumberOfNumberAfterPoint);
	return str;
}

void CSHInPlaceEdit::SetButtonCtrl(CButton *pBtn)
{
	ASSERT(pBtn);
	m_pBtn = pBtn;
}

void CSHInPlaceEdit::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
/*	CEdit::PreSubclassWindow();
	ASSERT(m_hWnd);
    DWORD dwStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	if(m_bOnlyRead)
		dwStyle  |= ES_READONLY;
	
	// Make sure we have report view and send edit label messages.
    SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);
	
*/	

}
