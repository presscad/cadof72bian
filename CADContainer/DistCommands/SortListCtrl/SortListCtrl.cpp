
//

#include "stdafx.h"
#include "SortListCtrl.h"
#include "MaskEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// Below styles MUST be present in a  ctrl
#define MUST_STYLE			(WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | LVS_REPORT)
#define MUST_EX_STYLE		(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES )

// Below styles MUST NOT be present in a ctrl
#define MUST_NOT_STYLE		(LVS_EDITLABELS | LVS_ICON | LVS_SMALLICON | LVS_LIST | LVS_NOSCROLL)
#define MUST_NOT_EX_STYLE	(0)

/////////////////////////////////////////////////////////////////////////////
// ItemData
struct ItemData
{
public:

	ItemData(){}


	CPtrList listTextPtr;
	

	CArray<COLORREF, COLORREF> cTextColors; // Sub item text colors

	CArray<COLORREF, COLORREF> cBkColors; // Sub item backgroud colors
	
private:
	// ban copying.

	ItemData( const ItemData& );

	ItemData& operator=( const ItemData& );
};


/////////////////////////////////////////////////////////////////////////////
// CSortListCtrl

CSortListCtrl::CSortListCtrl()
	: m_iNumColumns( 0 )
	, m_iSortColumn( -1 )
	, m_bSortAscending( -1 )
	,m_bAllowEdit(FALSE)
	,m_pWndEdit(NULL)
	,m_pWndNumEdit(NULL)
	,m_pWndDateEdit(NULL)
	,m_pWndCellEdit(NULL)
	,m_pWndCombo(NULL)
{
	m_pWndNumEdit = new CNumEdit;
	m_pWndDateEdit = new CMaskEdit;
	m_pWndCellEdit = new CEditCell;
	m_pWndCombo = new CNewComboBox;

	m_pWndEdit = m_pWndDateEdit;
	m_bComboBox = FALSE;

	m_nItem = -1;
	m_nSubItem = -1;
	m_bEscape = FALSE;
	m_dwPrevEditFmt = ES_LEFT;
	
	m_crEditTextColor = ::GetSysColor(COLOR_WINDOWTEXT);
	m_crEditBkColor = ::GetSysColor(COLOR_WINDOW);
	
	m_crColumnSelectedTextColor = ::GetSysColor(COLOR_WINDOWTEXT);
	m_crColumnSelectedBkColor = ::GetSysColor(COLOR_WINDOW);

	m_crItemSelectedTextColor = ::GetSysColor(COLOR_WINDOWTEXT);
	m_crItemSelectedBkColor = ::GetSysColor(COLOR_WINDOW);

	m_crTextColor = ::GetSysColor(COLOR_WINDOWTEXT); 
	m_crTextBkColor= ::GetSysColor(COLOR_WINDOW);
	
	m_crHGridLine = RGB(192,192,192);
	m_crVGridLine = RGB(192,192,192);	
}


CSortListCtrl::~CSortListCtrl()
{
	if (m_pWndNumEdit != NULL)
	{
		delete m_pWndNumEdit;
		m_pWndNumEdit = NULL;
	}
	if (m_pWndDateEdit != NULL)
	{
		delete m_pWndDateEdit;
		m_pWndDateEdit = NULL;
	}
	if (m_pWndCellEdit != NULL)
	{
		delete m_pWndCellEdit;
		m_pWndCellEdit = NULL;
	}
	
}



BEGIN_MESSAGE_MAP(CSortListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CSortListCtrl)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnClick)
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDBLCLK()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)	
	ON_WM_LBUTTONDOWN()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_CREATE()
//	ON_WM_PAINT()	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSortListCtrl message handlers

int CSortListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	lpCreateStruct->style &= ~MUST_NOT_STYLE;
	lpCreateStruct->style |= MUST_STYLE;
	
	
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
			
	return 0;
}


BOOL CSortListCtrl::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style &= ~MUST_NOT_STYLE;
	cs.style |= MUST_STYLE;

	return CListCtrl::PreCreateWindow(cs);
}

void CSortListCtrl::OnDestroy() 
{
	DeleteAllItems();
	m_pWndEdit->DestroyWindow();

	m_dwFreezeColumn.RemoveAll();
	m_arrColumnColor.RemoveAll();
	
	//字段属性 释放内存
	RemoveFieldArray();
	
	m_arrFieldProperty.RemoveAll();
	
	CListCtrl::OnDestroy();
}

void CSortListCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LPNMLVCUSTOMDRAW lplvcd = (LPNMLVCUSTOMDRAW)pNMHDR;
	
	// Take the default processing unless we set this to something else below.
    *pResult = 0;
	
    // First thing - check the draw stage. If it's the control's prepaint
    // stage, then tell Windows we want messages for every item.
	
	if (lplvcd->nmcd.dwDrawStage == CDDS_PREPAINT)
	{	
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
    else if (lplvcd->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
	{
		// This is the notification message for an item.  We'll request
        // notifications before each subitem's prepaint stage.

		*pResult = CDRF_NOTIFYSUBITEMDRAW;
	}
    else if (lplvcd->nmcd.dwDrawStage == (CDDS_ITEMPREPAINT | CDDS_SUBITEM))
    {
		// This is the prepaint stage for a subitem. Here's where we set the
        // item's text and background colors. Our return value will tell 
        // Windows to draw the subitem itself, but it will use the new colors
        // we set here.
		
		ItemData* p = (ItemData*)(CListCtrl::GetItemData(lplvcd->nmcd.dwItemSpec));
		if (p == NULL) return;
		//assert(p != NULL);
		assert(lplvcd->iSubItem >= 0 && lplvcd->iSubItem < p->cTextColors.GetSize());
		/*ASSERT(p != NULL);
		ASSERT(lplvcd->iSubItem >= 0 && lplvcd->iSubItem < p->cTextColors.GetSize());*/
		lplvcd->clrText = p->cTextColors[lplvcd->iSubItem];
		lplvcd->clrTextBk = p->cBkColors[lplvcd->iSubItem];

		// Tell Windows to paint the control itself.		
		*pResult = CDRF_DODEFAULT;
	}
}


void CSortListCtrl::PreSubclassWindow()
{
	// the list control must have the report style.
	ASSERT( GetStyle() & LVS_REPORT );

	CListCtrl::PreSubclassWindow();
	VERIFY( m_ctlHeader.SubclassWindow( GetHeaderCtrl()->GetSafeHwnd() ) );
}

DWORD CSortListCtrl::SetExtendedStyle(DWORD dwNewStyle)
{
	dwNewStyle &= ~MUST_NOT_EX_STYLE;
	dwNewStyle |= MUST_EX_STYLE;
	
	return CListCtrl::SetExtendedStyle(dwNewStyle);
}

BOOL CSortListCtrl::ModifyStyleEx(DWORD dwRemove, DWORD dwAdd, UINT nFlags)
{
	dwRemove &= ~MUST_EX_STYLE;
	dwRemove |= MUST_NOT_EX_STYLE;
	dwAdd &= ~MUST_NOT_EX_STYLE;
	dwAdd |= MUST_EX_STYLE;
	return CListCtrl::ModifyStyleEx(dwRemove, dwAdd, nFlags);
}

BOOL CSortListCtrl::ModifyStyle(DWORD dwRemove, DWORD dwAdd, UINT nFlags)
{
	dwRemove &= ~MUST_STYLE;
	dwRemove |= MUST_NOT_STYLE;
	dwAdd &= ~MUST_NOT_STYLE;
	dwAdd |= MUST_STYLE;
	return CListCtrl::ModifyStyle(dwRemove, dwAdd, nFlags);
}

void CSortListCtrl::SetGridLines(BOOL bSet)
{
	DWORD dwStype = GetExtendedStyle();
	if (bSet)
		dwStype |= LVS_EX_GRIDLINES;
	else
		dwStype &= ~LVS_EX_GRIDLINES;
	
	SetExtendedStyle(dwStype);	
}

void CSortListCtrl::SetCheckboxeStyle(BOOL bCheckBox)
{
	DWORD dwStype = GetExtendedStyle();
	if (bCheckBox)
	{
		dwStype |= LVS_EX_CHECKBOXES;
	}
	else
	{
		dwStype &= ~LVS_EX_CHECKBOXES;		
	}
	
	SetExtendedStyle(dwStype);
}

void CSortListCtrl::SetFullSelectedStyle(BOOL bFullSelect)
{
	DWORD dwStype = GetExtendedStyle();
	if (bFullSelect)
	{
		dwStype |= LVS_EX_FULLROWSELECT;
	}
	else
	{
		dwStype &= ~LVS_EX_FULLROWSELECT;		
	}
	
	SetExtendedStyle(dwStype);
}

BOOL CSortListCtrl::SetSortable(BOOL bSet)
{	
	if((GetStyle() & LVS_NOCOLUMNHEADER) == 0)
	{
		return FALSE;
	}

	LONG lStyle = ::GetWindowLong(GetHeaderCtrl()->GetSafeHwnd(), GWL_STYLE);
	if (bSet)
	{
		lStyle |= HDS_BUTTONS;
	}
	else
	{
		lStyle &= ~HDS_BUTTONS;
	}
	::SetWindowLong(GetHeaderCtrl()->GetSafeHwnd(), GWL_STYLE, lStyle);
	CListCtrl::GetHeaderCtrl()->RedrawWindow();

	return TRUE;
}

/*
void CSortListCtrl::OnPaint() 
{
	// First let the control do its default drawing.
	const MSG *msg = GetCurrentMessage();
	DefWindowProc( msg->message, msg->wParam, msg->lParam );
	
	// Draw the lines only for LVS_REPORT mode
	if ((GetStyle() & LVS_TYPEMASK) == LVS_REPORT) 
	{
		// Get the number of columns		
		CClientDC dc(this );
		CPen hNewPen(PS_SOLID,1,m_crHGridLine);
		CPen vNewPen(PS_SOLID,2,m_crVGridLine);
		CPen *oldpen = NULL;
		
		CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
		int nColumnCount = pHeader->GetItemCount();
		
		// The bottom of the header corresponds to the top of the line		
		RECT rect;
		pHeader->GetClientRect( &rect ); 	
		int top = rect.bottom;
		int	headerheight = rect.bottom - rect.top;
		// Now get the client rect so we know the line length and		
		// when to stop
		
		GetClientRect( &rect );
		
		// adjust rect.bottom to avoid drawing column for empty rows:
		int firstrow  = GetTopIndex();
		int	perpage   = GetCountPerPage();
		int realrows  = min(perpage,GetItemCount()-firstrow);

		int	rowheight = 0;
		if(perpage != 0)
		{
			rowheight = (rect.bottom - rect.top - headerheight) / perpage;
		}
		
		// adjustment for incomplete rows:
		if (realrows * rowheight < rect.bottom - rect.top - headerheight &&
			GetItemCount() > GetTopIndex() + realrows)
		{
			//rect.bottom	= realrows * rowheight + headerheight;
			;
		}
		else
		{
			rect.bottom   = realrows * rowheight + headerheight;
		}
		
		// The border of the column is offset by the horz scroll
		oldpen = dc.SelectObject(&vNewPen);
		
		int borderx = 0 - GetScrollPos( SB_HORZ );
		for( int i = 0; i < nColumnCount; i++ )
		{
			// Get the next border
			borderx += GetColumnWidth( i );
			// if next border is outside client area, break out
			if( borderx >= rect.right ) 
				break;			
			// Draw the line.
			dc.MoveTo( borderx, top);			
			dc.LineTo( borderx, rect.bottom );
		}
		
		// Draw the horizontal grid lines
		oldpen = dc.SelectObject(&hNewPen);
		
		// First get the height
		if( !GetItemRect( 0, &rect, LVIR_BOUNDS ))			
			return;
		int height = rect.bottom - rect.top;		
		GetClientRect( &rect );
		
		int width = rect.right; 	
		for( i = 0; i <= realrows; i++ ) 
		{
			dc.MoveTo( 0, top + height*i);
			dc.LineTo( width, top + height*i );
		}		
		
		dc.SelectObject(oldpen);
	}	
	
	// Do not call CListCtrl::OnPaint() for painting messages	
}
*/


//Stander Operations
////////////////////////////////////////////////////////////////////////////////////////////
void  CSortListCtrl::SetHGridLineColor(COLORREF crColor)
{
	m_crHGridLine = crColor;
}

COLORREF  CSortListCtrl::GetHGridLineColor()
{
	return m_crHGridLine;
}

void  CSortListCtrl::SetVGridLineColor(COLORREF crColor)
{
	m_crVGridLine = crColor;
}

COLORREF  CSortListCtrl::GetVGridLineColor()
{
	return m_crVGridLine;
}

BOOL CSortListCtrl::SetHeadings( UINT uiStringID )
{
	CString strHeadings;
	VERIFY( strHeadings.LoadString( uiStringID ) );
	return SetHeadings( strHeadings );
}


// the heading text is in the format column 1 text,column 1 width;column 2 text,column 3 width;etc.
BOOL CSortListCtrl::SetHeadings( const CString& strHeadings )
{
	m_iNumColumns = 0;
	
	int iStart = 0;

	for( ;; )
	{
		const int iComma = strHeadings.Find( _T(','), iStart );

		if( iComma == -1 )
			break;

		const CString strHeading = strHeadings.Mid( iStart, iComma - iStart );

		iStart = iComma + 1;

		int iSemiColon = strHeadings.Find( _T(';'), iStart );

		if( iSemiColon == -1 )
			iSemiColon = strHeadings.GetLength();

		const int iWidth = _ttoi( strHeadings.Mid( iStart, iSemiColon - iStart ) );
		
		iStart = iSemiColon + 1;

		if( CListCtrl::InsertColumn( m_iNumColumns++, strHeading, LVCFMT_LEFT, iWidth ) == -1 )
			return FALSE;
	}

	m_arrColumnColor.SetSize(m_iNumColumns);
	for(int i=0;i<m_iNumColumns;i++)
	{
		m_arrColumnColor[i] = 0;
	}


	return TRUE;
}

int CSortListCtrl::AddItem(long nType,  LPCTSTR pszText, ... )
{
	const int iIndex = InsertItem( GetItemCount(), pszText );

	m_dwFreezeColumn.Add(0);
	
	//设置字段属性
	FIELDPROPERTY type;
	type.m_iFieldType = nType;
	m_arrFieldProperty.Add(type);
	
	LPTSTR* arrpsz = new LPTSTR[ m_iNumColumns ];
	arrpsz[ 0 ] = new TCHAR[ lstrlen( pszText ) + 1 ];
	(void)lstrcpy( arrpsz[ 0 ], pszText );

 	va_list list;
	va_start( list, pszText );

	for( int iColumn = 1; iColumn < m_iNumColumns; iColumn++ )
	{
		pszText = va_arg( list, LPCTSTR );
		ASSERT_VALID_STRING( pszText );
		VERIFY( CListCtrl::SetItem( iIndex, iColumn, LVIF_TEXT, pszText, 0, 0, 0, 0 ) );

		arrpsz[ iColumn ] = new TCHAR[ lstrlen( pszText ) + 1 ];
		(void)lstrcpy( arrpsz[ iColumn ], pszText );
	}

	va_end( list );

	VERIFY( SetTextArray( iIndex, arrpsz ) );

	return iIndex;
}

int CSortListCtrl::AddItem(CStringList &listText)
{
	if(listText.GetCount() == 0)
	{
		return -1;
	}

	const int iIndex = InsertItem(GetItemCount(),_T(""));
	m_dwFreezeColumn.Add(0);
	
	//设置字段属性
	FIELDPROPERTY type;
	m_arrFieldProperty.Add(type);
	
	LPTSTR* arrpsz = new LPTSTR[ m_iNumColumns ];
	if(arrpsz == NULL)
	{
		return -1;
	}

	CString strText;
	int i = 0;
	for(POSITION pos = listText.GetHeadPosition();pos != NULL;)
	{
		strText = listText.GetNext(pos);

		arrpsz[ i ] = new TCHAR[ strText.GetLength() + 1 ];
		(void)lstrcpy( arrpsz[ i ], strText );

		CListCtrl::SetItem( iIndex, i, LVIF_TEXT, strText, 0, 0, 0, 0 );		

		i++;
	}
	
	SetTextArray( iIndex, arrpsz );
	
	return iIndex;
}


void CSortListCtrl::FreeItemMemory( const int iItem )
{
	ItemData* pid = reinterpret_cast<ItemData*>( CListCtrl::GetItemData( iItem ) );
	if(pid == NULL)
	{
		return;
	}

	LPTSTR arrpsz = NULL;

	for(POSITION pos = pid->listTextPtr.GetHeadPosition();pos != NULL;)
	{
		arrpsz = (LPTSTR)pid->listTextPtr.GetNext(pos);

		delete[] arrpsz;
		arrpsz = NULL;
	}

	pid->listTextPtr.RemoveAll();

	pid->cTextColors.RemoveAll();
	pid->cBkColors.RemoveAll();

	delete pid;
	pid = NULL;

	VERIFY( CListCtrl::SetItemData( iItem, NULL ) );
}


BOOL CSortListCtrl::DeleteItem( int iItem )
{	
	m_dwFreezeColumn.RemoveAt(iItem);

	//字段属性
	if(m_arrFieldProperty[iItem].m_pArrDomain != NULL)
	{
		delete m_arrFieldProperty[iItem].m_pArrDomain;
	}
	m_arrFieldProperty.RemoveAt(iItem);

	FreeItemMemory( iItem );
	return CListCtrl::DeleteItem( iItem );
}


BOOL CSortListCtrl::DeleteAllItems()
{
	m_dwFreezeColumn.RemoveAll();
	
	//字段属性
	RemoveFieldArray();

	for( int iItem = 0; iItem < GetItemCount(); iItem ++ )
		FreeItemMemory( iItem );

	return CListCtrl::DeleteAllItems();
}

BOOL CSortListCtrl::SetItemText( int nItem, int nSubItem, LPCTSTR lpszText )
{
	if( !CListCtrl::SetItemText( nItem, nSubItem, lpszText ) )
		return FALSE;
	
	CPtrList *listText = GetTextArray( nItem );

	POSITION pos = listText->FindIndex(nSubItem);
	if(pos == NULL)
	{
		return FALSE;
	}
	
	LPTSTR pszText =(LPTSTR) listText->GetAt(pos);
	if(pszText != NULL)
	{
		delete[] pszText;
	}
	pszText = new TCHAR[ lstrlen( lpszText ) + 1 ];
	(void)lstrcpy( pszText, lpszText );
	listText->SetAt(pos,pszText);
	
	return TRUE;
}

BOOL CSortListCtrl::GetItemTextColor(int nItem, int nSubItem,COLORREF& textColor)
{
	ItemData* pid = (ItemData*)(CListCtrl::GetItemData(nItem));
	if(pid != NULL)
	{
		textColor = pid->cTextColors[nSubItem];
	}

	return TRUE;
}

BOOL CSortListCtrl::SetItemTextColor(int nItem, int nSubItem,COLORREF textColor,BOOL bRedraw)
{
	//
	if(textColor == COLOR_INVALID)
	{
		textColor = ::GetSysColor(COLOR_WINDOWTEXT);
	}

	//
	const int ROWS = GetItemCount();
	const int COLS = GetColumnCount();
	BOOL bRowValid = nItem >= 0 && nItem < ROWS;
	BOOL bColValid = nSubItem >= 0 && nSubItem < COLS;
	
	if (bRowValid && bColValid)
	{
		// apply to individual grid
		ItemData* pid = (ItemData*)(CListCtrl::GetItemData(nItem));
		if(pid != NULL)
		{
			pid->cTextColors[nSubItem] = textColor;
		}
		
		if (bRedraw)
		{
			RedrawItems(nItem,nItem);
		}		
	}
	else if (bRowValid && !bColValid)
	{
		// apply to whole row for the existing item
		ItemData* pid = (ItemData*)(CListCtrl::GetItemData(nItem));
		if(pid != NULL)
		{
			for (int i = 0; i < COLS; i++)
			{
				pid->cTextColors[i] = textColor;
			}
		}
		if (bRedraw)
		{
			RedrawItems(nItem,nItem);
		}		
	}
	else if (!bRowValid && bColValid)
	{
		// apply to whole column for all existing items
		for (int i = 0; i < ROWS; i++)
		{
			ItemData* pid = (ItemData*)(CListCtrl::GetItemData(i));
			if(pid != NULL)
			{
				pid->cTextColors[nSubItem] = textColor;
			}
		}
		
		if (bRedraw)
		{
			RedrawWindow();
			UpdateWindow();
		}		
	}
	else
	{
		// apply to whole table for all existing items
		for (int i = 0; i < ROWS; i++)
		{
			ItemData* pid = (ItemData*)(CListCtrl::GetItemData(i));
			if(pid != NULL)
			{
				for (int j = 0; j < COLS; j++)
				{
					pid->cTextColors[j] = textColor;
				}
			}
		}

		m_crTextColor = textColor;	
		
		if (bRedraw)
		{
			RedrawWindow();
			UpdateWindow();
		}				
	}
	
	return TRUE;
}

BOOL CSortListCtrl::SetItemBkColor(int nItem, int nSubItem,COLORREF bkColor,BOOL bRedraw)
{
	//
	if(bkColor == COLOR_INVALID)
	{
		bkColor = ::GetSysColor(COLOR_WINDOW);
	}
	
	//
	const int ROWS = GetItemCount();
	const int COLS = GetColumnCount();
	BOOL bRowValid = nItem >= 0 && nItem < ROWS;
	BOOL bColValid = nSubItem >= 0 && nSubItem < COLS;
	
	if (bRowValid && bColValid)
	{
		// apply to individual grid
		ItemData* pid = (ItemData*)(CListCtrl::GetItemData(nItem));
		if(pid != NULL)
		{
			pid->cBkColors[nSubItem] = bkColor;			
		}
		if (bRedraw)
		{
			RedrawItems(nItem,nItem);
		}				
	}
	else if (bRowValid && !bColValid)
	{
		// apply to whole row for the existing item
		ItemData* pid = (ItemData*)(CListCtrl::GetItemData(nItem));
		if(pid != NULL)
		{
			for (int i = 0; i < COLS; i++)
			{
				pid->cBkColors[i] = bkColor;			
			}
		}
		if (bRedraw)
		{
			RedrawItems(nItem,nItem);
		}				
	}
	else if (!bRowValid && bColValid)
	{
		// apply to whole column for all existing items
		for (int i = 0; i < ROWS; i++)
		{
			ItemData* pid = (ItemData*)(CListCtrl::GetItemData(i));
			if(pid != NULL)
			{
				pid->cBkColors[nSubItem] = bkColor;							
			}
		}
		if (bRedraw)
		{
			RedrawWindow();
			UpdateWindow();
		}						
	}
	else
	{
		// apply to whole table for all existing items
		for (int i = 0; i < ROWS; i++)
		{
			ItemData* pid = (ItemData*)(CListCtrl::GetItemData(i));
			if(pid != NULL)
			{
				for (int j = 0; j < COLS; j++)
				{
					pid->cBkColors[j] = bkColor;							
				}
			}
		}

		m_crTextBkColor = bkColor;

		if (bRedraw)
		{
			RedrawWindow();
			UpdateWindow();
		}						
	}
	
	
	return TRUE;
}

BOOL CSortListCtrl::SetTextArray( int iItem, LPTSTR* arrpsz )
{
	ASSERT( CListCtrl::GetItemData( iItem ) == NULL );
	ItemData* pid = new ItemData;

	//Color info
	pid->cTextColors.SetSize(m_iNumColumns);
	pid->cBkColors.SetSize(m_iNumColumns);
	
	for (int i = 0; i < m_iNumColumns; i++)
	{
		pid->listTextPtr.AddTail(arrpsz[i]);

		pid->cTextColors[i] = ::GetSysColor(COLOR_WINDOWTEXT);
		pid->cBkColors[i] = ::GetSysColor(COLOR_WINDOW);
	}

	delete[]  arrpsz;

	return CListCtrl::SetItemData( iItem, reinterpret_cast<DWORD>( pid ) );
}


CPtrList* CSortListCtrl::GetTextArray( int iItem ) const
{
	ASSERT( iItem < GetItemCount() );
	
	ItemData* pid = reinterpret_cast<ItemData*>( CListCtrl::GetItemData( iItem ) );
	return &pid->listTextPtr;
}

void CSortListCtrl::SetColumnSelectedTextColor(COLORREF crColor)
{
	m_crColumnSelectedTextColor = crColor;
}
COLORREF CSortListCtrl::GetColumnSelectedTextColor()
{
	return m_crColumnSelectedTextColor;
}

void CSortListCtrl::SetColumnSelectedBkColor(COLORREF crColor)
{
	m_crColumnSelectedBkColor = crColor;
}

COLORREF CSortListCtrl::GetColumnSelectedBkColor()
{
	return m_crColumnSelectedBkColor;
}

void CSortListCtrl::SetItemSelectedTextColor(COLORREF crColor)
{
	m_crItemSelectedTextColor = crColor;
}

COLORREF CSortListCtrl::GetItemSelectedTextColor()
{
	return m_crItemSelectedTextColor;
}

void CSortListCtrl::SetItemSelectedBkColor(COLORREF crColor)
{
	m_crItemSelectedBkColor = crColor;
}

COLORREF CSortListCtrl::GetItemSelectedBkColor()
{
	return m_crItemSelectedBkColor;
}

int CSortListCtrl::GetColumnCount() const
{
	return m_ctlHeader.GetItemCount();
}

int CSortListCtrl::AddColumn(LPCTSTR lpszColumnHeading,LPTSTR lpszDefaultValue /* = NULL */ ,
				int iDefaultValueSize /* = 0 */,int nFormat/* = LVCFMT_LEFT */,
				int nWidth /* = -1 */,int nSubItem /* = -1 */ )
{
	int nItemCount = GetItemCount();
	LPTSTR *arrpsz = new LPTSTR[nItemCount];

	for(int i =0;i<nItemCount;i++)
	{
		ItemData* pid =(ItemData*) CListCtrl::GetItemData( i );
		
		if(lpszDefaultValue == NULL || iDefaultValueSize == 0)
		{
		    arrpsz[i] = new TCHAR[2];
			lstrcpy(arrpsz[i],_T("0"));
		}
		else
		{
			arrpsz[i] = new TCHAR[iDefaultValueSize + 1];
			lstrcpy(arrpsz[i],lpszDefaultValue);
		}

		pid->listTextPtr.AddTail(arrpsz[i]);

		//Color info
		pid->cTextColors.Add(::GetSysColor(COLOR_WINDOWTEXT));
		pid->cBkColors.Add(::GetSysColor(COLOR_WINDOW));
		
		CListCtrl::SetItemData( i, reinterpret_cast<DWORD>( pid ) );
	}
	delete[] arrpsz;

	m_iNumColumns++;
	
	int nColumn = GetColumnCount();
	m_dwFreezeColumn.Add(0);
	m_arrColumnColor.Add(0);
	
	//字段属性
	FIELDPROPERTY type;
	m_arrFieldProperty.Add(type);
	
	nWidth = 50;

	GetParent()->SendMessage(WM_ON_ADDFIELD,m_iNumColumns,0);

	int nIndex = CListCtrl::InsertColumn(nColumn,lpszColumnHeading,nFormat,nWidth,nSubItem);

	ReSetAllText();

	return nIndex;
}

int CSortListCtrl::InsertColumn(int nCol, LPCTSTR lpszColumnHeading,LPTSTR lpszDefaultValue /* = NULL */, 
				int iDefaultValueSize /* = 0 */,int nFormat /*= LVCFMT_LEFT */,
				int nWidth /* = -1 */, int nSubItem /* = -1 */)
{
	int nItemCount = GetItemCount();
	LPTSTR *arrpsz = new LPTSTR[nItemCount];
	
	for(int i =0;i<GetItemCount();i++)
	{
		ItemData* pid =(ItemData*) CListCtrl::GetItemData( i );
		
		if(lpszDefaultValue == NULL || iDefaultValueSize == 0)
		{
			arrpsz[i] = new TCHAR[2];
			lstrcpy(arrpsz[i],_T("0"));
		}
		else
		{
			arrpsz[i] = new TCHAR[iDefaultValueSize + 1];
			lstrcpy(arrpsz[i],lpszDefaultValue);
		}
		
		POSITION pos = pid->listTextPtr.FindIndex(nCol);
		pid->listTextPtr.InsertBefore(pos,arrpsz[i]);
		
		//Color info
		pid->cTextColors.InsertAt(nCol,::GetSysColor(COLOR_WINDOWTEXT),1);
		pid->cBkColors.InsertAt(nCol,::GetSysColor(COLOR_WINDOW),1);
		
		CListCtrl::SetItemData( i, reinterpret_cast<DWORD>( pid ) );
	}
	delete[] arrpsz;	
	
	
	m_iNumColumns++;

	m_dwFreezeColumn.InsertAt(nCol,0,1);
	m_arrColumnColor.InsertAt(nCol,0,1);

	//字段属性
	FIELDPROPERTY type;
	m_arrFieldProperty.InsertAt(nCol,type);
	
	nWidth = 50;

	GetParent()->SendMessage(WM_ON_ADDFIELD,nCol,0);
	
	int nIndex = CListCtrl::InsertColumn(nCol,lpszColumnHeading,nFormat,nWidth,nSubItem);

	ReSetAllText();

	return nIndex;
}

int CSortListCtrl::InsertColumn( int nCol, const LVCOLUMN* pColumn ,
				LPTSTR lpszDefaultValue /* = NULL */,int iDefaultValueSize /* = 0 */)
{
	int nItemCount = GetItemCount();
	LPTSTR *arrpsz = new LPTSTR[nItemCount];
	
	for(int i =0;i<GetItemCount();i++)
	{
		ItemData* pid =(ItemData*) CListCtrl::GetItemData( i );
		
		if(lpszDefaultValue == NULL || iDefaultValueSize == 0)
		{
			arrpsz[i] = new TCHAR[2];
			lstrcpy(arrpsz[i],_T("0"));
		}
		else
		{
			arrpsz[i] = new TCHAR[iDefaultValueSize + 1];
			lstrcpy(arrpsz[i],lpszDefaultValue);
		}
		
		POSITION pos = pid->listTextPtr.FindIndex(nCol);
		pid->listTextPtr.InsertBefore(pos,arrpsz[i]);
		
		//Color info
		pid->cTextColors.InsertAt(nCol,::GetSysColor(COLOR_WINDOWTEXT),1);
		pid->cBkColors.InsertAt(nCol,::GetSysColor(COLOR_WINDOW),1);
		
		CListCtrl::SetItemData( i, reinterpret_cast<DWORD>( pid ) );
	}
	delete[] arrpsz;

	m_iNumColumns++;
	
	m_dwFreezeColumn.InsertAt(nCol,0,1);
	m_arrColumnColor.InsertAt(nCol,0,1);

	//字段属性
	FIELDPROPERTY type;
	m_arrFieldProperty.InsertAt(nCol,type);

	GetParent()->SendMessage(WM_ON_ADDFIELD,nCol,0);

	int nIndex = CListCtrl::InsertColumn( nCol, pColumn );
	ReSetAllText();
	
	return nIndex;
}

void CSortListCtrl::ModifyColumnText(int nCol, LPCTSTR lpszColumnHeading,int iHeadCaptionSize)
{
	LVCOLUMN col;
	col.mask =  LVCF_TEXT;

	CListCtrl::GetColumn(nCol,&col);
	
	col.pszText = new TCHAR[iHeadCaptionSize + 1];
	col.cchTextMax = (iHeadCaptionSize + 1) * sizeof(TCHAR);

	lstrcpy(col.pszText,lpszColumnHeading);

	CListCtrl::SetColumn(nCol,&col);
}

BOOL CSortListCtrl::DeleteColumn( int nCol ,BOOL bDeleteAll /* = FALSE */)	
{
	for(int i =0;i<GetItemCount();i++)
	{
		ItemData* pid =(ItemData*) CListCtrl::GetItemData( i );
		
		POSITION pos = pid->listTextPtr.FindIndex(nCol);
		TCHAR *pTemp = (TCHAR*) pid->listTextPtr.GetAt(pos);
		if(pTemp != NULL) delete[] pTemp;
		
		pid->listTextPtr.RemoveAt(pos);

		//Color info
		pid->cTextColors.RemoveAt(nCol);
		pid->cBkColors.RemoveAt(nCol);
		
		CListCtrl::SetItemData( i, reinterpret_cast<DWORD>( pid ) );
	}
	
	m_iNumColumns--;
	
	m_arrColumnColor.RemoveAt(nCol);
	
	BOOL bReturn =  CListCtrl::DeleteColumn(nCol);

	if(!bDeleteAll)
	{
		ReSetAllText();	
	}

	return bReturn;
}

void CSortListCtrl::ReSetAllText()
{
	POSITION pos = NULL;

	for(int i=0;i<GetItemCount();i++)
	{
		CPtrList *list = GetTextArray(i);

		for(int j=0;j<GetColumnCount();j++)
		{
			pos = list->FindIndex(j);
			CListCtrl::SetItemText(i,j,(LPTSTR)list->GetAt(pos));
		}
	}
}

void CSortListCtrl::DeleteAllColumn()	
{
	int nColumnCount = GetColumnCount();
	
	// Delete all of the columns.
	for (int i=0;i < nColumnCount;i++)
	{
		DeleteColumn(0,TRUE);
	}
	
	m_arrColumnColor.RemoveAll();
}


//Sort Operations
/////////////////////////////////////////////////////////////////////////////////////////

BOOL IsNumber( LPCTSTR pszText )
{
	ASSERT_VALID_STRING( pszText );

	for( int i = 0; i < lstrlen( pszText ); i++ )
	{
		if( !_istdigit( pszText[ i ] ) && pszText[ i ] != '.' &&  pszText[ i ] != '-')
		{
			return FALSE;
		}
	}
	
	return TRUE;
}

int NumberCompare( LPCTSTR pszNumber1, LPCTSTR pszNumber2 )
{
	ASSERT_VALID_STRING( pszNumber1 );
	ASSERT_VALID_STRING( pszNumber2 );

	const int iNumber1 = _ttoi( pszNumber1 );
	const int iNumber2 = _ttoi( pszNumber2 );

	if( iNumber1 < iNumber2 )
		return -1;
	
	if( iNumber1 > iNumber2 )
		return 1;

	return 0;
}


BOOL IsDate( LPCTSTR pszText )
{
	ASSERT_VALID_STRING( pszText );

	// format should be 99/99/9999.

	if( lstrlen( pszText ) != 10 )
		return FALSE;

	return _istdigit( pszText[ 0 ] )
		&& _istdigit( pszText[ 1 ] )
		&& pszText[ 2 ] == _T('/')
		&& _istdigit( pszText[ 3 ] )
		&& _istdigit( pszText[ 4 ] )
		&& pszText[ 5 ] == _T('/')
		&& _istdigit( pszText[ 6 ] )
		&& _istdigit( pszText[ 7 ] )
		&& _istdigit( pszText[ 8 ] )
		&& _istdigit( pszText[ 9 ] );
}


int DateCompare( const CString& strDate1, const CString& strDate2 )
{
	const int iYear1 = _ttoi( strDate1.Mid( 6, 4 ) );
	const int iYear2 = _ttoi( strDate2.Mid( 6, 4 ) );

	if( iYear1 < iYear2 )
		return -1;

	if( iYear1 > iYear2 )
		return 1;

	const int iMonth1 = _ttoi( strDate1.Mid( 3, 2 ) );
	const int iMonth2 = _ttoi( strDate2.Mid( 3, 2 ) );

	if( iMonth1 < iMonth2 )
		return -1;

	if( iMonth1 > iMonth2 )
		return 1;

	const int iDay1 = _ttoi( strDate1.Mid( 0, 2 ) );
	const int iDay2 = _ttoi( strDate2.Mid( 0, 2 ) );

	if( iDay1 < iDay2 )
		return -1;

	if( iDay1 > iDay2 )
		return 1;

	return 0;
}


int CALLBACK CSortListCtrl::CompareFunction( LPARAM lParam1, LPARAM lParam2, LPARAM lParamData )
{
	CSortListCtrl* pListCtrl = reinterpret_cast<CSortListCtrl*>( lParamData );
	ASSERT( pListCtrl->IsKindOf( RUNTIME_CLASS( CListCtrl ) ) );

	ItemData* pid1 = reinterpret_cast<ItemData*>( lParam1 );
	ItemData* pid2 = reinterpret_cast<ItemData*>( lParam2 );

	ASSERT( pid1 );
	ASSERT( pid2 );

	POSITION pos1 = pid1->listTextPtr.FindIndex(pListCtrl->m_iSortColumn);
	POSITION pos2 = pid2->listTextPtr.FindIndex(pListCtrl->m_iSortColumn);
	
	LPCTSTR pszText1 =(LPCTSTR) pid1->listTextPtr.GetAt(pos1);
	LPCTSTR pszText2 =(LPCTSTR) pid2->listTextPtr.GetAt(pos2);

	ASSERT_VALID_STRING( pszText1 );
	ASSERT_VALID_STRING( pszText2 );

	if( IsNumber( pszText1 ) )
		return pListCtrl->m_bSortAscending ? NumberCompare( pszText1, pszText2 ) : NumberCompare( pszText2, pszText1 );
	else if( IsDate( pszText1 ) )
		return pListCtrl->m_bSortAscending ? DateCompare( pszText1, pszText2 ) : DateCompare( pszText2, pszText1 );
	else
		// text.
		return pListCtrl->m_bSortAscending ? lstrcmp( pszText1, pszText2 ) : lstrcmp( pszText2, pszText1 );
}


void CSortListCtrl::OnColumnClick( NMHDR* pNMHDR, LRESULT* pResult )
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	const int iColumn = pNMListView->iSubItem;	

	return;

	//设置列选择色
//	SetRedraw(FALSE);
//	this->LockWindowUpdate();

	if(m_arrColumnColor[iColumn] == 0)
	{
		m_arrColumnColor[iColumn] = 1;

		SetItemTextColor(-1,iColumn,m_crColumnSelectedTextColor,FALSE);
		SetItemBkColor(-1,iColumn,m_crColumnSelectedBkColor,FALSE);
			
//		SetColumnWidth(iColumn,GetColumnWidth(iColumn)+1);
	}
	else
	{
		m_arrColumnColor[iColumn] = 0;
				
		SetItemTextColor(-1,iColumn,m_crTextColor,FALSE);
		SetItemBkColor(-1,iColumn,m_crTextBkColor,FALSE);	

		//不能取消已经Checked的Item颜色
		BOOL bCheck = FALSE;
		for(int i=0;i<GetItemCount();i++)
		{
			bCheck = GetCheck(i);
			if(bCheck)
			{
				SetItemTextColor(i,iColumn,m_crItemSelectedTextColor,FALSE);
				SetItemBkColor(i,iColumn,m_crItemSelectedBkColor,FALSE);				
			}
		}

//		SetColumnWidth(iColumn,GetColumnWidth(iColumn)-1);
	}

	CRect rect;
	GetClientRect(rect);

	SCROLLINFO si;
	ZeroMemory(&si, sizeof(SCROLLINFO));
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_POS;
	GetScrollInfo(SB_HORZ, &si);
	rect.left -= si.nPos;
	
	
	for(int i=0;i<iColumn;i++)
	{
		rect.left += GetColumnWidth(i);
	}
	rect.right = rect.left + GetColumnWidth(iColumn);

	InvalidateRect(&rect);
	
//	SetRedraw(TRUE);
//	this->UnlockWindowUpdate();
//	RedrawWindow();
	
	*pResult = 0;
}


void CSortListCtrl::Sort( int iColumn, BOOL bAscending )
{
	m_iSortColumn = iColumn;
	m_bSortAscending = bAscending;

	// show the appropriate arrow in the header control.
	m_ctlHeader.SetSortArrow( m_iSortColumn, m_bSortAscending );

	VERIFY( SortItems( CompareFunction, reinterpret_cast<DWORD>( this ) ) );
}

BOOL CSortListCtrl::IsAscend(int iColumn)
{
	if(m_iSortColumn != iColumn)
	{
		return -1;
	}
	
	return m_bSortAscending;
}


//Edit Operations
//////////////////////////////////////////////////////////////////////////////////////////
CEditCell* CSortListCtrl::GetCellEditControl()
{
	return m_pWndCellEdit;
}

CNumEdit* CSortListCtrl::GetNumEditControl()
{
	return m_pWndNumEdit;
}

CMaskEdit* CSortListCtrl::GetDateEditControl()
{
	return m_pWndDateEdit;
}

const CHeaderCtrl* CSortListCtrl::GetHeaderCtrl() const
{
	// Yes, CListCtrl already provides "GetHeaderCtrl", but not const.	
	// I desperately need a "const" version of "GetHeaderCtrl" because that's the
	// only way to make "GetColumnCount" const, which, in turn, is being called by
	// A LOT of const member functions. So if "GetHeaderCtrl" is not const, there will
	// be almost no const member function at all in this class. Terrible.
	return (const CHeaderCtrl*)(CWnd::FromHandle(ListView_GetHeader(GetSafeHwnd())));
}

DWORD CSortListCtrl::GetHeaderTextFormat(int nColumn) const
{
	if ((GetStyle() & LVS_NOCOLUMNHEADER) == 0)
		return ES_LEFT;
	
	HDITEM hd;
	hd.mask = HDI_FORMAT;
	if (!GetHeaderCtrl()->GetItem(nColumn, &hd))
		return ES_LEFT;
	
	if (hd.fmt & HDF_CENTER)
	{	
		return ES_CENTER;
	}
	else if (hd.fmt & HDF_RIGHT)
	{
		return ES_RIGHT;
	}
	else
	{
		return ES_LEFT;
	}
}

BOOL CSortListCtrl::StartEdit (int Item, int Column)
{
    if(!m_bAllowEdit)
	{
		return FALSE;
	}
	
	// Make sure that nCol is valid
    CSortHeaderCtrl* pHeader = (CSortHeaderCtrl*) GetDlgItem(0);
    int nColumnCount = pHeader->GetItemCount();
    if (Item <0 || Item >= GetItemCount() || Column >= nColumnCount || Column < 0)
	{
		return FALSE;
	}

	//第一行不能编辑
	if(Column == 0)
	{
		return FALSE;
	}

	if(m_dwFreezeColumn[Item] == 1)
	{
		return FALSE;
	}
	
    // Make sure that the item is visible
    if (!EnsureVisible (Item, TRUE)) 
    {
		return FALSE;
    }
	
	if (m_nItem == Item && m_nSubItem == Column)
	{
		return TRUE;
	}
	
	//设定字段类型
	COleDateTime time ;
	CString strText = GetItemText(Item, Column);
	BOOL bValidDate = time.ParseDateTime(strText);
	long lItemText = _ttol(strText);
	m_bComboBox = FALSE;
	
	FIELDPROPERTY type;
	type = m_arrFieldProperty[Item];
	
	switch(type.m_iFieldType) 
	{
	case 0://char
		m_pWndEdit = m_pWndCellEdit;
		break;
	case 1://long
		m_pWndNumEdit->m_bIsDouble = FALSE;
		m_pWndNumEdit->m_bRange = type.m_bRange;
		m_pWndNumEdit->m_dMaxValue = type.m_dMaxValue;
		m_pWndNumEdit->m_dMinValue = type.m_dMinValue;
		m_pWndNumEdit->m_bHasPrecision = type.m_bPrecision;
		m_pWndNumEdit->m_iPrecssion = type.m_iPrecision;		
		strText.Format(_T("%d"),lItemText);
		m_pWndEdit = m_pWndNumEdit;
		break;
	case 2://double
		m_pWndNumEdit->m_bIsDouble = TRUE;
		m_pWndNumEdit->m_bRange = type.m_bRange;
		m_pWndNumEdit->m_dMaxValue = type.m_dMaxValue;
		m_pWndNumEdit->m_dMinValue = type.m_dMinValue;
		m_pWndNumEdit->m_bHasPrecision = type.m_bPrecision;
		m_pWndNumEdit->m_iPrecssion = type.m_iPrecision;
		m_pWndEdit = m_pWndNumEdit;
		break;
	case 3://date
		if(!bValidDate)
		{
			time = COleDateTime::GetCurrentTime();
//			strText = time.Format(_T("%Y-%m-%d %H:%M:%S"));			
			strText = time.Format(_T("%Y-%m-%d"));			
		}
		m_pWndDateEdit->m_bRange = type.m_bRange;
		m_pWndDateEdit->m_dtMaxDate = type.m_dtMaxDate;
		m_pWndDateEdit->m_dtMinDate = type.m_dtMinDate;
		m_pWndEdit = m_pWndDateEdit;
		break;
	case 4://domain
		if(type.m_pArrDomain == NULL && type.m_pArrDomain->GetSize()== 0)
		{
			m_pWndEdit = m_pWndCellEdit;
		}
		else
		{			
			m_bComboBox = TRUE;
		}
		break;
	case 5:
		if(type.m_strComboContext.IsEmpty())
		{
			m_pWndEdit = m_pWndCellEdit;
		}
		else
		{			
			m_bComboBox = TRUE;
		}
		break;
	default:
		break;
	}

	//
	m_nItem = Item;
	m_nSubItem = Column;
	
	//domain
// 	if(m_bComboBox)
// 	{
// 		if(m_pWndCombo->GetSafeHwnd() == NULL && !m_pWndCombo->Create(WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWNLIST,
// 			CRect(10,10,300,100), this, ID_EDIT_INLISTCTRL))
// 		{
// 			return FALSE;
// 		}
// 		m_pWndCombo->SetFont(GetFont());
// 		m_pWndCombo->ResetContent();
// 		
// 		switch(type.m_iFieldType)
// 		{
// 		case 4:
// 			{
// 				CArrayCodeDomain* pCodeDomain;
// 				CString szName,szValue;
// 				
// 				for(int i=0;i<type.m_pArrDomain->GetSize();i++)
// 				{
// 					pCodeDomain = type.m_pArrDomain;
// 					if (pCodeDomain)
// 					{
// 						szValue = pCodeDomain->GetAt(i)->domainValue;
// 						szName.Format("%s—%s",szValue,pCodeDomain->GetAt(i)->domainName);
// 						int index = m_pWndCombo->AddString(szName);
// 						m_pWndCombo->SetItemDataPtr(index,(void*)(LPCTSTR)szValue);
// 					}
// 					//strTemp.Format(_T("%s"),type.*m_pArrDomain[i]);
// 				}
// 			}
// 			break;
// 		case 5:
// 			{
// 				std::vector<CString> array = commonFunc::DivideString(type.m_strComboContext, ';');
// 				for (int i=0;i<array.size();i++)
// 				{
// 					m_pWndCombo->AddString(array[i]);
// 				}
// 			}
// 		    break;
// 		default:
// 		    break;
// 		}
// 
// 		
// 		//Get SubItem Rect
// 		CRect rcEdit;
// 		GetSubItemRect(Item,Column,LVIR_LABEL,rcEdit);
// 		m_pWndCombo->MoveWindow(&rcEdit);
// 		int nIndex = m_pWndCombo->SelectString(0,strText);
// 		if(nIndex == LB_ERR)
// 		{
// 			m_pWndCombo->SetCurSel(0);
// 		}
// 		
// 		m_pWndCombo->ShowWindow(SW_SHOW);
// 		m_pWndCombo->SetFocus();
// 
// 		return TRUE;
// 	}

	// determine editbox font and alignment
	const DWORD FMT = GetHeaderTextFormat(Column);
	if (FMT != m_dwPrevEditFmt)
	{
		m_dwPrevEditFmt = FMT;
		
		// Funny thing:
		// Changing CEdit style among ES_LEFT, ES_CENTER, ES_RIGHT at runtime works
		// sometimes and fails other times. It just cannot guarantee to be succeed.
		// So I decided to destroy and recreate the CEdit every time when the text
		// format changes.
		if (m_pWndEdit->GetSafeHwnd() != NULL)
			m_pWndEdit->DestroyWindow();
		
		if (!m_pWndEdit->Create(ES_AUTOHSCROLL | ES_NOHIDESEL | WS_CHILD | WS_BORDER | FMT, 
			CRect(0, 0, 1, 1), this, ID_EDIT_INLISTCTRL))
			return FALSE;
	}
	else
	{
		if (m_pWndEdit->GetSafeHwnd() == NULL
			&& !m_pWndEdit->Create(ES_AUTOHSCROLL | ES_NOHIDESEL | WS_CHILD | WS_BORDER | FMT, 
			CRect(0, 0, 1, 1), this, ID_EDIT_INLISTCTRL))
		{
			return FALSE;
		}
	}

	m_pWndEdit->SetFont(GetFont());

	//Get SubItem Rect
	CRect rcEdit;
	GetSubItemRect(Item,Column,LVIR_LABEL,rcEdit);
	rcEdit.InflateRect(1,1,1,1);
	
	m_pWndEdit->MoveWindow(&rcEdit);
	m_pWndEdit->SetWindowText(strText);
	m_pWndEdit->ShowWindow(SW_SHOW);
	m_pWndEdit->SetSel(0, -1);
	m_pWndEdit->SetFocus();
	
    return TRUE;
}

BOOL CSortListCtrl::EndEdit(BOOL bCommit)
{
	if (!(m_nItem >=0 && m_nItem < GetItemCount() && m_nSubItem >= 0 && m_nSubItem <GetColumnCount()))
	{
		return FALSE;
	}
	
	CString strText;
	if(m_bComboBox)
	{
		int i=m_pWndCombo->GetCurSel();
		int n = 0;
		if (i!=LB_ERR)
			 n = m_pWndCombo->GetLBTextLen(i);
		if (n!=CB_ERR)
			 m_pWndCombo->GetLBText(i,strText.GetBuffer(n));	
	}
	else
	{
		m_pWndEdit->GetWindowText(strText);
	}
	
	BOOL bChanged = !m_bEscape && bCommit && strText.Compare(GetItemText(m_nItem, m_nSubItem)) != 0;
	if (bChanged)
	{
		// update the list item
		CListCtrl::SetItemText(m_nItem, m_nSubItem, strText);
		GetParent()->SendMessage(WM_ON_ENDEDIT, (WPARAM)m_nItem, (LPARAM)m_nSubItem);
	}
	strText.ReleaseBuffer( );


	if(m_bComboBox)
	{
		m_pWndCombo->ShowWindow(SW_HIDE);
	}
	else
	{
		m_pWndEdit->ShowWindow(SW_HIDE);
	}
	
	m_nItem = -1;
	m_nSubItem = -1;
	m_bEscape = FALSE;

	return bChanged;
}

int CSortListCtrl::HitTestEx (CPoint& Point, int* pColumn)
{
    int ColumnNum = 0;
    int Row = HitTest (Point, NULL);
    
    if (pColumn)
		*pColumn = 0;
	
    // Make sure that the ListView is in LVS_REPORT
    if ((GetWindowLong (m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT)
		return Row;
	
    // Get the top and bottom row visible
    Row = GetTopIndex();
    int Bottom = Row + GetCountPerPage();
    if (Bottom > GetItemCount())
		Bottom = GetItemCount();
    
    // Get the number of columns
    CHeaderCtrl* pHeader = (CHeaderCtrl*) GetDlgItem(0);
    int nColumnCount = pHeader->GetItemCount();
	
    // Loop through the visible rows
    for(; Row <= Bottom; Row++)
    {
		// Get bounding rect of item and check whether point falls in it.
		CRect Rect;
		GetItemRect (Row, &Rect, LVIR_BOUNDS);
		if (Rect.PtInRect (Point))
		{
			// Now find the column
			for (ColumnNum = 0; ColumnNum < nColumnCount; ColumnNum++)
			{
				int ColWidth = GetColumnWidth (ColumnNum);
				if (Point.x >= Rect.left && Point.x <= (Rect.left + ColWidth))
				{
					if (pColumn)
						*pColumn = ColumnNum;
					return Row;
				}
				Rect.left += ColWidth;
			}
		}
    }
    return -1;
}

void CSortListCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	
	LVHITTESTINFO hti;
	hti.pt = point;
	SubItemHitTest(&hti);
	int nIndex = hti.iItem;
	int nSubIndex = hti.iSubItem;
	if(nIndex == -1 && nSubIndex > 0)
	{
		return;
	}
	
	BOOL bBefore = CListCtrl::GetCheck(nIndex) > 0;

	CListCtrl::OnLButtonDown(nFlags, point);
	
	BOOL bAfter = CListCtrl::GetCheck(nIndex) > 0;
	
	if(!((bBefore && bAfter) || (!bBefore && !bAfter)))
	{
		GetParent()->SendMessage(WM_ON_CHKBOX, (WPARAM)nIndex, (LPARAM)0);	

		if(GetCheck(nIndex))			
		{
			RedrawCheckItem(nIndex);
		}
		else
		{
			RedrawUnCheckItem(nIndex);	
		}	
	}
}

void CSortListCtrl::RedrawCheckItem(int nItem)
{
	SetItemTextColor(nItem,-1,m_crItemSelectedTextColor,FALSE);
	SetItemBkColor(nItem,-1,m_crItemSelectedBkColor,FALSE);	

	RedrawItems(nItem,nItem);
}

void CSortListCtrl::RedrawUnCheckItem(int nItem)
{
	SetItemTextColor(nItem,-1,m_crTextColor,FALSE);
	SetItemBkColor(nItem,-1,m_crTextBkColor,FALSE);	
	
	//不能取消Columnxuanze颜色
	for(int i =0 ;i<m_arrColumnColor.GetSize();i++)
	{
		if(m_arrColumnColor[i] == 1)
		{
			SetItemTextColor(nItem,i,m_crColumnSelectedTextColor,FALSE);
			SetItemBkColor(nItem,i,m_crColumnSelectedBkColor,FALSE);
		}
	}
	
	RedrawItems(nItem,nItem);
}

void CSortListCtrl::RefreshSortCtrl()
{
	//
	SetRedraw(FALSE);
	this->LockWindowUpdate();
	
	//刷新背景
	SetItemTextColor(-1,-1,m_crTextColor);
	SetItemBkColor(-1,-1,m_crTextBkColor);
	
	//刷新行选择色
	for(int i=0;i<GetItemCount();i++)
	{
		if(GetCheck(i))
		{
			SetItemTextColor(i,-1,m_crItemSelectedTextColor);
			SetItemBkColor(i,-1,m_crItemSelectedBkColor);			
		}
	}

	this->UnlockWindowUpdate();
	SetRedraw(TRUE);
}

void CSortListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CListCtrl::OnLButtonDblClk(nFlags, point);
	
	if (!m_bAllowEdit)
		return;

    int Index;
    int ColNum;
    if ((Index = HitTestEx (point, &ColNum)) != -1)
    {
		if (S_OK == GetParent()->SendMessage(WM_ON_BEGINEDIT, (WPARAM)Index, (LPARAM)ColNum))
		{
			StartEdit (Index, ColNum);
		}
    }
}

void CSortListCtrl::SetAllowEdit(BOOL bAllowEdit /* = TRUE */)
{
	m_bAllowEdit = bAllowEdit;
}

BOOL CSortListCtrl::GetAllowEdit()
{
	return m_bAllowEdit;
}

void CSortListCtrl::SetEditTextColor(COLORREF crColor)
{
	m_crEditTextColor = crColor;
}

COLORREF CSortListCtrl::GetEditTextColor()
{
	return m_crEditTextColor;
}

void CSortListCtrl::SetEditBkColor(COLORREF crColor)
{
	m_crEditBkColor = crColor;
}

COLORREF CSortListCtrl::GetEditBkColor()
{
	return m_crEditBkColor;
}


//////////////////////////////////////////////////////////////////////////



//AutoSize
//////////////////////////////////////////////////////////////////////////////////////////
void CSortListCtrl::AutoSizeColumn()
{
	SetRedraw(FALSE);

	int iColumnCount = GetHeaderCtrl()->GetItemCount();
	int iHeadWith = 0;
	int iItemWith = 0;
	int iMaxWidth = 0;
	for(int i =0;i<iColumnCount;i++)
	{
		if(GetColumnWidth(i) == 0)
		{
			continue;
		}

		SetColumnWidth(i,LVSCW_AUTOSIZE_USEHEADER);
		iHeadWith = GetColumnWidth(i);
		
		
		SetColumnWidth(i,LVSCW_AUTOSIZE);
		iItemWith = GetColumnWidth(i);
		
		iMaxWidth = max(iHeadWith,iItemWith);
		SetColumnWidth(i,iMaxWidth);	
	}

	SetRedraw(TRUE);
}


//Freeze/Unfreeze
//////////////////////////////////////////////////////////////////////////////////////////
void CSortListCtrl::FreezeColumn(const int iColumn)
{
	if(iColumn < GetItemCount())
	{
		m_dwFreezeColumn[iColumn] = 1;
	}
}

void CSortListCtrl::UnfreezeColumn(const int iColumn)
{
	if(iColumn < GetItemCount())
	{
		m_dwFreezeColumn[iColumn] = 0;
	}
}

BOOL CSortListCtrl::IsFreeze(const int iColumn)
{
	if(iColumn < GetItemCount())
	{ 
		return m_dwFreezeColumn[iColumn];
	}

	return -1;
}



//Other
///////////////////////////////////////////////////////////////////////////////////////////
HBRUSH CSortListCtrl::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CListCtrl::OnCtlColor(pDC, pWnd, nCtlColor);
	
	if(nCtlColor == CTLCOLOR_EDIT)	
	{
		// Set the text color to red
		pDC->SetTextColor(m_crEditTextColor);
		pDC->SetBkColor(m_crEditBkColor);
		
		// Set the background mode for text to transparent 
		// so background will show thru.
//		pDC->SetBkMode(TRANSPARENT);
	}
	
	// TODO: Return a different brush if the default is not desired
	return hbr;
}


void CSortListCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if (GetFocus() != this) 
	{
		SetFocus();
	}
	
	CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CSortListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if (GetFocus() != this)
	{
		SetFocus();
	}
	
	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}



LRESULT CSortListCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch(message)
	{
	case WM_ON_ENDEDIT:
		EndEdit(TRUE);
		break;
	default:
		break;
	}
	
	return CListCtrl::WindowProc(message, wParam, lParam);
}

BOOL CSortListCtrl::PreTranslateMessage(MSG* pMsg) 
{
	int nPrevItem = -1;
	int nPrevSubItem = -1;

	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case VK_ESCAPE: // Cancel edit
			m_bEscape = TRUE;
			SetFocus();
			return TRUE;
			
		case VK_RETURN: // Commit edit
			SetFocus();
			return TRUE;
			
		case VK_TAB: // switch edit sub items
			nPrevItem = m_nItem;
			nPrevSubItem = m_nSubItem;

			SetFocus();

			if (nPrevSubItem == GetColumnCount() - 1)
			{
				nPrevSubItem = 0;
			}
			else
			{
				nPrevSubItem++;
			}
			
			StartEdit(nPrevItem, nPrevSubItem);
			return TRUE;
			
		case VK_UP: // edit upper item
			if (m_nItem > 0)
			{
				nPrevItem = m_nItem;
			    nPrevSubItem = m_nSubItem;

				SetFocus();

				nPrevItem--;
				
				
				StartEdit(nPrevItem, nPrevSubItem);
				EnsureVisible(nPrevItem, FALSE);
				return TRUE;
			}
			break;
			
		case VK_DOWN: // edit lower item
			if (m_nItem < CListCtrl::GetItemCount() - 1)
			{
				nPrevItem = m_nItem;
				nPrevSubItem = m_nSubItem;

				SetFocus();

				nPrevItem++;
				
				
				StartEdit(nPrevItem, nPrevSubItem);
				EnsureVisible(nPrevItem, FALSE);
				return TRUE;
			}
			break;			
			
		default:
			break;				
		}
	}
	

	return CListCtrl::PreTranslateMessage(pMsg);
}

//Free Memory
void CSortListCtrl::RemoveFieldArray()
{
	try
	{
		for(int i=0;i<m_arrFieldProperty.GetSize();i++)
		{
			CArrayCodeDomain* pArrayDomain = m_arrFieldProperty[i].m_pArrDomain;
			if(pArrayDomain)
			{
				for (int j=0;j<pArrayDomain->GetSize();j++)
				{
					XCodeDomain* p= pArrayDomain->GetAt(j);
					if (p) delete p;
				}
				delete pArrayDomain;
			}
		}
		m_arrFieldProperty.RemoveAll();
	}
	catch (...) {
	
	}
}
