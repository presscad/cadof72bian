#include "StdAfx.h"
#include "resource.h"
#include "Dlg_GongShi.h"
#include "CommonFunc.h"

//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC (CDlgGongShi, CAcUiDialog)

BEGIN_MESSAGE_MAP(CDlgGongShi, CAcUiDialog)
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, OnAcadKeepFocus)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_BROWSE_EX, OnBnClickedBtnBrowse)
	ON_CBN_SELCHANGE(IDC_COMBO4, OnSelchangeCombo4)
	ON_BN_CLICKED(IDC_BTN_DELETE_EX, OnBnClickedBtnDeleteEx)
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
CDlgGongShi::CDlgGongShi (CWnd *pParent /*=NULL*/, HINSTANCE hInstance /*=NULL*/) 
             :CAcUiDialog (CDlgGongShi::IDD, pParent, hInstance) 
{
	m_nLimit = 0;
//	m_strFileName = "";
	m_bExt = FALSE;
}

//-----------------------------------------------------------------------------
void CDlgGongShi::DoDataExchange (CDataExchange *pDX) 
{
	CAcUiDialog::DoDataExchange (pDX) ;

//	DDX_Text(pDX, IDC_EDIT1, m_strGSBH);
//	DDV_MaxChars(pDX, m_strGSBH, 100);
	DDX_Text(pDX, IDC_EDIT2, m_strBJBH);
//	DDV_MaxChars(pDX, m_strBJBH, 100);
	DDX_Text(pDX, IDC_EDIT3, m_strXMMC); // 项目名称
//	DDV_MaxChars(pDX, m_strXMMC, 100);
	DDX_Text(pDX, IDC_EDIT4, m_strJSDW); // 建设单位
//	DDV_MaxChars(pDX, m_strJSDW, 100);
	DDX_Text(pDX, IDC_EDIT5, m_strYDXZ); // 用地位置
//	DDV_MaxChars(pDX, m_strYDXZ, 100);
	DDX_Text(pDX, IDC_EDIT6, m_strGCXZ); // 工程性质
//	DDV_MaxChars(pDX, m_strGCXZ, 100);

	DDX_Text(pDX, IDC_EDIT7, m_strYDZMJ); // 地块总面积
//	DDV_MaxChars(pDX, m_strYDZMJ, 100);
	DDX_Text(pDX, IDC_EDIT8, m_strYDJMJ); // 地块净面积
//	DDV_MaxChars(pDX, m_strYDJMJ, 100);
	DDX_Text(pDX, IDC_EDIT9, m_strYDDZMJ); // 代征道路绿化面积
//	DDV_MaxChars(pDX, m_strYDDZMJ, 100);
	DDX_Text(pDX, IDC_EDIT10, m_strRJL); // 容积率
//	DDV_MaxChars(pDX, m_strRJL, 100);
	DDX_Text(pDX, IDC_EDIT11, m_strJZMD); // 建筑密度
//	DDV_MaxChars(pDX, m_strJZMD, 100);
	DDX_Text(pDX, IDC_EDIT12, m_strJZGM); // 建筑规模
//	DDV_MaxChars(pDX, m_strJZGM, 100);
	DDX_Text(pDX, IDC_EDIT13, m_strJZZG); // 建筑总高
//	DDV_MaxChars(pDX, m_strJZZG, 100);
	DDX_Text(pDX, IDC_EDIT14, m_strLDL); // 绿地率
//	DDV_MaxChars(pDX, m_strLDL, 100);

	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_wndStartTime);
	DDX_Control(pDX, IDC_DATETIMEPICKER2, m_wndEndTime);

	DDX_Control(pDX, IDC_FILELIST, m_ctrlFileList);
}


LRESULT CDlgGongShi::OnAcadKeepFocus (WPARAM, LPARAM) 
{
	return (TRUE) ;
}


BOOL CDlgGongShi::OnInitDialog() 
{
	CAcUiDialog::OnInitDialog();

	CComboBox * pCombo = (CComboBox *)GetDlgItem(IDC_COMBO1);
	pCombo->SetCurSel(m_nClass);
	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO2);

	if (m_bExt)
	{
		pCombo->AddString("总体规划");
		pCombo->AddString("分区规划");
		pCombo->AddString("专项规划");
		pCombo->AddString("详细规划");
		pCombo->AddString("规划调整");
		pCombo->AddString("建筑方案");
	}

	pCombo->SetCurSel(m_nGSLM);
	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO3);
	pCombo->SetCurSel(0);

	// 流水号
	CStringArray codeArray;
	decodeCString(m_strGSBH, codeArray, ';');

	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO4);
	for (int i=0; i<codeArray.GetCount(); i++)
	{
		pCombo->AddString(codeArray.GetAt(i));
	}
	pCombo->SetCurSel(0);

	// 公示编号
	SetDlgItemText(IDC_EDIT1, codeArray.GetAt(0));

	// 联系电话
	codeArray.RemoveAll();
	decodeCString(m_strLXDH, codeArray, ';');

	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO5);
	for (int i=0; i<codeArray.GetCount(); i++)
	{
		pCombo->AddString(codeArray.GetAt(i));
	}
	pCombo->SetCurSel(0);

	// 联系地址
	codeArray.RemoveAll();
	decodeCString(m_strLXDZ, codeArray, ';');

	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO6);
	for (int i=0; i<codeArray.GetCount(); i++)
	{
		pCombo->AddString(codeArray.GetAt(i));
	}
	pCombo->SetCurSel(0);

	// 网站
	codeArray.RemoveAll();
	decodeCString(m_strWebSite, codeArray, ';');

	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO7);
	for (int i=0; i<codeArray.GetCount(); i++)
	{
		pCombo->AddString(codeArray.GetAt(i));
	}
	pCombo->SetCurSel(0);

	SetDlgItemInt(IDC_EDIT15, m_nLimit);

	if (m_bExt)
	{
//		GetDlgItem(IDC_STATIC_EX)->ShowWindow(TRUE);
//		GetDlgItem(IDC_EDT_FILENAME_EX)->ShowWindow(TRUE);
		GetDlgItem(IDC_FILELIST)->ShowWindow(TRUE);
		GetDlgItem(IDC_BTN_BROWSE_EX)->ShowWindow(TRUE);
		GetDlgItem(IDC_BTN_DELETE_EX)->ShowWindow(TRUE);
	}

	return TRUE;
}

void CDlgGongShi::OnBnClickedOk()
{
	if (m_bExt)
	{
/*		CEdit * pEdit = (CEdit *)GetDlgItem(IDC_EDT_FILENAME_EX);
		pEdit->GetWindowText(m_strFileName);

		if (m_strFileName.IsEmpty())
		{
			MessageBox("请选择DWG文件。");
			return;
		}

		if (!PathFileExists(m_strFileName))
		{
			MessageBox("输入的文件不存在，请输入正确的文件路径。");
			return;
		}
*/

		int nCount = m_ctrlFileList.GetCount();
		if (nCount <= 0)
		{
			MessageBox("请选择需要公示的DWG图。", "提示", MB_OK);
			return;
		}

		m_strFileArray.RemoveAll();

		for (int i = 0; i < nCount; i++)
		{
			CString strFullFileName;
			m_ctrlFileList.GetText(i, strFullFileName);
			m_strFileArray.Add(strFullFileName);
		}
	}

	m_nLimit = GetDlgItemInt(IDC_EDIT15);

	if (m_nLimit <= 0)
	{
		MessageBox("请输入公示期限。", "提示", MB_OK);
		return;
	}

	// TODO: 在此添加控件通知处理程序代码
	m_wndStartTime.GetTime(&m_tmStart);
	m_wndEndTime.GetTime(&m_tmEnd);

	// 公示类型
	CComboBox * pCombo = (CComboBox *)GetDlgItem(IDC_COMBO1);
	m_nClass = pCombo->GetCurSel();
	pCombo->GetWindowText(m_strGSLX);
	// 公示栏目
	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO2);
	m_nGSLM = pCombo->GetCurSel();
	pCombo->GetWindowText(m_strGSLM);
	// 公示形式
	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO3);
	pCombo->GetWindowText(m_strGSXX);

	// 编号规则
	CStringArray ruleArray;
	decodeCString(m_strBHGZ, ruleArray, ';');

	// 编号规则
	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO4);
	m_strBHGZ = ruleArray.GetAt(pCombo->GetCurSel());
	pCombo->GetWindowText(m_strLSH);

	GetDlgItemText(IDC_EDIT1, m_strGSBH);

	// 联系电话
	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO5);
	pCombo->GetWindowText(m_strLXDH);

	// 联系电话
	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO6);
	pCombo->GetWindowText(m_strLXDZ);

	// 网站
	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO7);
	pCombo->GetWindowText(m_strWebSite);

	CAcUiDialog::OnOK();
}

void CDlgGongShi::OnBnClickedBtnBrowse()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strFileName;

	CString strFilter =	"DWG (*.DWG)|*.DWG|JPEG (*.JPG;*.JPEG;*.JPE;*.JFIF)|*.JPG;*.JPEG;*.JPE;*.JFIF|WORD (*.DOC)|*.DOC|EXCEL (*.XLS)|*.XLS|所有文件 (*.*)|*.*||";
	CFileDialog Filedlg( TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT ,strFilter);

	DWORD MAXFILE = 2412;					// allocate enough memory space
	Filedlg.m_ofn.nMaxFile = MAXFILE;		// set the buffer size
	char* buf = new char[MAXFILE];
	Filedlg.m_ofn.lpstrFile = buf;
	Filedlg.m_ofn.lpstrFile[0] = NULL;
	int iReturn = Filedlg.DoModal();
	if(iReturn == IDOK)
	{
		POSITION pos = Filedlg.GetStartPosition();
		while (pos != NULL)
		{
//			m_strFileName=Filedlg.GetNextPathName(pos);
//			CEdit * pEdit = (CEdit *)GetDlgItem(IDC_EDT_FILENAME_EX);
//			pEdit->SetWindowText(m_strFileName);

			BOOL bExists = FALSE;
			CString strFileName = Filedlg.GetNextPathName(pos);
			for (int i=0; i<m_ctrlFileList.GetCount(); i++)
			{
				CString strListItem;
				m_ctrlFileList.GetText(i, strListItem);
				if (strFileName.CompareNoCase(strListItem) == 0)
				{
					bExists = TRUE;
					break;
				}
			}

			if (!bExists)
				m_ctrlFileList.AddString(strFileName);							
		}
	}

	delete [] buf;
}

void CDlgGongShi::OnSelchangeCombo4() 
{
	CComboBox * pCombo = (CComboBox *)GetDlgItem(IDC_COMBO4);

	int nIndex = pCombo->GetCurSel();
	pCombo->GetWindowText(m_strLSH);

	SetDlgItemText(IDC_EDIT1, m_strLSH);

	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO5);
	pCombo->SetCurSel(nIndex);

	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO6);
	pCombo->SetCurSel(nIndex);
}

void CDlgGongShi::OnBnClickedBtnDeleteEx()
{
	int nSelectRes = m_ctrlFileList.GetCurSel();
	if (LB_ERR != nSelectRes)
		m_ctrlFileList.DeleteString(nSelectRes);
}
