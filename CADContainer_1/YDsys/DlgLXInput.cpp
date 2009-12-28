// DlgLXInput.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DlgLXInput.h"
#include "resource.h"
#include "DistXData.h"

#include "DistEntityTool.h"
#include "DistSelSet.h"
#include "AccessWebServiceInfo.h"

#include "..\SdeData\DistBaseInterface.h"
#include "..\SdeData\DistExchangeInterface.h"

#include "..\WSAccessor\WsAccessor_export.h"
#include "..\XMLData\DistBaseInterface.h"

#include "DistXData.h"
#include "DistUiPrompts.h"
#include "DistSysFun.h"

//�������������뺯��()
void MyRound(double& dValue,int nNumPer)
{
	INT64 nValue =0;
	for(int i=0; i<nNumPer+1;i++)
		dValue = dValue * 10;
	nValue = (INT64)dValue;
	if(nValue % 10 >= 5)
		nValue = nValue/10 + 1;
	else
		nValue = nValue/10;

	dValue = nValue;
	for(i=0; i<nNumPer; i++)
		dValue = dValue /10.0;
}

extern HWND  g_DAPHwnd; 
// CDlgLXInput �Ի���

IMPLEMENT_DYNAMIC(CDlgLXInput, CAcUiDialog)
CDlgLXInput::CDlgLXInput(CWnd* pParent)
	: CAcUiDialog(CDlgLXInput::IDD, pParent)
{
	m_strNameArrayDK.RemoveAll();           m_strFNameArrayDK.RemoveAll();               m_strNameArrayXM.RemoveAll();           m_strFNameArrayXM.RemoveAll();
	m_strNameArrayDK.Add("PROJECT_CODE");   m_strFNameArrayDK.Add("��Ŀ��ʾ���");		 m_strNameArrayXM.Add("PROJECT_CODE");   m_strFNameArrayXM.Add("��Ŀ��ʾ���");
	m_strNameArrayDK.Add("LICENSE_CODE");   m_strFNameArrayDK.Add("��֤���");			 m_strNameArrayXM.Add("LICENSE_CODE");   m_strFNameArrayXM.Add("��֤���");
	m_strNameArrayDK.Add("REG_TIME");       m_strFNameArrayDK.Add("�Ǽ�����");			 m_strNameArrayXM.Add("REG_TIME");       m_strFNameArrayXM.Add("�Ǽ�����");
	m_strNameArrayDK.Add("PROJECT_NAME");   m_strFNameArrayDK.Add("��Ŀ����");			 m_strNameArrayXM.Add("PROJECT_NAME");   m_strFNameArrayXM.Add("��Ŀ����");
	m_strNameArrayDK.Add("BUILD_UNIT");     m_strFNameArrayDK.Add("���赥λ");			 m_strNameArrayXM.Add("BUILD_UNIT");     m_strFNameArrayXM.Add("���赥λ");
	m_strNameArrayDK.Add("USELAND_TYPEID"); m_strFNameArrayDK.Add("�õ����ʱ��");		 m_strNameArrayXM.Add("USELAND_TYPEID"); m_strFNameArrayXM.Add("�õ����ʱ��");
	m_strNameArrayDK.Add("LXLX");           m_strFNameArrayDK.Add("��������");			 m_strNameArrayXM.Add("LXLX");           m_strFNameArrayXM.Add("��������");
	m_strNameArrayDK.Add("JBRY");           m_strFNameArrayDK.Add("������Ա");			 m_strNameArrayXM.Add("JBRY");           m_strFNameArrayXM.Add("������Ա");
	m_strNameArrayDK.Add("DKBH");           m_strFNameArrayDK.Add("�ؿ���");			 m_strNameArrayXM.Add("JSYDMJ");         m_strFNameArrayXM.Add("�����õ����");
	m_strNameArrayDK.Add("YDLX");           m_strFNameArrayDK.Add("�õ�����");			 m_strNameArrayXM.Add("DLYDMJ");         m_strFNameArrayXM.Add("��·�õ����");
	m_strNameArrayDK.Add("YDMJ");           m_strFNameArrayDK.Add("�õ����");			 m_strNameArrayXM.Add("LHYDMJ");         m_strFNameArrayXM.Add("�̻��õ����");
	m_strNameArrayDK.Add("ZG");             m_strFNameArrayDK.Add("�ָ�");				 m_strNameArrayXM.Add("QTYDMJ");         m_strFNameArrayXM.Add("�����õ����");
	m_strNameArrayDK.Add("XK");             m_strFNameArrayDK.Add("�߿�");				 m_strNameArrayXM.Add("ZYDMJ");          m_strFNameArrayXM.Add("���õ����");
	m_strNameArrayDK.Add("X");              m_strFNameArrayDK.Add("Xֵ"); 
	m_strNameArrayDK.Add("Y");              m_strFNameArrayDK.Add("Yֵ");
	m_strNameArrayDK.Add("BZ");             m_strFNameArrayDK.Add("��ע");

	m_NewEntityArray.RemoveAll();
}


CDlgLXInput::~CDlgLXInput()
{

}

void CDlgLXInput::DoDataExchange(CDataExchange* pDX)
{
	CAcUiDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_BTN_YD,m_sBtnYD);
	DDX_Control(pDX, IDC_BTN_XM,m_sBtnXM);
	DDX_Control(pDX, IDC_BTN_PT,m_sBtnPT);
	DDX_Control(pDX, IDC_BTN_PT_T,m_sBtnPT_T);
	DDX_Control(pDX, IDC_LIST_BASE, m_list_base);
	DDX_Control(pDX, IDC_LIST_YDXX, m_list_ydxx);
}

LRESULT CDlgLXInput::OnAcadKeepFocus (WPARAM, LPARAM)
{
	return (TRUE) ;
}


BEGIN_MESSAGE_MAP(CDlgLXInput, CAcUiDialog)
	ON_BN_CLICKED(IDC_BTN_XM, OnBnClickedSelectXM)
	ON_BN_CLICKED(IDC_BTN_YD, OnBnClickedSelectYD)
	ON_BN_CLICKED(IDC_BTN_PT, OnBnClickedSelectPT)
	ON_BN_CLICKED(IDC_BTN_PT_T, OnBnClickedSelectPT_T)
	ON_BN_CLICKED(IDC_BTN_ADD, OnBnClickedBtnAdd)
	ON_BN_CLICKED(IDC_BTN_MODIFY, OnBnClickedBtnModify)
	ON_BN_CLICKED(IDC_BTN_DELETE, OnBnClickedBtnDelete)
	ON_BN_CLICKED(IDC_BTN_SDE, OnBnClickedBtnSde)
	ON_BN_CLICKED(IDC_BTN_EXIT, OnBnClickedBtnExit)
	ON_BN_CLICKED(IDC_BTN_POS, OnBnClickedBtnPos)
	ON_NOTIFY(NM_CLICK, IDC_LIST_YDXX, OnNMClickListYdxx)
	ON_CBN_SELCHANGE(IDC_CMB_YDLX, OnCbnSelchangeCmbYdlx)
	ON_BN_CLICKED(IDC_CHECK_XM, OnBnClickedCheckXm)
	ON_CBN_SELCHANGE(IDC_CMB_DKBH, OnCbnSelchangeCmbDkbh)
	ON_CBN_SELCHANGE(IDC_CMB_ZG, OnCbnSelchangeCmbZg)
	ON_CBN_SELCHANGE(IDC_CMB_XK, OnCbnSelchangeCmbXk)
	ON_EN_KILLFOCUS(IDC_EDIT_BZ, OnEnKillfocusEditBz)
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()


//��ʼ��
BOOL CDlgLXInput::OnInitDialog()
{
	CAcUiDialog::OnInitDialog();
	if(NULL == m_pConnect)
	{
		AfxMessageBox("SDE���ݿ�û�����ӣ�"); SendMessage(WM_CLOSE,0,0);
		return FALSE;
	}

	//���ذ�ť
	m_sBtnYD.AutoLoad();
	m_sBtnXM.AutoLoad();
	m_sBtnPT.AutoLoad();
	m_sBtnPT_T.AutoLoad();

	//��ʼ��������Ϣ�б�
	m_list_base.SetExtendedStyle(m_list_base.GetExtendedStyle()| LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_list_base.InsertColumn(0,"��Ϣ����",LVCFMT_LEFT,90);
	m_list_base.InsertColumn(1,"��Ϣ����",LVCFMT_LEFT,230);
	m_list_base.DeleteAllItems();

	m_list_base.InsertItem(0,"��Ŀ��ʾ���");  m_list_base.SetItemText(0,1,m_PrjBaseInfo.strPROJECT_CODE);
	m_list_base.InsertItem(1,"��Ŀ���");  m_list_base.SetItemText(1,1,m_PrjBaseInfo.strPROJECT_ID);
	m_list_base.InsertItem(2,"��֤���");      m_list_base.SetItemText(2,1,m_PrjBaseInfo.strLICENCE_CODE);
	m_list_base.InsertItem(3,"�Ǽ�����");      m_list_base.SetItemText(3,1,m_PrjBaseInfo.strREG_TIME);
	m_list_base.InsertItem(4,"��Ŀ����");      m_list_base.SetItemText(4,1,m_PrjBaseInfo.strPROJECT_NAME);
	m_list_base.InsertItem(5,"���赥λ");      m_list_base.SetItemText(5,1,m_PrjBaseInfo.strBUILD_UNIT);
	m_list_base.InsertItem(6,"�õ����ʱ��");  m_list_base.SetItemText(6,1,m_PrjBaseInfo.strUSELAND_TYPEID);
	m_list_base.InsertItem(7,"��������");      m_list_base.SetItemText(7,1,m_PrjBaseInfo.strCADTYPE);
	m_list_base.InsertItem(8,"Ͷ���ܹ�ģ");    m_list_base.SetItemText(8,1,m_PrjBaseInfo.strINVEST_SUM);
	m_list_base.InsertItem(9,"�ݻ���");        m_list_base.SetItemText(9,1,m_PrjBaseInfo.strFLOOR_AREA_RATIO);
	m_list_base.InsertItem(10,"�����ܶ�");      m_list_base.SetItemText(10,1,m_PrjBaseInfo.strBUILDING_DENSITY);
	m_list_base.InsertItem(11,"������Ա");     m_list_base.SetItemText(11,1,m_PrjBaseInfo.strJBRY);


	m_list_ydxx.SetExtendedStyle(m_list_ydxx.GetExtendedStyle()| LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_list_ydxx.InsertColumn(0,"�ؿ���",LVCFMT_LEFT,60);	
	m_list_ydxx.InsertColumn(1,"�õ�����",LVCFMT_LEFT,60);
	m_list_ydxx.InsertColumn(2,"�õ����",LVCFMT_LEFT,80);
	m_list_ydxx.InsertColumn(3,"�ָ�",LVCFMT_LEFT,40);
	m_list_ydxx.InsertColumn(4,"�߿�",LVCFMT_LEFT,40);
	m_list_ydxx.InsertColumn(5,"X ֵ",LVCFMT_LEFT,40);
	m_list_ydxx.InsertColumn(6,"Y ֵ",LVCFMT_LEFT,40);
	m_list_ydxx.InsertColumn(7,"��ע",LVCFMT_LEFT,100);
	m_list_ydxx.InsertColumn(8,"ʵ��ID",LVCFMT_LEFT,0);

	m_bIsModify = FALSE;  //�Ƿ����޸ı��

	((CComboBox*)GetDlgItem(IDC_CMB_ZG))->SetWindowText("8");    //�ָ�
	((CComboBox*)GetDlgItem(IDC_CMB_ZG_T))->SetWindowText("8");    //�ָ�
	((CComboBox*)GetDlgItem(IDC_CMB_XK))->SetWindowText("1.0");    //�߿�
	((CComboBox*)GetDlgItem(IDC_EDIT_BZ))->SetWindowText("û�б�ע!"); //��ע

	ReadEntityInfo();

	return TRUE;
}

//���ó�ʼ��Ϣ
void CDlgLXInput::SetInfo(IDistConnection * pConnect,CString strCadLyNameDK,CString strCadLyNameXM,CString strSdeLyNameDK,
						  CString strSdeLyNameXM,CString strURL,CString strProcessId)
						  
{
	m_pConnect = pConnect;
	m_strCadLyNameDK = strCadLyNameDK;
	m_strCadLyNameXM = strCadLyNameXM;
	m_strSdeLyNameDK = strSdeLyNameDK;
	m_strSdeLyNameXM = strSdeLyNameXM;
	m_strURL =  strURL;
	m_PrjBaseInfo = CBaseCfgInfo::m_basePrjInfo;
	m_strDapProcessId = strProcessId;
}


int CDlgLXInput::ReadXmPos(AcGePoint3d& pt)
{
	IDistCommand* pCmd = CreateSDECommandObjcet();
	pCmd->SetConnectObj(m_pConnect);
	char strSQL[100]={0};
	sprintf(strSQL,"F:OBJECTID,SHAPE,T:%s,W:PROJECT_CODE='%s'",m_strSdeLyNameXM,m_PrjBaseInfo.strPROJECT_CODE);
	IDistRecordSet* pRcdSet = NULL;
	if(pCmd->SelectData(strSQL,&pRcdSet)!=1)
	{
		pCmd->Release(); return 0;
	}
	int nRcdCount = pRcdSet->GetRecordCount();
	if(nRcdCount == 0)
	{
		if(NULL != pRcdSet) pRcdSet->Release();
		pCmd->Release(); return 2;
	}

	char strValue[20] = {0};
	if(pRcdSet->BatchRead()!=1)
	{
		if(NULL != pRcdSet) pRcdSet->Release(); 
		pCmd->Release(); return 0;
	}
	pRcdSet->GetValueAsString(strValue,1); //��ȡSHAPE
	IDistPoint* ptArray = NULL;
	int nType = 0;
	int nPtCount = 0;
	int nShape =atoi(strValue);
	if(1 != pCmd->GetShapePointArray(&nType,&ptArray,&nPtCount,&nShape))
	{
		if(NULL != pRcdSet) pRcdSet->Release();
		pCmd->Release(); return 0;
	}
	pt.x = ptArray[0].x; pt.y = ptArray[0].y; pt.z =0;

	pRcdSet->Release();
	pCmd->Release();
	return 1;
}



BOOL CDlgLXInput::ReadEntityInfo()
{
	m_list_ydxx.DeleteAllItems();
	m_OldEntityArray.RemoveAll();

	//ע����չ��������
	char strReg[260]={0}; memset(strReg,0,sizeof(char)*260);
	sprintf(strReg,"DIST_%s",m_strSdeLyNameDK);

	//ѡ��ͼ��������ʵ��
	AcDbObjectIdArray IdArray;
	CDistEntityTool tempTool;
	tempTool.SelectEntityInLayer(IdArray,m_strCadLyNameDK); 
	int nCount = IdArray.length();


	//��ȡ��ǰ��Ŀ��CADʵ��
	for(int k=0; k<nCount; k++)
	{
		//1.����չ����
		AcDbObjectId tempId = IdArray.at(k);
		CDistXData tempXData(tempId,strReg);
		CStringArray strValueArray,strFNameArray; 
		tempXData.Select(strFNameArray,strValueArray);
		tempXData.Close();
		int nNum = strValueArray.GetCount();
		if(nNum == 0) continue;  //û����չ����,����

		//2.ȡ��Ŀ���
		CString strProjectCode; 
		for(int ii=0; ii<nNum; ii++)
		{
			CString strTempName = strFNameArray.GetAt(ii);
			if(strTempName.CompareNoCase("PROJECT_CODE")==0)
			{
				strProjectCode = strValueArray.GetAt(ii);
				break;
			}
		}

		//3.�ж��Ƿ�Ϊ��ǰ��Ŀ
		if(strProjectCode.CompareNoCase(m_PrjBaseInfo.strPROJECT_CODE) != 0) continue;  

		//4.��ȡ��ǰ��Ŀ�ĵؿ���Ϣ
		DIST_CADENTITY_STRUCT tempEntity;
		for(int jj =0; jj<nNum; jj++)
		{
			CString strTempName = strFNameArray.GetAt(jj);
			if(strTempName.CompareNoCase("YDMJ")==0)  //�õ���� 
			{
				tempEntity.strArea = strValueArray.GetAt(jj);
			}
			else if(strTempName.CompareNoCase("YDLX")==0)//�õ�����
			{
				tempEntity.strYDType = strValueArray.GetAt(jj);
			}
			else if(strTempName.CompareNoCase("DKBH")==0)//�ؿ���
			{
				tempEntity.strDKCode = strValueArray.GetAt(jj);
			}
			else if(strTempName.CompareNoCase("ZG")==0)//�ָ�
			{
				tempEntity.strTxtHeight = strValueArray.GetAt(jj);
			}
			else if(strTempName.CompareNoCase("XK")==0)//�߿�
			{
				tempEntity.strLnWidth = strValueArray.GetAt(jj);
			}
			else if(strTempName.CompareNoCase("X")==0)//X ֵ
			{
				tempEntity.strX= strValueArray.GetAt(jj);
			}
			else if(strTempName.CompareNoCase("Y")==0)//Y ֵ
			{
				tempEntity.strY = strValueArray.GetAt(jj);
			}
			else if(strTempName.CompareNoCase("BZ")==0)//��ע
			{
				tempEntity.strBZ = strValueArray.GetAt(jj);
			}
		}
		tempEntity.strObjId.Format("%d",tempId.asOldId());

		//5.���浽����,���ָ�ʹ��
		m_OldEntityArray.Add(tempEntity);
		m_NewEntityArray.Add(tempEntity);

		//6.����Ϣ��ӵ��б�
		int nPos = m_list_ydxx.GetItemCount();
		m_list_ydxx.InsertItem(nPos,tempEntity.strDKCode);
		m_list_ydxx.SetItemText(nPos,1,tempEntity.strYDType);
		m_list_ydxx.SetItemText(nPos,2,tempEntity.strArea);
		m_list_ydxx.SetItemText(nPos,3,tempEntity.strTxtHeight);
		m_list_ydxx.SetItemText(nPos,4,tempEntity.strLnWidth);
		m_list_ydxx.SetItemText(nPos,5,tempEntity.strX);
		m_list_ydxx.SetItemText(nPos,6,tempEntity.strY);
		m_list_ydxx.SetItemText(nPos,7,tempEntity.strBZ);
		m_list_ydxx.SetItemText(nPos,8,tempEntity.strObjId);
	}

	//��ѯ�ؿ���Ϣ
	nCount = m_list_ydxx.GetItemCount();
	IDistCommand* pCommand = CreateSDECommandObjcet();
	pCommand->SetConnectObj(m_pConnect);
	char strSQL[200]={0}; memset(strSQL,0,sizeof(char)*200);
	sprintf(strSQL,"F:OBJECTID,T:%s,W:PROJECT_CODE='%s',##",m_strSdeLyNameDK,m_PrjBaseInfo.strPROJECT_CODE);
	IDistRecordSet* pRcdSet = NULL;
	if(1 != pCommand->SelectData(strSQL,&pRcdSet))
	{
		pCommand->Release(); MessageBox("���ʿռ��ʧ��!"); SendMessage(WM_CLOSE);
		return FALSE;
	}

	int nRcdCount = pRcdSet->GetRecordCount();
	if(nRcdCount >  nCount)  //û�е���,����û�е�ȫ
	{
		MessageBox("��ǰ��Ŀ�ؿ���Ϣ�Ѵ���, ��û�е���; ���ȼ���, �ٱ༭��","��Ϣ��ʾ");
		pCommand->Release(); pRcdSet->Release(); SendMessage(WM_CLOSE);
		return FALSE;
	}
	pCommand->Release(); pRcdSet->Release();


	return TRUE;
}


//ѡ��ؿ�
void CDlgLXInput::OnBnClickedSelectYD()
{
	//����ѡ��ʵ��
	BeginEditorCommand();

	//���ù�����
	ads_name ssName;
	char strLyName[260]={0}; memset(strLyName,0,sizeof(char)*260);
	strcpy(strLyName,m_strCadLyNameDK.GetBuffer(0));
	struct resbuf filter;
	filter.restype=AcDb::kDxfLayerName;
	filter.resval.rstring = strLyName;
	filter.rbnext=NULL;

	//ѡ��ʵ��
	if (acedSSGet(":S", NULL, NULL, &filter, ssName) != RTNORM)
	{
		AfxMessageBox("\n ѡ��ʵ������!"); acedSSFree(ssName); 
		CompleteEditorCommand(); return;
	}
	AcDbObjectId tempObjId;
	ads_name ssTemp;
	acedSSName(ssName, 0, ssTemp);
	acdbGetObjectId(tempObjId, ssTemp);
	acedSSFree(ssName);

	//��ʵ��
	AcDbPolyline* pline = NULL;
	if(Acad::eOk != acdbOpenObject(pline,tempObjId,AcDb::kForRead))
	{
		AfxMessageBox("\n ʵ����������!");
		CompleteEditorCommand(); return;
	}

	//�ж��Ƿ�պ�
	if(pline->isClosed()==Adesk::kFalse)
	{
		AfxMessageBox("ѡ��ʵ�岻�պ�!");
		pline->close(); CompleteEditorCommand(); return;
	}
	
	//�ж��Ƿ�Ϊ�Ѿ����ڵؿ�
	char strRegName[260]={0};memset(strRegName,0,sizeof(char)*260);
	sprintf(strRegName,"DIST_%s",m_strSdeLyNameDK);
	CDistXData tempXData(pline,strRegName);
	CStringArray strFNameArray,strValueArray;
	tempXData.Select(strFNameArray,strValueArray);
	if(strValueArray.GetCount() > 0)
	{
		pline->close();
		AfxMessageBox("ѡ��ʵ�������Ѿ����ã�������������!");
		CompleteEditorCommand(); return;
	}

	AcGePoint3dArray pts;
	AcGeDoubleArray TempBlgArray;
	CDistEntityTool tempTool;
	tempTool.GetPolylinePts(pline,pts);

	if(CheckCrossOrContain(pts,m_strSdeLyNameDK)==FALSE)  //����ָ������ͼ����
	{
		pline->close();
		CompleteEditorCommand(); return;
	}

	int nCountItem = m_list_ydxx.GetItemCount();
	for(int ii=0; ii<nCountItem; ii++)
	{
		CString strId = m_list_ydxx.GetItemText(ii,8);
		AcDbObjectId tempId;
		tempId.setFromOldId(atoi(strId));

		if(tempId == pline->objectId())
		{
			pline->close();
			AfxMessageBox("ʵ���Ѿ��������б�ѡ��ʧ��!");
			CompleteEditorCommand(); return;
		}
	
		AcGeDoubleArray BlgArray;
		AcGePoint3dArray tempPtArray;
		AcDbPolyline* plineOther = NULL;
		if(Acad::eOk == acdbOpenObject(plineOther,tempId,AcDb::kForRead))
		{
			tempTool.GetPolylinePts(plineOther,tempPtArray,BlgArray);
			plineOther->close();
		}
		else
		{
			acutPrintf("\n��ʵ��ʧ��!");
			CompleteEditorCommand(); return;
		}
		

		AcGePoint3dArray tempPts;
		BlgArray.setPhysicalLength(0);
		tempTool.GetPolylinePts(pline,tempPts,BlgArray);

		
		if(PolygonIsInPolygon(tempPts,tempPtArray)==TRUE)
		{
			if(AfxMessageBox("�õؿ��뵱ǰ�����ؿ��н�����ظ������Ƿ������",MB_YESNO)!=IDYES)
			{
				pline->close(); CompleteEditorCommand(); return;
			}
		}
	}


	//�ռ��ѯ,�Ƿ�λ�����ص�
	int nPtCount = pts.length();
	DIST_POINT_STRUCT* ptArray = new DIST_POINT_STRUCT[nPtCount];
	for(int i=0; i<nPtCount; i++)
	{
		ptArray[i].x = pts[i].x; ptArray[i].y = pts[i].y;
	}

	char strSQL[255]={0};
	sprintf(strSQL,"F:OBJECTID,T:%s,W:PROJECT_ID<>'%s',##",m_strSdeLyNameDK,m_PrjBaseInfo.strPROJECT_ID);
	if(1 != gCheckSdeData(m_pConnect,m_strSdeLyNameDK,strSQL,ptArray,nPtCount))
	{
		if(AfxMessageBox("������λ�������������н�����ص������Ƿ������",MB_YESNO)!=IDYES)
		{
			delete [] ptArray; ptArray = NULL;
			pline->close();CompleteEditorCommand(); return;
		}	
	}
	delete[] ptArray; ptArray= NULL;

	double dArea = 0.0;
	pline->getArea(dArea);
	pline->close();
	dArea = dArea;
	CString strTemp,strArea;
	strArea.Format("%0.5f",dArea);
	((CEdit*)GetDlgItem(IDC_EDIT_ZMJ))->SetWindowText(strArea);
	strTemp.Format("%d",tempObjId.asOldId());
	((CEdit*)GetDlgItem(IDC_EDIT_OBJ))->SetWindowText(strTemp);
	
	CompleteEditorCommand();

	int nSelect = m_list_ydxx.GetSelectionMark();
	if(nSelect < 0) return;	
	m_list_ydxx.SetItemText(nSelect,2,strArea);
	m_list_ydxx.SetItemText(nSelect,8,strTemp);
}


BOOL CDlgLXInput::GetBlockNameByBlkRefId(CString &strName,AcDbObjectId objId)
{
	ads_name eName;
	struct resbuf* resBlockRef;
	acdbGetAdsName(eName, objId);
	resBlockRef = acdbEntGet(eName);
	acedSSFree(eName);

	while (resBlockRef)
	{
		if (resBlockRef->restype == 0)
		{
			if(stricmp(resBlockRef->resval.rstring, "insert") != 0)
			{
				acutRelRb(resBlockRef);
				return FALSE;
			}
		}

		if (resBlockRef->restype == 2) // ����Ϊ2��ʾ���ǿ���
		{
			strName.Format("%s", resBlockRef->resval.rstring);
			acutRelRb(resBlockRef);
			return TRUE;
		}

		resBlockRef = resBlockRef->rbnext;
	}

	acutRelRb(resBlockRef);
	return FALSE;
}


void CDlgLXInput::OnBnClickedSelectXM()
{
	BeginEditorCommand();
	acutPrintf("\nѡ����Ŀ��Ϣ���Կ�");
	ads_name ssName;
	struct resbuf* prebFilter = acutBuildList(RTDXF0, "INSERT", 100, "AcDbBlockReference", 0);
	int nResult = acedSSGet(":S", NULL, NULL, prebFilter, ssName);//"X"
	acutRelRb(prebFilter);
	if(nResult != RTNORM) 
	{
		CompleteEditorCommand(); return;
	}

	long lCount = 0;
	acedSSLength(ssName, &lCount);
	if(lCount ==0) return;

	ads_name ssEnt;
	acedSSName(ssName,0,ssEnt);

	AcDbObjectId ObjId;
	acdbGetObjectId(ObjId, ssEnt);	
	acedSSFree(ssName);
	AcDbBlockReference* pBlkRef = NULL;
	if(Acad::eOk != acdbOpenObject(pBlkRef,ObjId,AcDb::kForRead))
	{
		AcDbObjectId blkId = pBlkRef->blockId();
		CString strName;
		GetBlockNameByBlkRefId(strName,blkId);
		if(strName.CompareNoCase("_��Ŀ����")!=0)
		{
			acutPrintf("\n��ѡ����Ŀ���Կ�!");
			pBlkRef->close(); CompleteEditorCommand();  return;
		}
	}

	AcDbObjectIterator * pObjIter = pBlkRef->attributeIterator();
	if (pObjIter == NULL) 
	{
		CompleteEditorCommand(); return ;
	}

	CString strPrjCode,strLXLX;
	for ( pObjIter->start(); !pObjIter->done(); pObjIter->step() )
	{
		AcDbObject * pEnt = NULL;
		AcDbObjectId IdTmp = pObjIter->objectId();
		if (acdbOpenObject(pEnt, IdTmp, AcDb::kForRead)!=Acad::eOk) continue;
		pEnt->close();

		AcDbAttributeDefinition * pAttr = NULL;
		pAttr = (AcDbAttributeDefinition*)pEnt;
		if (pAttr == NULL) continue;

		//ARX 2005
		//CString strTemp = pAttr->tagConst();
		//ARX 2004
		CString strTemp = pAttr->tag();
		if(strTemp.CompareNoCase("��Ŀ��ʾ���")==0)
		{
			//ARX 2005
			//strPrjCode = pAttr->textStringConst();
			//ARX 2004
			strPrjCode  = pAttr->textString();
		}
		if(strTemp.CompareNoCase("��������")==0)
		{
			//ARX 2005
			//strLXLX = pAttr->textStringConst();
			//ARX 2004
			strLXLX = pAttr->textString();
		}
	}
	pBlkRef->close();

	if(strLXLX.CompareNoCase("ѡַ")!= 0)
	{
		acutPrintf("\n��ѡ��ѡַ��Ŀ!"); CompleteEditorCommand(); return;
	}

	CString strSdeLyName;
	CDistXData tempXData(ObjId,"ROWID_OBJS");
	tempXData.Select("SDELYNAME",strSdeLyName);
	CompleteEditorCommand();

	strSdeLyName = strSdeLyName.Left(strSdeLyName.GetLength()-1);
	strSdeLyName = strSdeLyName+"OLYGON";

	int nRcdNum = 0;

	char** pStrDkInfoArray = NULL;
	if(1 == gReadShapeInfoFromSDE(&pStrDkInfoArray,&nRcdNum,m_pConnect,strSdeLyName.GetBuffer(0),
		                          m_strCadLyNameDK.GetBuffer(0),strPrjCode.GetBuffer(0)))
	{
		m_list_ydxx.DeleteAllItems();
		for(int k=0; k<nRcdNum; k++)
		{
            m_list_ydxx.InsertItem(k," ");
			int nPos = 0,nRowNum = 0,nFind = 0;
			CString strTempBuf = pStrDkInfoArray[k];
			while((nFind=strTempBuf.Find('@',nPos))>0)
			{
				CString strTemp = strTempBuf.Mid(nPos,nFind-nPos);
				m_list_ydxx.SetItemText(k,nRowNum,strTemp);
				nRowNum++;
				nPos = nFind+1;
			}
		}
	}
}


void CDlgLXInput::OnBnClickedSelectPT()
{
	//����ѡ��ʵ��
	BeginEditorCommand();

	ads_point ptResult;
	if(RTNORM != acedGetPoint(NULL,"�ؿ�����ʾλ�õ�",ptResult))
	{
		CompleteEditorCommand(); return;
	}

	AcGePoint3d pt(ptResult[0],ptResult[1],0);
	CString strTempX,strTempY;
	strTempX.Format("%0.3f",pt.x);
	((CEdit*)GetDlgItem(IDC_EDIT_X))->SetWindowText(strTempX);
	strTempY.Format("%0.3f",pt.y);
	((CEdit*)GetDlgItem(IDC_EDIT_Y))->SetWindowText(strTempY);
	CompleteEditorCommand();

	int nSelect = m_list_ydxx.GetSelectionMark();
	if(nSelect < 0) return;
	CEdit* pEdt = (CEdit*)GetDlgItem(IDC_EDIT_X);
	m_list_ydxx.SetItemText(nSelect,5,strTempX);	
	m_list_ydxx.SetItemText(nSelect,6,strTempY);



}

void CDlgLXInput::OnBnClickedSelectPT_T()
{
	//����ѡ��ʵ��
	BeginEditorCommand();

	ads_point ptResult;
	if(RTNORM != acedGetPoint(NULL,"��Ŀ��Ϣ��ʾλ�õ�",ptResult))
	{
		acutPrintf("\n������ȡ��");
		CompleteEditorCommand(); return;
	}

	AcGePoint3d pt(ptResult[0],ptResult[1],0);
	CString strTemp;
	strTemp.Format("%0.3f",pt.x);
	((CEdit*)GetDlgItem(IDC_EDIT_X_T))->SetWindowText(strTemp);
	strTemp.Format("%0.3f",pt.y);
	((CEdit*)GetDlgItem(IDC_EDIT_Y_T))->SetWindowText(strTemp);

	CompleteEditorCommand();

}

void CDlgLXInput::OnBnClickedBtnPos()
{
	int nSelect = m_list_ydxx.GetSelectionMark();
	if(nSelect < 0) return;

	CString strTemp = m_list_ydxx.GetItemText(nSelect,8);

	//��ʵ��
	AcDbObjectId tempId;
	tempId.setFromOldId(atoi(strTemp));
	AcDbPolyline* pline = NULL;
	if(Acad::eOk != acdbOpenObject(pline,tempId,AcDb::kForRead)) return;
    
	//ȡʵ��߽�
	AcDbExtents Ext;
	pline->getGeomExtents(Ext);
	pline->close();//������ʾ  pline->highlight();

	AcGePoint3d ptMax = Ext.maxPoint();
	AcGePoint3d ptMin = Ext.minPoint();
	
	//�����ӿ�
	CDistEntityTool tempTool;
	tempTool.DyZoom(AcGePoint2d((ptMax.x+ptMin.x)/2,(ptMax.y+ptMin.y)/2),ptMax.x-ptMin.y+50,ptMax.y-ptMin.y+50);

}

void CDlgLXInput::SetValueToControls()
{
	int nSelect = m_list_ydxx.GetSelectionMark();
	if(nSelect < 0) return;
	
	((CComboBox*)GetDlgItem(IDC_CMB_DKBH))->SetWindowText(m_list_ydxx.GetItemText(nSelect,0));  //�ؿ���	
	((CComboBox*)GetDlgItem(IDC_CMB_YDLX))->SelectString(0,m_list_ydxx.GetItemText(nSelect,1)); //�õ�����
	((CEdit*)GetDlgItem(IDC_EDIT_ZMJ))->SetWindowText(m_list_ydxx.GetItemText(nSelect,2));      //�õ����
	((CComboBox*)GetDlgItem(IDC_CMB_ZG))->SetWindowText(m_list_ydxx.GetItemText(nSelect,3));    //�ָ�
	((CComboBox*)GetDlgItem(IDC_CMB_XK))->SetWindowText(m_list_ydxx.GetItemText(nSelect,4));    //�߿�
	((CEdit*)GetDlgItem(IDC_EDIT_X))->SetWindowText(m_list_ydxx.GetItemText(nSelect,5));        //��עXֵ
	((CEdit*)GetDlgItem(IDC_EDIT_Y))->SetWindowText(m_list_ydxx.GetItemText(nSelect,6));        //��עYֵ
	((CEdit*)GetDlgItem(IDC_EDIT_BZ))->SetWindowText(m_list_ydxx.GetItemText(nSelect,7));       //��ע
	((CEdit*)GetDlgItem(IDC_EDIT_OBJ))->SetWindowText(m_list_ydxx.GetItemText(nSelect,8));      //ʵ��ID
}

BOOL CDlgLXInput::GetValueFromControls(DIST_CADENTITY_STRUCT& Value)
{
	((CEdit*)GetDlgItem(IDC_EDIT_ZMJ))->GetWindowText(Value.strArea);//�õ����
	Value.strArea.TrimLeft(); Value.strArea.TrimRight();
	if(Value.strArea.IsEmpty())
	{
		AfxMessageBox("��ѡ��ؿ�ʵ��!"); return FALSE;
	}
	int nSelect = ((CComboBox*)GetDlgItem(IDC_CMB_YDLX))->GetCurSel();
	if(nSelect >=0) 
		((CComboBox*)GetDlgItem(IDC_CMB_YDLX))->GetLBText(nSelect,Value.strYDType);//�õ�����
	else
	{
		AfxMessageBox("�õ����Ͳ���Ϊ��!"); return FALSE;
	}

	((CComboBox*)GetDlgItem(IDC_CMB_DKBH))->GetWindowText(Value.strDKCode); //�ؿ���
	Value.strDKCode.TrimLeft(); Value.strDKCode.TrimRight();
	if(Value.strDKCode.IsEmpty())
	{
		AfxMessageBox("�ؿ��Ų���Ϊ��!"); return FALSE;
	}

	((CComboBox*)GetDlgItem(IDC_CMB_ZG))->GetWindowText(Value.strTxtHeight);//�ָ�
	if(Value.strTxtHeight.IsEmpty())
	{
		((CComboBox*)GetDlgItem(IDC_CMB_ZG_T))->GetWindowText(Value.strTxtHeight);
		if(Value.strTxtHeight.IsEmpty())
		{
			AfxMessageBox("�ָ߲���Ϊ��!"); return FALSE;
		}	
	}

	((CComboBox*)GetDlgItem(IDC_CMB_XK))->GetWindowText(Value.strLnWidth); //�߿�
	if(Value.strLnWidth.IsEmpty())
	{
		AfxMessageBox("�߿���Ϊ��!"); return FALSE;
	}

	((CEdit*)GetDlgItem(IDC_EDIT_X))->GetWindowText(Value.strX); //��עXֵ
	Value.strX.TrimLeft(); Value.strX.TrimRight();
	if(Value.strX.IsEmpty())
	{
		AfxMessageBox("��ѡ��ؿ��עλ��!"); return FALSE;
	}
	((CEdit*)GetDlgItem(IDC_EDIT_Y))->GetWindowText(Value.strY); //��עYֵ
	((CEdit*)GetDlgItem(IDC_EDIT_BZ))->GetWindowText(Value.strBZ); //��ע
	((CEdit*)GetDlgItem(IDC_EDIT_OBJ))->GetWindowText(Value.strObjId); //ʵ��ID

	return TRUE;
}



void CDlgLXInput::SetValueToXData(AcDbObjectId& ObjId,const char* strRegName,DIST_CADENTITY_STRUCT& Value)
{
	CStringArray strValueArray;
	strValueArray.RemoveAll();
	strValueArray.Add(m_PrjBaseInfo.strPROJECT_CODE);  //��Ŀ��ʾ���
	strValueArray.Add(m_PrjBaseInfo.strLICENCE_CODE);  //��֤���
	strValueArray.Add(m_PrjBaseInfo.strREG_TIME);      //�Ǽ�����
	strValueArray.Add(m_PrjBaseInfo.strPROJECT_NAME);  //��Ŀ����
	strValueArray.Add(m_PrjBaseInfo.strBUILD_UNIT);    //���赥λ
	strValueArray.Add(m_PrjBaseInfo.strUSELAND_TYPEID);//�õ����ʱ��
	strValueArray.Add(m_PrjBaseInfo.strCADTYPE);       //��������
	strValueArray.Add(m_PrjBaseInfo.strJBRY);          //������Ա

	strValueArray.Add(Value.strDKCode);                //�ؿ���  
	strValueArray.Add(Value.strYDType);                //�õ�����
	strValueArray.Add(Value.strArea);                  //�õ����
	strValueArray.Add(Value.strTxtHeight);             //�ָ�
	strValueArray.Add(Value.strLnWidth);               //�߿�
	strValueArray.Add(Value.strX);                     //Xֵ
	strValueArray.Add(Value.strY);                     //Yֵ
	strValueArray.Add(Value.strBZ);                    //��ע


	AcDbEntity* pEnt = NULL;
	if(Acad::eOk == acdbOpenObject(pEnt,ObjId,AcDb::kForWrite))
	{
		CDistXData tempXData(pEnt,strRegName);
		tempXData.Add(m_strNameArrayDK,strValueArray);
		tempXData.Update();
		tempXData.Close();
		pEnt->close();
	}
}


//���
void CDlgLXInput::OnBnClickedBtnAdd()
{
	//�ӱ༭�ؼ��ж�ȡ����
	DIST_CADENTITY_STRUCT Value;
	if(GetValueFromControls(Value)==FALSE) return;

	//��������
	int nCountNew = m_NewEntityArray.GetCount();
	for(int qq=0; qq<nCountNew; qq++)
	{
		DIST_CADENTITY_STRUCT tempData = m_NewEntityArray.GetAt(qq);
		if(tempData.strObjId.CompareNoCase(Value.strObjId)==0)
		{
			acutPrintf("\n��ʵ���Ѵ���,���ʧ��!"); return;
		}
	}
	m_NewEntityArray.Add(Value);

	//��ӵ��б�
	int nCount = m_list_ydxx.GetItemCount();
	m_list_ydxx.InsertItem(nCount,Value.strDKCode);         //�ؿ���
	m_list_ydxx.SetItemText(nCount,1,Value.strYDType);      //�õ�����
	m_list_ydxx.SetItemText(nCount,2,Value.strArea);        //�õ����
	m_list_ydxx.SetItemText(nCount,3,Value.strTxtHeight);   //�ָ�
	m_list_ydxx.SetItemText(nCount,4,Value.strLnWidth);     //�߿�
	m_list_ydxx.SetItemText(nCount,5,Value.strX);           //X ֵ
	m_list_ydxx.SetItemText(nCount,6,Value.strY);           //Y ֵ
	m_list_ydxx.SetItemText(nCount,7,Value.strBZ);          //��ע
	m_list_ydxx.SetItemText(nCount,8,Value.strObjId);       //ʵ��ID


	//����ʵ��ID�޸��߿�������ʾ
	AcDbObjectId tempId;
	tempId.setFromOldId(atoi(Value.strObjId));
	double dW = atof(Value.strLnWidth);
	AcDbPolyline * pline = NULL;
	if(Acad::eOk == acdbOpenObject(pline,tempId,AcDb::kForWrite))
	{
		int nNumVert = pline->numVerts(); //��ȡ������
		for(int i=0; i<nNumVert; i++)
			pline->setWidthsAt(i,dW,dW);
		pline->close();
	}


	//���
	((CEdit*)GetDlgItem(IDC_EDIT_OBJ))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_EDIT_ZMJ))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_EDIT_X))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_EDIT_Y))->SetWindowText("");
	((CComboBox*)GetDlgItem(IDC_CMB_DKBH))->SetWindowText("");
	((CComboBox*)GetDlgItem(IDC_CMB_ZG))->SetWindowText("8");    //�ָ�
	((CComboBox*)GetDlgItem(IDC_CMB_XK))->SetWindowText("1.0");    //�߿�
	((CComboBox*)GetDlgItem(IDC_EDIT_BZ))->SetWindowText("û�б�ע!"); //��ע


	m_bIsModify = TRUE;
	m_list_ydxx.SetSelectionMark(-1);
}


//�޸�
void CDlgLXInput::OnBnClickedBtnModify()
{
	int nSelect = m_list_ydxx.GetSelectionMark();
	if(nSelect < 0) return;


	//�ӱ༭�ؼ��ж�ȡ����
	DIST_CADENTITY_STRUCT Value;
	if(GetValueFromControls(Value)==FALSE) return;


	int nCountNew = m_NewEntityArray.GetCount();
	
	m_NewEntityArray.SetAt(nSelect,Value);  //�޸�


	//�޸��б�ֵ
	m_list_ydxx.SetItemText(nSelect,0,Value.strDKCode);    //�ؿ���
	m_list_ydxx.SetItemText(nSelect,1,Value.strYDType);    //�õ�����
	m_list_ydxx.SetItemText(nSelect,2,Value.strArea);      //�õ����
	m_list_ydxx.SetItemText(nSelect,3,Value.strTxtHeight); //�ָ�
	m_list_ydxx.SetItemText(nSelect,4,Value.strLnWidth);   //�߿�
	m_list_ydxx.SetItemText(nSelect,5,Value.strX);         //X ֵ
	m_list_ydxx.SetItemText(nSelect,6,Value.strY);         //Y ֵ
	m_list_ydxx.SetItemText(nSelect,7,Value.strBZ);        //��ע
	m_list_ydxx.SetItemText(nSelect,8,Value.strObjId);     //ʵ��ID

	//����ʵ��ID�޸��߿�������ʾ
	AcDbObjectId tempId;
	tempId.setFromOldId(atoi(Value.strObjId));
	double dW = atof(Value.strLnWidth);
	AcDbPolyline * pline = NULL;
	if(Acad::eOk == acdbOpenObject(pline,tempId,AcDb::kForWrite))
	{
		int nNumVert = pline->numVerts(); //��ȡ������
		for(int i=0; i<nNumVert; i++)
			pline->setWidthsAt(i,dW,dW);
		pline->close();
	}

	//���
	((CEdit*)GetDlgItem(IDC_EDIT_OBJ))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_EDIT_ZMJ))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_EDIT_X))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_EDIT_Y))->SetWindowText("");
	((CComboBox*)GetDlgItem(IDC_CMB_DKBH))->SetWindowText("");
	((CComboBox*)GetDlgItem(IDC_CMB_ZG))->SetWindowText("8");    //�ָ�
	((CComboBox*)GetDlgItem(IDC_CMB_XK))->SetWindowText("1.0");    //�߿�
	((CComboBox*)GetDlgItem(IDC_EDIT_BZ))->SetWindowText("û�б�ע!"); //��ע

	m_bIsModify = TRUE;
	m_list_ydxx.SetSelectionMark(-1);

}


//ɾ��
void CDlgLXInput::OnBnClickedBtnDelete()
{
	int nSelect = m_list_ydxx.GetSelectionMark();
	if(nSelect < 0) return;

	m_NewEntityArray.RemoveAt(nSelect);

	CString strDKBH = m_list_ydxx.GetItemText(nSelect,0);
	CString strObjId = m_list_ydxx.GetItemText(nSelect,8);
	m_list_ydxx.DeleteItem(nSelect);
	m_list_ydxx.SetSelectionMark(-1);
	
/*
	int nId = atoi(strObjId);
	AcDbObjectId tempId;
	tempId.setFromOldId(nId);

	char strRegName[255]={0};
	sprintf(strRegName,"DIST_%s",m_strSdeLyNameDK.GetBuffer(0));
	CDistXData tempXData(tempId,strRegName);
	tempXData.DeleteAllRecord();
	tempXData.Close();

	DeleteBlkAttRef("_�ؿ�����",strDKBH);
*/
	//���
	((CEdit*)GetDlgItem(IDC_EDIT_OBJ))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_EDIT_ZMJ))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_EDIT_X))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_EDIT_Y))->SetWindowText("");
	((CComboBox*)GetDlgItem(IDC_CMB_DKBH))->SetWindowText("");
	((CComboBox*)GetDlgItem(IDC_CMB_ZG))->SetWindowText("8");    //�ָ�
	((CComboBox*)GetDlgItem(IDC_CMB_XK))->SetWindowText("0.0");    //�߿�
	((CComboBox*)GetDlgItem(IDC_EDIT_BZ))->SetWindowText("û�б�ע!"); //��ע




	m_bIsModify = TRUE;
	m_list_ydxx.SetSelectionMark(-1);
}


BOOL CDlgLXInput::StringToDate(DIST_DATETIME_X* TempDate,char* strDateBuf)
{
	if(strDateBuf == NULL) return FALSE;

	char strData[100]={0};
	memset(strData,0,sizeof(char)*100);
	strcpy(strData,strDateBuf);
	int nLen = strlen(strData);
	if(nLen < 8 || nLen > 10) return FALSE;
	int nPos =0;
	int nNum = 0;
	char strBuf[20]={0};
	memset(strBuf,0,sizeof(char)*20);
	int nValue = 0;
	for(int i=0; i<nLen; i++)
	{
		if((strData[i] == '-') &&(nNum == 0))
		{
			try
			{
				nValue = atoi(strBuf);
				if (nValue<=2000 || nValue > 2100) 
					nValue = 2008;

				TempDate->tm_year = nValue-1900;
			}
			catch (CException* e)
			{
				TempDate->tm_year = 2008-1900;
			}
			
			nNum++; nPos = 0;
			memset(strBuf,0,sizeof(char)*20);
			continue;	
		}
		else if((strData[i] == '-') && (nNum == 1))
		{
			try
			{
				nValue = atoi(strBuf);
				if (nValue<=0 || nValue>12)   //ȡֵ��Χ 0-11
					nValue = 1;
				TempDate->tm_mon = nValue-1;
			}
			catch (CException* e)
			{
				TempDate->tm_mon = 0;
			}
			nNum++; nPos = 0;
			memset(strBuf,0,sizeof(char)*20);
			continue;	
		}
		else
		{
			strBuf[nPos] = strData[i];
			nPos++;
		}
	}

	if(nPos > 0)
	{
		try
		{
			nValue = atoi(strBuf);
			if (nValue<=0 || nValue>31) 
				nValue = 1;
			TempDate->tm_mday = nValue;
		}
		catch (CException* e)
		{
			TempDate->tm_mday = 1;
		}
	}	
	return TRUE;
}


void CDlgLXInput::OnBnClickedBtnSde()
{
	//�Ϸ��Լ��
	if(NULL == m_pConnect)
	{
		acutPrintf("\n�ռ����ݿ�û�����ӣ�"); return;
	}
	if(m_pConnect->CheckConnect(60)!=1)
	{
		acutPrintf("\n�ռ����ݿ�����ʧ�ܣ�");  return;
	}

	int nCount = m_list_ydxx.GetItemCount();
	if(nCount == 0)
	{
		BOOL bFound = IsXMDKInfoExists();
		if(bFound == TRUE)
		{
			if(MessageBox("��ȷ��Ҫɾ���ռ���ж�Ӧ����Ŀ��Ϣ��","��Ϣ��ʾ",MB_YESNO) != IDYES) return;
			ClearOldInfo();
			DeleteXMDKINFO();
			m_bIsModify = FALSE;
			return;
		}
		else return;
	}

	//ȡ��Ŀ��Ϣ��עλ��
	CEdit* pEdt = (CEdit*)GetDlgItem(IDC_EDIT_X_T);
	CString strTempValue;
	pEdt->GetWindowText(strTempValue);
	strTempValue.Trim();
	if(strTempValue.IsEmpty())
	{
		MessageBox("��ʰȡ��Ŀ��Ϣ��ע��λ��!"); return;
	}
	ads_point pt;
	pt[0] = atof(strTempValue);
	pEdt = (CEdit*)GetDlgItem(IDC_EDIT_Y_T);
	pEdt->GetWindowText(strTempValue);
	pt[1] = atof(strTempValue);

	//ȡ��Ŀ��Ϣ��ע���ָ�
	CComboBox* pCmb = (CComboBox*)GetDlgItem(IDC_CMB_ZG_T);
	pCmb->GetWindowText(strTempValue);
	strTempValue.Trim();
	if(strTempValue.IsEmpty())
	{
		MessageBox("��ѡ����Ŀ��Ϣ��ע����ĸ߶�!"); return;
	}
	int nHeight = 8;
	try
	{
		nHeight = atoi(strTempValue);
	}
	catch (...)
	{
		nHeight = 8;
	}
	

	//ȷ���ύ
	if(MessageBox("��ȷ��Ҫ�ύ���еؿ����ݵ�SDE�ռ����","��Ϣ��ʾ",MB_YESNO) != IDYES) return;

	//ɾ���Ѵ��ڼ�¼
	IDistCommand* pCommand = CreateSDECommandObjcet();
	if(NULL == pCommand) return;
	pCommand->SetConnectObj(m_pConnect);

	const int MAXLEN = 33; //��������31��,��1��SHAPE
	char** strFldValueArray = new char*[MAXLEN];
	for(int i=0;i<MAXLEN;i++)
	{
		strFldValueArray[i] = new char[200];
		memset(strFldValueArray[i],0,sizeof(char)*200);
	}

	//��Ŀ������Ϣ 
	strcpy(strFldValueArray[0],m_PrjBaseInfo.strPROJECT_ID.GetBuffer(0));             //��Ŀ���
	strcpy(strFldValueArray[1],m_PrjBaseInfo.strPROJECT_CODE.GetBuffer(0));           //��Ŀ��ʾ���
	strcpy(strFldValueArray[2],m_PrjBaseInfo.strRELATION_CODE.GetBuffer(0));          //�������
	//strcpy(strFldValueArray[3],m_PrjBaseInfo.strREG_TIME.GetBuffer(0));               //�Ǽ�ʱ��
	strcpy(strFldValueArray[4],m_PrjBaseInfo.strPROJECT_NAME.GetBuffer(0));           //��Ŀ����
	strcpy(strFldValueArray[5],m_PrjBaseInfo.strPRE_PROJECT_ID.GetBuffer(0));         //��һҵ����ĿĿ���
	strcpy(strFldValueArray[6],m_PrjBaseInfo.strBUILD_UNIT.GetBuffer(0));             //���赥λ
	strcpy(strFldValueArray[7],m_PrjBaseInfo.strBUILD_UNIT_LINKMAN.GetBuffer(0));     //��ϵ��
	strcpy(strFldValueArray[8],m_PrjBaseInfo.strBUILD_UNIT_PHONE.GetBuffer(0));       //�绰
	//strcpy(strFldValueArray[9],m_PrjBaseInfo.strOVER_TIME.GetBuffer(0));              //�᰸ʱ��
	strcpy(strFldValueArray[10],m_PrjBaseInfo.strBUILD_ADDRESS.GetBuffer(0));         //����ص�
	strcpy(strFldValueArray[11],m_PrjBaseInfo.strLICENCE_CODE.GetBuffer(0));          //��֤���
	strcpy(strFldValueArray[12],m_PrjBaseInfo.strUSELAND_TYPE.GetBuffer(0));          //�õ�����
	strcpy(strFldValueArray[13],m_PrjBaseInfo.strUSELAND_TYPEID.GetBuffer(0));        //�õ����ʱ��
	strcpy(strFldValueArray[14],m_PrjBaseInfo.strARCHIITECTUREPRO_TYPE.GetBuffer(0)); //���蹤������
	strcpy(strFldValueArray[15],m_PrjBaseInfo.strHOURSEARCHI_TYPE.GetBuffer(0));      //��ס��������
	
	m_PrjBaseInfo.strINVEST_SUM.Trim();
	if(m_PrjBaseInfo.strINVEST_SUM.IsEmpty())
		m_PrjBaseInfo.strINVEST_SUM="0.000";
	strcpy(strFldValueArray[16],m_PrjBaseInfo.strINVEST_SUM.GetBuffer(0));            //Ͷ���ܹ�ģ
	strcpy(strFldValueArray[17],m_PrjBaseInfo.strFLOOR_AREA_RATIO.GetBuffer(0));      //�ݻ���

	strcpy(strFldValueArray[18],m_PrjBaseInfo.strBUILDING_DENSITY.GetBuffer(0));      //�����ܶ�
	strcpy(strFldValueArray[19],m_PrjBaseInfo.strPRO_STATUS.GetBuffer(0));            //��Ŀ״̬

	CString strTempDateReg  = m_PrjBaseInfo.strREG_TIME;
	CString strTempDateOver = m_PrjBaseInfo.strOVER_TIME;
	
	DIST_DATETIME_X tempDateReg,tempDateOver;
	memset(&tempDateReg,0,sizeof(DIST_DATETIME_X));
	memset(&tempDateOver,0,sizeof(DIST_DATETIME_X));

	char strDataBuf[50]={0};
	strTempDateReg.Trim(); strTempDateOver.Trim();
	memset(strFldValueArray[3],0,sizeof(char)*200);
	if(!strTempDateReg.IsEmpty())
	{
		memset(strDataBuf,0,sizeof(char)*50);
		strcpy(strDataBuf,strTempDateReg.GetBuffer(0));
		if(TRUE == StringToDate(&tempDateReg,strDataBuf))
		{
			CString strValue;
			strValue.Format("%d",int(&tempDateReg));
			strcpy(strFldValueArray[3],strValue.GetBuffer(0));
		}
	}

	memset(strFldValueArray[9],0,sizeof(char)*200);
	if(!strTempDateOver.IsEmpty())
	{
		memset(strDataBuf,0,sizeof(char)*50);
		strcpy(strDataBuf,strTempDateReg.GetBuffer(0));
		if(TRUE == StringToDate(&tempDateOver,strDataBuf))
		{
			CString strValue;
			strValue.Format("%d",int(&tempDateOver));
			strcpy(strFldValueArray[9],strValue.GetBuffer(0));
		}
	}

	pCommand->BeginTrans();

	//ɾ���ɵؿ��¼
	char strSQL[512]={0};
	sprintf(strSQL,"T:%s,W:PROJECT_ID='%s',##",m_strSdeLyNameDK,m_PrjBaseInfo.strPROJECT_ID);
	if(1 != pCommand->DeleteData(strSQL))
	{
		for(i=0; i<MAXLEN; i++)
			delete[] strFldValueArray[i];
		delete[] strFldValueArray;

		AfxMessageBox("\n�ύ����ʧ�ܣ�");
		pCommand->RollbackTrans(); pCommand->Release(); return;
	}

	sprintf(strSQL,"T:%s,W:PROJECT_ID='%s',##",m_strSdeLyNameXM,m_PrjBaseInfo.strPROJECT_ID);
	if(1 != pCommand->DeleteData(strSQL))
	{
		for(i=0; i<MAXLEN; i++)
			delete[] strFldValueArray[i];
		delete[] strFldValueArray;

		AfxMessageBox("\n�ύ����ʧ�ܣ�");
		pCommand->RollbackTrans(); pCommand->Release();return;
	}

	//����¼�¼
	sprintf(strSQL,"F:PROJECT_ID,"             //��Ŀ���          0
		           "PROJECT_CODE,"			   //��Ŀ��ʾ���      1
				   "RELATION_CODE,"			   //�������          2
				   "REG_TIME,"				   //�Ǽ�ʱ��          3  *
				   "PROJECT_NAME,"			   //��Ŀ����          4
				   "PRE_PROJECT_ID,"		   //��һҵ����ĿĿ��  5
		           "BUILD_UNIT,"			   //���赥λ          6
				   "BUILD_UNIT_LINKMAN,"	   //��ϵ��            7
				   "BUILD_UNIT_PHONE,"		   //�绰              8
				   "OVER_TIME,"				   //�᰸ʱ��          9  *
				   "BUILD_ADDRESS,"			   //����ص�          10
		           "LICENCE_CODE,"			   //��֤���          11
				   "USELAND_TYPE,"			   //�õ�����          12
				   "USELAND_TYPEID,"		   //�õ����ʱ��      13
				   "ARCHIITECTUREPRO_TYPE,"	   //���蹤������      14
				   "HOURSEARCHI_TYPE,"		   //��ס��������      15
				   "INVEST_SUM,"               //Ͷ���ܹ�ģ        16 *
				   "FLOOR_AREA_RATIO,"	       //�ݻ���            17
				   "BUILDING_DENSITY,"	       //�����ܶ�          18
				   "PRO_STATUS,"		       //��Ŀ״̬          19
				   "DKBH,"		  //�ؿ���	20	
				   "YDLX,"		  //�õ�����    21
				   "YDMJ,"		  //�õ����    22  *
				   "ZG,"		  //�ָ�");     23
				   "XK,"		  //�߿�");     24
				   "X,"			  //Xֵ");      25
				   "Y,"			  //Yֵ");      26
				   "BZ,"		  //��ע");     27
				   "JBRY,"		  //������Ա    28
				   "SHAPE,"
                   "T:%s,##",m_strSdeLyNameDK);

	ClearOldInfo();
	nCount = m_list_ydxx.GetItemCount();
	for(int j=0; j<nCount; j++)
	{
		//�ؿ���Ϣ
		CString strTemp;  //������Ϣ
		strcpy(strFldValueArray[20],m_list_ydxx.GetItemText(j,0).GetBuffer(0));
		strcpy(strFldValueArray[21],m_list_ydxx.GetItemText(j,1).GetBuffer(0));
		CString strYDMJ = m_list_ydxx.GetItemText(j,2);
		double dYDMJ = atof(strYDMJ);
		double dTemp = dYDMJ;
		MyRound(dTemp,3);
		strTemp.Format("%0.3f",dTemp);
		strYDMJ = strTemp;
		strcpy(strFldValueArray[22],strYDMJ.GetBuffer(0));
		strcpy(strFldValueArray[23],m_list_ydxx.GetItemText(j,3).GetBuffer(0));
		strcpy(strFldValueArray[24],m_list_ydxx.GetItemText(j,4).GetBuffer(0));
		strcpy(strFldValueArray[25],m_list_ydxx.GetItemText(j,5).GetBuffer(0));
		strcpy(strFldValueArray[26],m_list_ydxx.GetItemText(j,6).GetBuffer(0));
		strcpy(strFldValueArray[27],m_list_ydxx.GetItemText(j,7).GetBuffer(0));
		strcpy(strFldValueArray[28],m_PrjBaseInfo.strJBRY.GetBuffer(0));

		//������Ϣ
		AcDbObjectId tempId;
		strTemp = m_list_ydxx.GetItemText(j,8);  //ȡʵ��ID
		tempId.setFromOldId(atoi(strTemp));

		DIST_CADENTITY_STRUCT Value;
		Value.strDKCode= strFldValueArray[20];     
		Value.strYDType= strFldValueArray[21];     
		Value.strArea= strFldValueArray[22];       
		Value.strTxtHeight= strFldValueArray[23];  
		Value.strLnWidth= strFldValueArray[24];    
		Value.strX= strFldValueArray[25];          
		Value.strY= strFldValueArray[26];          
		Value.strBZ= strFldValueArray[27];         
		Value.strObjId = strTemp;  

		char strRegName[255]={0};
		sprintf(strRegName,"DIST_%s",m_strSdeLyNameDK.GetBuffer(0));
		SetValueToXData(tempId,strRegName,Value);

		CDistEntityTool tempTool;
		AcGePoint3dArray ptArray;
		tempTool.GetPolylinePts(tempId,ptArray);
		int nNum = ptArray.length();
		DIST_POINT_STRUCT * pts = new DIST_POINT_STRUCT[nNum];
		for(i=0; i<nNum; i++)
		{
			pts[i].x = ptArray[i].x; pts[i].y = ptArray[i].y;
		}
		
		if(1 != gSaveOneLXToSDE(m_pConnect,strSQL,m_strSdeLyNameDK,strFldValueArray,29+1,pts,nNum))
		{
			for(i=0; i<MAXLEN; i++)
				delete[] strFldValueArray[i];
			delete[] strFldValueArray;

			AfxMessageBox("\n�ύ����ʧ�ܣ�");
			pCommand->RollbackTrans(); pCommand->Release(); return;
		}

		double dH = atof(strFldValueArray[23]);
		double dW = atof(strFldValueArray[24]);
		double dX = atof(strFldValueArray[25]);
		double dY = atof(strFldValueArray[26]);

		CStringArray strValueArray;
		strValueArray.Add(m_PrjBaseInfo.strPROJECT_CODE);  //��Ŀ��ʾ���"
		strValueArray.Add(m_PrjBaseInfo.strLICENCE_CODE);  //��֤���
		strValueArray.Add(m_PrjBaseInfo.strREG_TIME);      //�Ǽ�����
		strValueArray.Add(m_PrjBaseInfo.strPROJECT_NAME);  //��Ŀ����
		strValueArray.Add(m_PrjBaseInfo.strBUILD_UNIT);    //���赥λ
		strValueArray.Add(m_PrjBaseInfo.strUSELAND_TYPEID);//�õ����ʱ��
		strValueArray.Add(m_PrjBaseInfo.strCADTYPE);       //��������
		strValueArray.Add(m_PrjBaseInfo.strJBRY);          //������Ա

		strValueArray.Add(strFldValueArray[20]);     //�ؿ���
		strValueArray.Add(strFldValueArray[21]);     //�õ�����
		strValueArray.Add(strFldValueArray[22]);     //�õ����
		strValueArray.Add(strFldValueArray[23]);     //�ָ�
		strValueArray.Add(strFldValueArray[24]);     //�߿�
		strValueArray.Add(strFldValueArray[25]);     //X ֵ
		strValueArray.Add(strFldValueArray[26]);     //Y ֵ
		strValueArray.Add(strFldValueArray[27]);     //��ע


		
		CString strCadLyName;
		strCadLyName.Format("%s�ؿ��ע",m_strCadLyNameDK);
		AcDbObjectId resultId;
		tempTool.InsertBlkAttrRef(resultId,"_�ؿ�����",strCadLyName.GetBuffer(),AcGePoint3d(dX,dY,0),dH,m_strFNameArrayDK,strValueArray);
		CDistXData tempXData(resultId,"ROWID_OBJS");
		tempXData.Add("wang","hui");
		tempXData.Update(); tempXData.Close();

	}

	
	//�������
	double dTotalArea=0.00000;
	double dA1=0.00000,dA2=0.00000,dA3=0.00000,dA4=0.00000;
	CString strTemp;
	for(int i=0; i<nCount;i++)
	{
		strTemp = m_list_ydxx.GetItemText(i,1); //�ؿ���  �����õ�;��·�õ�;�̻��õ�;�����õ�;
		if(strTemp.Find("�����õ�") >=0 )
		{
			strTemp = m_list_ydxx.GetItemText(i,2);
			dA1 = dA1+atof(strTemp.GetBuffer(0));
		}
		else if(strTemp.Find("��·�õ�") >=0 )
		{
			strTemp = m_list_ydxx.GetItemText(i,2);
			dA2 = dA2+atof(strTemp.GetBuffer(0));
		}
		else if(strTemp.Find("�̻��õ�") >=0 )
		{
			strTemp = m_list_ydxx.GetItemText(i,2);
			dA3 = dA3+atof(strTemp.GetBuffer(0));
		}
		else if(strTemp.Find("�����õ�") >=0 )
		{
			strTemp = m_list_ydxx.GetItemText(i,2);
			dA4 = dA4+atof(strTemp.GetBuffer(0));
		}
	}
	dTotalArea = dA1+dA2+dA3+dA4;

	//д��Ŀ��Ϣ
	sprintf(strSQL,"F:PROJECT_ID,"                //��Ŀ���          0
		             "PROJECT_CODE,"			  //��Ŀ��ʾ���      1
					 "RELATION_CODE,"			  //�������          2
					 "REG_TIME,"				  //�Ǽ�ʱ��          3  *
					 "PROJECT_NAME,"			  //��Ŀ����          4
					 "PRE_PROJECT_ID,"			  //��һҵ����ĿĿ��  5
		             "BUILD_UNIT,"				  //���赥λ          6
					 "BUILD_UNIT_LINKMAN,"		  //��ϵ��            7
					 "BUILD_UNIT_PHONE,"		  //�绰              8
					 "OVER_TIME,"				  //�᰸ʱ��          9  *
					 "BUILD_ADDRESS,"			  //����ص�          10
					 "LICENCE_CODE,"			  //��֤���          11
		             "USELAND_TYPE,"			  //�õ�����          12
					 "USELAND_TYPEID,"			  //�õ����ʱ��      13
					 "ARCHIITECTUREPRO_TYPE,"	  //���蹤������      14
					 "HOURSEARCHI_TYPE,"		  //��ס��������      15
					 "INVEST_SUM,"				  //Ͷ���ܹ�ģ        16 *
		             "FLOOR_AREA_RATIO,"		  //�ݻ���            17
					 "BUILDING_DENSITY,"		  //�����ܶ�          18
					 "PRO_STATUS,"				  //��Ŀ״̬          19
					 "TFBH,"                  //ͼ����� 20
					 "SUM_LAND_AREA_SUM,"     //���õ�������ģ�21  *
					 "BUILD_LAND_AREA_SUM,"   //�����õ���������ģ�22*
					 "BUILD_AREA_SUM,"        //������������ģ�23*
				     "JBRY,"   //������Ա24
					 "RKYY,"   //���ԭ��25
					 "JSYDMJ," //�����õ����26*
					 "DLYDMJ," //��·�õ����27*
					 "LHYDMJ," //�̻��õ����28*
					 "QTYDMJ," //�����õ����29*
					 "ZYDMJ,"  //���õ����30*
					 "BZ,"     //��ע31
					 "SHAPE,T:%s,##",m_strSdeLyNameXM);



	
	strcpy(strFldValueArray[20],m_PrjBaseInfo.strTFBH.GetBuffer(0));         //ͼ�����

	m_PrjBaseInfo.strSUM_LAND_AREA_SUM.TrimLeft();
	m_PrjBaseInfo.strSUM_LAND_AREA_SUM.TrimRight();
	if(m_PrjBaseInfo.strSUM_LAND_AREA_SUM.IsEmpty())
		m_PrjBaseInfo.strSUM_LAND_AREA_SUM="0.000";
	strcpy(strFldValueArray[21],m_PrjBaseInfo.strSUM_LAND_AREA_SUM.GetBuffer(0));   //���õ�������ģ�

	m_PrjBaseInfo.strBUILD_LAND_AREA_SUM.TrimLeft();
	m_PrjBaseInfo.strBUILD_LAND_AREA_SUM.TrimRight();
	if(m_PrjBaseInfo.strBUILD_LAND_AREA_SUM.IsEmpty())
		m_PrjBaseInfo.strBUILD_LAND_AREA_SUM="0.000";
	strcpy(strFldValueArray[22],m_PrjBaseInfo.strBUILD_LAND_AREA_SUM.GetBuffer(0)); //�����õ���������ģ�

	m_PrjBaseInfo.strBUILD_AREA_SUM.TrimLeft();
	m_PrjBaseInfo.strBUILD_AREA_SUM.TrimRight();
	if(m_PrjBaseInfo.strBUILD_AREA_SUM.IsEmpty())
		m_PrjBaseInfo.strBUILD_AREA_SUM="0.000";
	strcpy(strFldValueArray[23],m_PrjBaseInfo.strBUILD_AREA_SUM.GetBuffer(0));      //������������ģ�

    strcpy(strFldValueArray[24],m_PrjBaseInfo.strJBRY.GetBuffer(0));         //������Ա
	strcpy(strFldValueArray[25]," ");         //���ԭ��

	CString strJSYDMJ,strZMJ;
	double dTemp = dA1; MyRound(dTemp,3);
	strTemp.Format("%0.3f",dTemp); strJSYDMJ = strTemp;
	strcpy(strFldValueArray[26],strTemp.GetBuffer(0)); //�����õ����

	dTemp = dA2; MyRound(dTemp,3);
	strTemp.Format("%0.3f",dTemp);
	strcpy(strFldValueArray[27],strTemp.GetBuffer(0)); //��·�õ����

	dTemp = dA3; MyRound(dTemp,3);
	strTemp.Format("%0.3f",dTemp);
	strcpy(strFldValueArray[28],strTemp.GetBuffer(0)); //�̻��õ����

	dTemp = dA4; MyRound(dTemp,3);
	strTemp.Format("%0.3f",dTemp);
	strcpy(strFldValueArray[29],strTemp.GetBuffer(0)); //�����õ����

	dTotalArea = dA1+dA2+dA3+dA4;
	dTemp = dTotalArea; MyRound(dTemp,3);
	strTemp.Format("%0.3f",dTemp); strZMJ = strTemp;
	strcpy(strFldValueArray[30],strTemp.GetBuffer(0)); //���õ����
	strcpy(strFldValueArray[31]," "); //��ע




	int nRowId = -1;
	if(1 != gSaveOneXMToSDE(m_pConnect,strSQL,m_strSdeLyNameXM,strFldValueArray,32+1,pt[0],pt[1]))
	{
		for(i=0; i<MAXLEN; i++)
			delete[] strFldValueArray[i];
		delete[] strFldValueArray;

		AfxMessageBox("\n�ύ����ʧ�ܣ�");
		pCommand->RollbackTrans(); pCommand->Release(); return;
	}

	pCommand->CommitTrans(); pCommand->Release();

	double dTempTemp = atof(strJSYDMJ);
	if(fabs(dTempTemp)<0.01) strJSYDMJ = "0.000";
	//��ʱ
	//CallDap(g_DAPHwnd,m_strDapProcessId,m_strURL,strZMJ,strJSYDMJ);

	AfxMessageBox("\n�ύ���ݳɹ���");

	CStringArray strValueArray,strNameArray;
	strValueArray.Add(m_PrjBaseInfo.strPROJECT_CODE);     
	strValueArray.Add(m_PrjBaseInfo.strLICENCE_CODE);   
	strValueArray.Add(m_PrjBaseInfo.strREG_TIME);       
	strValueArray.Add(m_PrjBaseInfo.strPROJECT_NAME);   
	strValueArray.Add(m_PrjBaseInfo.strBUILD_UNIT);     
	strValueArray.Add(m_PrjBaseInfo.strUSELAND_TYPEID); 
	strValueArray.Add(m_PrjBaseInfo.strCADTYPE);                    
	strValueArray.Add(m_PrjBaseInfo.strJBRY);           
	strValueArray.Add(strFldValueArray[26]);            
	strValueArray.Add(strFldValueArray[27]);            
	strValueArray.Add(strFldValueArray[28]);            
	strValueArray.Add(strFldValueArray[29]);            
	strValueArray.Add(strFldValueArray[30]);            

	for(i=0; i<MAXLEN; i++)
		delete[] strFldValueArray[i];
	delete[] strFldValueArray;



	CString strCadLyName;
	CDistEntityTool tempTool;
	AcDbObjectId tempId;
	tempTool.InsertBlkAttrRef(tempId,"_��Ŀ����",m_strCadLyNameXM.GetBuffer(0),AcGePoint3d(pt[0],pt[1],0),nHeight,m_strFNameArrayXM,strValueArray);
	CDistXData tempXData(tempId,"ROWID_OBJS");
	tempXData.Add("wang","hui");
	tempXData.Update(); tempXData.Close();

	m_OldEntityArray.RemoveAll();
	int nnnn = m_list_ydxx.GetItemCount();
	for(int qq=0; qq<nnnn; qq++)
	{
		DIST_CADENTITY_STRUCT temp;
		temp.strDKCode = m_list_ydxx.GetItemText(qq,0);
		temp.strYDType = m_list_ydxx.GetItemText(qq,1);
		temp.strArea = m_list_ydxx.GetItemText(qq,2);
		temp.strTxtHeight = m_list_ydxx.GetItemText(qq,3);
		temp.strLnWidth = m_list_ydxx.GetItemText(qq,4);
		temp.strX = m_list_ydxx.GetItemText(qq,5);
		temp.strY = m_list_ydxx.GetItemText(qq,6);
		temp.strBZ = m_list_ydxx.GetItemText(qq,7);
		temp.strObjId = m_list_ydxx.GetItemText(qq,8);
		m_OldEntityArray.Add(temp);	
	}

	m_bIsModify = FALSE;
}


/*
void CDlgLXInput::CallDap(HWND hDapHwnd,CString strXMBH,CString strURL,CString strZMJ,CString strJSYDMJ)
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
		CString strValue = "";
		CString strBdeData = (char*)((*lpRst)["BDE_DATA"]);
		CString strDescription = (char*)((*lpRst)["DESCRIPTION"]);

		if(strDescription.CompareNoCase("���õ����")==0)
		{
			strValue = strZMJ;
		}
		else if(strDescription.CompareNoCase("�����õ����")==0)
		{
			strValue = strJSYDMJ;
		}

		if(!strValue.IsEmpty())
		{
			sData += strBdeData;
			sData += "=";
			sData += strValue;//GetAttrValue(strDescription);
			sData += ";";
		}

		bOK = lpRst->MoveNext();
	}

	COPYDATASTRUCT cds;
	cds.dwData = 0;
	cds.cbData = sData.GetLength() + 1;
	cds.lpData = (void*)sData.GetBuffer(0);
	::SendMessage(hDapHwnd, WM_COPYDATA, (WPARAM)nCadWnd, (LPARAM)&cds);



	lpRst->Release();
	lpCmd->Release();
	lpConn->Release();
}
*/

void CDlgLXInput::OnBnClickedBtnExit()
{
	if(m_bIsModify == TRUE)
	{
		if(MessageBox("�������޸�,��û���ύ���;�������,���ݽ��ָ��ĳ�ʼ״̬,ȷ�Ϸ�����β�����?","��Ϣ��ʾ",MB_YESNO) == IDNO) return;
		
	}
	CAcUiDialog::OnOK();
}




void CDlgLXInput::OnNMClickListYdxx(NMHDR *pNMHDR, LRESULT *pResult)
{
	SetValueToControls();
	*pResult = 0;
}

void CDlgLXInput::OnCbnSelchangeCmbYdlx()
{
	CComboBox* pCmb = (CComboBox*)GetDlgItem(IDC_CMB_YDLX);
	int nIndex = pCmb->GetCurSel();
	if(nIndex < 0) return;
	CString strYDLX;
	pCmb->GetLBText(nIndex,strYDLX);
	
	pCmb = (CComboBox*)GetDlgItem(IDC_CMB_DKBH);
	pCmb->ResetContent();
	pCmb->SetWindowText("");
	CString strTag;
	strTag.Format("A%d",nIndex+1);
	pCmb->AddString(strTag);

	CString strTemp;
	for(int j=1; j<=20; j++)
	{
		strTemp.Format("%s-%d",strTag,j);
		pCmb->AddString(strTemp);
	} 
	pCmb->SetCurSel(0);

	int nSelect = m_list_ydxx.GetSelectionMark();
	if(nSelect < 0) return;
	m_list_ydxx.SetItemText(nSelect,1,strYDLX);

	pCmb->GetLBText(0,strTemp);
	m_list_ydxx.SetItemText(nSelect,0,strTemp);

    //�ؿ���,�õ�����,�õ����,�ָ�,�߿�,X ֵ,Y ֵ,��ע,ʵ��ID
	

}

BOOL CDlgLXInput::IsXMDKInfoExists()
{
	//ɾ���Ѵ��ڼ�¼
	IDistCommand* pCommand = CreateSDECommandObjcet();
	pCommand->SetConnectObj(m_pConnect);

	//ɾ���ɵؿ��¼
	char strSQL[512]={0};
	sprintf(strSQL,"F:OBJECTID,T:%s,W:PROJECT_ID='%s',##",m_strSdeLyNameDK,m_PrjBaseInfo.strPROJECT_ID);
	IDistRecordSet* pRcdSet = NULL;
	if( 1!= pCommand->SelectData(strSQL,&pRcdSet,NULL))
	{
		pCommand->Release(); return FALSE;
	}
	if(pRcdSet->GetRecordCount()>0)
	{
		pRcdSet->Release(); pCommand->Release(); return TRUE;
	}
	pRcdSet->Release(); pCommand->Release();
	return FALSE;
}


void CDlgLXInput::DeleteXMDKINFO()
{
	//ɾ���Ѵ��ڼ�¼
	IDistCommand* pCommand = CreateSDECommandObjcet();
	if(NULL == pCommand) return;
	pCommand->SetConnectObj(m_pConnect);

	pCommand->BeginTrans();

	//ɾ���ɵؿ��¼
	char strSQL[512]={0};
	sprintf(strSQL,"T:%s,W:PROJECT_ID='%s',##",m_strSdeLyNameDK,m_PrjBaseInfo.strPROJECT_ID);
	if(1 != pCommand->DeleteData(strSQL))
	{
		AfxMessageBox("\nɾ������ʧ�ܣ�");
		pCommand->RollbackTrans(); pCommand->Release(); return;
	}

	sprintf(strSQL,"T:%s,W:PROJECT_ID='%s',##",m_strSdeLyNameXM,m_PrjBaseInfo.strPROJECT_ID);
	if(1 != pCommand->DeleteData(strSQL))
	{
		AfxMessageBox("\nɾ������ʧ�ܣ�");
		pCommand->RollbackTrans(); pCommand->Release();return ;
	}
	pCommand->CommitTrans();

	pCommand->Release();

	AfxMessageBox("\nɾ�����ݳɹ���");
}


void CDlgLXInput::DeleteBlkAttRef(CString strBlkName)
{
	CString strTempCadLyName;
	if(strBlkName.Find("_�ؿ�����")>=0)
	{
		strTempCadLyName.Format("%s�ؿ��ע",m_strCadLyNameDK);
	}
	else if(strBlkName.Find("_��Ŀ����")>=0)
		strTempCadLyName = m_strCadLyNameXM;
	else
		return;

	char strBuf[260]={0};
	strcpy(strBuf,strTempCadLyName.GetBuffer(0));

	struct resbuf fileter;
	fileter.restype = 8;
	fileter.resval.rstring = strBuf;
	fileter.rbnext = NULL;

	CDistSelSet tempSet;
	tempSet.AllSelect(&fileter);

	AcDbObjectIdArray IdArray;
	tempSet.AsArray(IdArray);

	int nCount = IdArray.length();
	for(int k=0; k<nCount;k++)
	{
		AcDbObjectId Id = IdArray.at(k);
		AcDbBlockReference* pRef = NULL;
		if(Acad::eOk != acdbOpenObject(pRef,Id,AcDb::kForWrite)) continue; 
		CString strValue;
		ReadAttrFromRef(strValue,"��Ŀ��ʾ���",pRef); //��ȡ���Կ���Ŀ��ʾ���ֵ

		strValue.TrimLeft(); strValue.TrimRight();

		if(strValue.IsEmpty())
		{
			pRef->close();
		}
		else
		{
			if(strValue.CompareNoCase(m_PrjBaseInfo.strPROJECT_CODE)==0)  //�뵱ǰ��Ŀ���ƱȽ�
			{
				pRef->erase(); pRef->close(); 
			}
			else
			{
				pRef->close(); 
			}
		}
	}
}


//�ӿ������л�ȡָ���������Ե�����
void CDlgLXInput::ReadAttrFromRef(CString& strValue,CString strTag,AcDbBlockReference* pBckRef)
{
	strValue.Empty();

	AcDbObjectIterator * pObjIter = pBckRef->attributeIterator();
	if (pObjIter == NULL)  return ;

	for ( pObjIter->start(); !pObjIter->done(); pObjIter->step() )
	{
		AcDbObject * pEnt = NULL;
		AcDbObjectId IdTmp = pObjIter->objectId();
		Acad::ErrorStatus es;
		if ((es= acdbOpenObject(pEnt, IdTmp, AcDb::kForRead))!=Acad::eOk) continue;
		pEnt->close();

		AcDbAttributeDefinition * pAttr = NULL;
		pAttr = (AcDbAttributeDefinition*)pEnt;
		if (pAttr == NULL) continue;

		//ARX 2005
		//CString strTemp = pAttr->tagConst();
		//ARX 2004
		CString strTemp = pAttr->tag();
		if(strTemp.CompareNoCase(strTag) == 0)
		{
			//ARX 2005
			//strValue = pAttr->textStringConst();
			//ARX 2004
			strValue = pAttr->textString();

			break;
		}
	}
}


void CDlgLXInput::ClearOldInfo()
{
	int nCount = m_OldEntityArray.GetCount();

	//�����չ����
	for(int i=0; i<nCount; i++)
	{
		CString strObjId = m_OldEntityArray.GetAt(i).strObjId;
		int nId = atoi(strObjId);
		AcDbObjectId tempId;
		tempId.setFromOldId(nId);

		char strRegName[255]={0};
		sprintf(strRegName,"DIST_%s",m_strSdeLyNameDK.GetBuffer(0));
		CDistXData tempXData(tempId,strRegName);
		tempXData.DeleteAllRecord();
		tempXData.Close();
	}

	DeleteBlkAttRef("_�ؿ�����");
	DeleteBlkAttRef("_��Ŀ����");

}




void CDlgLXInput::RestoreEntityInfo()
{

	int nCountOld = m_OldEntityArray.GetCount();
	int nCountNew = m_list_ydxx.GetItemCount();


	if(nCountOld == 0) //�½������
	{
		if(nCountNew == 0) return;  //ʲô��û��
		for(int i=0; i<nCountNew; i++)
		{
			CString strObjId = m_list_ydxx.GetItemText(i,8);
			int nId = atoi(strObjId);
			AcDbObjectId tempId;
			tempId.setFromOldId(nId);

			char strRegName[255]={0};
			sprintf(strRegName,"DIST_%s",m_strSdeLyNameDK.GetBuffer(0));
			CDistXData tempXData(tempId,strRegName);
			tempXData.DeleteAllRecord();
			tempXData.Close();
		}
	}
	else  //���½������
	{
		if(nCountNew > 0)  //������е�
		{
			for(int i=0; i<nCountNew; i++)
			{
				CString strObjId = m_list_ydxx.GetItemText(i,8);
				int nId = atoi(strObjId);
				AcDbObjectId tempId;
				tempId.setFromOldId(nId);

				char strRegName[255]={0};
				sprintf(strRegName,"DIST_%s",m_strSdeLyNameDK.GetBuffer(0));
				CDistXData tempXData(tempId,strRegName);
				tempXData.DeleteAllRecord();
				tempXData.Close();
			}
		}

		DIST_CADENTITY_STRUCT temp;
		for(int j=0; j<nCountOld; j++)  //�ָ�ԭ��
		{
			temp = m_OldEntityArray.GetAt(j); 
			AcDbObjectId tempId;
			tempId.setFromOldId(atoi(temp.strObjId));
			char strRegName[255]={0};
			sprintf(strRegName,"DIST_%s",m_strSdeLyNameDK.GetBuffer(0));
			SetValueToXData(tempId,strRegName,temp);
		}
	}
}


void CDlgLXInput::OnBnClickedCheckXm()
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_XM);
	if(m_PrjBaseInfo.strCADTYPE.CompareNoCase("�õ�")!=0) 
	{
		pBtn->SetCheck(0);
		m_sBtnXM.EnableWindow(FALSE);
		AfxMessageBox("��Ŀͼ�θ��ƹ���,ֻΪ�õ�������Ŀ�ṩ!"); 
	}
	else
	{
		pBtn->SetCheck(1);
		m_sBtnXM.EnableWindow(TRUE);
	}
}

void CDlgLXInput::OnCbnSelchangeCmbDkbh()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int nSelect = m_list_ydxx.GetSelectionMark();
	if(nSelect < 0) return;
	CComboBox* pCmb = (CComboBox*)GetDlgItem(IDC_CMB_DKBH);

	int nCurSel = pCmb->GetCurSel();
	if(nCurSel<0) return;
	CString strTemp;
	pCmb->GetLBText(nCurSel,strTemp);	
	m_list_ydxx.SetItemText(nSelect,0,strTemp);
	m_bIsModify = TRUE;

}

void CDlgLXInput::OnCbnSelchangeCmbZg()
{
	 //�ؿ���,�õ�����,�õ����,�ָ�,�߿�,X ֵ,Y ֵ,��ע,ʵ��ID
	int nSelect = m_list_ydxx.GetSelectionMark();
	if(nSelect < 0) return;
	CComboBox* pCmb = (CComboBox*)GetDlgItem(IDC_CMB_ZG);

	int nCurSel = pCmb->GetCurSel();
	if(nCurSel<0) return;
	CString strTemp;
	pCmb->GetLBText(nCurSel,strTemp);	
	m_list_ydxx.SetItemText(nSelect,3,strTemp);
	m_bIsModify = TRUE;
}

void CDlgLXInput::OnCbnSelchangeCmbXk()
{
	//�ؿ���,�õ�����,�õ����,�ָ�,�߿�,X ֵ,Y ֵ,��ע,ʵ��ID
	//  0        1       2        3    4    5    6    7    8
	int nSelect = m_list_ydxx.GetSelectionMark();
	if(nSelect < 0) return;
	CComboBox* pCmb = (CComboBox*)GetDlgItem(IDC_CMB_XK);

	int nCurSel = pCmb->GetCurSel();
	if(nCurSel<0) return;
	CString strTemp;
	pCmb->GetLBText(nCurSel,strTemp);	
	m_list_ydxx.SetItemText(nSelect,4,strTemp);
	m_bIsModify = TRUE;
}

void CDlgLXInput::OnEnKillfocusEditBz()
{
	int nSelect = m_list_ydxx.GetSelectionMark();
	if(nSelect < 0) return;
	CEdit* pEdt = (CEdit*)GetDlgItem(IDC_EDIT_BZ);

	CString strTemp;
	pEdt->GetWindowText(strTemp);
	m_list_ydxx.SetItemText(nSelect,7,strTemp);
	m_bIsModify = TRUE;
}

void CDlgLXInput::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CAcUiDialog::OnActivate(nState, pWndOther, bMinimized);
	
	CEdit* pEdt = (CEdit*) GetDlgItem(IDC_EDIT_X_T);
	CString strTempX,strTempY;
	pEdt->GetWindowText(strTempX);
	strTempX.Trim();
	if(!strTempX.IsEmpty()) return;

	AcGePoint3d pt;
	if(ReadXmPos(pt) == 1)
	{
		strTempX.Format("%0.3f",pt.x);
		((CEdit*)GetDlgItem(IDC_EDIT_X_T))->SetWindowText(strTempX);
		strTempY.Format("%0.3f",pt.y);
		((CEdit*)GetDlgItem(IDC_EDIT_Y_T))->SetWindowText(strTempY);
	}
	else
	{
		/*
		BeginEditorCommand();
		ads_point ptResult;
		if(RTNORM == acedGetPoint(NULL,"�ؿ�����ʾλ�õ�",ptResult))
		{
			strTempX.Format("%0.3f",ptResult[0]);
			((CEdit*)GetDlgItem(IDC_EDIT_X_T))->SetWindowText(strTempX);
			strTempY.Format("%0.3f",ptResult[1]);
			((CEdit*)GetDlgItem(IDC_EDIT_Y_T))->SetWindowText(strTempY);
		}	
		CompleteEditorCommand();
		*/

		//::SendMessage(m_sBtnPT_T.m_hWnd,WM_LBUTTONDOWN,MK_LBUTTON,NULL);//ģ�������   
		//::SendMessage(m_sBtnPT_T.m_hWnd,WM_LBUTTONUP,MK_LBUTTON,NULL);   
	}

}

//���ܣ��жϵ� pt �Ƿ������� ptArr(��β����ͬ) ��
//ʵ�֣��������߷��󽻵������ż���������⣬������������
//������pt ָ���� ptArr �ж�����
//���أ������� TRUE ���� FALSE
BOOL CDlgLXInput::PointIsInPolygon(AcGePoint3d pt, AcGePoint3dArray& ptArr)
{
	//����ڽڵ���,ֱ�ӷ���
	int ptNum = ptArr.length();
	for(int j=0; j<ptNum-1; j++)
	{
		if((fabs(ptArr[j].x-pt.x)<0.01) && (fabs(ptArr[j].y-pt.y)<0.01))
		{
			return false;
		}
	}

	//���߷�
	AcGePoint3d ptA,ptB;
	ads_point pt0,pt1,pt2,ptIns,ptX;
	pt0[0] =  -10000000; pt0[1] = 0.0; pt0[2] = 0.0;
	ptX[0] = pt.x;ptX[1] = pt.y; ptX[2] = pt.z;

	int interNum = 0;
	for(int i = 0;i < ptNum - 1;i++)
	{
		ptA = ptArr.at(i);
		ptB = ptArr.at(i + 1);
		pt1[0] = ptA.x;pt1[1] = ptA.y;pt1[2] = 0.0;
		pt2[0] = ptB.x;pt2[1] = ptB.y;pt2[2] = 0.0;

		if (acdbInters(ptX,pt0,pt1,pt2,1,ptIns) == RTNORM)
		{
			interNum++;
		}
	}

	if (interNum % 2 == 0)
		return false;
	else
		return true;
}


//ע�������պ�(��β����ͬ)
BOOL CDlgLXInput::PolygonIsInPolygon(AcGePoint3dArray& ptsA, AcGePoint3dArray& ptsB)
{
	int nLenA = ptsA.length();
	int nLenB = ptsB.length();
	AcGePoint3d tempPt;
	for(int i=0; i<nLenA-1; i++)
	{
		tempPt = ptsA.at(i);
		if(PointIsInPolygon(tempPt,ptsB)==TRUE)
		{
			return TRUE;
		}
	}
	for(int j=0; j<nLenB-1;j++)
	{
		tempPt = ptsB.at(j);
		if(PointIsInPolygon(tempPt,ptsA)==TRUE)
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CDlgLXInput::ReadCfgLyNameArray(CStringArray& strLyNameArray,CString strTag)
{
	//���ļ�
	CFile file;
	CString strFName;
	strLyNameArray.RemoveAll();
	strFName.Format("%sLayerInfo.ini",gGetCurArxAppPath("shYDsys.arx"));
	if(file.Open(strFName,CFile::modeRead)==FALSE) return FALSE; //���ļ�ʧ��,ֱ���˳�
	int nLen = file.GetLength();
	if(nLen == 0)
	{
		file.Close(); return FALSE;
	}
	nLen++;
	char* strBuf = new char[nLen];
	memset(strBuf,0,sizeof(char)*nLen);
	file.Read(strBuf,nLen);
	file.Close();
	CString strValue = strBuf;
	delete [] strBuf;

	//��ȡ��Ϣ
	int nPos = 0;	
	char chTag[3]={0x0D,0x0A,0};
	int nTagLen = strTag.GetLength();
	while((nPos=strValue.Find(strTag))>=0)
	{
		int nPosDA = strValue.Find(chTag,nPos); //Ѱ�һس�����λ��
		CString strTemp= strValue.Mid(nPos+nTagLen,nPosDA-nPos-nTagLen); 
		strTemp.TrimLeft(); strTemp.TrimRight();
		strLyNameArray.Add(strTemp);
		strValue = strValue.Right(strValue.GetLength()-nPosDA);
	}

	return TRUE;
}


//�ؿ��ص�������(ע���������Ҫ�պ�)
BOOL CDlgLXInput::CheckCrossOrContain(AcGePoint3dArray& ptArray,CString strSelfSdeLyName)
{
	if(m_pConnect == NULL)
	{
		acutPrintf("\nSDE ���ݿ� û�����ӣ�"); return FALSE;
	}

	CStringArray strArrayCheckLy;
	ReadCfgLyNameArray(strArrayCheckLy,"[���÷���ͼ��]=");
	int nCheck = strArrayCheckLy.GetCount();
	if(nCheck == 0) return TRUE; //û������ͼ�㣬��Ĭ��Ϊͨ��

	
	int nPtCount = ptArray.length();
	IDistPoint* pts = new IDistPoint[nPtCount];
	for(int k=0; k<nPtCount; k++)
	{
		pts[k].x = ptArray[k].x; pts[k].y = ptArray[k].y;
	}

	for(int i=0; i<nCheck; i++)
	{
		CString strCheckSde = strArrayCheckLy.GetAt(i);
		if (strCheckSde.CompareNoCase(strSelfSdeLyName) == 0) continue;// ���������ͼ��
		char strTempSQL[100]={0};
		sprintf(strTempSQL,"F:OBJECTID,T:%s,##",strCheckSde);
		if(1!=gCheckCrossOrContain(m_pConnect,strCheckSde.GetBuffer(0),strTempSQL,pts,nPtCount))
		{
			CString strError;
			strError.Format("��ǰ�ؿ���SDEͼ��[%s]�н����ص�,����Ҫ����������?[��:���� ��:�˳�]",strCheckSde);
			if(AfxMessageBox(strError,MB_YESNO)!=IDYES)
			{
				delete [] pts; pts = NULL;
				return FALSE;
			}
		}	
	}
	delete [] pts; pts = NULL;

	return TRUE;
}
