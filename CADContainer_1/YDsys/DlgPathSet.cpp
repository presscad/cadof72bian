// DlgPathSet.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DlgPathSet.h"
#include "DistSysFun.h"
#include "SHFolder.h"
#include "whXMLFile.h"


// CDlgPathSet �Ի���

IMPLEMENT_DYNAMIC(CDlgPathSet, CDialog)
CDlgPathSet::CDlgPathSet(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPathSet::IDD, pParent)
{
}

CDlgPathSet::~CDlgPathSet()
{
}

void CDlgPathSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgPathSet, CDialog)
	ON_BN_CLICKED(IDC_BTN_CT, OnBnClickedBtnCt)
	ON_BN_CLICKED(IDC_BTN_XM, OnBnClickedBtnXm)
	ON_BN_CLICKED(IDC_BTN_SET, OnBnClickedBtnSet)
END_MESSAGE_MAP()


CString CDlgPathSet::GetFilePath(CString strFileName)
{
	int nLen = strFileName.GetLength();
	int nPos =0;
	for(int i=nLen-1;i>=0; i--)
	{
		if(strFileName.GetAt(i) == '\\')
		{
			nPos = i; break;
		}
	}
	CString strValue;
	if(nPos == 0)
		strValue = "";
	else
		strValue = strFileName.Left(nPos+1);

	return strValue;
}

BOOL  CDlgPathSet::ReadCfgLyNameArray(CString& strCT,CString& strPrj)
{
	CString strFName;
	strFName.Format("%sPathInfo.xml",gGetCurArxAppPath("shYDsys.arx"));
	CXMLFile file(strFName);
	std::string str = file.GetString("·��","��ͼ�ļ�","");
	strCT = str.c_str();

	str = file.GetString("·��","��Ŀ·��","C:\\");
	strPrj = str.c_str();

	return TRUE;
}


//��ʼ��
BOOL CDlgPathSet::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString strCT,strPrj;
	ReadCfgLyNameArray(strCT,strPrj);
	CEdit* pEdt = (CEdit*)GetDlgItem(IDC_EDIT_CT);
	pEdt->SetWindowText(strCT);

	pEdt = (CEdit*)GetDlgItem(IDC_EDIT_XM);
	pEdt->SetWindowText(strPrj);
	
	return TRUE;
}

// CDlgPathSet ��Ϣ�������

void CDlgPathSet::OnBnClickedBtnCt()
{
	CEdit* pEdt = (CEdit*)GetDlgItem(IDC_EDIT_CT);
	CString strOld;
	pEdt->GetWindowText(strOld);
	if(strOld.IsEmpty()) strOld = "C:\\��ͼ.dwg";
	struct resbuf *rb = acutNewRb(RTSTR);
	acedGetFileD("��ѡ������ͼ�ļ�", strOld.GetBuffer(0), "dwg", 0, rb);
	CString strFileCT = rb->resval.rstring;
	acutRelRb(rb);
    pEdt->SetWindowText(strFileCT);
}

void CDlgPathSet::OnBnClickedBtnXm()
{
	CEdit* pEdt = (CEdit*)GetDlgItem(IDC_EDIT_XM);
	CString strPath;
	pEdt->GetWindowText(strPath);
	strPath.Trim();
	if(strPath.IsEmpty()) strPath = "C:\\";
	if(g_fSelectFolderDlg(&strPath,strPath,false))
		pEdt->SetWindowText(strPath);
}

void CDlgPathSet::OnBnClickedBtnSet()
{
	CString strPathCT="",strPathXM="";
	CEdit* pEdt = (CEdit*)GetDlgItem(IDC_EDIT_CT);
	pEdt->GetWindowText(strPathCT);
	strPathCT.Trim();
	if(strPathCT.IsEmpty()){
		MessageBox("��ѡ���ͼ����·��!"); return;
	}

	pEdt = (CEdit*)GetDlgItem(IDC_EDIT_XM);
	pEdt->GetWindowText(strPathXM);
	strPathXM.Trim();
	if(strPathXM.IsEmpty()){
		MessageBox("��ѡ����Ŀ����·��!"); return;
	}

	CString strFName ;
	strFName.Format("%sPathInfo.xml",gGetCurArxAppPath("shYDsys.arx"));
	CXMLFile temp(strFName);
	temp.SetString("·��","��ͼ�ļ�",strPathCT);
	temp.SetString("·��","��Ŀ·��",strPathXM);
	temp.save();

	SendMessage(WM_CLOSE);

}

