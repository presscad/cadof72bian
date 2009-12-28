
//

#include "stdafx.h"
#include "NewComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewComboBox
#define WM_ON_ENDEDIT	   (WM_APP + 1001)

CNewComboBox::CNewComboBox()
{
}

CNewComboBox::~CNewComboBox()
{
}


BEGIN_MESSAGE_MAP(CNewComboBox, CComboBox)
	//{{AFX_MSG_MAP(CNewComboBox)
	ON_CONTROL_REFLECT(CBN_KILLFOCUS, OnKillfocus)
	ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCloseup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewComboBox message handlers

void CNewComboBox::OnKillfocus() 
{
	GetParent()->SendMessage(WM_ON_ENDEDIT,0,0);	
}

void CNewComboBox::OnCloseup() 
{
	GetParent()->SendMessage(WM_ON_ENDEDIT,0,0);	
}