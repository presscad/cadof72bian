#include "StdAfx.h"
#include "resource.h"
#include "Dlg_GongShi2.h"
#include "CommonFunc.h"

//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC (CDlgGongShi2, CAcUiDialog)

BEGIN_MESSAGE_MAP(CDlgGongShi2, CAcUiDialog)
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, OnAcadKeepFocus)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
CDlgGongShi2::CDlgGongShi2 (CWnd *pParent /*=NULL*/, HINSTANCE hInstance /*=NULL*/) 
             :CAcUiDialog (CDlgGongShi2::IDD, pParent, hInstance) 
{
}

//-----------------------------------------------------------------------------
void CDlgGongShi2::DoDataExchange (CDataExchange *pDX) 
{
	CAcUiDialog::DoDataExchange (pDX) ;
}


LRESULT CDlgGongShi2::OnAcadKeepFocus (WPARAM, LPARAM) 
{
	return (TRUE) ;
}

BOOL CDlgGongShi2::OnInitDialog() 
{
	CAcUiDialog::OnInitDialog();

	TCHAR lpCadProjectFileName[MAX_PATH] = {0};
	GetModuleFileName(GetModuleHandle("shDistCommands.arx"), lpCadProjectFileName, MAX_PATH); 
	PathRemoveFileSpec(lpCadProjectFileName);

	PathAppend(lpCadProjectFileName, "CadProject.ini");

	TCHAR lpReceiver[100] = {0};
	GetPrivateProfileString("公示", "审批人", "", lpReceiver, 100, lpCadProjectFileName);

	CString strReceiver = lpReceiver;

	CStringArray userArray;
	decodeCString(strReceiver, userArray, ';');

	CComboBox * pCombo = (CComboBox *)GetDlgItem(IDC_COMBO1);
	for (int i=0; i<userArray.GetCount(); i++)
	{
		pCombo->AddString(userArray.GetAt(i));
	}

	pCombo->SetCurSel(0);

	return TRUE;
}

void CDlgGongShi2::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码

	// 公示类型
	CComboBox * pCombo = (CComboBox *)GetDlgItem(IDC_COMBO1);
	pCombo->GetWindowText(m_strReceiver);

	CAcUiDialog::OnOK();
}
