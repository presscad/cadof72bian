#include "StdAfx.h"
#include "resource.h"
#include "DlgXmInput.h"
#include "DistXData.h"

#include "DistEntityTool.h"
#include "DistSelSet.h"
#include "AccessWebServiceInfo.h"
#include "DistUiPrompts.h"

#include "..\SdeData\DistBaseInterface.h"
#include "..\SdeData\DistExchangeInterface.h"

#include "..\WSAccessor\WsAccessor_export.h"
#include "..\XMLData\DistBaseInterface.h"

CStringArray  CDlgXmInput::m_FNameArray;       //�ֶ�����
CStringArray  CDlgXmInput::m_FAliasNameArray;  //�ֶ����Ʊ���
CStringArray  CDlgXmInput::m_LxsxArray;        //��Ŀ��������

extern HWND  g_DAPHwnd; 
//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC (CDlgXmInput, CAcUiDialog)

BEGIN_MESSAGE_MAP(CDlgXmInput, CAcUiDialog)
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, OnAcadKeepFocus)
	ON_BN_CLICKED(IDC_BTN_YD, OnBnClickedSelectYD)
	ON_BN_CLICKED(IDC_BTN_PT, OnBnClickedSelectPT)

	ON_CBN_SELCHANGE(IDC_CMB_YDLX, OnCbnSelchangeCmbYdlx)
	ON_NOTIFY(NM_CLICK, IDC_LIST_YDXX, OnNMClickListYdxx)
	ON_BN_CLICKED(IDC_BTN_ADD, OnBnClickedBtnAdd)
	ON_BN_CLICKED(IDC_BTN_MODIFY, OnBnClickedBtnModify)
	ON_BN_CLICKED(IDC_BTN_DELETE, OnBnClickedBtnDelete)
	ON_BN_CLICKED(IDC_BTN_SDE, OnBnClickedBtnSde)
	ON_BN_CLICKED(IDC_BTN_EXIT, OnBnClickedBtnExit)
	ON_BN_CLICKED(IDC_BTN_POS, OnBnClickedBtnPos)
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
CDlgXmInput::CDlgXmInput (CWnd *pParent /*=NULL*/, HINSTANCE hInstance /*=NULL*/) 
            :CAcUiDialog (CDlgXmInput::IDD, pParent, hInstance) 
{
	//////////////////////////////////////////// �ַ�����Ϣ����  //////////////////////////////////////////////////
	// ��Ŀ�������ƶ���
	m_LxsxArray.RemoveAll();
	m_LxsxArray.Add("���̺�");m_LxsxArray.Add("֤��");m_LxsxArray.Add("����ʱ��");m_LxsxArray.Add("��Ŀ����");
	m_LxsxArray.Add("���赥λ");m_LxsxArray.Add("�õ�����");m_LxsxArray.Add("��������");m_LxsxArray.Add("�����");
	m_LxsxArray.Add("�����õ����");m_LxsxArray.Add("��·�õ����");m_LxsxArray.Add("�̻��õ����");m_LxsxArray.Add("�����õ����"); 

	// �������Զ��壨��չ�ֶ����Ʊ�����
	m_FAliasNameArray.RemoveAll();	   
	m_FAliasNameArray.Add("���̺�");m_FAliasNameArray.Add("֤��");m_FAliasNameArray.Add("����");m_FAliasNameArray.Add("��Ŀ����"); 
	m_FAliasNameArray.Add("���赥λ");m_FAliasNameArray.Add("�õ�����");m_FAliasNameArray.Add("��������");m_FAliasNameArray.Add("�õ����"); 
	m_FAliasNameArray.Add("�õ�����");m_FAliasNameArray.Add("�ؿ���");m_FAliasNameArray.Add("�ָ�");m_FAliasNameArray.Add("�߿�");	   
	m_FAliasNameArray.Add("X ֵ");m_FAliasNameArray.Add("Y ֵ");m_FAliasNameArray.Add("������");m_FAliasNameArray.Add("��ע");	   

	// ��չ�������ƣ�SDE���ݱ����ƣ�
	m_FNameArray.RemoveAll();
	m_FNameArray.Add("LCH");m_FNameArray.Add("ZH");m_FNameArray.Add("RQ");m_FNameArray.Add("XMMC");
	m_FNameArray.Add("JSDW");m_FNameArray.Add("YDXZ");m_FNameArray.Add("LXLX");m_FNameArray.Add("MJ");
	m_FNameArray.Add("YDLX");m_FNameArray.Add("DKBH");m_FNameArray.Add("ZG");m_FNameArray.Add("XK");
	m_FNameArray.Add("X");m_FNameArray.Add("Y");m_FNameArray.Add("JBR");m_FNameArray.Add("BZ");


	///////////////////////////////////////////// ��Ŀ������Ϣ���� ////////////////////////////////////////////////

	// ��Ŀ������Ϣ����DAP�ṩ��
	m_LXSX.m_strLCH = CBaseCfgInfo::m_BaseProjectInfo.strLCH;  // ���̺�
	m_LXSX.m_strZH  = CBaseCfgInfo::m_BaseProjectInfo.strZH;   // ֤��
	m_LXSX.m_strRQ  = CBaseCfgInfo::m_BaseProjectInfo.strRQ;   // ����ʱ��
	m_LXSX.m_strXMMC= CBaseCfgInfo::m_BaseProjectInfo.strXMMC; // ��Ŀ����
	m_LXSX.m_strJSDW= CBaseCfgInfo::m_BaseProjectInfo.strJSDW; // ���赥λ
	m_LXSX.m_strYDXZ= CBaseCfgInfo::m_BaseProjectInfo.strYDXZ; // �õ�����
	m_LXSX.m_strLXLX= CBaseCfgInfo::m_BaseProjectInfo.strLXLX; // ��������
	//m_LXSX.m_strJBR = CBaseCfgInfo::m_BaseProjectInfo.strJBR;

	m_LXSX.m_strZG = "18";
	m_LXSX.m_strXK = "2.0";

	m_pConnect = NULL;
}


//-----------------------------------------------------------------------------
void CDlgXmInput::DoDataExchange (CDataExchange *pDX) 
{

	CAcUiDialog::DoDataExchange (pDX);
	DDX_Control(pDX, IDC_BTN_YD,m_sBtnYD);
	DDX_Control(pDX, IDC_BTN_PT,m_sBtnPT);


	DDX_Control(pDX, IDC_LIST_BASE, m_list_base);
	DDX_Control(pDX, IDC_LIST_YDXX, m_list_ydxx);
}


LRESULT CDlgXmInput::OnAcadKeepFocus (WPARAM, LPARAM)
{
	return (TRUE) ;
}


BOOL CDlgXmInput::OnInitDialog() 
{
	CAcUiDialog::OnInitDialog();
	if(NULL == m_pConnect)
	{
		AfxMessageBox("SDE���ݿ�û�����ӣ�");
		SendMessage(WM_CLOSE,0,0);
		return FALSE;
	}

	//���ذ�ť
	m_sBtnYD.AutoLoad();
	m_sBtnPT.AutoLoad();

	//��ʼ��������Ϣ�б�
	m_list_base.SetExtendedStyle(m_list_base.GetExtendedStyle()| LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_list_base.InsertColumn(0,"��Ϣ����",LVCFMT_LEFT,90);
	m_list_base.InsertColumn(1,"��Ϣ����",LVCFMT_LEFT,230);

	m_list_ydxx.SetExtendedStyle(m_list_ydxx.GetExtendedStyle()| LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_list_ydxx.InsertColumn(0,"ʵ��ID",LVCFMT_LEFT,0);
	m_list_ydxx.InsertColumn(1,"�õ����",LVCFMT_LEFT,80);
	m_list_ydxx.InsertColumn(2,"�õ�����",LVCFMT_LEFT,60);
	m_list_ydxx.InsertColumn(3,"�ؿ���",LVCFMT_LEFT,60);
	m_list_ydxx.InsertColumn(4,"�ָ�",LVCFMT_LEFT,40);
	m_list_ydxx.InsertColumn(5,"�߿�",LVCFMT_LEFT,40);
	m_list_ydxx.InsertColumn(6,"X ֵ",LVCFMT_LEFT,40);
	m_list_ydxx.InsertColumn(7,"Y ֵ",LVCFMT_LEFT,40);
	m_list_ydxx.InsertColumn(8,"������",LVCFMT_LEFT,60);
	m_list_ydxx.InsertColumn(9,"��ע",LVCFMT_LEFT,100);


	

    //����Ŀ������Ϣ���Ի���
	SetValueToControls();

	//��ָ����Ŀ���̺��еĸ��ؿ���Ϣ
	ReadDataToList(m_LXSX.m_strLCH,m_strCadLyName,m_strSdeLyName);
	
	return TRUE;
}

void CDlgXmInput::SetInfo(IDistConnection * pConnect,CString strCadLyName,
						  CString strSdeLyName,CString strURL,CString strProjectId,CString strPrePrjId)
{
	m_pConnect = pConnect;
	m_strCadLyName = strCadLyName;
	m_strSdeLyName = strSdeLyName;
	m_strURL = strURL;
	m_strProjectId = strProjectId;
	m_strPrePrjId= strPrePrjId;
}

void CDlgXmInput::ReadDataToList(const char* strLCH,const char* strCadLyName,const char* strSdeLyName)
{
	m_list_ydxx.DeleteAllItems();
	
	//ѭ����ȡ��Ŀ�õ���Ϣ
	AcDbObjectIdArray IdArray;
	CDistEntityTool tempTool;
	tempTool.SelectEntityInLayer(IdArray,strCadLyName);
	int nCount = IdArray.length();

	char strReg[260]={0};
	memset(strReg,0,sizeof(char)*260);
	sprintf(strReg,"DIST_%s",strSdeLyName);
	int nPos = 0;
	for(int k=0; k<nCount; k++)
	{
		AcDbObjectId tempId = IdArray.at(k);
		CDistXData tempXData(tempId,strReg);
		CStringArray strValueArray,strFNameArray; 
		strValueArray.RemoveAll();strFNameArray.RemoveAll();
		tempXData.Select(strFNameArray,strValueArray);
		tempXData.Close();
		int nNum = strValueArray.GetCount();
		if(nNum == 0) continue;
		CString strTempLCH = strValueArray.GetAt(0);
		if(strTempLCH.CompareNoCase(strLCH) == 0)
		{
			CString strID;
			strID.Format("%d",tempId.asOldId());
			m_list_ydxx.InsertItem(nPos,strID);
			for(int j=7; j<nNum; j++) //ע�⣺����λ��
				m_list_ydxx.SetItemText(nPos,j-6,strValueArray.GetAt(j));
			nPos++;
		}
	}

	nCount = m_list_ydxx.GetItemCount();
	if(nCount == 0)
	{
		IDistCommand* pCommand = CreateSDECommandObjcet();
		pCommand->SetConnectObj(m_pConnect);

		char strSQL[255]={0};
		sprintf(strSQL,"F:LCH,ZH,T:TB_PROJECT,W:LCH='%s',##",strLCH);
		IDistRecordSet* pRcdSet = NULL;
		if(1 != pCommand->SelectData(strSQL,&pRcdSet))
		{
			pCommand->Release();
			MessageBox("���ʿռ��ʧ��!"); 
			SendMessage(WM_CLOSE);
			return;
		}
		if(pRcdSet->GetRecordCount()>0)
		{
			MessageBox("���ȵ�����ǰ��Ŀ���ߵؿ飬Ȼ����ܱ༭������!");
			pCommand->Release(); pRcdSet->Release();
			SendMessage(WM_CLOSE);
			return;
		}
	}
}


//ѡ��ؿ�
void CDlgXmInput::OnBnClickedSelectYD()
{
	//����ѡ��ʵ��
	BeginEditorCommand();

	//ѡ�����
	ads_name ssName;
	struct resbuf *filter=NULL;
	filter=acutNewRb(AcDb::kDxfLayerName);
	filter->restype=AcDb::kDxfLayerName;
	filter->resval.rstring = (char*) malloc((m_strCadLyName.GetLength()+1)*sizeof(char));
	strcpy(filter->resval.rstring,m_strCadLyName.GetBuffer(0));
	filter->rbnext=NULL;

	if (acedSSGet(":S", NULL, NULL, filter, ssName) != RTNORM)
	{
		acutPrintf("\n ѡ��ʵ������!");
		acedSSFree(ssName); acutRelRb(filter);
		CompleteEditorCommand();
		return;
	}
	acutRelRb(filter);

	AcDbObjectId tempObjId;
	ads_name ssTemp;
	acedSSName(ssName, 0, ssTemp);
	acdbGetObjectId(tempObjId, ssTemp);
	acedSSFree(ssName);

	AcDbEntity* pEnt = NULL;
	if(Acad::eOk != acdbOpenAcDbEntity(pEnt,tempObjId,AcDb::kForRead)) 
	{
		CompleteEditorCommand();
		return;
	}
	if(pEnt->isKindOf(AcDbPolyline::desc()))
	{
		AcDbPolyline* pline = (AcDbPolyline*)pEnt;
		if(pline->isClosed())
		{
			char strRegName[255]={0};
			sprintf(strRegName,"DIST_%s",m_strSdeLyName);
			CDistXData tempXData(pEnt,strRegName);
			CStringArray strFNameArray,strValueArray;
			tempXData.Select(strFNameArray,strValueArray);
			if(strValueArray.GetCount() > 0)
			{
				pEnt->close();
				AfxMessageBox("ѡ��ʵ�������Ѿ����ã�������������!");
				CompleteEditorCommand(); return;
			}

			AcGePoint3dArray pts;
			CDistEntityTool tempTool;
			tempTool.GetPolylinePts(pline,pts);
			int nPtCount = pts.length();
			DIST_POINT_STRUCT* ptArray = new DIST_POINT_STRUCT[nPtCount];
			for(int i=0; i<nPtCount; i++)
			{
				ptArray[i].x = pts[i].x; ptArray[i].y = pts[i].y;
			}

			char strSQL[255]={0};
			sprintf(strSQL,"F:SHAPE,T:%s,W:PROJECTID<>'%s',##",m_strSdeLyName,m_strProjectId);
			if(1 != gCheckSdeData(m_pConnect,m_strSdeLyName,strSQL,ptArray,nPtCount))
			{
				AfxMessageBox("������λ�����ص���"); delete [] ptArray; ptArray = NULL;
				pEnt->close(); CompleteEditorCommand(); return;
			}
			delete[] ptArray; ptArray= NULL;

			double dArea = 0.0;
			pline->getArea(dArea);
			CString strTemp;
			strTemp.Format("%0.3f",dArea);
			((CEdit*)GetDlgItem(IDC_EDIT_ZMJ))->SetWindowText(strTemp);
			int nId = tempObjId.asOldId();
			strTemp.Format("%d",nId);
			((CEdit*)GetDlgItem(IDC_EDIT_OBJ))->SetWindowText(strTemp);
		}
		else
			AfxMessageBox("ѡ��ʵ�岻�պ�!");
	}
	pEnt->close();

	CompleteEditorCommand();
}


void CDlgXmInput::OnBnClickedSelectPT()
{
	//����ѡ��ʵ��
	BeginEditorCommand();

	ads_point ptResult;
	if(RTNORM != acedGetPoint(NULL,"ѡ������",ptResult))
	{
		CompleteEditorCommand();
		return;
	}

	AcGePoint3d pt(ptResult[0],ptResult[1],0);
	CString strTemp;
	strTemp.Format("%0.3f",pt.x);
	((CEdit*)GetDlgItem(IDC_EDIT_X))->SetWindowText(strTemp);
	strTemp.Format("%0.3f",pt.y);
	((CEdit*)GetDlgItem(IDC_EDIT_Y))->SetWindowText(strTemp);

	CompleteEditorCommand();

}


void CDlgXmInput::OnCbnSelchangeCmbYdlx()
{
	int nIndex = -1;
	nIndex = ((CComboBox*)GetDlgItem(IDC_CMB_YDLX))->GetCurSel();
	if(nIndex < 0) return;

	CComboBox* pCmb = (CComboBox*)GetDlgItem(IDC_CMB_DKBH);
	pCmb->ResetContent();
	pCmb->SetWindowText("");
	CString strTag;
	strTag.Format("A%d",nIndex+1);
	pCmb->AddString(strTag);

	CString strTemp;
	for(int j=1; j<=8; j++)
	{
		strTemp.Format("%s-%d",strTag,j);
		pCmb->AddString(strTemp);
	} 
}

void CDlgXmInput::OnNMClickListYdxx(NMHDR *pNMHDR, LRESULT *pResult)
{
	int nSelect = m_list_ydxx.GetSelectionMark();
	if(nSelect < 0) return;

	m_LXSX.m_strObjId = m_list_ydxx.GetItemText(nSelect,0);//ʵ��ID
	m_LXSX.m_strYDMJ = m_list_ydxx.GetItemText(nSelect,1); //�õ����
	m_LXSX.m_strYDLX = m_list_ydxx.GetItemText(nSelect,2); //�õ�����
	m_LXSX.m_strDKBH = m_list_ydxx.GetItemText(nSelect,3); //�ؿ���
	m_LXSX.m_strZG = m_list_ydxx.GetItemText(nSelect,4);   //�ָ�
	m_LXSX.m_strXK = m_list_ydxx.GetItemText(nSelect,5);   //�߿�
	m_LXSX.m_strX = m_list_ydxx.GetItemText(nSelect,6);    //X ֵ
	m_LXSX.m_strY = m_list_ydxx.GetItemText(nSelect,7);    //Y ֵ
	m_LXSX.m_strJBR = m_list_ydxx.GetItemText(nSelect,8);  //������
	m_LXSX.m_strBZ = m_list_ydxx.GetItemText(nSelect,9);   //��ע

	SetValueToControls();

	/*
	double dx,dy;
	CDistEntityTool tempTool;
	AcDbViewTableRecord view = tempTool.GetCurrentView();

	dx = atof(m_LXSX.m_strX);
	dy = atof(m_LXSX.m_strY);
	tempTool.DyZoom(AcGePoint2d(dx,dy),view.height(),view.width());

	m_list_ydxx.SetSelectionMark(-1);
	*/
	
	*pResult = 0;
}

void CDlgXmInput::OnBnClickedBtnPos()
{
	int nSelect = m_list_ydxx.GetSelectionMark();
	if(nSelect < 0) return;

	double dx=0.0,dy=0.0;
	CString strTemp = m_list_ydxx.GetItemText(nSelect,6);    //X ֵ
	dx = atof(strTemp);
	strTemp = m_list_ydxx.GetItemText(nSelect,7);    //Y ֵ
	dy = atof(strTemp);

	CDistEntityTool tempTool;
	tempTool.DyZoom(AcGePoint2d(dx,dy),1000,1000);

	strTemp = m_list_ydxx.GetItemText(nSelect,0);
	int nId = atoi(strTemp);
	AcDbObjectId tempId;
	tempId.setFromOldId(nId);
	AcDbEntity* pEnt = NULL;
	if(Acad::eOk != acdbOpenObject(pEnt,tempId,AcDb::kForRead)) return;
    pEnt->highlight();
	pEnt->close();
}


//���
void CDlgXmInput::OnBnClickedBtnAdd()
{
	GetValueFromControls();

	if(m_LXSX.m_strYDMJ.IsEmpty() || m_LXSX.m_strYDLX.IsEmpty() || m_LXSX.m_strX.IsEmpty())
	{
		acutPrintf("\n �������ݲ���Ϊ��!"); return;
	}

	int nCount = m_list_ydxx.GetItemCount();

	m_list_ydxx.InsertItem(nCount,m_LXSX.m_strObjId);   //ʵ��ID
	m_list_ydxx.SetItemText(nCount,1,m_LXSX.m_strYDMJ); //�õ����
	m_list_ydxx.SetItemText(nCount,2,m_LXSX.m_strYDLX); //�õ�����
	m_list_ydxx.SetItemText(nCount,3,m_LXSX.m_strDKBH); //�ؿ���
	m_list_ydxx.SetItemText(nCount,4,m_LXSX.m_strZG);   //�ָ�
	m_list_ydxx.SetItemText(nCount,5,m_LXSX.m_strXK);   //�߿�
	m_list_ydxx.SetItemText(nCount,6,m_LXSX.m_strX);    //X ֵ
	m_list_ydxx.SetItemText(nCount,7,m_LXSX.m_strY);    //Y ֵ
	m_list_ydxx.SetItemText(nCount,8,m_LXSX.m_strJBR);  //������
	m_list_ydxx.SetItemText(nCount,9,m_LXSX.m_strBZ);   //��ע

	int nId = atoi(m_LXSX.m_strObjId);
	AcDbObjectId tempId;
	tempId.setFromOldId(nId);
	char strRegName[255]={0};
	sprintf(strRegName,"DIST_%s",m_strSdeLyName.GetBuffer(0));
	SetValueToXData(tempId,strRegName);


	CStringArray strTextArray;
	strTextArray.Add(m_LXSX.m_strLCH);strTextArray.Add(m_LXSX.m_strZH);strTextArray.Add(m_LXSX.m_strRQ);strTextArray.Add(m_LXSX.m_strXMMC);
	strTextArray.Add(m_LXSX.m_strJSDW);strTextArray.Add(m_LXSX.m_strYDXZ);strTextArray.Add(m_LXSX.m_strLXLX);strTextArray.Add(m_LXSX.m_strYDMJ);
	strTextArray.Add(m_LXSX.m_strYDLX);strTextArray.Add(m_LXSX.m_strDKBH);strTextArray.Add(m_LXSX.m_strZG);strTextArray.Add(m_LXSX.m_strXK);
	strTextArray.Add(m_LXSX.m_strX);strTextArray.Add(m_LXSX.m_strY);strTextArray.Add(m_LXSX.m_strJBR);strTextArray.Add(m_LXSX.m_strBZ);

	double dX = atof(m_LXSX.m_strX.GetBuffer(0));
	double dY = atof(m_LXSX.m_strY.GetBuffer(0));
	double dH = atof(m_LXSX.m_strZG.GetBuffer(0));

	CString strCadLyName;
	strCadLyName.Format("%s���Ա�ע",m_strCadLyName);
	CDistEntityTool tempTool;
	AcDbObjectId resultId;
	tempTool.InsertBlkAttrRef(resultId,"_��������",strCadLyName.GetBuffer(),AcGePoint3d(dX,dY,0),dH,m_FAliasNameArray,strTextArray);
	CDistXData tempXData(resultId,"ROWID_OBJS");
	tempXData.Add("wang","hui");
	tempXData.Update(); tempXData.Close();

	//���
	((CEdit*)GetDlgItem(IDC_EDIT_OBJ))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_EDIT_ZMJ))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_EDIT_X))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_EDIT_Y))->SetWindowText("");
	((CComboBox*)GetDlgItem(IDC_CMB_DKBH))->SetWindowText("");

	//����ʵ��ID�޸��߿�������ʾ
	double dWidth = atof(m_LXSX.m_strXK.GetBuffer(0));
	AcDbEntity *pEnt = NULL;
	if(Acad::eOk != acdbOpenObject(pEnt, tempId,AcDb::kForWrite)) return;
	if(pEnt->isKindOf(AcDbPolyline::desc()))
	{
		AcDbPolyline *pLine = AcDbPolyline::cast(pEnt);
		int nNumVert = pLine->numVerts(); //��ȡ������
		for(int i=0; i<nNumVert; i++)
			pLine->setWidthsAt(i,dWidth,dWidth);
		
		pLine->close();
		acedUpdateDisplay();
	}
	else
		pEnt->close();
	
	m_list_ydxx.SetSelectionMark(-1);
}


//�޸�
void CDlgXmInput::OnBnClickedBtnModify()
{
	int nSelect = m_list_ydxx.GetSelectionMark();
	if(nSelect < 0) return;
	
	GetValueFromControls();
	if(m_LXSX.m_strYDMJ.IsEmpty() || m_LXSX.m_strYDLX.IsEmpty() || m_LXSX.m_strX.IsEmpty() || m_LXSX.m_strZG.IsEmpty())
	{
		acutPrintf("\n �������ݲ���Ϊ��!"); return;
	}

	m_list_ydxx.SetItemText(nSelect,0,m_LXSX.m_strObjId);//ʵ��ID
	m_list_ydxx.SetItemText(nSelect,1,m_LXSX.m_strYDMJ); //�õ����
	m_list_ydxx.SetItemText(nSelect,2,m_LXSX.m_strYDLX); //�õ�����
	m_list_ydxx.SetItemText(nSelect,3,m_LXSX.m_strDKBH); //�ؿ���
	m_list_ydxx.SetItemText(nSelect,4,m_LXSX.m_strZG);   //�ָ�
	m_list_ydxx.SetItemText(nSelect,5,m_LXSX.m_strXK);   //�߿�
	m_list_ydxx.SetItemText(nSelect,6,m_LXSX.m_strX);    //X ֵ
	m_list_ydxx.SetItemText(nSelect,7,m_LXSX.m_strY);    //Y ֵ
	m_list_ydxx.SetItemText(nSelect,8,m_LXSX.m_strJBR);  //������
	m_list_ydxx.SetItemText(nSelect,9,m_LXSX.m_strBZ);   //��ע

	int nId = atoi(m_LXSX.m_strObjId);
	AcDbObjectId tempId;
	tempId.setFromOldId(nId);

	char strRegName[255]={0};
	sprintf(strRegName,"DIST_%s",m_strSdeLyName.GetBuffer(0));
	SetValueToXData(tempId,strRegName);


	AcDbEntity * pEnt = NULL;
	if(Acad::eOk != acdbOpenAcDbEntity(pEnt,tempId,AcDb::kForRead)) return;
	pEnt->close();

	CDistEntityTool tempTool;
	AcGePoint3dArray ptArray;
	tempTool.GetPolylinePts(tempId,ptArray);
	CString strTempCadLyName;
	strTempCadLyName.Format("%s���Ա�ע",m_strCadLyName);
	resbuf* fileter = acutBuildList(-4, "<or", 
		                            -4, "<and", 8,strTempCadLyName, -4, "and>", 
		                            -4, "<and", RTDXF0, "INSERT", 100, "AcDbBlockReference", -4, "and>", 
		                            -4, "or>", 0);
	CDistSelSet tempSet;
	tempSet.WindowPolygonSelect(ptArray,fileter);
	acutRelRb(fileter);
	AcDbObjectIdArray IdArray;
	tempSet.AsArray(IdArray);
	int nCount = IdArray.length();
	for(int k=0; k<nCount;k++)
	{
		AcDbObjectId Id = IdArray.at(k);
		if(Acad::eOk != acdbOpenAcDbEntity(pEnt,Id,AcDb::kForWrite))continue;
		pEnt->erase();
		pEnt->close();	
	}

	CStringArray strTextArray;
	strTextArray.Add(m_LXSX.m_strLCH);strTextArray.Add(m_LXSX.m_strZH);strTextArray.Add(m_LXSX.m_strRQ);strTextArray.Add(m_LXSX.m_strXMMC);
	strTextArray.Add(m_LXSX.m_strJSDW);strTextArray.Add(m_LXSX.m_strYDXZ);strTextArray.Add(m_LXSX.m_strLXLX);strTextArray.Add(m_LXSX.m_strYDMJ);
	strTextArray.Add(m_LXSX.m_strYDLX);strTextArray.Add(m_LXSX.m_strDKBH);strTextArray.Add(m_LXSX.m_strZG);strTextArray.Add(m_LXSX.m_strXK);
	strTextArray.Add(m_LXSX.m_strX);strTextArray.Add(m_LXSX.m_strY);strTextArray.Add(m_LXSX.m_strJBR);strTextArray.Add(m_LXSX.m_strBZ);

	double dX = atof(m_LXSX.m_strX.GetBuffer(0));
	double dY = atof(m_LXSX.m_strY.GetBuffer(0));
	double dH = atof(m_LXSX.m_strZG.GetBuffer(0));

	CString strCadLyName;
	strCadLyName.Format("%s���Ա�ע",m_strCadLyName);
	
	AcDbObjectId resultId;
	tempTool.InsertBlkAttrRef(resultId,"_��������",strCadLyName.GetBuffer(),AcGePoint3d(dX,dY,0),dH,m_FAliasNameArray,strTextArray);
	CDistXData tempXData(resultId,"ROWID_OBJS");
	tempXData.Add("wang","hui");
	tempXData.Update(); tempXData.Close();

	//���
	((CEdit*)GetDlgItem(IDC_EDIT_OBJ))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_EDIT_X))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_EDIT_X))->SetWindowText("");
	((CComboBox*)GetDlgItem(IDC_CMB_DKBH))->SetWindowText("");

	//����ʵ��ID�޸��߿�������ʾ
	double dWidth = atof(m_LXSX.m_strXK.GetBuffer(0));
	if(Acad::eOk != acdbOpenObject(pEnt, tempId,AcDb::kForWrite)) return;

	if(pEnt->isKindOf(AcDbPolyline::desc()))
	{
		AcDbPolyline *pLine = AcDbPolyline::cast(pEnt);
		int nNumVert = pLine->numVerts(); //��ȡ������
		for(int i=0; i<nNumVert; i++)
			pLine->setWidthsAt(i,dWidth,dWidth);
		//pLine->highlight();
		pEnt->close();
		acedUpdateDisplay();
	}
	else
		pEnt->close();

	m_list_ydxx.SetSelectionMark(-1);

}


//ɾ��
void CDlgXmInput::OnBnClickedBtnDelete()
{
	// TODO: Add your control notification handler code here
	int nSelect = m_list_ydxx.GetSelectionMark();
	if(nSelect < 0) return;
	m_list_ydxx.DeleteItem(nSelect);
	m_list_ydxx.SetSelectionMark(-1);

	int nId = atoi(m_LXSX.m_strObjId);
	AcDbObjectId tempId;
	tempId.setFromOldId(nId);

	char strRegName[255]={0};
	sprintf(strRegName,"DIST_%s",m_strSdeLyName.GetBuffer(0));
	CDistXData tempXData(tempId,strRegName);
	tempXData.DeleteAllRecord();
	tempXData.Close();

	AcDbEntity * pEnt = NULL;
	if(Acad::eOk != acdbOpenAcDbEntity(pEnt,tempId,AcDb::kForRead)) return;
	pEnt->close();

	CDistEntityTool tempTool;
	AcGePoint3dArray ptArray;
	tempTool.GetPolylinePts(tempId,ptArray);
	CString strTempCadLyName;
	strTempCadLyName.Format("%s���Ա�ע",m_strCadLyName);
	resbuf* fileter = acutBuildList(-4, "<or", 
		                            -4, "<and", 8,strTempCadLyName, -4, "and>", 
		                            -4, "<and", RTDXF0, "INSERT", 100, "AcDbBlockReference", -4, "and>", 
								    -4, "or>", 0);
	CDistSelSet tempSet;
	tempSet.WindowPolygonSelect(ptArray,fileter);
	acutRelRb(fileter);
	AcDbObjectIdArray IdArray;
	tempSet.AsArray(IdArray);
	int nCount = IdArray.length();
	for(int k=0; k<nCount;k++)
	{
		AcDbObjectId Id = IdArray.at(k);
		if(Acad::eOk != acdbOpenAcDbEntity(pEnt,Id,AcDb::kForWrite))continue;
		pEnt->erase();
		pEnt->close();	
	}

	//���
	((CEdit*)GetDlgItem(IDC_EDIT_OBJ))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_EDIT_X))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_EDIT_X))->SetWindowText("");
	((CComboBox*)GetDlgItem(IDC_CMB_DKBH))->SetWindowText("");

	m_list_ydxx.SetSelectionMark(-1);
}

void CDlgXmInput::OnBnClickedBtnSde()
{
	if(MessageBox("��ȷ��Ҫ�ύ���еؿ����ݵ�SDE�ռ����","��Ϣ��ʾ",MB_YESNO) != IDYES) return;
	if(NULL == m_pConnect)
	{
		acutPrintf("\n �ռ����ݿ�û�����ӣ�"); return;
	}
	int nCount = m_list_ydxx.GetItemCount();
	if(nCount == 0) return;

	BeginEditorCommand();
	ads_point pt;
	if(RTNORM!= acedGetPoint(NULL,"\n ��Ŀ��ע��Ϣ����㣺",pt)) 
	{
		acutPrintf("\n ������ȡ��!");
		CompleteEditorCommand();
		return;
	}

	//����ѡ���������
	int nHeight = 18;
	CDistUiPrIntDef prIntType("\n �������ָ�<18>",NULL,CDistUiPrInt::kRange,18); 
	prIntType.SetRange(3,72);
	if(prIntType.Go()!=CDistUiPrBase::kOk)
	{
		acutPrintf("\n ������ȡ��!"); CompleteEditorCommand(); return;
	}
	nHeight = prIntType.GetValue();

	CompleteEditorCommand();


	IDistCommand* pCommand = CreateSDECommandObjcet();
	if(NULL == pCommand) return;
	pCommand->SetConnectObj(m_pConnect);
	if(m_pConnect->CheckConnect(60)!=1)
	{
		AfxMessageBox("\n ���ݿ�û������!"); 
		pCommand->Release();
		return;
	}

	char** strFldValueArray = new char*[19];
	for(int i=0;i<19;i++)
	{
		strFldValueArray[i] = new char[200];
		memset(strFldValueArray[i],0,sizeof(char)*200);
	}

	//��Ŀ������Ϣ
	strcpy(strFldValueArray[0],m_LXSX.m_strLCH.GetBuffer(0));  // ���̺�
	strcpy(strFldValueArray[1],m_LXSX.m_strZH.GetBuffer(0));   // ֤��
	strcpy(strFldValueArray[2],m_LXSX.m_strRQ.GetBuffer(0));   // ����ʱ��
	strcpy(strFldValueArray[3],m_LXSX.m_strXMMC.GetBuffer(0)); // ��Ŀ����
	strcpy(strFldValueArray[4],m_LXSX.m_strJSDW.GetBuffer(0)); // ���赥λ
	strcpy(strFldValueArray[5],m_LXSX.m_strYDXZ.GetBuffer(0)); // �õ�����
	strcpy(strFldValueArray[6],m_LXSX.m_strLXLX.GetBuffer(0)); // ��������       

	pCommand->BeginTrans();

	//ɾ���ɼ�¼
	char strSQL[255]={0};
	sprintf(strSQL,"T:%s,W:LCH='%s',##",m_strSdeLyName,m_LXSX.m_strLCH);
	if(1 != pCommand->DeleteData(strSQL))
	{
		for(i=0; i<19; i++)
			free(strFldValueArray[i]);
		free(strFldValueArray);
		AfxMessageBox("\n �ύ����ʧ�ܣ�");
		pCommand->RollbackTrans(); pCommand->Release(); CompleteEditorCommand();return;
	}

	sprintf(strSQL,"T:TB_PROJECT,W:LCH='%s',##",m_LXSX.m_strLCH);
	if(1 != pCommand->DeleteData(strSQL))
	{
		for(i=0; i<19; i++)
			free(strFldValueArray[i]);
		free(strFldValueArray);
		AfxMessageBox("\n �ύ����ʧ�ܣ�");
		pCommand->RollbackTrans(); pCommand->Release(); CompleteEditorCommand();return;
	}
	
	//����¼�¼
	sprintf(strSQL,"F:LCH,ZH,RQ,XMMC,JSDW,YDXZ,LXLX,MJ,YDLX,DKBH,ZG,XK,X,Y,JBR,BZ,PROJECTID,PRE_PROJECT_ID,SHAPE,T:%s,##",m_strSdeLyName);
	for(int j=0; j<nCount; j++)
	{
		AcDbObjectId tempId;
		CString strTemp;
		strTemp = m_list_ydxx.GetItemText(j,0);
		tempId.setFromOldId(atoi(strTemp.GetBuffer(0)));
		for(int k=1;k<10;k++)
		{
			strTemp =  m_list_ydxx.GetItemText(j,k);
			strcpy(strFldValueArray[6+k],strTemp.GetBuffer(0));
		}
		strcpy(strFldValueArray[16],m_strProjectId.GetBuffer(0));  //����� PROJECTID

		m_strPrePrjId.TrimLeft();m_strPrePrjId.TrimRight();
		if(m_strPrePrjId.IsEmpty()) m_strPrePrjId = "0";
		strcpy(strFldValueArray[17],m_strPrePrjId.GetBuffer(0));   //����� PRE_PROJECT_ID
		CDistEntityTool tempTool;
		AcGePoint3dArray ptArray;
		tempTool.GetPolylinePts(tempId,ptArray);
		int nNum = ptArray.length();
		DIST_POINT_STRUCT * pts = new DIST_POINT_STRUCT[nNum];
		for(i=0; i<nNum; i++)
		{
			pts[i].x = ptArray[i].x; pts[i].y = ptArray[i].y;
		}

		if(1 != gSaveOneLXToSDE(m_pConnect,strSQL,m_strSdeLyName,strFldValueArray,19,pts,nNum))
		{
			for(i=0; i<19; i++)
				free(strFldValueArray[i]);
			free(strFldValueArray);
			AfxMessageBox("\n �ύ����ʧ�ܣ�");
			pCommand->RollbackTrans(); pCommand->Release();CompleteEditorCommand(); return;
		}
	}


	double dTotalArea=0.0;
	double dA1=0.0,dA2=0.0,dA3=0.0,dA4=0.0;
	CString strTemp;
	for(int i=0; i<nCount;i++)
	{
		strTemp = m_list_ydxx.GetItemText(i,3); //�ؿ���
		if(strTemp.Find("A1") >=0 )
		{
			strTemp = m_list_ydxx.GetItemText(i,1);
			dA1 = dA1+atof(strTemp.GetBuffer(0));
		}
		else if(strTemp.Find("A2") >=0 )
		{
			strTemp = m_list_ydxx.GetItemText(i,1);
			dA2 = dA2+atof(strTemp.GetBuffer(0));
		}
		else if(strTemp.Find("A3") >=0 )
		{
			strTemp = m_list_ydxx.GetItemText(i,1);
			dA3 = dA3+atof(strTemp.GetBuffer(0));
		}
		else if(strTemp.Find("A4") >=0 )
		{
			strTemp = m_list_ydxx.GetItemText(i,1);
			dA4 = dA4+atof(strTemp.GetBuffer(0));
		}
	}
	dTotalArea = dA1+dA2+dA3+dA4;

	sprintf(strSQL,"F:LCH,ZH,RQ,XMMC,JSDW,YDXZ,LXLX,ZMJ,JSYDMJ,DLYDMJ,LHYDMJ,QTYDMJ,ZG,X,Y,PROJECTID,T:TB_PROJECT,##");

	for(int k=0;k<18;k++)
		memset(strFldValueArray[k],0,sizeof(char)*200);

	strcpy(strFldValueArray[0],m_LXSX.m_strLCH.GetBuffer());
	strcpy(strFldValueArray[1],m_LXSX.m_strZH.GetBuffer());
	strcpy(strFldValueArray[2],m_LXSX.m_strRQ.GetBuffer());
	strcpy(strFldValueArray[3],m_LXSX.m_strXMMC.GetBuffer());
	strcpy(strFldValueArray[4],m_LXSX.m_strJSDW.GetBuffer());
	strcpy(strFldValueArray[5],m_LXSX.m_strYDXZ.GetBuffer());
	strcpy(strFldValueArray[6],m_LXSX.m_strLXLX.GetBuffer());
	
	strTemp.Format("%0.3f",dTotalArea);
	strcpy(strFldValueArray[7],strTemp.GetBuffer());
	strTemp.Format("%0.3f",dA1);
	strcpy(strFldValueArray[8],strTemp.GetBuffer());
	strTemp.Format("%0.3f",dA2);
	strcpy(strFldValueArray[9],strTemp.GetBuffer());
	strTemp.Format("%0.3f",dA3);
	strcpy(strFldValueArray[10],strTemp.GetBuffer());
	strTemp.Format("%0.3f",dA4);
	strcpy(strFldValueArray[11],strTemp.GetBuffer());
	strTemp.Format("%d",nHeight);
	strcpy(strFldValueArray[12],strTemp.GetBuffer());
	strTemp.Format("%0.3f",pt[0]);
	strcpy(strFldValueArray[13],strTemp.GetBuffer());
	strTemp.Format("%0.3f",pt[1]);
	strcpy(strFldValueArray[14],strTemp.GetBuffer());
	strcpy(strFldValueArray[15],m_strProjectId.GetBuffer());


	int nRowId = -1;
	if(1!=pCommand->InsertDataAsString(&nRowId,strSQL,strFldValueArray))
	{
		for(i=0; i<19; i++)
			free(strFldValueArray[i]);
		free(strFldValueArray);
		AfxMessageBox("\n �ύ����ʧ�ܣ�");
		pCommand->RollbackTrans(); pCommand->Release(); return;
	}

	pCommand->CommitTrans(); pCommand->Release();


	
	strTemp.Format("%0.3f",dTotalArea);
	CallDap(g_DAPHwnd,CBaseCfgInfo::m_BaseProjectInfo.strProcessId,m_strURL,strTemp);//CBaseCfgInfo::m_BaseProjectInfo.strXMBH 75
	
	AfxMessageBox("\n �ύ���ݳɹ���");

	CStringArray strTextArray;
	for(int k=0; k<12; k++)
		strTextArray.Add(strFldValueArray[k]);

	for(i=0; i<19; i++)
		free(strFldValueArray[i]);
	free(strFldValueArray);

	CString strCadLyName;
	strCadLyName.Format("%s��Ŀ��ע",m_strCadLyName);
	CDistEntityTool tempTool;
	AcDbObjectId tempId;
	tempTool.InsertBlkAttrRef(tempId,"_��Ŀ����",strCadLyName.GetBuffer(),AcGePoint3d(pt[0],pt[1],0),nHeight,m_LxsxArray,strTextArray);
	CDistXData tempXData(tempId,"ROWID_OBJS");
	tempXData.Add("wang","hui");
	tempXData.Update(); tempXData.Close();
}

void CDlgXmInput::OnBnClickedBtnExit()
{
	CAcUiDialog::OnOK();
}


void CDlgXmInput::SetValueToControls()
{
	m_list_base.DeleteAllItems();
	m_list_base.InsertItem(0,"���̺�");
	m_list_base.SetItemText(0,1,m_LXSX.m_strLCH);

	m_list_base.InsertItem(1,"֤��"); 
	m_list_base.SetItemText(1,1,m_LXSX.m_strZH);

	m_list_base.InsertItem(2,"����");
	m_list_base.SetItemText(2,1,m_LXSX.m_strRQ);

	m_list_base.InsertItem(3,"��Ŀ����"); 
	m_list_base.SetItemText(3,1,m_LXSX.m_strXMMC);

	m_list_base.InsertItem(4,"���赥λ"); 
	m_list_base.SetItemText(4,1,m_LXSX.m_strJSDW);

	m_list_base.InsertItem(5,"�õ�����"); 
	m_list_base.SetItemText(5,1,m_LXSX.m_strYDXZ);

	m_list_base.InsertItem(6,"��������");
	m_list_base.SetItemText(6,1,m_LXSX.m_strLXLX);

	((CEdit*)GetDlgItem(IDC_EDIT_ZMJ))->SetWindowText(m_LXSX.m_strYDMJ);//�õ����

	((CComboBox*)GetDlgItem(IDC_CMB_YDLX))->SelectString(0,m_LXSX.m_strYDLX);//�õ�����
	((CComboBox*)GetDlgItem(IDC_CMB_DKBH))->SetWindowText(m_LXSX.m_strDKBH); //�ؿ���
	((CComboBox*)GetDlgItem(IDC_CMB_ZG))->SetWindowText(m_LXSX.m_strZG);//�ָ�
	((CComboBox*)GetDlgItem(IDC_CMB_XK))->SetWindowText(m_LXSX.m_strXK); //�߿�

	((CEdit*)GetDlgItem(IDC_EDIT_X))->SetWindowText(m_LXSX.m_strX); //��עXֵ
	((CEdit*)GetDlgItem(IDC_EDIT_Y))->SetWindowText(m_LXSX.m_strY); //��עYֵ
	((CEdit*)GetDlgItem(IDC_EDIT_JBR))->SetWindowText(m_LXSX.m_strJBR); //������
	((CEdit*)GetDlgItem(IDC_EDIT_BZ))->SetWindowText(m_LXSX.m_strBZ); //��ע
	((CEdit*)GetDlgItem(IDC_EDIT_OBJ))->SetWindowText(m_LXSX.m_strObjId); //ʵ��ID
}


void CDlgXmInput::GetValueFromControls()
{
	((CEdit*)GetDlgItem(IDC_EDIT_ZMJ))->GetWindowText(m_LXSX.m_strYDMJ);//�õ����
	int nSelect = ((CComboBox*)GetDlgItem(IDC_CMB_YDLX))->GetCurSel(); if(nSelect < 0) return;
	((CComboBox*)GetDlgItem(IDC_CMB_YDLX))->GetLBText(nSelect,m_LXSX.m_strYDLX);//�õ�����

	((CComboBox*)GetDlgItem(IDC_CMB_DKBH))->GetWindowText(m_LXSX.m_strDKBH); //�ؿ���
	((CComboBox*)GetDlgItem(IDC_CMB_ZG))->GetWindowText(m_LXSX.m_strZG);//�ָ�
	((CComboBox*)GetDlgItem(IDC_CMB_XK))->GetWindowText(m_LXSX.m_strXK); //�߿�


	((CEdit*)GetDlgItem(IDC_EDIT_X))->GetWindowText(m_LXSX.m_strX); //��עXֵ
	((CEdit*)GetDlgItem(IDC_EDIT_Y))->GetWindowText(m_LXSX.m_strY); //��עYֵ
	((CEdit*)GetDlgItem(IDC_EDIT_JBR))->GetWindowText(m_LXSX.m_strJBR); //������
	((CEdit*)GetDlgItem(IDC_EDIT_BZ))->GetWindowText(m_LXSX.m_strBZ); //��ע
	((CEdit*)GetDlgItem(IDC_EDIT_OBJ))->GetWindowText(m_LXSX.m_strObjId); //ʵ��ID
}



void CDlgXmInput::SetValueToXData(AcDbObjectId& ObjId,const char* strRegName)
{
	CStringArray strValueArray;
	strValueArray.RemoveAll();
	strValueArray.Add(m_LXSX.m_strLCH); 
	strValueArray.Add(m_LXSX.m_strZH);  
	strValueArray.Add(m_LXSX.m_strRQ);   
	strValueArray.Add(m_LXSX.m_strXMMC); 
	strValueArray.Add(m_LXSX.m_strJSDW);  
	strValueArray.Add(m_LXSX.m_strYDXZ);  
	strValueArray.Add(m_LXSX.m_strLXLX); 
	strValueArray.Add(m_LXSX.m_strYDMJ); 
	strValueArray.Add(m_LXSX.m_strYDLX);  
	strValueArray.Add(m_LXSX.m_strDKBH);   
	strValueArray.Add(m_LXSX.m_strZG);    
	strValueArray.Add(m_LXSX.m_strXK);    
	strValueArray.Add(m_LXSX.m_strX);    
	strValueArray.Add(m_LXSX.m_strY);
	strValueArray.Add(m_LXSX.m_strJBR);
	strValueArray.Add(m_LXSX.m_strBZ);    

	CDistXData tempXData(ObjId,strRegName);
	tempXData.Add(m_FNameArray,strValueArray);
	tempXData.Update();
	tempXData.Close();

}



void CDlgXmInput::GetValueFromXData(AcDbObjectId& ObjId,const char* strRegName)
{
	CStringArray strValueArray,strFNameArray;
	strValueArray.RemoveAll();strFNameArray.RemoveAll();
	CDistXData tempXData(ObjId,strRegName);
	tempXData.Select(strFNameArray,strValueArray);
	if(strValueArray.GetCount() < 16) return;
	m_LXSX.m_strLCH = strValueArray.GetAt(0); 
	m_LXSX.m_strZH = strValueArray.GetAt(1); 
	m_LXSX.m_strRQ = strValueArray.GetAt(2);   
	m_LXSX.m_strXMMC = strValueArray.GetAt(3); 
	m_LXSX.m_strJSDW = strValueArray.GetAt(4); 
	m_LXSX.m_strYDXZ = strValueArray.GetAt(5); 
	m_LXSX.m_strLXLX = strValueArray.GetAt(6); 
	m_LXSX.m_strYDMJ = strValueArray.GetAt(7); 
	m_LXSX.m_strYDLX = strValueArray.GetAt(8); 
	m_LXSX.m_strDKBH = strValueArray.GetAt(9); 
	m_LXSX.m_strZG = strValueArray.GetAt(10);   
	m_LXSX.m_strXK = strValueArray.GetAt(11);  
	m_LXSX.m_strX  = strValueArray.GetAt(12); 
	m_LXSX.m_strY  = strValueArray.GetAt(13); 
	m_LXSX.m_strJBR  = strValueArray.GetAt(14);
	m_LXSX.m_strBZ = strValueArray.GetAt(15);    
}





void CDlgXmInput::CallDap(HWND hDapHwnd,CString strXMBH,CString strURL,CString strValue)
{
	if (NULL == hDapHwnd) return;
	HWND hCadWnd = adsw_acadMainWnd();
	int nCadWnd = (int)hCadWnd;

	IWSAccessor* pWsAccessor = XSingletonFactory::Instance()->CreateVarWSAccessorObject();
	if (pWsAccessor == NULL) return;

	HRESULT hr = pWsAccessor->Connect(strURL.GetBuffer());

	TCHAR lpMsg[512]={0};

	// ������ӳ���
	if( hr != S_OK )
	{
		pWsAccessor->Answer(lpMsg);MessageBox( lpMsg);return;
	}

	CString sSql;
	sSql.Format("select * from TBCAD_WEB_INTERACT where REF_PROCESS_ID = %s",strXMBH);


	// ע�⣬�������鳤��һ��Ҫ������һЩ
	_variant_t varParams[5];
	varParams[0] = sSql.GetBuffer();
	_variant_t varResult;

	// ���÷���
	hr = pWsAccessor->CallMethod("ExecSqlQuery",varParams, &varResult);

	// ���������ó�����Ϣ
	if (hr != S_OK )
	{
		pWsAccessor->Answer(lpMsg);
	    MessageBox( lpMsg);
		return;
	}
	IDistConnectionXML* lpConn = CreateConnectObjcet();
	if (S_OK != lpConn->Connect(NULL, (_bstr_t)varResult, NULL, NULL, NULL))
	{
		AfxMessageBox("Connect to file failed!\n");
		return;
	}

	IDistCommandXML* lpCmd = NULL;
	lpConn->QueryCommand(&lpCmd);

	char strXmlPath[] = "/NewDataSet/Table"; // XML�ڵ������ɵ�ʱ���Ѿ�Լ�����˽��������
	IDistRecordSetXML* lpRst = NULL;
	lpCmd->SelectData(strXmlPath, &lpRst);

	if (NULL == lpRst)
	{
#ifdef _DEBUG
		AfxMessageBox("TBCAD_WEB_INTERACT����δ�ҵ�����");
#endif
		lpCmd->Release();
		lpConn->Release();
		return;
	}

	BOOL bOK = lpRst->MoveFirst();
	static CString sData = "";
	sData.Empty();
	while (bOK)
	{
		CString strBdeData = (char*)((*lpRst)["BDE_DATA"]);
		CString strDescription = (char*)((*lpRst)["DESCRIPTION"]);

		sData += strBdeData;
		sData += "=";
		sData += strValue;//GetAttrValue(strDescription);
		sData += ";";

		bOK = lpRst->MoveNext();
	}


	COPYDATASTRUCT cds;
	cds.dwData = 0;
	cds.cbData = sData.GetLength() + 1;
	cds.lpData = (void*)sData.GetBuffer(0);
	::SendMessage(hDapHwnd, WM_COPYDATA, (WPARAM)nCadWnd, (LPARAM)&cds);
	acutPrintf("\n Handle %d  Call DAP -> %s",(int)hDapHwnd,sData);
	lpRst->Release();
	lpCmd->Release();
	lpConn->Release();
}



CString CDlgXmInput::GetAttrValue(CString sKey)
{
	CString strRet = "";
	/*
	if (sKey.CollateNoCase("��Ŀ���") == 0)
	{
		return m_LXSX.m_strLCH;
	}

	else if (sKey.CollateNoCase("���̺�") == 0)
	{
	strRet = g_Lch;
	}
	else if (sKey.CollateNoCase("֤��") == 0)
	{
	strRet = g_Zh;
	}
	else if (sKey.CollateNoCase("����") == 0)
	{
	strRet = g_Rq;
	}
	else if (sKey.CollateNoCase("��Ŀ����") == 0)
	{
	strRet = g_Xmmc;
	}
	else if (sKey.CollateNoCase("���赥λ") == 0)
	{
	strRet = g_Jsdw;
	}
	else if (sKey.CollateNoCase("�����ַ") == 0)
	{
	strRet = g_Jsdz;
	}
	else if (sKey.CollateNoCase("�õ����ʱ��") == 0)
	{
	strRet = g_Ydxz;
	}
	else if (sKey.CollateNoCase("���õ����") == 0)
	{
	strRet = g_Zydmj;
	}
	*/
	return strRet;
}



