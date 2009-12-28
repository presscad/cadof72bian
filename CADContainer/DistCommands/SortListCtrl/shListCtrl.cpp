// SHListCtrl.cpp : implementation file
//
#include "stdafx.h"
#include "resource.h"
#include "SHListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSHListCtrl

CSHListCtrl::CSHListCtrl()
{
	m_CurSubItem = -1;
	m_bClickEdit = TRUE;
	m_bKeyEvent = FALSE;
}

CSHListCtrl::~CSHListCtrl()
{
}


BEGIN_MESSAGE_MAP(CSHListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CSHListCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSHListCtrl message handlers
void CSHListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	LVHITTESTINFO ht;
	ht.pt = point;
	int rval = SubItemHitTest(&ht);
	
	int oldsubitem = m_CurSubItem;
	m_CurSubItem = IndexToOrder(ht.iSubItem);
	
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	// Make the column fully visible.
	// We have to take into account that the columns may be reordered
	MakeColumnVisible(Header_OrderToIndex(pHeader->m_hWnd, m_CurSubItem));
	
	// Store old state of the item.
	int state = GetItemState(ht.iItem, LVIS_FOCUSED);

	if (m_CurSubItem == -1 || ht.iItem == -1)
	{
		if(ht.iItem == -1)
		{
			NMLISTVIEW dispinfo;
			dispinfo.hdr.hwndFrom = m_hWnd;
			dispinfo.hdr.idFrom = GetDlgCtrlID();
			dispinfo.hdr.code = NM_CLICK;
			
			dispinfo.iItem = -1;
			dispinfo.iSubItem = ht.iSubItem;
			
			GetParent()->SendMessage( WM_NOTIFY, GetDlgCtrlID(), 
				(LPARAM)&dispinfo );
		}
		return;
	}
	
	
	// Call default left button click is here just before we might bail.
	// Also updates the state of the item.
	if (m_bClickEdit)
		m_bKeyEvent = FALSE;

	CListCtrl::OnLButtonDown(nFlags, point);
	
	// Bail if the state from before was not focused or the 
	// user has not already clicked on this cell.
	if (!m_bClickEdit)
	{
		if( !state || m_CurSubItem == -1 || oldsubitem != m_CurSubItem )
			return;
	
		int doedit = 0;
		// If we are in column 0 make sure that the user clicked on 
		// the item label.
		if( 0 == ht.iSubItem )
		{
			if (ht.flags & LVHT_ONITEMLABEL)
				doedit = 1;
		}
		else
		{
			doedit = 1;
		}
		if ( !doedit )
			return;
	}	
    // Send Notification to parent of ListView ctrl
	CString str;
	str = GetItemText( ht.iItem, ht.iSubItem );
    LV_DISPINFO dispinfo;
	dispinfo.hdr.hwndFrom = m_hWnd;
	dispinfo.hdr.idFrom = GetDlgCtrlID();
	dispinfo.hdr.code = LVN_BEGINLABELEDIT;
	
	dispinfo.item.mask = LVIF_TEXT;
	dispinfo.item.iItem = ht.iItem;
	dispinfo.item.iSubItem = ht.iSubItem;
	dispinfo.item.pszText = (LPTSTR)((LPCTSTR)str);
	dispinfo.item.cchTextMax = str.GetLength();
	
	GetParent()->SendMessage( WM_NOTIFY, GetDlgCtrlID(), 
		(LPARAM)&dispinfo );
}

BOOL CSHListCtrl::PositionControl(CWnd * pWnd, int iItem, int iSubItem)
{
	ASSERT( pWnd && pWnd->m_hWnd );
	ASSERT( iItem >= 0 );
	// Make sure that the item is visible
	if (!EnsureVisible(iItem, TRUE))
		return NULL;
	
	// Make sure that nCol is valid
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();
	ASSERT(iSubItem >= 0 && iSubItem < nColumnCount);
	if (iSubItem >= nColumnCount || 
		// We have to take into account that the header may be reordered
		GetColumnWidth(Header_OrderToIndex( pHeader->m_hWnd, iSubItem)) < 5)
	{
		return 0;
	}
	
	// Get the header order array to sum the column widths up to the selected cell.
	int *orderarray = new int[nColumnCount];
	Header_GetOrderArray(pHeader->m_hWnd, nColumnCount, orderarray);
	int offset = 0;
	int i;
	for (i = 0; orderarray[i] != iSubItem; i++)
		offset += GetColumnWidth(orderarray[i]);
	int colwidth = GetColumnWidth(iSubItem);
	delete[] orderarray;
	
	CRect rect;
	GetItemRect(iItem, &rect, LVIR_BOUNDS);
	
	// Scroll if we need to expose the column
	CRect rcClient;
	GetClientRect(&rcClient);
	if (offset + rect.left < 0 || offset + colwidth + rect.left > rcClient.right)
	{
		CSize size;
		size.cx = offset + rect.left;
		size.cy = 0;
		Scroll( size );
		rect.left -= size.cx;
	}
	
	rect.left += offset + 4;
	rect.right = rect.left + colwidth - 3;
	// The right end of the control should not go past the edge 
	// of the grid control.
	if (rect.right > rcClient.right)
		rect.right = rcClient.right;
	pWnd->MoveWindow(&rect);
	
	return 1;
}

void CSHListCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// This function is called by the control in different 
	// stages during the control drawing process.
	if (!m_bKeyEvent)
		return;

	NMLVCUSTOMDRAW *pCD = (NMLVCUSTOMDRAW*)pNMHDR;
	// By default set the return value to do the default behavior.
	*pResult = 0;
	
	switch( pCD->nmcd.dwDrawStage )
	{
	case  CDDS_PREPAINT:  // First stage (for the whole control)
		// Tell the control we want to receive drawing messages  
		// for drawing items.
		*pResult = CDRF_NOTIFYITEMDRAW;
		// The next stage is handled in the default:
		break;
	case CDDS_ITEMPREPAINT | CDDS_SUBITEM: // Stage three (called for each subitem of the focused item)
		{
			// We don't want to draw anything here, but we need to respond 
			// of DODEFAULT will be the next stage.
			// Tell the control we want to handle drawing after the subitem 
			// is drawn.
			*pResult = CDRF_NOTIFYSUBITEMDRAW | CDRF_NOTIFYPOSTPAINT;
		}
		break;
	case CDDS_ITEMPOSTPAINT | CDDS_SUBITEM: // Stage four (called for each subitem of the focused item)
		{
			// We do the drawing here (well maybe).
			// This is actually after the control has done its drawing
			// on the subitem.  Since drawing a cell is near instantaneous
			// the user won't notice.
			int subitem = pCD->iSubItem;
			// Only do our own drawing if this subitem has focus at the item level.
			if( (pCD->nmcd.uItemState & CDIS_FOCUS) )
			{
				// If this subitem is the subitem with the current focus,
				// draw it.  Otherwise let the control draw it.  
				CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
				// We have to take into account the possibility that the 
				// columns may be reordered.
				if( subitem == Header_OrderToIndex( pHeader->m_hWnd,  m_CurSubItem ) )
				{
					// POSTERASE
					CDC* pDC = CDC::FromHandle(pCD->nmcd.hdc);
					// Calculate the offset of the text from the right and left of the cell.
					int offset = pDC->GetTextExtent(_T(" "), 1 ).cx*2;
					// The rect for the cell gives correct left and right values.
					CRect rect = pCD->nmcd.rc;
					CRect bounds;
					GetItemRect( pCD->nmcd.dwItemSpec, &bounds, LVIR_BOUNDS );
					// Get the top and bottom from the item itself.
					rect.top = bounds.top;
					rect.bottom = bounds.bottom;
					// Adjust rectangle for horizontal scroll and first column label
					{
						if( subitem == 0 )
						{
							CRect lrect;
							GetItemRect( pCD->nmcd.dwItemSpec, &lrect, LVIR_LABEL );
							rect.left = lrect.left;
							rect.right = lrect.right;
						}
						else
						{
							rect.right += bounds.left;
							rect.left  += bounds.left;
						}
					}
					// Clear the background with button face color
					pDC->FillRect(rect, &CBrush(::GetSysColor(COLOR_3DFACE)));
					// PREPAINT
					CString str;
					str = GetItemText( pCD->nmcd.dwItemSpec, pCD->iSubItem );
					// Deflate the rect by the horizontal offset.
					rect.DeflateRect( offset, 0 );
					// You could also make this column alignment sensitive here.
					pDC->DrawText( str, rect, 
						DT_SINGLELINE|DT_NOPREFIX|DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS);
					// POSTPAINT
					// Draw rounded edge
					rect.InflateRect( offset, 0 );
					pDC->Draw3dRect( &rect, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DFACE) );
					rect.DeflateRect( 1, 1 );
					pDC->Draw3dRect( &rect, ::GetSysColor(COLOR_3DDKSHADOW), ::GetSysColor(COLOR_3DHILIGHT) );
					// Tell the control that we handled the drawing for this subitem.
					*pResult = CDRF_SKIPDEFAULT;
				}
			}
		}
		break;
	default: // Stage two handled here. (called for each item)
		if( !(pCD->nmcd.uItemState & CDIS_FOCUS) )
		{
			// If this item does not have focus, let the 
			// control draw the whole item.
			*pResult = CDRF_DODEFAULT;
		}
		else
		{
			// If this item has focus, tell the control we want
			// to handle subitem drawing.
			*pResult = CDRF_NOTIFYSUBITEMDRAW;
		}
		break;
	}
}
	
void CSHListCtrl::MakeColumnVisible(int nCol)
{
	if( nCol < 0 )
		return;
	// Get the order array to total the column offset.
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int colcount = pHeader->GetItemCount();
	ASSERT(nCol < colcount);
	int *orderarray = new int[colcount];
	Header_GetOrderArray(pHeader->m_hWnd, colcount, orderarray);
	// Get the column offset
	int offset = 0;
	for (int i = 0; orderarray[i] != nCol; i++)
		offset += GetColumnWidth(orderarray[i]);
	int colwidth = GetColumnWidth(nCol);
	delete[] orderarray;
	
	CRect rect;
	GetItemRect(0, &rect, LVIR_BOUNDS);
	
	// Now scroll if we need to expose the column
	CRect rcClient;
	GetClientRect(&rcClient);
	if (offset + rect.left < 0 || offset + colwidth + rect.left > rcClient.right)
	{
		CSize size;
		size.cx = offset + rect.left;
		size.cy = 0;
		Scroll(size);
		rect.left -= size.cx;
	}
}
	
BOOL CSHListCtrl::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN)
	{
		// Handle the keystrokes for the left and right keys
		// to move the cell selection left and right.
		// Handle F2 to commence edit mode from the keyboard.
		// Only handle these if the grid control has the focus.
		// (Messages also come through here for the edit control
		// and we don't want them.
		if( this == GetFocus() )
		{
			if (m_bClickEdit)
				m_bKeyEvent = TRUE;
			switch( pMsg->wParam )
			{
			case VK_LEFT:
				{
					// Decrement the order number.
					m_CurSubItem--;
					if( m_CurSubItem < -1 ) 
					{
						// This indicates that the whole row is selected and F2 means nothing.
						m_CurSubItem = -1;
					}
					else
					{
						CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
						// Make the column visible.
						// We have to take into account that the header
						// may be reordered.
						MakeColumnVisible( Header_OrderToIndex( pHeader->m_hWnd, m_CurSubItem ) );
						// Invalidate the item.
						int iItem = GetNextItem( -1, LVNI_FOCUSED );
						if( iItem != -1 )
						{
							CRect rcBounds;
							GetItemRect(iItem, rcBounds, LVIR_BOUNDS);
							InvalidateRect( &rcBounds );
						}
					}
				}
				return TRUE;
			case VK_RIGHT:
				{
					// Increment the order number.
					m_CurSubItem++;
					CHeaderCtrl* pHeader = (CHeaderCtrl*) GetDlgItem(0);
					int nColumnCount = pHeader->GetItemCount();
					// Don't go beyond the last column.
					if( m_CurSubItem > nColumnCount -1 ) 
					{
						m_CurSubItem = nColumnCount - 1;
					}
					else
					{
						// We have to take into account that the header
						// may be reordered.
						MakeColumnVisible( Header_OrderToIndex( pHeader->m_hWnd, m_CurSubItem ) );
						int iItem = GetNextItem( -1, LVNI_FOCUSED );
						// Invalidate the item.
						if( iItem != -1 )
						{
							CRect rcBounds;
							GetItemRect(iItem, rcBounds, LVIR_BOUNDS);
							InvalidateRect( &rcBounds );
						}
					}
				}
				return TRUE;
			case VK_F2: // Enter nondestructive edit mode.
				{
					int iItem = GetNextItem( -1, LVNI_FOCUSED );
					if( m_CurSubItem != -1 && iItem != -1 )
					{
						// Send Notification to parent of ListView ctrl
						CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
						CString str;
						// We have to take into account that the header
						// may be reordered.
						str = GetItemText( iItem, Header_OrderToIndex( pHeader->m_hWnd, m_CurSubItem ) );
						LV_DISPINFO dispinfo;
						dispinfo.hdr.hwndFrom = m_hWnd;
						dispinfo.hdr.idFrom = GetDlgCtrlID();
						dispinfo.hdr.code = LVN_BEGINLABELEDIT;
						
						dispinfo.item.mask = LVIF_TEXT;
						dispinfo.item.iItem = iItem;
						// We have to take into account that the header
						// may be reordered.
						dispinfo.item.iSubItem = Header_OrderToIndex( pHeader->m_hWnd, m_CurSubItem );
						dispinfo.item.pszText = (LPTSTR)((LPCTSTR)str);
						dispinfo.item.cchTextMax = str.GetLength();
						// Send message to the parent that we are ready to edit.
						GetParent()->SendMessage( WM_NOTIFY, GetDlgCtrlID(), 
							(LPARAM)&dispinfo );
					}
				}
				break;
			case VK_RETURN:
				{
					// Increment the order number.
					m_CurSubItem++;
					CHeaderCtrl* pHeader = (CHeaderCtrl*) GetDlgItem(0);
					int nColumnCount = pHeader->GetItemCount();
					// Don't go beyond the last column.
					if( m_CurSubItem > nColumnCount -1 ) 
					{
						m_CurSubItem = nColumnCount - 1;
					}
					else
					{
						// We have to take into account that the header
						// may be reordered.
						MakeColumnVisible( Header_OrderToIndex( pHeader->m_hWnd, m_CurSubItem ) );
						int iItem = GetNextItem( -1, LVNI_FOCUSED );
						// Invalidate the item.
						if( iItem != -1 )
						{
							CRect rcBounds;
							GetItemRect(iItem, rcBounds, LVIR_BOUNDS);
							InvalidateRect( &rcBounds );
						}
					}
					//
					int iItem = GetNextItem( -1, LVNI_FOCUSED );
					if( m_CurSubItem != -1 && iItem != -1 )
					{
						// Send Notification to parent of ListView ctrl
						CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
						CString str;
						// We have to take into account that the header
						// may be reordered.
						str = GetItemText( iItem, Header_OrderToIndex( pHeader->m_hWnd, m_CurSubItem ) );
						LV_DISPINFO dispinfo;
						dispinfo.hdr.hwndFrom = m_hWnd;
						dispinfo.hdr.idFrom = GetDlgCtrlID();
						dispinfo.hdr.code = LVN_BEGINLABELEDIT;
						
						dispinfo.item.mask = LVIF_TEXT;
						dispinfo.item.iItem = iItem;
						// We have to take into account that the header
						// may be reordered.
						dispinfo.item.iSubItem = Header_OrderToIndex( pHeader->m_hWnd, m_CurSubItem );
						dispinfo.item.pszText = (LPTSTR)((LPCTSTR)str);
						dispinfo.item.cchTextMax = str.GetLength();
						// Send message to the parent that we are ready to edit.
						GetParent()->SendMessage(WM_NOTIFY, GetDlgCtrlID(), 
							(LPARAM)&dispinfo);
					}
				}
				return TRUE;
			default:
				break;
			}
		}

	}
	m_tooltip.RelayEvent(pMsg);	

	return CListCtrl::PreTranslateMessage(pMsg);
}
	
int CSHListCtrl::IndexToOrder( int iIndex )
{
	// Since the control only provide the OrderToIndex macro,
	// we have to provide the IndexToOrder.  This translates
	// a column index value to a column order value.
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int colcount = pHeader->GetItemCount();
	int *orderarray = new int[ colcount ];
	Header_GetOrderArray( pHeader->m_hWnd, colcount, orderarray );
	int i;
	for( i=0; i<colcount; i++ )
	{
		if( orderarray[i] == iIndex )
		{
 			delete[] orderarray;
			return i;
		}
	}
 	delete[] orderarray;
	return -1;
}
	
void CSHListCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if(GetFocus() != this)
		SetFocus();
	CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}
	
void CSHListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if(GetFocus() != this)
		SetFocus();
	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}
CComboBox* CSHListCtrl::ShowInPlaceComboBox(int nItem, int nSubitem,DWORD dwStyle)
{
	// The returned pointer should not be saved
	// Make sure that the item is visible
	// My code start here
	if (!EnsureVisible(nItem, TRUE) )
		return NULL;
	// Make sure that nCol is valid 
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();
	if( nSubitem >= nColumnCount || GetColumnWidth(nSubitem) < 10 ) 
		return NULL;
	// Get the column offset
	int offset = 0;
	for(int i = 0; i < nSubitem; i++)
		offset += GetColumnWidth(i);

	CRect rect;
	GetItemRect(nItem, &rect, LVIR_BOUNDS);

	// Now scroll if we need to expose the column
	CRect rcClient;
	GetClientRect(&rcClient);
	if(offset + rect.left < 0 || offset + rect.left > rcClient.right)
	{
		CSize size;
		size.cx = offset + rect.left;
		size.cy = 0;
		Scroll(size);
		rect.left -= size.cx;
	}

	rect.left += offset;
	rect.right = rect.left + GetColumnWidth( nSubitem ) ;
	int height = rect.bottom - rect.top;
	rect.bottom += 22 * height;
	if (rect.right > rcClient.right)
		rect.right = rcClient.right;

	dwStyle |= WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DISABLENOSCROLL;

	CComboBox* pList = new CSHInPlaceComboBox(nItem, nSubitem);
	pList->Create(dwStyle, rect, this, IDC_INPLACE_COMBOBOX);
	pList->SetItemHeight(-1, height);
	pList->SetHorizontalExtent(GetColumnWidth(nSubitem));

	CString strWindowText = GetItemText(nItem, nSubitem);
	if(dwStyle & CBS_DROPDOWN && !(dwStyle & CBS_DROPDOWNLIST))
	{
		CEdit* m_pEdit = new CEdit();
		m_pEdit->SubclassWindow(pList->GetDlgItem(1001)->GetSafeHwnd());
		m_pEdit->SetWindowText(strWindowText);
		m_pEdit->SetSel(0,-1);
	}

	return pList;
	// My code end here
}
CDateTimeCtrl* CSHListCtrl::ShowInPlaceCDateTimeCtrl(int nItem, int nSubitem,DWORD dwStyle)
{
	// The returned pointer should not be saved
	// Make sure that the item is visible
	// My code start here
	if (!EnsureVisible(nItem, TRUE) )
		return NULL;
	// Make sure that nCol is valid 
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();
	if( nSubitem >= nColumnCount || GetColumnWidth(nSubitem) < 10 ) 
		return NULL;
	// Get the column offset
	int offset = 0;
	for(int i = 0; i < nSubitem; i++)
		offset += GetColumnWidth(i);

	CRect rect;
	GetItemRect(nItem, &rect, LVIR_BOUNDS);

	// Now scroll if we need to expose the column
	CRect rcClient;
	GetClientRect(&rcClient);
/*	if(offset + rect.left < 0 || offset + rect.left > rcClient.right)
	{
		CSize size;
		size.cx = offset + rect.left;
		size.cy = 0;
		Scroll(size);
		rect.left -= size.cx;
	}
*/
	rect.left += offset;
	rect.right = rect.left + GetColumnWidth( nSubitem ) ;
	rect.bottom +=5;
	if (rect.right > rcClient.right)
		rect.right = rcClient.right;

	dwStyle |= WS_CHILD | WS_VISIBLE; //| DTS_UPDOWN | DTS_LONGDATEFORMAT | DTS_RIGHTALIGN;
	CDateTimeCtrl* pData = new SHDataTimeCtrl(nItem, nSubitem);
	pData->Create(dwStyle, rect, this, IDC_INPLACE_DataTime);

	return pData;
	// My code end here
}

CSHInPlaceEdit* CSHListCtrl::ShowInPlaceEdit(int nItem, int nSubitem,DWORD dwStyle)
{
	CSHInPlaceEdit* pListEdit = new CSHInPlaceEdit(nItem, nSubitem);
	// Start with a small rectangle.  We'll change it later.
	CRect rect(0,0,1,1);
	dwStyle |= ES_LEFT | WS_BORDER | WS_CHILD | WS_VISIBLE | 
		ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL;
	pListEdit->Create(dwStyle, rect, this, IDC_INPLACE_EDIT);
	// Have the Grid position and size the custom edit control
	PositionControl(pListEdit, nItem, nSubitem);
	// Have the edit box size itself to its content.
	pListEdit->CalculateSize();
	CString sText = GetItemText(nItem, nSubitem);
	pListEdit->SetWindowText(sText);
	pListEdit->SetSel(0, -1);
	// Return TRUE so that the list control will hnadle NOT edit label itself. 
	return pListEdit;
}

CSHInPlaceEdit* CSHListCtrl::ShowInPlaceEditEx(int nItem, int nSubitem, DWORD dwStyle)
{
	CSHInPlaceEdit* pEdit = new CSHInPlaceEdit(nItem, nSubitem);
	CRect rect(0,0,1,1);
	dwStyle |= ES_LEFT | WS_BORDER | WS_CHILD | WS_VISIBLE | 
		ES_AUTOHSCROLL;
	pEdit->Create(dwStyle, rect, this, IDC_INPLACE_EDIT);

	CSHInPlaceButton* pButton = new CSHInPlaceButton(nItem, nSubitem);
	dwStyle = WS_CHILD | WS_VISIBLE;
	pButton->Create("...", dwStyle, rect, this, IDC_INPLACE_BUTTON);
	pEdit->SetButtonCtrl(pButton);
	pButton->SetEditCtrl(pEdit);
	//
	if (!EnsureVisible(nItem, TRUE))
		return NULL;
	
	// Make sure that nCol is valid
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();
	ASSERT(nSubitem >= 0 && nSubitem < nColumnCount);
	if (nSubitem >= nColumnCount || 
		// We have to take into account that the header may be reordered
		GetColumnWidth(Header_OrderToIndex( pHeader->m_hWnd, nSubitem)) < 5)
	{
		return 0;
	}
	
	// Get the header order array to sum the column widths up to the selected cell.
	int *orderarray = new int[nColumnCount];
	Header_GetOrderArray(pHeader->m_hWnd, nColumnCount, orderarray);
	int offset = 0;
	int i;
	for (i = 0; orderarray[i] != nSubitem; i++)
		offset += GetColumnWidth(orderarray[i]);
	int colwidth = GetColumnWidth(nSubitem);
	delete[] orderarray;
	
	GetItemRect(nItem, &rect, LVIR_BOUNDS);
	
	// Scroll if we need to expose the column
	CRect rcClient;
	GetClientRect(&rcClient);
	if (offset + rect.left < 0 || offset + colwidth + rect.left > rcClient.right)
	{
		CSize size;
		size.cx = offset + rect.left;
		size.cy = 0;
		Scroll( size );
		rect.left -= size.cx;
	}
	
	rect.left += offset + 4;
	rect.right = rect.left + colwidth - 3;
	// The right end of the control should not go past the edge 
	// of the grid control.
	if (rect.right > rcClient.right)
		rect.right = rcClient.right;
	rect.bottom--;
	CRect rcButton;
	rcButton = rect;
	rcButton.left = rcButton.right - rcButton.Height();
	rect.right = rect.right - rect.Height();
	pEdit->MoveWindow(&rect);
	pButton->MoveWindow(&rcButton);

	//
	PositionControl(pEdit, nItem, nSubitem);
	pEdit->CalculateSize();
	pEdit->GetWindowRect( &rect );
	rect.left = rect.right;
	rect.right += rect.Height();
	ScreenToClient( &rect );
	pButton->MoveWindow(&rect);

	CString sText = GetItemText(nItem, nSubitem);
	pEdit->SetWindowText(sText);
	pEdit->SetSel(0, -1);

	return pEdit;
}

void CSHListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	LVHITTESTINFO ht;
	ht.pt = point;
	// Test for which subitem was clicked.
	// Use macro since this is new and not in MFC.
	int rval = ListView_SubItemHitTest( m_hWnd, &ht );
	
	// Store the old column number and set the new column value.
	int oldsubitem = m_CurSubItem;
	m_CurSubItem = IndexToOrder( ht.iSubItem );
	
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	// Make the column fully visible.
	// We have to take into account that the columns may be reordered
	MakeColumnVisible( Header_OrderToIndex( pHeader->m_hWnd, m_CurSubItem ) );
	
	// Store old state of the item.
	int state = GetItemState( ht.iItem, LVIS_FOCUSED );
	
	// Bail if the state from before was not focused or the 
	// user has not already clicked on this cell.
	if( !state 
		|| m_CurSubItem == -1 
		|| oldsubitem != m_CurSubItem ) return;
	
    // Send Notification to parent of ListView ctrl
	CString str;
	str = GetItemText( ht.iItem, ht.iSubItem );
    LV_DISPINFO dispinfo;
	dispinfo.hdr.hwndFrom = m_hWnd;
	dispinfo.hdr.idFrom = GetDlgCtrlID();
	dispinfo.hdr.code = LVN_BEGINLABELEDIT;
	
	dispinfo.item.mask = LVIF_TEXT;
	dispinfo.item.iItem = ht.iItem;
	dispinfo.item.iSubItem = ht.iSubItem;
	dispinfo.item.pszText = (LPTSTR)((LPCTSTR)str);
	dispinfo.item.cchTextMax = str.GetLength();
	
	GetParent()->SendMessage( WM_NOTIFY, GetDlgCtrlID(), 
		(LPARAM)&dispinfo );
	// My code end here
	
	CListCtrl::OnLButtonDblClk(nFlags, point);
}

void CSHListCtrl::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	CListCtrl::PreSubclassWindow();

	m_tooltip.Create(this);

	ASSERT(m_hWnd);
    DWORD dwStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	dwStyle &= ~(LVS_TYPEMASK);
	dwStyle &= ~(LVS_EDITLABELS);
	
	// Make sure we have report view and send edit label messages.
    SetWindowLong(m_hWnd, GWL_STYLE, dwStyle | LVS_REPORT);
	
	// Enable the full row selection and the drag drop of headers.
	DWORD dwStylesEx = 
		LVS_EX_FULLROWSELECT | 
		LVS_EX_GRIDLINES | 
		LVS_EX_INFOTIP;
	// Use macro since this is new and not in MFC.
	ListView_SetExtendedListViewStyleEx(m_hWnd, dwStylesEx, dwStylesEx);
}

BOOL CSHListCtrl::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	// TODO: Add your specialized code here and/or call the base class
	HD_NOTIFY	*pHDN = (HD_NOTIFY*)lParam;

	//if((pHDN->hdr.code == HDN_ENDTRACKA || pHDN->hdr.code == HDN_ENDTRACKW))
	//{
	//	// Update the tooltip info
	//	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	//	int nColumnCount = pHeader->GetItemCount();

	//	CToolInfo toolinfo;
	//	toolinfo.cbSize = sizeof( toolinfo );

	//	// Cycle through the tooltipinfo for each effected column
	//	for( int i = pHDN->iItem; i <= nColumnCount; i++ )
	//	{
	//		m_tooltip.GetToolInfo( toolinfo, pHeader, i + 1 );

	//		int dx;				// store change in width
	//		if( i == pHDN->iItem )
	//			dx = pHDN->pitem->cxy - 
	//				(toolinfo.rect.right - toolinfo.rect.left);
	//		else 
	//			toolinfo.rect.left += dx;
	//		toolinfo.rect.right += dx;
	//		m_tooltip.SetToolInfo( &toolinfo );
	//	}
	//}
	
	return CListCtrl::OnNotify(wParam, lParam, pResult);

}

BOOL CSHListCtrl::AddHeaderToolTip(int nCol, LPCTSTR sTip )
{
	const int TOOLTIP_LENGTH = 80;
	char buf[TOOLTIP_LENGTH+1];

	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();
	if( nCol >= nColumnCount)
		return FALSE;

	if( (GetStyle() & LVS_TYPEMASK) != LVS_REPORT )
		return FALSE;

	// Get the header height
	RECT rect;
	pHeader->GetClientRect( &rect );
	int height = rect.bottom;

	RECT rctooltip;
	rctooltip.top = 0;
	rctooltip.bottom = rect.bottom;

	// Now get the left and right border of the column
	rctooltip.left = 0 - GetScrollPos( SB_HORZ );
	for( int i = 0; i < nCol; i++ )
		rctooltip.left += GetColumnWidth( i );
	rctooltip.right = rctooltip.left + GetColumnWidth( nCol );

	if( sTip == NULL )
	{
		// Get column heading
		LV_COLUMN lvcolumn;
		lvcolumn.mask = LVCF_TEXT;
		lvcolumn.pszText = buf;
		lvcolumn.cchTextMax = TOOLTIP_LENGTH;
		if( !GetColumn( nCol, &lvcolumn ) )
			return FALSE;
	}


	m_tooltip.AddTool( GetDlgItem(0), sTip ? sTip : buf, &rctooltip, nCol+1 );
	return TRUE;

}

int CSHListCtrl::InsertColumn( int nCol, const LVCOLUMN* pColumn )
{

	int nRet = CListCtrl::InsertColumn(nCol, pColumn);
	AddHeaderToolTip(nCol, pColumn->pszText);
	return nRet;
}

int CSHListCtrl::InsertColumn(int nCol, LPCTSTR lpszColumnHeading, int nFormat,
							  int nWidth, int nSubItem)
{
	LVCOLUMN column;
	column.mask = LVCF_TEXT|LVCF_FMT;
	column.pszText = (LPTSTR)lpszColumnHeading;
	column.fmt = nFormat;
	if (nWidth != -1)
	{
		column.mask |= LVCF_WIDTH;
		column.cx = nWidth;
	}
	if (nSubItem != -1)
	{
		column.mask |= LVCF_SUBITEM;
		column.iSubItem = nSubItem;
	}
	return CSHListCtrl::InsertColumn(nCol, &column);
}
