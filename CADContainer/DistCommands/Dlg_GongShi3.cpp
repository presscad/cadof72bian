#include "StdAfx.h"
#include "resource.h"
#include "Dlg_GongShi3.h"
#include "CommonFunc.h"

//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC (CDlgGongShi3, CAcUiDialog)

BEGIN_MESSAGE_MAP(CDlgGongShi3, CAcUiDialog)
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, OnAcadKeepFocus)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
CDlgGongShi3::CDlgGongShi3 (CWnd *pParent /*=NULL*/, HINSTANCE hInstance /*=NULL*/) 
             :CAcUiDialog (CDlgGongShi3::IDD, pParent, hInstance) 
{
}

//-----------------------------------------------------------------------------
void CDlgGongShi3::DoDataExchange (CDataExchange *pDX) 
{
	CAcUiDialog::DoDataExchange (pDX) ;
}


LRESULT CDlgGongShi3::OnAcadKeepFocus (WPARAM, LPARAM) 
{
	return (TRUE) ;
}

BOOL CDlgGongShi3::OnInitDialog() 
{
	CAcUiDialog::OnInitDialog();

	CComboBox * pCombo = (CComboBox *)GetDlgItem(IDC_COMBO1);
	pCombo->SetCurSel(0);
	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO2);
	pCombo->SetCurSel(0);

	return TRUE;
}

void CDlgGongShi3::OnBnClickedOk()
{
	// 公示类型
	CComboBox * pCombo = (CComboBox *)GetDlgItem(IDC_COMBO1);
	m_nClass = pCombo->GetCurSel();
	pCombo->GetWindowText(m_strGSLX);
	// 公示栏目
	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO2);
	m_nGSLM = pCombo->GetCurSel();
	pCombo->GetWindowText(m_strGSLM);

	CAcUiDialog::OnOK();
}
