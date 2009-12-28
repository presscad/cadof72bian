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
	DDX_Text(pDX, IDC_EDIT3, m_strXMMC); // ��Ŀ����
//	DDV_MaxChars(pDX, m_strXMMC, 100);
	DDX_Text(pDX, IDC_EDIT4, m_strJSDW); // ���赥λ
//	DDV_MaxChars(pDX, m_strJSDW, 100);
	DDX_Text(pDX, IDC_EDIT5, m_strYDXZ); // �õ�λ��
//	DDV_MaxChars(pDX, m_strYDXZ, 100);
	DDX_Text(pDX, IDC_EDIT6, m_strGCXZ); // ��������
//	DDV_MaxChars(pDX, m_strGCXZ, 100);

	DDX_Text(pDX, IDC_EDIT7, m_strYDZMJ); // �ؿ������
//	DDV_MaxChars(pDX, m_strYDZMJ, 100);
	DDX_Text(pDX, IDC_EDIT8, m_strYDJMJ); // �ؿ龻���
//	DDV_MaxChars(pDX, m_strYDJMJ, 100);
	DDX_Text(pDX, IDC_EDIT9, m_strYDDZMJ); // ������·�̻����
//	DDV_MaxChars(pDX, m_strYDDZMJ, 100);
	DDX_Text(pDX, IDC_EDIT10, m_strRJL); // �ݻ���
//	DDV_MaxChars(pDX, m_strRJL, 100);
	DDX_Text(pDX, IDC_EDIT11, m_strJZMD); // �����ܶ�
//	DDV_MaxChars(pDX, m_strJZMD, 100);
	DDX_Text(pDX, IDC_EDIT12, m_strJZGM); // ������ģ
//	DDV_MaxChars(pDX, m_strJZGM, 100);
	DDX_Text(pDX, IDC_EDIT13, m_strJZZG); // �����ܸ�
//	DDV_MaxChars(pDX, m_strJZZG, 100);
	DDX_Text(pDX, IDC_EDIT14, m_strLDL); // �̵���
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
		pCombo->AddString("����滮");
		pCombo->AddString("�����滮");
		pCombo->AddString("ר��滮");
		pCombo->AddString("��ϸ�滮");
		pCombo->AddString("�滮����");
		pCombo->AddString("��������");
	}

	pCombo->SetCurSel(m_nGSLM);
	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO3);
	pCombo->SetCurSel(0);

	// ��ˮ��
	CStringArray codeArray;
	decodeCString(m_strGSBH, codeArray, ';');

	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO4);
	for (int i=0; i<codeArray.GetCount(); i++)
	{
		pCombo->AddString(codeArray.GetAt(i));
	}
	pCombo->SetCurSel(0);

	// ��ʾ���
	SetDlgItemText(IDC_EDIT1, codeArray.GetAt(0));

	// ��ϵ�绰
	codeArray.RemoveAll();
	decodeCString(m_strLXDH, codeArray, ';');

	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO5);
	for (int i=0; i<codeArray.GetCount(); i++)
	{
		pCombo->AddString(codeArray.GetAt(i));
	}
	pCombo->SetCurSel(0);

	// ��ϵ��ַ
	codeArray.RemoveAll();
	decodeCString(m_strLXDZ, codeArray, ';');

	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO6);
	for (int i=0; i<codeArray.GetCount(); i++)
	{
		pCombo->AddString(codeArray.GetAt(i));
	}
	pCombo->SetCurSel(0);

	// ��վ
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
			MessageBox("��ѡ��DWG�ļ���");
			return;
		}

		if (!PathFileExists(m_strFileName))
		{
			MessageBox("������ļ������ڣ���������ȷ���ļ�·����");
			return;
		}
*/

		int nCount = m_ctrlFileList.GetCount();
		if (nCount <= 0)
		{
			MessageBox("��ѡ����Ҫ��ʾ��DWGͼ��", "��ʾ", MB_OK);
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
		MessageBox("�����빫ʾ���ޡ�", "��ʾ", MB_OK);
		return;
	}

	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_wndStartTime.GetTime(&m_tmStart);
	m_wndEndTime.GetTime(&m_tmEnd);

	// ��ʾ����
	CComboBox * pCombo = (CComboBox *)GetDlgItem(IDC_COMBO1);
	m_nClass = pCombo->GetCurSel();
	pCombo->GetWindowText(m_strGSLX);
	// ��ʾ��Ŀ
	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO2);
	m_nGSLM = pCombo->GetCurSel();
	pCombo->GetWindowText(m_strGSLM);
	// ��ʾ��ʽ
	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO3);
	pCombo->GetWindowText(m_strGSXX);

	// ��Ź���
	CStringArray ruleArray;
	decodeCString(m_strBHGZ, ruleArray, ';');

	// ��Ź���
	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO4);
	m_strBHGZ = ruleArray.GetAt(pCombo->GetCurSel());
	pCombo->GetWindowText(m_strLSH);

	GetDlgItemText(IDC_EDIT1, m_strGSBH);

	// ��ϵ�绰
	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO5);
	pCombo->GetWindowText(m_strLXDH);

	// ��ϵ�绰
	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO6);
	pCombo->GetWindowText(m_strLXDZ);

	// ��վ
	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO7);
	pCombo->GetWindowText(m_strWebSite);

	CAcUiDialog::OnOK();
}

void CDlgGongShi::OnBnClickedBtnBrowse()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strFileName;

	CString strFilter =	"DWG (*.DWG)|*.DWG|JPEG (*.JPG;*.JPEG;*.JPE;*.JFIF)|*.JPG;*.JPEG;*.JPE;*.JFIF|WORD (*.DOC)|*.DOC|EXCEL (*.XLS)|*.XLS|�����ļ� (*.*)|*.*||";
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
