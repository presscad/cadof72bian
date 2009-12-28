// DlgLXInput.cpp : 实现文件
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

//浮点数四舍五入函数()
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
// CDlgLXInput 对话框

IMPLEMENT_DYNAMIC(CDlgLXInput, CAcUiDialog)
CDlgLXInput::CDlgLXInput(CWnd* pParent)
	: CAcUiDialog(CDlgLXInput::IDD, pParent)
{
	m_strNameArrayDK.RemoveAll();           m_strFNameArrayDK.RemoveAll();               m_strNameArrayXM.RemoveAll();           m_strFNameArrayXM.RemoveAll();
	m_strNameArrayDK.Add("PROJECT_CODE");   m_strFNameArrayDK.Add("项目显示编号");		 m_strNameArrayXM.Add("PROJECT_CODE");   m_strFNameArrayXM.Add("项目显示编号");
	m_strNameArrayDK.Add("LICENSE_CODE");   m_strFNameArrayDK.Add("发证编号");			 m_strNameArrayXM.Add("LICENSE_CODE");   m_strFNameArrayXM.Add("发证编号");
	m_strNameArrayDK.Add("REG_TIME");       m_strFNameArrayDK.Add("登记日期");			 m_strNameArrayXM.Add("REG_TIME");       m_strFNameArrayXM.Add("登记日期");
	m_strNameArrayDK.Add("PROJECT_NAME");   m_strFNameArrayDK.Add("项目名称");			 m_strNameArrayXM.Add("PROJECT_NAME");   m_strFNameArrayXM.Add("项目名称");
	m_strNameArrayDK.Add("BUILD_UNIT");     m_strFNameArrayDK.Add("建设单位");			 m_strNameArrayXM.Add("BUILD_UNIT");     m_strFNameArrayXM.Add("建设单位");
	m_strNameArrayDK.Add("USELAND_TYPEID"); m_strFNameArrayDK.Add("用地性质编号");		 m_strNameArrayXM.Add("USELAND_TYPEID"); m_strFNameArrayXM.Add("用地性质编号");
	m_strNameArrayDK.Add("LXLX");           m_strFNameArrayDK.Add("蓝线类型");			 m_strNameArrayXM.Add("LXLX");           m_strFNameArrayXM.Add("蓝线类型");
	m_strNameArrayDK.Add("JBRY");           m_strFNameArrayDK.Add("经办人员");			 m_strNameArrayXM.Add("JBRY");           m_strFNameArrayXM.Add("经办人员");
	m_strNameArrayDK.Add("DKBH");           m_strFNameArrayDK.Add("地块编号");			 m_strNameArrayXM.Add("JSYDMJ");         m_strFNameArrayXM.Add("建设用地面积");
	m_strNameArrayDK.Add("YDLX");           m_strFNameArrayDK.Add("用地类型");			 m_strNameArrayXM.Add("DLYDMJ");         m_strFNameArrayXM.Add("道路用地面积");
	m_strNameArrayDK.Add("YDMJ");           m_strFNameArrayDK.Add("用地面积");			 m_strNameArrayXM.Add("LHYDMJ");         m_strFNameArrayXM.Add("绿化用地面积");
	m_strNameArrayDK.Add("ZG");             m_strFNameArrayDK.Add("字高");				 m_strNameArrayXM.Add("QTYDMJ");         m_strFNameArrayXM.Add("其他用地面积");
	m_strNameArrayDK.Add("XK");             m_strFNameArrayDK.Add("线宽");				 m_strNameArrayXM.Add("ZYDMJ");          m_strFNameArrayXM.Add("总用地面积");
	m_strNameArrayDK.Add("X");              m_strFNameArrayDK.Add("X值"); 
	m_strNameArrayDK.Add("Y");              m_strFNameArrayDK.Add("Y值");
	m_strNameArrayDK.Add("BZ");             m_strFNameArrayDK.Add("备注");

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


//初始化
BOOL CDlgLXInput::OnInitDialog()
{
	CAcUiDialog::OnInitDialog();
	if(NULL == m_pConnect)
	{
		AfxMessageBox("SDE数据库没有连接！"); SendMessage(WM_CLOSE,0,0);
		return FALSE;
	}

	//加载按钮
	m_sBtnYD.AutoLoad();
	m_sBtnXM.AutoLoad();
	m_sBtnPT.AutoLoad();
	m_sBtnPT_T.AutoLoad();

	//初始化配置信息列表
	m_list_base.SetExtendedStyle(m_list_base.GetExtendedStyle()| LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_list_base.InsertColumn(0,"信息名称",LVCFMT_LEFT,90);
	m_list_base.InsertColumn(1,"信息内容",LVCFMT_LEFT,230);
	m_list_base.DeleteAllItems();

	m_list_base.InsertItem(0,"项目显示编号");  m_list_base.SetItemText(0,1,m_PrjBaseInfo.strPROJECT_CODE);
	m_list_base.InsertItem(1,"项目编号");  m_list_base.SetItemText(1,1,m_PrjBaseInfo.strPROJECT_ID);
	m_list_base.InsertItem(2,"发证编号");      m_list_base.SetItemText(2,1,m_PrjBaseInfo.strLICENCE_CODE);
	m_list_base.InsertItem(3,"登记日期");      m_list_base.SetItemText(3,1,m_PrjBaseInfo.strREG_TIME);
	m_list_base.InsertItem(4,"项目名称");      m_list_base.SetItemText(4,1,m_PrjBaseInfo.strPROJECT_NAME);
	m_list_base.InsertItem(5,"建设单位");      m_list_base.SetItemText(5,1,m_PrjBaseInfo.strBUILD_UNIT);
	m_list_base.InsertItem(6,"用地性质编号");  m_list_base.SetItemText(6,1,m_PrjBaseInfo.strUSELAND_TYPEID);
	m_list_base.InsertItem(7,"蓝线类型");      m_list_base.SetItemText(7,1,m_PrjBaseInfo.strCADTYPE);
	m_list_base.InsertItem(8,"投资总规模");    m_list_base.SetItemText(8,1,m_PrjBaseInfo.strINVEST_SUM);
	m_list_base.InsertItem(9,"容积率");        m_list_base.SetItemText(9,1,m_PrjBaseInfo.strFLOOR_AREA_RATIO);
	m_list_base.InsertItem(10,"建筑密度");      m_list_base.SetItemText(10,1,m_PrjBaseInfo.strBUILDING_DENSITY);
	m_list_base.InsertItem(11,"经办人员");     m_list_base.SetItemText(11,1,m_PrjBaseInfo.strJBRY);


	m_list_ydxx.SetExtendedStyle(m_list_ydxx.GetExtendedStyle()| LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_list_ydxx.InsertColumn(0,"地块编号",LVCFMT_LEFT,60);	
	m_list_ydxx.InsertColumn(1,"用地类型",LVCFMT_LEFT,60);
	m_list_ydxx.InsertColumn(2,"用地面积",LVCFMT_LEFT,80);
	m_list_ydxx.InsertColumn(3,"字高",LVCFMT_LEFT,40);
	m_list_ydxx.InsertColumn(4,"线宽",LVCFMT_LEFT,40);
	m_list_ydxx.InsertColumn(5,"X 值",LVCFMT_LEFT,40);
	m_list_ydxx.InsertColumn(6,"Y 值",LVCFMT_LEFT,40);
	m_list_ydxx.InsertColumn(7,"备注",LVCFMT_LEFT,100);
	m_list_ydxx.InsertColumn(8,"实体ID",LVCFMT_LEFT,0);

	m_bIsModify = FALSE;  //是否已修改标记

	((CComboBox*)GetDlgItem(IDC_CMB_ZG))->SetWindowText("8");    //字高
	((CComboBox*)GetDlgItem(IDC_CMB_ZG_T))->SetWindowText("8");    //字高
	((CComboBox*)GetDlgItem(IDC_CMB_XK))->SetWindowText("1.0");    //线宽
	((CComboBox*)GetDlgItem(IDC_EDIT_BZ))->SetWindowText("没有备注!"); //备注

	ReadEntityInfo();

	return TRUE;
}

//设置初始信息
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
	pRcdSet->GetValueAsString(strValue,1); //获取SHAPE
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

	//注册扩展属性名称
	char strReg[260]={0}; memset(strReg,0,sizeof(char)*260);
	sprintf(strReg,"DIST_%s",m_strSdeLyNameDK);

	//选择图层中所有实体
	AcDbObjectIdArray IdArray;
	CDistEntityTool tempTool;
	tempTool.SelectEntityInLayer(IdArray,m_strCadLyNameDK); 
	int nCount = IdArray.length();


	//读取当前项目的CAD实体
	for(int k=0; k<nCount; k++)
	{
		//1.读扩展属性
		AcDbObjectId tempId = IdArray.at(k);
		CDistXData tempXData(tempId,strReg);
		CStringArray strValueArray,strFNameArray; 
		tempXData.Select(strFNameArray,strValueArray);
		tempXData.Close();
		int nNum = strValueArray.GetCount();
		if(nNum == 0) continue;  //没有扩展属性,继续

		//2.取项目编号
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

		//3.判断是否为当前项目
		if(strProjectCode.CompareNoCase(m_PrjBaseInfo.strPROJECT_CODE) != 0) continue;  

		//4.读取当前项目的地块信息
		DIST_CADENTITY_STRUCT tempEntity;
		for(int jj =0; jj<nNum; jj++)
		{
			CString strTempName = strFNameArray.GetAt(jj);
			if(strTempName.CompareNoCase("YDMJ")==0)  //用地面积 
			{
				tempEntity.strArea = strValueArray.GetAt(jj);
			}
			else if(strTempName.CompareNoCase("YDLX")==0)//用地类型
			{
				tempEntity.strYDType = strValueArray.GetAt(jj);
			}
			else if(strTempName.CompareNoCase("DKBH")==0)//地块编号
			{
				tempEntity.strDKCode = strValueArray.GetAt(jj);
			}
			else if(strTempName.CompareNoCase("ZG")==0)//字高
			{
				tempEntity.strTxtHeight = strValueArray.GetAt(jj);
			}
			else if(strTempName.CompareNoCase("XK")==0)//线宽
			{
				tempEntity.strLnWidth = strValueArray.GetAt(jj);
			}
			else if(strTempName.CompareNoCase("X")==0)//X 值
			{
				tempEntity.strX= strValueArray.GetAt(jj);
			}
			else if(strTempName.CompareNoCase("Y")==0)//Y 值
			{
				tempEntity.strY = strValueArray.GetAt(jj);
			}
			else if(strTempName.CompareNoCase("BZ")==0)//备注
			{
				tempEntity.strBZ = strValueArray.GetAt(jj);
			}
		}
		tempEntity.strObjId.Format("%d",tempId.asOldId());

		//5.保存到数组,供恢复使用
		m_OldEntityArray.Add(tempEntity);
		m_NewEntityArray.Add(tempEntity);

		//6.将信息添加到列表
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

	//查询地块信息
	nCount = m_list_ydxx.GetItemCount();
	IDistCommand* pCommand = CreateSDECommandObjcet();
	pCommand->SetConnectObj(m_pConnect);
	char strSQL[200]={0}; memset(strSQL,0,sizeof(char)*200);
	sprintf(strSQL,"F:OBJECTID,T:%s,W:PROJECT_CODE='%s',##",m_strSdeLyNameDK,m_PrjBaseInfo.strPROJECT_CODE);
	IDistRecordSet* pRcdSet = NULL;
	if(1 != pCommand->SelectData(strSQL,&pRcdSet))
	{
		pCommand->Release(); MessageBox("访问空间库失败!"); SendMessage(WM_CLOSE);
		return FALSE;
	}

	int nRcdCount = pRcdSet->GetRecordCount();
	if(nRcdCount >  nCount)  //没有调出,或者没有调全
	{
		MessageBox("当前项目地块信息已存在, 且没有调出; 请先加载, 再编辑！","信息提示");
		pCommand->Release(); pRcdSet->Release(); SendMessage(WM_CLOSE);
		return FALSE;
	}
	pCommand->Release(); pRcdSet->Release();


	return TRUE;
}


//选择地块
void CDlgLXInput::OnBnClickedSelectYD()
{
	//交互选择实体
	BeginEditorCommand();

	//设置过滤器
	ads_name ssName;
	char strLyName[260]={0}; memset(strLyName,0,sizeof(char)*260);
	strcpy(strLyName,m_strCadLyNameDK.GetBuffer(0));
	struct resbuf filter;
	filter.restype=AcDb::kDxfLayerName;
	filter.resval.rstring = strLyName;
	filter.rbnext=NULL;

	//选择实体
	if (acedSSGet(":S", NULL, NULL, &filter, ssName) != RTNORM)
	{
		AfxMessageBox("\n 选择实体有误!"); acedSSFree(ssName); 
		CompleteEditorCommand(); return;
	}
	AcDbObjectId tempObjId;
	ads_name ssTemp;
	acedSSName(ssName, 0, ssTemp);
	acdbGetObjectId(tempObjId, ssTemp);
	acedSSFree(ssName);

	//打开实体
	AcDbPolyline* pline = NULL;
	if(Acad::eOk != acdbOpenObject(pline,tempObjId,AcDb::kForRead))
	{
		AfxMessageBox("\n 实体类型有误!");
		CompleteEditorCommand(); return;
	}

	//判断是否闭合
	if(pline->isClosed()==Adesk::kFalse)
	{
		AfxMessageBox("选择实体不闭合!");
		pline->close(); CompleteEditorCommand(); return;
	}
	
	//判断是否为已经存在地块
	char strRegName[260]={0};memset(strRegName,0,sizeof(char)*260);
	sprintf(strRegName,"DIST_%s",m_strSdeLyNameDK);
	CDistXData tempXData(pline,strRegName);
	CStringArray strFNameArray,strValueArray;
	tempXData.Select(strFNameArray,strValueArray);
	if(strValueArray.GetCount() > 0)
	{
		pline->close();
		AfxMessageBox("选择实体属性已经设置，不能重新设置!");
		CompleteEditorCommand(); return;
	}

	AcGePoint3dArray pts;
	AcGeDoubleArray TempBlgArray;
	CDistEntityTool tempTool;
	tempTool.GetPolylinePts(pline,pts);

	if(CheckCrossOrContain(pts,m_strSdeLyNameDK)==FALSE)  //与其指定避让图层检测
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
			AfxMessageBox("实体已经存在于列表，选择失败!");
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
			acutPrintf("\n打开实体失败!");
			CompleteEditorCommand(); return;
		}
		

		AcGePoint3dArray tempPts;
		BlgArray.setPhysicalLength(0);
		tempTool.GetPolylinePts(pline,tempPts,BlgArray);

		
		if(PolygonIsInPolygon(tempPts,tempPtArray)==TRUE)
		{
			if(AfxMessageBox("该地块与当前其他地块有交叉或重复，您是否继续？",MB_YESNO)!=IDYES)
			{
				pline->close(); CompleteEditorCommand(); return;
			}
		}
	}


	//空间查询,是否位置有重叠
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
		if(AfxMessageBox("该区域位置与已有蓝线有交叉或重叠，您是否继续？",MB_YESNO)!=IDYES)
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

		if (resBlockRef->restype == 2) // 组码为2表示的是块名
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
	acutPrintf("\n选择项目信息属性块");
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
		if(strName.CompareNoCase("_项目属性")!=0)
		{
			acutPrintf("\n请选择项目属性块!");
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
		if(strTemp.CompareNoCase("项目显示编号")==0)
		{
			//ARX 2005
			//strPrjCode = pAttr->textStringConst();
			//ARX 2004
			strPrjCode  = pAttr->textString();
		}
		if(strTemp.CompareNoCase("蓝线类型")==0)
		{
			//ARX 2005
			//strLXLX = pAttr->textStringConst();
			//ARX 2004
			strLXLX = pAttr->textString();
		}
	}
	pBlkRef->close();

	if(strLXLX.CompareNoCase("选址")!= 0)
	{
		acutPrintf("\n请选择选址项目!"); CompleteEditorCommand(); return;
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
	//交互选择实体
	BeginEditorCommand();

	ads_point ptResult;
	if(RTNORM != acedGetPoint(NULL,"地块编号显示位置点",ptResult))
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
	//交互选择实体
	BeginEditorCommand();

	ads_point ptResult;
	if(RTNORM != acedGetPoint(NULL,"项目信息显示位置点",ptResult))
	{
		acutPrintf("\n操作被取消");
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

	//打开实体
	AcDbObjectId tempId;
	tempId.setFromOldId(atoi(strTemp));
	AcDbPolyline* pline = NULL;
	if(Acad::eOk != acdbOpenObject(pline,tempId,AcDb::kForRead)) return;
    
	//取实体边界
	AcDbExtents Ext;
	pline->getGeomExtents(Ext);
	pline->close();//高亮显示  pline->highlight();

	AcGePoint3d ptMax = Ext.maxPoint();
	AcGePoint3d ptMin = Ext.minPoint();
	
	//设置视口
	CDistEntityTool tempTool;
	tempTool.DyZoom(AcGePoint2d((ptMax.x+ptMin.x)/2,(ptMax.y+ptMin.y)/2),ptMax.x-ptMin.y+50,ptMax.y-ptMin.y+50);

}

void CDlgLXInput::SetValueToControls()
{
	int nSelect = m_list_ydxx.GetSelectionMark();
	if(nSelect < 0) return;
	
	((CComboBox*)GetDlgItem(IDC_CMB_DKBH))->SetWindowText(m_list_ydxx.GetItemText(nSelect,0));  //地块编号	
	((CComboBox*)GetDlgItem(IDC_CMB_YDLX))->SelectString(0,m_list_ydxx.GetItemText(nSelect,1)); //用地类型
	((CEdit*)GetDlgItem(IDC_EDIT_ZMJ))->SetWindowText(m_list_ydxx.GetItemText(nSelect,2));      //用地面积
	((CComboBox*)GetDlgItem(IDC_CMB_ZG))->SetWindowText(m_list_ydxx.GetItemText(nSelect,3));    //字高
	((CComboBox*)GetDlgItem(IDC_CMB_XK))->SetWindowText(m_list_ydxx.GetItemText(nSelect,4));    //线宽
	((CEdit*)GetDlgItem(IDC_EDIT_X))->SetWindowText(m_list_ydxx.GetItemText(nSelect,5));        //标注X值
	((CEdit*)GetDlgItem(IDC_EDIT_Y))->SetWindowText(m_list_ydxx.GetItemText(nSelect,6));        //标注Y值
	((CEdit*)GetDlgItem(IDC_EDIT_BZ))->SetWindowText(m_list_ydxx.GetItemText(nSelect,7));       //备注
	((CEdit*)GetDlgItem(IDC_EDIT_OBJ))->SetWindowText(m_list_ydxx.GetItemText(nSelect,8));      //实体ID
}

BOOL CDlgLXInput::GetValueFromControls(DIST_CADENTITY_STRUCT& Value)
{
	((CEdit*)GetDlgItem(IDC_EDIT_ZMJ))->GetWindowText(Value.strArea);//用地面积
	Value.strArea.TrimLeft(); Value.strArea.TrimRight();
	if(Value.strArea.IsEmpty())
	{
		AfxMessageBox("请选择地块实体!"); return FALSE;
	}
	int nSelect = ((CComboBox*)GetDlgItem(IDC_CMB_YDLX))->GetCurSel();
	if(nSelect >=0) 
		((CComboBox*)GetDlgItem(IDC_CMB_YDLX))->GetLBText(nSelect,Value.strYDType);//用地类型
	else
	{
		AfxMessageBox("用地类型不能为空!"); return FALSE;
	}

	((CComboBox*)GetDlgItem(IDC_CMB_DKBH))->GetWindowText(Value.strDKCode); //地块编号
	Value.strDKCode.TrimLeft(); Value.strDKCode.TrimRight();
	if(Value.strDKCode.IsEmpty())
	{
		AfxMessageBox("地块编号不能为空!"); return FALSE;
	}

	((CComboBox*)GetDlgItem(IDC_CMB_ZG))->GetWindowText(Value.strTxtHeight);//字高
	if(Value.strTxtHeight.IsEmpty())
	{
		((CComboBox*)GetDlgItem(IDC_CMB_ZG_T))->GetWindowText(Value.strTxtHeight);
		if(Value.strTxtHeight.IsEmpty())
		{
			AfxMessageBox("字高不能为空!"); return FALSE;
		}	
	}

	((CComboBox*)GetDlgItem(IDC_CMB_XK))->GetWindowText(Value.strLnWidth); //线宽
	if(Value.strLnWidth.IsEmpty())
	{
		AfxMessageBox("线宽不能为空!"); return FALSE;
	}

	((CEdit*)GetDlgItem(IDC_EDIT_X))->GetWindowText(Value.strX); //标注X值
	Value.strX.TrimLeft(); Value.strX.TrimRight();
	if(Value.strX.IsEmpty())
	{
		AfxMessageBox("请选择地块标注位置!"); return FALSE;
	}
	((CEdit*)GetDlgItem(IDC_EDIT_Y))->GetWindowText(Value.strY); //标注Y值
	((CEdit*)GetDlgItem(IDC_EDIT_BZ))->GetWindowText(Value.strBZ); //备注
	((CEdit*)GetDlgItem(IDC_EDIT_OBJ))->GetWindowText(Value.strObjId); //实体ID

	return TRUE;
}



void CDlgLXInput::SetValueToXData(AcDbObjectId& ObjId,const char* strRegName,DIST_CADENTITY_STRUCT& Value)
{
	CStringArray strValueArray;
	strValueArray.RemoveAll();
	strValueArray.Add(m_PrjBaseInfo.strPROJECT_CODE);  //项目显示编号
	strValueArray.Add(m_PrjBaseInfo.strLICENCE_CODE);  //发证编号
	strValueArray.Add(m_PrjBaseInfo.strREG_TIME);      //登记日期
	strValueArray.Add(m_PrjBaseInfo.strPROJECT_NAME);  //项目名称
	strValueArray.Add(m_PrjBaseInfo.strBUILD_UNIT);    //建设单位
	strValueArray.Add(m_PrjBaseInfo.strUSELAND_TYPEID);//用地性质编号
	strValueArray.Add(m_PrjBaseInfo.strCADTYPE);       //蓝线类型
	strValueArray.Add(m_PrjBaseInfo.strJBRY);          //经办人员

	strValueArray.Add(Value.strDKCode);                //地块编号  
	strValueArray.Add(Value.strYDType);                //用地类型
	strValueArray.Add(Value.strArea);                  //用地面积
	strValueArray.Add(Value.strTxtHeight);             //字高
	strValueArray.Add(Value.strLnWidth);               //线宽
	strValueArray.Add(Value.strX);                     //X值
	strValueArray.Add(Value.strY);                     //Y值
	strValueArray.Add(Value.strBZ);                    //备注


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


//添加
void CDlgLXInput::OnBnClickedBtnAdd()
{
	//从编辑控件中读取数据
	DIST_CADENTITY_STRUCT Value;
	if(GetValueFromControls(Value)==FALSE) return;

	//数据数组
	int nCountNew = m_NewEntityArray.GetCount();
	for(int qq=0; qq<nCountNew; qq++)
	{
		DIST_CADENTITY_STRUCT tempData = m_NewEntityArray.GetAt(qq);
		if(tempData.strObjId.CompareNoCase(Value.strObjId)==0)
		{
			acutPrintf("\n该实体已存在,添加失败!"); return;
		}
	}
	m_NewEntityArray.Add(Value);

	//添加到列表
	int nCount = m_list_ydxx.GetItemCount();
	m_list_ydxx.InsertItem(nCount,Value.strDKCode);         //地块编号
	m_list_ydxx.SetItemText(nCount,1,Value.strYDType);      //用地类型
	m_list_ydxx.SetItemText(nCount,2,Value.strArea);        //用地面积
	m_list_ydxx.SetItemText(nCount,3,Value.strTxtHeight);   //字高
	m_list_ydxx.SetItemText(nCount,4,Value.strLnWidth);     //线宽
	m_list_ydxx.SetItemText(nCount,5,Value.strX);           //X 值
	m_list_ydxx.SetItemText(nCount,6,Value.strY);           //Y 值
	m_list_ydxx.SetItemText(nCount,7,Value.strBZ);          //备注
	m_list_ydxx.SetItemText(nCount,8,Value.strObjId);       //实体ID


	//根据实体ID修改线宽并高亮显示
	AcDbObjectId tempId;
	tempId.setFromOldId(atoi(Value.strObjId));
	double dW = atof(Value.strLnWidth);
	AcDbPolyline * pline = NULL;
	if(Acad::eOk == acdbOpenObject(pline,tempId,AcDb::kForWrite))
	{
		int nNumVert = pline->numVerts(); //获取点数量
		for(int i=0; i<nNumVert; i++)
			pline->setWidthsAt(i,dW,dW);
		pline->close();
	}


	//清空
	((CEdit*)GetDlgItem(IDC_EDIT_OBJ))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_EDIT_ZMJ))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_EDIT_X))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_EDIT_Y))->SetWindowText("");
	((CComboBox*)GetDlgItem(IDC_CMB_DKBH))->SetWindowText("");
	((CComboBox*)GetDlgItem(IDC_CMB_ZG))->SetWindowText("8");    //字高
	((CComboBox*)GetDlgItem(IDC_CMB_XK))->SetWindowText("1.0");    //线宽
	((CComboBox*)GetDlgItem(IDC_EDIT_BZ))->SetWindowText("没有备注!"); //备注


	m_bIsModify = TRUE;
	m_list_ydxx.SetSelectionMark(-1);
}


//修改
void CDlgLXInput::OnBnClickedBtnModify()
{
	int nSelect = m_list_ydxx.GetSelectionMark();
	if(nSelect < 0) return;


	//从编辑控件中读取数据
	DIST_CADENTITY_STRUCT Value;
	if(GetValueFromControls(Value)==FALSE) return;


	int nCountNew = m_NewEntityArray.GetCount();
	
	m_NewEntityArray.SetAt(nSelect,Value);  //修改


	//修改列表值
	m_list_ydxx.SetItemText(nSelect,0,Value.strDKCode);    //地块编号
	m_list_ydxx.SetItemText(nSelect,1,Value.strYDType);    //用地类型
	m_list_ydxx.SetItemText(nSelect,2,Value.strArea);      //用地面积
	m_list_ydxx.SetItemText(nSelect,3,Value.strTxtHeight); //字高
	m_list_ydxx.SetItemText(nSelect,4,Value.strLnWidth);   //线宽
	m_list_ydxx.SetItemText(nSelect,5,Value.strX);         //X 值
	m_list_ydxx.SetItemText(nSelect,6,Value.strY);         //Y 值
	m_list_ydxx.SetItemText(nSelect,7,Value.strBZ);        //备注
	m_list_ydxx.SetItemText(nSelect,8,Value.strObjId);     //实体ID

	//根据实体ID修改线宽并高亮显示
	AcDbObjectId tempId;
	tempId.setFromOldId(atoi(Value.strObjId));
	double dW = atof(Value.strLnWidth);
	AcDbPolyline * pline = NULL;
	if(Acad::eOk == acdbOpenObject(pline,tempId,AcDb::kForWrite))
	{
		int nNumVert = pline->numVerts(); //获取点数量
		for(int i=0; i<nNumVert; i++)
			pline->setWidthsAt(i,dW,dW);
		pline->close();
	}

	//清空
	((CEdit*)GetDlgItem(IDC_EDIT_OBJ))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_EDIT_ZMJ))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_EDIT_X))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_EDIT_Y))->SetWindowText("");
	((CComboBox*)GetDlgItem(IDC_CMB_DKBH))->SetWindowText("");
	((CComboBox*)GetDlgItem(IDC_CMB_ZG))->SetWindowText("8");    //字高
	((CComboBox*)GetDlgItem(IDC_CMB_XK))->SetWindowText("1.0");    //线宽
	((CComboBox*)GetDlgItem(IDC_EDIT_BZ))->SetWindowText("没有备注!"); //备注

	m_bIsModify = TRUE;
	m_list_ydxx.SetSelectionMark(-1);

}


//删除
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

	DeleteBlkAttRef("_地块属性",strDKBH);
*/
	//清空
	((CEdit*)GetDlgItem(IDC_EDIT_OBJ))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_EDIT_ZMJ))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_EDIT_X))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_EDIT_Y))->SetWindowText("");
	((CComboBox*)GetDlgItem(IDC_CMB_DKBH))->SetWindowText("");
	((CComboBox*)GetDlgItem(IDC_CMB_ZG))->SetWindowText("8");    //字高
	((CComboBox*)GetDlgItem(IDC_CMB_XK))->SetWindowText("0.0");    //线宽
	((CComboBox*)GetDlgItem(IDC_EDIT_BZ))->SetWindowText("没有备注!"); //备注




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
				if (nValue<=0 || nValue>12)   //取值范围 0-11
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
	//合法性检测
	if(NULL == m_pConnect)
	{
		acutPrintf("\n空间数据库没有连接！"); return;
	}
	if(m_pConnect->CheckConnect(60)!=1)
	{
		acutPrintf("\n空间数据库连接失败！");  return;
	}

	int nCount = m_list_ydxx.GetItemCount();
	if(nCount == 0)
	{
		BOOL bFound = IsXMDKInfoExists();
		if(bFound == TRUE)
		{
			if(MessageBox("你确认要删除空间库中对应的项目信息吗？","信息提示",MB_YESNO) != IDYES) return;
			ClearOldInfo();
			DeleteXMDKINFO();
			m_bIsModify = FALSE;
			return;
		}
		else return;
	}

	//取项目信息标注位置
	CEdit* pEdt = (CEdit*)GetDlgItem(IDC_EDIT_X_T);
	CString strTempValue;
	pEdt->GetWindowText(strTempValue);
	strTempValue.Trim();
	if(strTempValue.IsEmpty())
	{
		MessageBox("请拾取项目信息标注点位置!"); return;
	}
	ads_point pt;
	pt[0] = atof(strTempValue);
	pEdt = (CEdit*)GetDlgItem(IDC_EDIT_Y_T);
	pEdt->GetWindowText(strTempValue);
	pt[1] = atof(strTempValue);

	//取项目信息标注的字高
	CComboBox* pCmb = (CComboBox*)GetDlgItem(IDC_CMB_ZG_T);
	pCmb->GetWindowText(strTempValue);
	strTempValue.Trim();
	if(strTempValue.IsEmpty())
	{
		MessageBox("请选择项目信息标注字体的高度!"); return;
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
	

	//确认提交
	if(MessageBox("你确认要提交所有地块数据到SDE空间库吗？","信息提示",MB_YESNO) != IDYES) return;

	//删除已存在记录
	IDistCommand* pCommand = CreateSDECommandObjcet();
	if(NULL == pCommand) return;
	pCommand->SetConnectObj(m_pConnect);

	const int MAXLEN = 33; //属性数据31行,加1个SHAPE
	char** strFldValueArray = new char*[MAXLEN];
	for(int i=0;i<MAXLEN;i++)
	{
		strFldValueArray[i] = new char[200];
		memset(strFldValueArray[i],0,sizeof(char)*200);
	}

	//项目基本信息 
	strcpy(strFldValueArray[0],m_PrjBaseInfo.strPROJECT_ID.GetBuffer(0));             //项目编号
	strcpy(strFldValueArray[1],m_PrjBaseInfo.strPROJECT_CODE.GetBuffer(0));           //项目显示编号
	strcpy(strFldValueArray[2],m_PrjBaseInfo.strRELATION_CODE.GetBuffer(0));          //关联编号
	//strcpy(strFldValueArray[3],m_PrjBaseInfo.strREG_TIME.GetBuffer(0));               //登记时间
	strcpy(strFldValueArray[4],m_PrjBaseInfo.strPROJECT_NAME.GetBuffer(0));           //项目名称
	strcpy(strFldValueArray[5],m_PrjBaseInfo.strPRE_PROJECT_ID.GetBuffer(0));         //上一业务项目目编号
	strcpy(strFldValueArray[6],m_PrjBaseInfo.strBUILD_UNIT.GetBuffer(0));             //建设单位
	strcpy(strFldValueArray[7],m_PrjBaseInfo.strBUILD_UNIT_LINKMAN.GetBuffer(0));     //联系人
	strcpy(strFldValueArray[8],m_PrjBaseInfo.strBUILD_UNIT_PHONE.GetBuffer(0));       //电话
	//strcpy(strFldValueArray[9],m_PrjBaseInfo.strOVER_TIME.GetBuffer(0));              //结案时间
	strcpy(strFldValueArray[10],m_PrjBaseInfo.strBUILD_ADDRESS.GetBuffer(0));         //建设地点
	strcpy(strFldValueArray[11],m_PrjBaseInfo.strLICENCE_CODE.GetBuffer(0));          //发证编号
	strcpy(strFldValueArray[12],m_PrjBaseInfo.strUSELAND_TYPE.GetBuffer(0));          //用地性质
	strcpy(strFldValueArray[13],m_PrjBaseInfo.strUSELAND_TYPEID.GetBuffer(0));        //用地性质编号
	strcpy(strFldValueArray[14],m_PrjBaseInfo.strARCHIITECTUREPRO_TYPE.GetBuffer(0)); //建设工程性质
	strcpy(strFldValueArray[15],m_PrjBaseInfo.strHOURSEARCHI_TYPE.GetBuffer(0));      //居住建筑类型
	
	m_PrjBaseInfo.strINVEST_SUM.Trim();
	if(m_PrjBaseInfo.strINVEST_SUM.IsEmpty())
		m_PrjBaseInfo.strINVEST_SUM="0.000";
	strcpy(strFldValueArray[16],m_PrjBaseInfo.strINVEST_SUM.GetBuffer(0));            //投资总规模
	strcpy(strFldValueArray[17],m_PrjBaseInfo.strFLOOR_AREA_RATIO.GetBuffer(0));      //容积率

	strcpy(strFldValueArray[18],m_PrjBaseInfo.strBUILDING_DENSITY.GetBuffer(0));      //建筑密度
	strcpy(strFldValueArray[19],m_PrjBaseInfo.strPRO_STATUS.GetBuffer(0));            //项目状态

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

	//删除旧地块记录
	char strSQL[512]={0};
	sprintf(strSQL,"T:%s,W:PROJECT_ID='%s',##",m_strSdeLyNameDK,m_PrjBaseInfo.strPROJECT_ID);
	if(1 != pCommand->DeleteData(strSQL))
	{
		for(i=0; i<MAXLEN; i++)
			delete[] strFldValueArray[i];
		delete[] strFldValueArray;

		AfxMessageBox("\n提交数据失败！");
		pCommand->RollbackTrans(); pCommand->Release(); return;
	}

	sprintf(strSQL,"T:%s,W:PROJECT_ID='%s',##",m_strSdeLyNameXM,m_PrjBaseInfo.strPROJECT_ID);
	if(1 != pCommand->DeleteData(strSQL))
	{
		for(i=0; i<MAXLEN; i++)
			delete[] strFldValueArray[i];
		delete[] strFldValueArray;

		AfxMessageBox("\n提交数据失败！");
		pCommand->RollbackTrans(); pCommand->Release();return;
	}

	//添加新记录
	sprintf(strSQL,"F:PROJECT_ID,"             //项目编号          0
		           "PROJECT_CODE,"			   //项目显示编号      1
				   "RELATION_CODE,"			   //关联编号          2
				   "REG_TIME,"				   //登记时间          3  *
				   "PROJECT_NAME,"			   //项目名称          4
				   "PRE_PROJECT_ID,"		   //上一业务项目目编  5
		           "BUILD_UNIT,"			   //建设单位          6
				   "BUILD_UNIT_LINKMAN,"	   //联系人            7
				   "BUILD_UNIT_PHONE,"		   //电话              8
				   "OVER_TIME,"				   //结案时间          9  *
				   "BUILD_ADDRESS,"			   //建设地点          10
		           "LICENCE_CODE,"			   //发证编号          11
				   "USELAND_TYPE,"			   //用地性质          12
				   "USELAND_TYPEID,"		   //用地性质编号      13
				   "ARCHIITECTUREPRO_TYPE,"	   //建设工程性质      14
				   "HOURSEARCHI_TYPE,"		   //居住建筑类型      15
				   "INVEST_SUM,"               //投资总规模        16 *
				   "FLOOR_AREA_RATIO,"	       //容积率            17
				   "BUILDING_DENSITY,"	       //建筑密度          18
				   "PRO_STATUS,"		       //项目状态          19
				   "DKBH,"		  //地块编号	20	
				   "YDLX,"		  //用地类型    21
				   "YDMJ,"		  //用地面积    22  *
				   "ZG,"		  //字高");     23
				   "XK,"		  //线宽");     24
				   "X,"			  //X值");      25
				   "Y,"			  //Y值");      26
				   "BZ,"		  //备注");     27
				   "JBRY,"		  //经办人员    28
				   "SHAPE,"
                   "T:%s,##",m_strSdeLyNameDK);

	ClearOldInfo();
	nCount = m_list_ydxx.GetItemCount();
	for(int j=0; j<nCount; j++)
	{
		//地块信息
		CString strTemp;  //属性信息
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

		//坐标信息
		AcDbObjectId tempId;
		strTemp = m_list_ydxx.GetItemText(j,8);  //取实体ID
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

			AfxMessageBox("\n提交数据失败！");
			pCommand->RollbackTrans(); pCommand->Release(); return;
		}

		double dH = atof(strFldValueArray[23]);
		double dW = atof(strFldValueArray[24]);
		double dX = atof(strFldValueArray[25]);
		double dY = atof(strFldValueArray[26]);

		CStringArray strValueArray;
		strValueArray.Add(m_PrjBaseInfo.strPROJECT_CODE);  //项目显示编号"
		strValueArray.Add(m_PrjBaseInfo.strLICENCE_CODE);  //发证编号
		strValueArray.Add(m_PrjBaseInfo.strREG_TIME);      //登记日期
		strValueArray.Add(m_PrjBaseInfo.strPROJECT_NAME);  //项目名称
		strValueArray.Add(m_PrjBaseInfo.strBUILD_UNIT);    //建设单位
		strValueArray.Add(m_PrjBaseInfo.strUSELAND_TYPEID);//用地性质编号
		strValueArray.Add(m_PrjBaseInfo.strCADTYPE);       //蓝线类型
		strValueArray.Add(m_PrjBaseInfo.strJBRY);          //经办人员

		strValueArray.Add(strFldValueArray[20]);     //地块编号
		strValueArray.Add(strFldValueArray[21]);     //用地类型
		strValueArray.Add(strFldValueArray[22]);     //用地面积
		strValueArray.Add(strFldValueArray[23]);     //字高
		strValueArray.Add(strFldValueArray[24]);     //线宽
		strValueArray.Add(strFldValueArray[25]);     //X 值
		strValueArray.Add(strFldValueArray[26]);     //Y 值
		strValueArray.Add(strFldValueArray[27]);     //备注


		
		CString strCadLyName;
		strCadLyName.Format("%s地块标注",m_strCadLyNameDK);
		AcDbObjectId resultId;
		tempTool.InsertBlkAttrRef(resultId,"_地块属性",strCadLyName.GetBuffer(),AcGePoint3d(dX,dY,0),dH,m_strFNameArrayDK,strValueArray);
		CDistXData tempXData(resultId,"ROWID_OBJS");
		tempXData.Add("wang","hui");
		tempXData.Update(); tempXData.Close();

	}

	
	//计算面积
	double dTotalArea=0.00000;
	double dA1=0.00000,dA2=0.00000,dA3=0.00000,dA4=0.00000;
	CString strTemp;
	for(int i=0; i<nCount;i++)
	{
		strTemp = m_list_ydxx.GetItemText(i,1); //地块编号  建设用地;道路用地;绿化用地;其他用地;
		if(strTemp.Find("建设用地") >=0 )
		{
			strTemp = m_list_ydxx.GetItemText(i,2);
			dA1 = dA1+atof(strTemp.GetBuffer(0));
		}
		else if(strTemp.Find("道路用地") >=0 )
		{
			strTemp = m_list_ydxx.GetItemText(i,2);
			dA2 = dA2+atof(strTemp.GetBuffer(0));
		}
		else if(strTemp.Find("绿化用地") >=0 )
		{
			strTemp = m_list_ydxx.GetItemText(i,2);
			dA3 = dA3+atof(strTemp.GetBuffer(0));
		}
		else if(strTemp.Find("其他用地") >=0 )
		{
			strTemp = m_list_ydxx.GetItemText(i,2);
			dA4 = dA4+atof(strTemp.GetBuffer(0));
		}
	}
	dTotalArea = dA1+dA2+dA3+dA4;

	//写项目信息
	sprintf(strSQL,"F:PROJECT_ID,"                //项目编号          0
		             "PROJECT_CODE,"			  //项目显示编号      1
					 "RELATION_CODE,"			  //关联编号          2
					 "REG_TIME,"				  //登记时间          3  *
					 "PROJECT_NAME,"			  //项目名称          4
					 "PRE_PROJECT_ID,"			  //上一业务项目目编  5
		             "BUILD_UNIT,"				  //建设单位          6
					 "BUILD_UNIT_LINKMAN,"		  //联系人            7
					 "BUILD_UNIT_PHONE,"		  //电话              8
					 "OVER_TIME,"				  //结案时间          9  *
					 "BUILD_ADDRESS,"			  //建设地点          10
					 "LICENCE_CODE,"			  //发证编号          11
		             "USELAND_TYPE,"			  //用地性质          12
					 "USELAND_TYPEID,"			  //用地性质编号      13
					 "ARCHIITECTUREPRO_TYPE,"	  //建设工程性质      14
					 "HOURSEARCHI_TYPE,"		  //居住建筑类型      15
					 "INVEST_SUM,"				  //投资总规模        16 *
		             "FLOOR_AREA_RATIO,"		  //容积率            17
					 "BUILDING_DENSITY,"		  //建筑密度          18
					 "PRO_STATUS,"				  //项目状态          19
					 "TFBH,"                  //图幅编号 20
					 "SUM_LAND_AREA_SUM,"     //总用地面积（文）21  *
					 "BUILD_LAND_AREA_SUM,"   //建设用地总面积（文）22*
					 "BUILD_AREA_SUM,"        //建筑总面积（文）23*
				     "JBRY,"   //经办人员24
					 "RKYY,"   //入库原因25
					 "JSYDMJ," //建设用地面积26*
					 "DLYDMJ," //道路用地面积27*
					 "LHYDMJ," //绿化用地面积28*
					 "QTYDMJ," //其他用地面积29*
					 "ZYDMJ,"  //总用地面积30*
					 "BZ,"     //备注31
					 "SHAPE,T:%s,##",m_strSdeLyNameXM);



	
	strcpy(strFldValueArray[20],m_PrjBaseInfo.strTFBH.GetBuffer(0));         //图幅编号

	m_PrjBaseInfo.strSUM_LAND_AREA_SUM.TrimLeft();
	m_PrjBaseInfo.strSUM_LAND_AREA_SUM.TrimRight();
	if(m_PrjBaseInfo.strSUM_LAND_AREA_SUM.IsEmpty())
		m_PrjBaseInfo.strSUM_LAND_AREA_SUM="0.000";
	strcpy(strFldValueArray[21],m_PrjBaseInfo.strSUM_LAND_AREA_SUM.GetBuffer(0));   //总用地面积（文）

	m_PrjBaseInfo.strBUILD_LAND_AREA_SUM.TrimLeft();
	m_PrjBaseInfo.strBUILD_LAND_AREA_SUM.TrimRight();
	if(m_PrjBaseInfo.strBUILD_LAND_AREA_SUM.IsEmpty())
		m_PrjBaseInfo.strBUILD_LAND_AREA_SUM="0.000";
	strcpy(strFldValueArray[22],m_PrjBaseInfo.strBUILD_LAND_AREA_SUM.GetBuffer(0)); //建设用地总面积（文）

	m_PrjBaseInfo.strBUILD_AREA_SUM.TrimLeft();
	m_PrjBaseInfo.strBUILD_AREA_SUM.TrimRight();
	if(m_PrjBaseInfo.strBUILD_AREA_SUM.IsEmpty())
		m_PrjBaseInfo.strBUILD_AREA_SUM="0.000";
	strcpy(strFldValueArray[23],m_PrjBaseInfo.strBUILD_AREA_SUM.GetBuffer(0));      //建筑总面积（文）

    strcpy(strFldValueArray[24],m_PrjBaseInfo.strJBRY.GetBuffer(0));         //经办人员
	strcpy(strFldValueArray[25]," ");         //入库原因

	CString strJSYDMJ,strZMJ;
	double dTemp = dA1; MyRound(dTemp,3);
	strTemp.Format("%0.3f",dTemp); strJSYDMJ = strTemp;
	strcpy(strFldValueArray[26],strTemp.GetBuffer(0)); //建设用地面积

	dTemp = dA2; MyRound(dTemp,3);
	strTemp.Format("%0.3f",dTemp);
	strcpy(strFldValueArray[27],strTemp.GetBuffer(0)); //道路用地面积

	dTemp = dA3; MyRound(dTemp,3);
	strTemp.Format("%0.3f",dTemp);
	strcpy(strFldValueArray[28],strTemp.GetBuffer(0)); //绿化用地面积

	dTemp = dA4; MyRound(dTemp,3);
	strTemp.Format("%0.3f",dTemp);
	strcpy(strFldValueArray[29],strTemp.GetBuffer(0)); //其他用地面积

	dTotalArea = dA1+dA2+dA3+dA4;
	dTemp = dTotalArea; MyRound(dTemp,3);
	strTemp.Format("%0.3f",dTemp); strZMJ = strTemp;
	strcpy(strFldValueArray[30],strTemp.GetBuffer(0)); //总用地面积
	strcpy(strFldValueArray[31]," "); //备注




	int nRowId = -1;
	if(1 != gSaveOneXMToSDE(m_pConnect,strSQL,m_strSdeLyNameXM,strFldValueArray,32+1,pt[0],pt[1]))
	{
		for(i=0; i<MAXLEN; i++)
			delete[] strFldValueArray[i];
		delete[] strFldValueArray;

		AfxMessageBox("\n提交数据失败！");
		pCommand->RollbackTrans(); pCommand->Release(); return;
	}

	pCommand->CommitTrans(); pCommand->Release();

	double dTempTemp = atof(strJSYDMJ);
	if(fabs(dTempTemp)<0.01) strJSYDMJ = "0.000";
	//临时
	//CallDap(g_DAPHwnd,m_strDapProcessId,m_strURL,strZMJ,strJSYDMJ);

	AfxMessageBox("\n提交数据成功！");

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
	tempTool.InsertBlkAttrRef(tempId,"_项目属性",m_strCadLyNameXM.GetBuffer(0),AcGePoint3d(pt[0],pt[1],0),nHeight,m_strFNameArrayXM,strValueArray);
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

	// 如果连接出错！
	if( hr != S_OK )
	{
		pWsAccessor->Answer(lpMsg);MessageBox( lpMsg);return;
	}

	CString sSql;
	sSql.Format("select * from TBCAD_WEB_INTERACT where REF_PROCESS_ID = %s",strXMBH);


	// 注意，参数数组长度一定要多声明一些
	_variant_t varParams[5];
	varParams[0] = sSql.GetBuffer();
	_variant_t varResult;

	// 调用方法
	hr = pWsAccessor->CallMethod("ExecSqlQuery",varParams, &varResult);

	// 如果出错，获得出错信息
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

	char strXmlPath[] = "/NewDataSet/Table"; // XML节点在生成的时候已经约定好了叫这个名字
	IDistRecordSetXML* lpRst = NULL;
	lpCmd->SelectData(strXmlPath, &lpRst);

	if (NULL == lpRst)
	{
#ifdef _DEBUG
		AfxMessageBox("TBCAD_WEB_INTERACT表中未找到数据");
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

		if(strDescription.CompareNoCase("总用地面积")==0)
		{
			strValue = strZMJ;
		}
		else if(strDescription.CompareNoCase("建设用地面积")==0)
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
		if(MessageBox("内容已修改,但没有提交入库;如果放弃,数据将恢复的初始状态,确认放弃这次操作吗?","信息提示",MB_YESNO) == IDNO) return;
		
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

    //地块编号,用地类型,用地面积,字高,线宽,X 值,Y 值,备注,实体ID
	

}

BOOL CDlgLXInput::IsXMDKInfoExists()
{
	//删除已存在记录
	IDistCommand* pCommand = CreateSDECommandObjcet();
	pCommand->SetConnectObj(m_pConnect);

	//删除旧地块记录
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
	//删除已存在记录
	IDistCommand* pCommand = CreateSDECommandObjcet();
	if(NULL == pCommand) return;
	pCommand->SetConnectObj(m_pConnect);

	pCommand->BeginTrans();

	//删除旧地块记录
	char strSQL[512]={0};
	sprintf(strSQL,"T:%s,W:PROJECT_ID='%s',##",m_strSdeLyNameDK,m_PrjBaseInfo.strPROJECT_ID);
	if(1 != pCommand->DeleteData(strSQL))
	{
		AfxMessageBox("\n删除数据失败！");
		pCommand->RollbackTrans(); pCommand->Release(); return;
	}

	sprintf(strSQL,"T:%s,W:PROJECT_ID='%s',##",m_strSdeLyNameXM,m_PrjBaseInfo.strPROJECT_ID);
	if(1 != pCommand->DeleteData(strSQL))
	{
		AfxMessageBox("\n删除数据失败！");
		pCommand->RollbackTrans(); pCommand->Release();return ;
	}
	pCommand->CommitTrans();

	pCommand->Release();

	AfxMessageBox("\n删除数据成功！");
}


void CDlgLXInput::DeleteBlkAttRef(CString strBlkName)
{
	CString strTempCadLyName;
	if(strBlkName.Find("_地块属性")>=0)
	{
		strTempCadLyName.Format("%s地块标注",m_strCadLyNameDK);
	}
	else if(strBlkName.Find("_项目属性")>=0)
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
		ReadAttrFromRef(strValue,"项目显示编号",pRef); //读取属性块项目显示编号值

		strValue.TrimLeft(); strValue.TrimRight();

		if(strValue.IsEmpty())
		{
			pRef->close();
		}
		else
		{
			if(strValue.CompareNoCase(m_PrjBaseInfo.strPROJECT_CODE)==0)  //与当前项目名称比较
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


//从块引用中获取指定名称属性的内容
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

	//清除扩展属性
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

	DeleteBlkAttRef("_地块属性");
	DeleteBlkAttRef("_项目属性");

}




void CDlgLXInput::RestoreEntityInfo()
{

	int nCountOld = m_OldEntityArray.GetCount();
	int nCountNew = m_list_ydxx.GetItemCount();


	if(nCountOld == 0) //新建的情况
	{
		if(nCountNew == 0) return;  //什么都没有
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
	else  //非新建的情况
	{
		if(nCountNew > 0)  //清空现有的
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
		for(int j=0; j<nCountOld; j++)  //恢复原有
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
	if(m_PrjBaseInfo.strCADTYPE.CompareNoCase("用地")!=0) 
	{
		pBtn->SetCheck(0);
		m_sBtnXM.EnableWindow(FALSE);
		AfxMessageBox("项目图形复制功能,只为用地类型项目提供!"); 
	}
	else
	{
		pBtn->SetCheck(1);
		m_sBtnXM.EnableWindow(TRUE);
	}
}

void CDlgLXInput::OnCbnSelchangeCmbDkbh()
{
	// TODO: 在此添加控件通知处理程序代码
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
	 //地块编号,用地类型,用地面积,字高,线宽,X 值,Y 值,备注,实体ID
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
	//地块编号,用地类型,用地面积,字高,线宽,X 值,Y 值,备注,实体ID
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
		if(RTNORM == acedGetPoint(NULL,"地块编号显示位置点",ptResult))
		{
			strTempX.Format("%0.3f",ptResult[0]);
			((CEdit*)GetDlgItem(IDC_EDIT_X_T))->SetWindowText(strTempX);
			strTempY.Format("%0.3f",ptResult[1]);
			((CEdit*)GetDlgItem(IDC_EDIT_Y_T))->SetWindowText(strTempY);
		}	
		CompleteEditorCommand();
		*/

		//::SendMessage(m_sBtnPT_T.m_hWnd,WM_LBUTTONDOWN,MK_LBUTTON,NULL);//模拟鼠标点击   
		//::SendMessage(m_sBtnPT_T.m_hWnd,WM_LBUTTONUP,MK_LBUTTON,NULL);   
	}

}

//功能：判断点 pt 是否在区域 ptArr(首尾点相同) 内
//实现：根据射线法求交点个数，偶数－区域外，奇数－区域内
//变量：pt 指定点 ptArr 判断区域
//返回：在区域 TRUE 不在 FALSE
BOOL CDlgLXInput::PointIsInPolygon(AcGePoint3d pt, AcGePoint3dArray& ptArr)
{
	//如果在节点上,直接返回
	int ptNum = ptArr.length();
	for(int j=0; j<ptNum-1; j++)
	{
		if((fabs(ptArr[j].x-pt.x)<0.01) && (fabs(ptArr[j].y-pt.y)<0.01))
		{
			return false;
		}
	}

	//射线法
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


//注意点数组闭合(首尾点相同)
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
	//读文件
	CFile file;
	CString strFName;
	strLyNameArray.RemoveAll();
	strFName.Format("%sLayerInfo.ini",gGetCurArxAppPath("shYDsys.arx"));
	if(file.Open(strFName,CFile::modeRead)==FALSE) return FALSE; //打开文件失败,直接退出
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

	//提取信息
	int nPos = 0;	
	char chTag[3]={0x0D,0x0A,0};
	int nTagLen = strTag.GetLength();
	while((nPos=strValue.Find(strTag))>=0)
	{
		int nPosDA = strValue.Find(chTag,nPos); //寻找回车换行位置
		CString strTemp= strValue.Mid(nPos+nTagLen,nPosDA-nPos-nTagLen); 
		strTemp.TrimLeft(); strTemp.TrimRight();
		strLyNameArray.Add(strTemp);
		strValue = strValue.Right(strValue.GetLength()-nPosDA);
	}

	return TRUE;
}


//地块重叠交叉检测(注意点数组需要闭合)
BOOL CDlgLXInput::CheckCrossOrContain(AcGePoint3dArray& ptArray,CString strSelfSdeLyName)
{
	if(m_pConnect == NULL)
	{
		acutPrintf("\nSDE 数据库 没有连接！"); return FALSE;
	}

	CStringArray strArrayCheckLy;
	ReadCfgLyNameArray(strArrayCheckLy,"[避让分析图层]=");
	int nCheck = strArrayCheckLy.GetCount();
	if(nCheck == 0) return TRUE; //没有配置图层，就默认为通过

	
	int nPtCount = ptArray.length();
	IDistPoint* pts = new IDistPoint[nPtCount];
	for(int k=0; k<nPtCount; k++)
	{
		pts[k].x = ptArray[k].x; pts[k].y = ptArray[k].y;
	}

	for(int i=0; i<nCheck; i++)
	{
		CString strCheckSde = strArrayCheckLy.GetAt(i);
		if (strCheckSde.CompareNoCase(strSelfSdeLyName) == 0) continue;// 不检测自身图层
		char strTempSQL[100]={0};
		sprintf(strTempSQL,"F:OBJECTID,T:%s,##",strCheckSde);
		if(1!=gCheckCrossOrContain(m_pConnect,strCheckSde.GetBuffer(0),strTempSQL,pts,nPtCount))
		{
			CString strError;
			strError.Format("当前地块与SDE图层[%s]有交叉重叠,您需要继续操作吗?[是:忽略 否:退出]",strCheckSde);
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
