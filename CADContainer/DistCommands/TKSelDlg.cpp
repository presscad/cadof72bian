#include "StdAfx.h"
#include "resource.h"
#include "TKSelDlg.h"


//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC (CTKSelDlg, CAcUiDialog)

BEGIN_MESSAGE_MAP(CTKSelDlg, CAcUiDialog)
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, OnAcadKeepFocus)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
CTKSelDlg::CTKSelDlg (CWnd *pParent /*=NULL*/, HINSTANCE hInstance /*=NULL*/) 
             :CAcUiDialog (CTKSelDlg::IDD, pParent, hInstance) 
{
}

//-----------------------------------------------------------------------------
void CTKSelDlg::DoDataExchange (CDataExchange *pDX) 
{
	CAcUiDialog::DoDataExchange (pDX) ;
}


LRESULT CTKSelDlg::OnAcadKeepFocus (WPARAM, LPARAM) 
{
	return (TRUE) ;
}


BOOL CTKSelDlg::OnInitDialog() 
{
	CAcUiDialog::OnInitDialog();

	CComboBox * pCombo = (CComboBox *)GetDlgItem(IDC_COMBO1);
	pCombo->SetCurSel(0);

	return TRUE;
}

void CTKSelDlg::OnBnClickedOk()
{
	// 公示类型
	CComboBox * pCombo = (CComboBox *)GetDlgItem(IDC_COMBO1);
	pCombo->GetWindowText(m_strTKName);

	CAcUiDialog::OnOK();
}
