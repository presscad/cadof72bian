#include "StdAfx.h"
#include "resource.h"
#include "Dlg_TKYongDi.h"


//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC (CTKYongDiDlg, CAcUiDialog)

BEGIN_MESSAGE_MAP(CTKYongDiDlg, CAcUiDialog)
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, OnAcadKeepFocus)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
CTKYongDiDlg::CTKYongDiDlg (CWnd *pParent /*=NULL*/, HINSTANCE hInstance /*=NULL*/) 
             :CAcUiDialog (CTKYongDiDlg::IDD, pParent, hInstance) 
{
}

//-----------------------------------------------------------------------------
void CTKYongDiDlg::DoDataExchange (CDataExchange *pDX) 
{
	CAcUiDialog::DoDataExchange (pDX) ;
}


LRESULT CTKYongDiDlg::OnAcadKeepFocus (WPARAM, LPARAM) 
{
	return (TRUE) ;
}


BOOL CTKYongDiDlg::OnInitDialog() 
{
	CAcUiDialog::OnInitDialog();

	UINT nID[] = 
	{
		IDC_EDIT1,IDC_EDIT4,IDC_EDIT5,IDC_EDIT9,
	};
	CString strValues[] = 
	{
		strBicode,strOwner,strOwnerProperty,strDate,
	};

	long nCount = sizeof(nID)/sizeof(UINT);

	for (int i=0; i< nCount; i++)
	{
		CEdit * pEdit = (CEdit *)GetDlgItem(nID[i]);
		pEdit->SetWindowText(strValues[i]);
	}

	return TRUE;
}

void CTKYongDiDlg::OnBnClickedOk()
{
	CString strTags[] = 
	{
		"项目编号","许可证编号","比例","用地单位","用地性质","图幅名","签发","备注","日期","审批意见",
	};
	UINT nID[] = 
	{
		IDC_EDIT1,IDC_EDIT2,IDC_EDIT3,IDC_EDIT4,IDC_EDIT5,IDC_EDIT6,IDC_EDIT7,IDC_EDIT8,IDC_EDIT9,IDC_EDIT10,
	};

	long nCount = sizeof(strTags)/sizeof(CString);

	for (int i=0; i< nCount; i++)
	{
		CString strTag = strTags[i];
		CString strValue;

		CEdit * pEdit = (CEdit *)GetDlgItem(nID[i]);
		pEdit->GetWindowText(strValue);

		arrayTag.Add(strTag);
		arrayValue.Add(strValue);
	}

	CAcUiDialog::OnOK();
}
