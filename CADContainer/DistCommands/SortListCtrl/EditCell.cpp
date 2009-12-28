
//

#include "stdafx.h"
#include "EditCell.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditCell

CEditCell::CEditCell()
{

}

CEditCell::~CEditCell()
{

}


BEGIN_MESSAGE_MAP(CEditCell, CEdit)
	//{{AFX_MSG_MAP(CEditCell)
	ON_WM_KILLFOCUS()
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_WM_GETDLGCODE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditCell message handlers

int CEditCell::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}



void CEditCell::OnKillFocus(CWnd* pNewWnd) 
{
	CEdit::OnKillFocus(pNewWnd);
	
    GetParent()->SendMessage(WM_ON_ENDEDIT,0,0);	
}

void CEditCell::OnDestroy() 
{
	CEdit::OnDestroy();
	
//	delete this;
	
}


UINT CEditCell::OnGetDlgCode() 
{	
	return CEdit::OnGetDlgCode() | DLGC_WANTARROWS | DLGC_WANTTAB;
}

