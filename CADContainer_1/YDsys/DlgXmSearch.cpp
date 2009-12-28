#include "StdAfx.h"
#include "resource.h"
#include "DlgXmSearch.h"
#include "resource.h"

#include "DistEntityTool.h"
#include "DistSelSet.h"
#include "DistXData.h"
#include "..\SdeData\DistBaseInterface.h"

extern IDistConnection*    g_pConnection;         //SDE���ݿ����Ӷ���ָ��
//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC (CDlgXmSearch, CAcUiDialog)

BEGIN_MESSAGE_MAP(CDlgXmSearch, CAcUiDialog)
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, OnAcadKeepFocus)
	ON_BN_CLICKED(IDC_BTN_CONDITION, OnBnClickedBtnCondition)
	ON_BN_CLICKED(IDC_BTN_RECT, OnBnClickedBtnRect)
	ON_NOTIFY(NM_CLICK, IDC_LIST_XM, OnNMClickListXm)
	ON_BN_CLICKED(IDC_BTN_POS, OnBnClickedBtnPos)
	ON_BN_CLICKED(IDC_BTN_OUT, OnBnClickedBtnOut)
	ON_BN_CLICKED(IDC_BTN_EXIT, OnBnClickedBtnExit)
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
CDlgXmSearch::CDlgXmSearch (CWnd *pParent /*=NULL*/, HINSTANCE hInstance /*=NULL*/) : CAcUiDialog (CDlgXmSearch::IDD, pParent, hInstance) {
}

//-----------------------------------------------------------------------------
void CDlgXmSearch::DoDataExchange (CDataExchange *pDX) {
	CAcUiDialog::DoDataExchange (pDX) ;
	DDX_Control(pDX, IDC_BTN_RECT, m_btn_select);
	DDX_Control(pDX, IDC_LIST_XM, m_list_xm);
	DDX_Control(pDX, IDC_LIST_XMXX, m_list_xmxx);
}

//-----------------------------------------------------------------------------
//----- Needed for modeless dialogs to keep focus.
//----- Return FALSE to not keep the focus, return TRUE to keep the focus
LRESULT CDlgXmSearch::OnAcadKeepFocus (WPARAM, LPARAM) {
	return (TRUE) ;
}

void CDlgXmSearch::SetInfo(CString strCadLyName,CString strSdeLyName)
{
	m_strCadLyName = strCadLyName;
	m_strSdeLyName = strSdeLyName;
}

BOOL CDlgXmSearch::OnInitDialog() 
{
	CAcUiDialog::OnInitDialog();

	m_CenterX = -1;
	m_CenterY = -1;


	//���ذ�ť
	m_btn_select.AutoLoad();

	//��ʼ��������Ϣ�б�
	m_list_xm.SetExtendedStyle(m_list_xm.GetExtendedStyle()| LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_list_xm.InsertColumn(0,"��Ŀ����",LVCFMT_LEFT,130);
	m_list_xm.InsertColumn(1,"��Ŀ����ʵ��",LVCFMT_LEFT,0);

	m_list_xmxx.SetExtendedStyle(m_list_xmxx.GetExtendedStyle()| LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_list_xmxx.InsertColumn(0,"�ֶ�����",LVCFMT_LEFT,120);
	m_list_xmxx.InsertColumn(1,"�ֶ�����",LVCFMT_LEFT,120);

	//�����ʾ��Ϣ
	int nPos = 0;
	m_list_xmxx.DeleteAllItems();
	m_list_xmxx.InsertItem(nPos,"���̺�");m_list_xmxx.SetItemText(nPos,1," ");nPos++;
	m_list_xmxx.InsertItem(nPos,"֤��");m_list_xmxx.SetItemText(nPos,1," ");nPos++;
	m_list_xmxx.InsertItem(nPos,"����ʱ��");m_list_xmxx.SetItemText(nPos,1," ");nPos++;
	m_list_xmxx.InsertItem(nPos,"��Ŀ����");m_list_xmxx.SetItemText(nPos,1," ");nPos++;
	m_list_xmxx.InsertItem(nPos,"���赥λ");m_list_xmxx.SetItemText(nPos,1," ");nPos++;
	m_list_xmxx.InsertItem(nPos,"�õ�����");m_list_xmxx.SetItemText(nPos,1," ");nPos++;
	m_list_xmxx.InsertItem(nPos,"��������");m_list_xmxx.SetItemText(nPos,1," ");nPos++;
	m_list_xmxx.InsertItem(nPos,"�����");m_list_xmxx.SetItemText(nPos,1," ");nPos++;
	m_list_xmxx.InsertItem(nPos,"�����õ����");m_list_xmxx.SetItemText(nPos,1," ");nPos++;
	m_list_xmxx.InsertItem(nPos,"��·�õ����");m_list_xmxx.SetItemText(nPos,1," ");nPos++;
	m_list_xmxx.InsertItem(nPos,"�̻��õ����");m_list_xmxx.SetItemText(nPos,1," ");nPos++;
	m_list_xmxx.InsertItem(nPos,"�����õ����");m_list_xmxx.SetItemText(nPos,1," ");nPos++;
	m_list_xmxx.InsertItem(nPos,"������");m_list_xmxx.SetItemText(nPos,1," ");nPos++;
	m_list_xmxx.InsertItem(nPos,"��ע");m_list_xmxx.SetItemText(nPos,1," ");nPos++;
   
	return TRUE;
}




void CDlgXmSearch::OnBnClickedBtnCondition()
{
	if(NULL == g_pConnection)
	{
		acutPrintf("\n�ռ����ݿ�û�����ӣ�"); return;
	}

	CComboBox* pCmb = (CComboBox*) GetDlgItem(IDC_CMB_FLD);
	int nSelect = pCmb->GetCurSel();
	if(nSelect < 0){
		AfxMessageBox("��ѡ���ѯ�ֶΣ�"); return;
	}
	CString strFld;
	pCmb->GetLBText(nSelect,strFld);

	pCmb = (CComboBox*) GetDlgItem(IDC_CMB_OP);
	nSelect = pCmb->GetCurSel();
	if(nSelect < 0){
		AfxMessageBox("��ѡ���ѯ������"); return;
	}
	CString strOP;
	pCmb->GetLBText(nSelect,strOP);

	CString strValue;
	((CEdit*)GetDlgItem(IDC_EDIT_CONDITION))->GetWindowText(strValue);
	if(strValue.IsEmpty()){
		AfxMessageBox("�������ѯ�������ݣ�"); return;
	}

	CString strFldName;
	if(strFld.CompareNoCase("���̺�")==0)
		strFldName = "LCH";
	else if(strFld.CompareNoCase("֤��")==0)
		strFldName = "ZH";
	else if(strFld.CompareNoCase("����ʱ��")==0)
		strFldName = "RQ";
	else if(strFld.CompareNoCase("��Ŀ����")==0)
		strFldName = "XMMC";
	else if(strFld.CompareNoCase("���赥λ")==0)
		strFldName = "JSDW";
	else if(strFld.CompareNoCase("�õ�����")==0)
		strFldName = "YDXZ";

	m_list_xm.DeleteAllItems();

	char strSQL[255]={0};
	sprintf(strSQL,"F:LCH,ZH,RQ,XMMC,JSDW,YDXZ,LXLX,ZMJ,JSYDMJ,DLYDMJ,LHYDMJ,QTYDMJ,X,Y,T:TB_PROJECT,W:%s='%s',##",strFldName,strValue);
	IDistCommand *pCommand = CreateSDECommandObjcet();
	pCommand->SetConnectObj(g_pConnection);
	IDistRecordSet* pRcdSet = NULL;
	if(pCommand->SelectData(strSQL,&pRcdSet) != 1)
	{
		pCommand->Release(); return;
	}

	CString strValues,strXmmc,strTemp;
	strValues.Empty();
	m_list_xm.DeleteAllItems();
	int nPos = 0;
	while(pRcdSet->BatchRead()==1)
	{
		strTemp = (char*)(*pRcdSet)["LCH"];
		strValues = strValues + strTemp +",";
		strTemp = (char*)(*pRcdSet)["ZH"];
		strValues = strValues + strTemp +",";
		strTemp = (char*)(*pRcdSet)["RQ"];
		strValues = strValues + strTemp +",";
		strTemp = (char*)(*pRcdSet)["XMMC"];
		strXmmc = strTemp;
		strValues = strValues + strTemp +",";
		strTemp = (char*)(*pRcdSet)["JSDW"];
		strValues = strValues + strTemp +",";
		strTemp = (char*)(*pRcdSet)["YDXZ"];
		strValues = strValues + strTemp +",";
		strTemp = (char*)(*pRcdSet)["LXLX"];
		strValues = strValues + strTemp +",";
		strTemp = (char*)(*pRcdSet)["ZMJ"];
		strValues = strValues + strTemp +",";
		strTemp = (char*)(*pRcdSet)["JSYDMJ"];
		strValues = strValues + strTemp +",";
		strTemp = (char*)(*pRcdSet)["DLYDMJ"];
		strValues = strValues + strTemp +",";
		strTemp = (char*)(*pRcdSet)["LHYDMJ"];
		strValues = strValues + strTemp +",";
		strTemp = (char*)(*pRcdSet)["QTYDMJ"];
		strValues = strValues + strTemp +",";
		strTemp = (char*)(*pRcdSet)["X"];
		strValues = strValues + strTemp +",";
		strTemp = (char*)(*pRcdSet)["Y"];
		strValues = strValues + strTemp +",";

		m_list_xm.InsertItem(nPos,strXmmc);
		m_list_xm.SetItemText(nPos,1,strValues);
		nPos++;
	}
	
}



void CDlgXmSearch::OnBnClickedBtnRect()
{
	m_list_xm.DeleteAllItems();
	//����ѡ��ʵ��
	BeginEditorCommand();

	//ȡ�þ��η�Χ��ʵ������
	ads_point  startpt,endpt;    
	int nRet = acedGetPoint(NULL, "\n��ָ�����ε����Ͻ�:",startpt);
	if (nRet != RTNORM)
	{
		CompleteEditorCommand();return ;
	}
	nRet = acedGetCorner(startpt, "\n��ָ�����ε����½�:",endpt);
	if (nRet != RTNORM)
	{
		CompleteEditorCommand(); return;
	}

	double dminx,dminy,dmaxx,dmaxy;
	if(startpt[0] > endpt[0])
	{
		dmaxx = startpt[0];
		dminx = endpt[0];
	}
	else
	{
		dmaxx = endpt[0];
		dminx = startpt[0];
	}
	if(startpt[1] > endpt[1])
	{
		dmaxy = startpt[1];
		dminy = endpt[1];
	}
	else
	{
		dmaxy = endpt[1];
		dminy = startpt[1];
	}
	CompleteEditorCommand();

	char strSQL[255]={0};
	sprintf(strSQL,"F:LCH,ZH,RQ,XMMC,JSDW,YDXZ,LXLX,ZMJ,JSYDMJ,DLYDMJ,LHYDMJ,QTYDMJ,X,Y,T:TB_PROJECT,##");
	IDistCommand *pCommand = CreateSDECommandObjcet();
	pCommand->SetConnectObj(g_pConnection);
	IDistRecordSet* pRcdSet = NULL;
	if(pCommand->SelectData(strSQL,&pRcdSet) != 1)
	{
		pCommand->Release(); return;
	}

	CString strValues,strXmmc,strTemp;
	strValues.Empty();
	m_list_xm.DeleteAllItems();
	int nPos = 0;
	while(pRcdSet->BatchRead()==1)
	{
		strValues.Empty();
		strTemp = (char*)(*pRcdSet)["LCH"];
		strValues = strValues + strTemp +",";
		strTemp = (char*)(*pRcdSet)["ZH"];
		strValues = strValues + strTemp +",";
		strTemp = (char*)(*pRcdSet)["RQ"];
		strValues = strValues + strTemp +",";
		strTemp = (char*)(*pRcdSet)["XMMC"];
		strXmmc = strTemp;
		strValues = strValues + strTemp +",";
		strTemp = (char*)(*pRcdSet)["JSDW"];
		strValues = strValues + strTemp +",";
		strTemp = (char*)(*pRcdSet)["YDXZ"];
		strValues = strValues + strTemp +",";
		strTemp = (char*)(*pRcdSet)["LXLX"];
		strValues = strValues + strTemp +",";
		strTemp = (char*)(*pRcdSet)["ZMJ"];
		strValues = strValues + strTemp +",";
		strTemp = (char*)(*pRcdSet)["JSYDMJ"];
		strValues = strValues + strTemp +",";
		strTemp = (char*)(*pRcdSet)["DLYDMJ"];
		strValues = strValues + strTemp +",";
		strTemp = (char*)(*pRcdSet)["LHYDMJ"];
		strValues = strValues + strTemp +",";
		strTemp = (char*)(*pRcdSet)["QTYDMJ"];
		strValues = strValues + strTemp +",";

		double dx,dy;
		strTemp = (char*)(*pRcdSet)["X"];
		strValues = strValues + strTemp +","; dx = atof(strTemp);
		strTemp = (char*)(*pRcdSet)["Y"];
		strValues = strValues + strTemp +","; dy = atof(strTemp);

		if(dx<dminx || dx>dmaxx || dy<dminy || dy>dmaxy) continue;
		m_list_xm.InsertItem(nPos,strXmmc);
		m_list_xm.SetItemText(nPos,1,strValues);
		nPos++;
	}
}



void CDlgXmSearch::OnNMClickListXm(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_CenterX = -1; m_CenterY = -1;
	int nSelect = m_list_xm.GetSelectionMark();
	if(nSelect < 0) return;
	
	CString strTempValue = m_list_xm.GetItemText(nSelect,1);

	//��ȡ��Ŀ��Ϣ
	int nFound = strTempValue.Find(',');
	int nPos = 0;
	while(nFound > 0)
	{
		CString strTemp = strTempValue.Left(nFound);
		if(nPos<12)
			m_list_xmxx.SetItemText(nPos,1,strTemp.GetBuffer());
		else if(nPos == 12)
			m_CenterX = atof(strTemp);
		else if(nPos == 13)
			m_CenterY = atof(strTemp);

		strTempValue = strTempValue.Right(strTempValue.GetLength()-nFound-1);
		nFound = strTempValue.Find(',');
		nPos++;
	}
	*pResult = 0;
}

void CDlgXmSearch::OnBnClickedBtnPos()
{
	if(m_CenterX<0 || m_CenterY<0)
	{
		AfxMessageBox("��ѡ����Ŀ��"); return;
	}

	CDistEntityTool tempTool;
	tempTool.DyZoom(AcGePoint2d(m_CenterX,m_CenterY),200,200);
}

void CDlgXmSearch::OnBnClickedBtnOut()
{
	int nSelect = m_list_xm.GetSelectionMark();
	if(nSelect < 0) 
	{
		AfxMessageBox("��ѡ�������Ŀ��");
		return;
	}
	int nCount = m_list_xmxx.GetItemCount();

	FILE *pf = fopen("C:\\Out.txt","a");
	if(NULL == pf) return;
	for(int i=0; i<nCount; i++)
	{
		CString strTemp;
		strTemp.Format("%s��%s",m_list_xmxx.GetItemText(i,0),m_list_xmxx.GetItemText(i,1));
		fwrite(strTemp.GetBuffer(),strTemp.GetLength(),1,pf);
	}
	fwrite("\n\n",2,1,pf);
	fclose(pf);
	ShellExecute(0,"OPEN","C:\\Out.txt",NULL,NULL,SW_NORMAL);
}

void CDlgXmSearch::OnBnClickedBtnExit()
{
	CAcUiDialog::OnOK();
}
