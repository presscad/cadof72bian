#include "StdAfx.h"
#include "..\SdeData\DistExchangeInterface.h"
#include "..\SdeData\DistBaseInterface.h"

#include "DistPrjCommand.h"
#include "DistSysFun.h"
#include "DistEntityTool.h"
#include "AccessWebServiceInfo.h"
#include "DlgReadInfo.h"
#include "DistPersistentObjReactor.h"
#include "DistDocManagerReactor.h"
#include "DistUiPrompts.h" 
#include "DistXData.h"
#include "DlgPathSet.h"
#include "DkDataCheck.h"
#include "DistSelSet.h"

//-----------------------------------------------------------------------------
#define szRDS _RXST("sh")

IDistConnection*    g_pConnection=NULL;         //SDE数据库连接对象指针
IDistExchangeData*  g_pExchangeData=NULL;       //SDE<->CAD数据交换对象指针
HWND                g_DAPHwnd = NULL;           //保存DAP主窗体窗口函数指针

BOOL				g_bSupperPower = FALSE;     //是否为超级用户标记
BOOL				g_bIsInitial   = FALSE;     //是否正在初始化

//删除从 SDE 库中调出的CAD图层
__declspec(dllimport) int WINAPI DistDeleteSdeLys();

//创建图层树
__declspec(dllimport) int WINAPI DistCreateTree(char* strServerName,char* strInstanceName,
												char* strDBName,char* strUserName,char* strPassword);

//设置权限
__declspec(dllimport) int WINAPI DistSetTreeLyPowerAndReCreate(int* pIdArray,int* pPowerArray,int* pNum);


//获取SDE图层名称和权限值
__declspec(dllimport) int WINAPI DistGetTreeLyPower(char*** strSdeLyNameArray,int** pPowerArray,int* pNum);


////材料袋 FTP 下载
//__declspec(dllimport) int WINAPI DistDownLoadFromFTP(char* strServer,char* strUserName,char* strPassword,int nPort,char* strPrjId);


//根据内容，重新创建工具条
__declspec(dllimport) int WINAPI ReCreateToolBar(CStringArray* pArray);

//设置图层树钩选状态
__declspec(dllimport) int WINAPI DistSetTreeCheckState(CStringArray* strArrayCheck);

extern long __cdecl acdbSetDbmod(class AcDbDatabase *,long);

//----- ObjectARX EntryPoint
//类声明

class CYDsysApp : public AcRxArxApp 
{
public:

	CYDsysApp ();
public:
	//Arx初始消息函数
	virtual AcRx::AppRetCode On_kInitAppMsg (void *pkt);

	//Arx卸载消息函数
	virtual AcRx::AppRetCode On_kUnloadAppMsg (void *pkt);

	//注册控件服务函数
	virtual void RegisterServerComponents ();

	//线程函数
	static DWORD WINAPI ThreadFunc(LPVOID threadNum);

	//自定义窗口函数
	static LRESULT CALLBACK DistProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	//系统初始化函数
	static void InitSystem();


	///////////////////////////////////////// 命令函数 //////////////////////////////////////

	static void shYDsysDISTChk();
	static void shYDsysDISTLX();
	static void shYDsysDISTLXSX();
	static void shYDsysDISTXMXX();
	static void shYDsysDISTLXZB();

	static void shYDsysDISTLookInfo();
	//static void shYDsysDISTFtpDownload();
	//static void shYDsysDISTSaveBackDwg();
	//static void shYDsysDISTOpenBackDwg();

	static void shYDsysDISTReadSDE();
	//static void shYDsysDISTPATHSET();
	//static void shYDsysDISTPower();

	static void shYDsysDISTGWS(); //DISTGetWorkState

	static void shYDsysDISTSaveWorkDWG();


private:
	///////////////////////////////////////// 辅助函数 //////////////////////////////////////

	static void ProcessDataFromDap(CString strData);
	static void ProcessCommandFromDap(CString strData);

	static BOOL IsRoleHavePower(CString strCmd);

private:

	static 	WNDPROC	m_OldProc;              //保存CAD主窗体窗口函数指针
	static  CDistCommandManagerReactor*     m_pCmdReactor;  //文档反应器
	
	//DAP 系统信息
	static CString m_strWorkFile;		   // 当前工作文件
	static CString m_strWebServiceURL;     // WebService访问地址
    static CString m_strDapUserId;	       // 登录用户ID
	static CString m_strPROJECTID;         // 当前项目编号
	static CString m_strXMLX;              // 项目类型（"选址","用地","红线","预选","储备"）
	static CString m_strRoleId;            // 用户角色ID
	static CString m_strPreProjectId;      // 上阶段流程ID(业务系统要求添加)
	static CString m_strDapProcessId;      // DAP进程ID
	///临时
	//static CString m_strXZQH;              // 行政区划名称(控制分局的显示范围)
	static CString m_strSDEConnectionXML;  // SDE连接XML


	// 根据当前项目类型确定操作图层名称
	static CString m_strCadLyNameDK;
	static CString m_strSdeLyNameDK;

	static CString m_strCadLyNameXM;
	static CString m_strSdeLyNameXM;

	static CDlgReadInfo* m_pDlgInfo;

	static CStringArray  m_strPowerArray;


} ;

//注册CAD命令
IMPLEMENT_ARX_ENTRYPOINT(CYDsysApp)
ACED_ARXCOMMAND_ENTRY_AUTO(CYDsysApp, shYDsys, DISTChk,DISTChk, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CYDsysApp, shYDsys, DISTLX,DISTLX, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CYDsysApp, shYDsys, DISTXMXX, DISTXMXX, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CYDsysApp, shYDsys, DISTLXSX, DISTLXSX, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CYDsysApp, shYDsys, DISTLXZB, DISTLXZB, ACRX_CMD_TRANSPARENT, NULL)
//ACED_ARXCOMMAND_ENTRY_AUTO(CYDsysApp, shYDsys, DISTLXZBALL, DISTLXZBAll, ACRX_CMD_TRANSPARENT, NULL)

ACED_ARXCOMMAND_ENTRY_AUTO(CYDsysApp, shYDsys, DISTReadSDE, DISTReadSDE, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CYDsysApp, shYDsys, DISTLookInfo, DISTLookInfo, ACRX_CMD_TRANSPARENT, NULL)
//ACED_ARXCOMMAND_ENTRY_AUTO(CYDsysApp, shYDsys, DISTFtpDownload, DISTFtpDownload, ACRX_CMD_TRANSPARENT, NULL)
//ACED_ARXCOMMAND_ENTRY_AUTO(CYDsysApp, shYDsys, DISTSaveBackDwg, DISTSaveBackDwg, ACRX_CMD_TRANSPARENT, NULL)
//ACED_ARXCOMMAND_ENTRY_AUTO(CYDsysApp, shYDsys, DISTOpenBackDwg, DISTOpenBackDwg, ACRX_CMD_TRANSPARENT, NULL)

//ACED_ARXCOMMAND_ENTRY_AUTO(CYDsysApp, shYDsys, DISTPATHSET, DISTPATHSET, ACRX_CMD_TRANSPARENT, NULL)
//ACED_ARXCOMMAND_ENTRY_AUTO(CYDsysApp, shYDsys, DISTPower, DISTPower, ACRX_CMD_TRANSPARENT, NULL)

ACED_ARXCOMMAND_ENTRY_AUTO(CYDsysApp, shYDsys, DISTGWS, DISTGWS, ACRX_CMD_TRANSPARENT, NULL) //DISTGetWorkState
ACED_ARXCOMMAND_ENTRY_AUTO(CYDsysApp, shYDsys, DISTSaveWorkDWG, DISTSaveWorkDWG, ACRX_CMD_TRANSPARENT, NULL)

//类实现部分

WNDPROC        CYDsysApp::m_OldProc = NULL;       //保存CAD主窗体窗口函数指针

CDistCommandManagerReactor*   CYDsysApp::m_pCmdReactor = NULL;

//DAP 系统信息  
CString CYDsysApp::m_strWorkFile="";

///临时
CString CYDsysApp::m_strWebServiceURL="";
//CString CYDsysApp::m_strWebServiceURL="http://192.9.200.87/xmweb/WebService/xmwebService.asmx?WSDL";
//"http://192.168.1.41/wh/webservice/XMWebService.asmx?WSDL";  // WebService访问地址
CString CYDsysApp::m_strSDEConnectionXML="";
//CString CYDsysApp::m_strXZQH="市区";            // 行政区划名称

CString CYDsysApp::m_strDapUserId="";	        // 登录用户ID
CString CYDsysApp::m_strPROJECTID="";           // 当前项目编号
CString CYDsysApp::m_strXMLX="";                // 项目类型
CString CYDsysApp::m_strRoleId="";              // 用户角色ID
CString CYDsysApp::m_strDapProcessId ="";      
CString CYDsysApp::m_strPreProjectId="";        // 上阶段流程ID(业务系统要求添加)

CString CYDsysApp::m_strCadLyNameDK = "SDE_选址图层";
// 临时
//CString CYDsysApp::m_strSdeLyNameDK = "XMGHSDE.SP_XZLX_ZB_POLYGON";
CString CYDsysApp::m_strSdeLyNameDK = "SDE.SP_XZLX_ZB_POLYGON";

CString CYDsysApp::m_strCadLyNameXM = "SDE_选址项目信息图层";
// 临时
//CString CYDsysApp::m_strSdeLyNameXM = "XMGHSDE.SP_XZLX_ZB_PT";
CString CYDsysApp::m_strSdeLyNameXM = "SDE.SP_XZLX_ZB_PT";

CDlgReadInfo* CYDsysApp::m_pDlgInfo = NULL;

CStringArray  CYDsysApp::m_strPowerArray;

                             
//////////////////////////////////////////// 消息函数 ////////////////////////////////////////////////
CYDsysApp::CYDsysApp () : AcRxArxApp () 
{
	//
}

AcRx::AppRetCode CYDsysApp::On_kInitAppMsg (void *pkt) 
{
	AcRx::AppRetCode retCode =AcRxArxApp::On_kInitAppMsg (pkt) ;

	m_strPowerArray.RemoveAll();

	CDistPersistentObjReactor::rxInit();
	if(NULL == m_pCmdReactor)
	{
		m_pCmdReactor = new CDistCommandManagerReactor(true);
		m_pCmdReactor->m_bSave = FALSE;
	}
	acrxBuildClassHierarchy();

	CMDIFrameWnd* lpMainFrame = acedGetAcadFrame();
	HWND hFrame = lpMainFrame->m_hWnd;

	//更改窗口消息处理函数
	m_OldProc = (WNDPROC)SetWindowLong(hFrame, GWL_WNDPROC, (LONG)DistProc);

	//创建互斥对象
	HANDLE hSHPlan = CreateEvent(NULL, TRUE, FALSE,"SH-YDCSP-3AC01994-FD16-4F4F-9BA7-A5F27AE8CC76");
	HANDLE hEvent  = CreateEvent(NULL, TRUE, FALSE,"hust_{62B6D001-7766-514f-D8BB-58549A777A7A}");
	SetEvent(hEvent);
	CloseHandle(hEvent);

	//设置标题
	lpMainFrame->SetWindowText("用地审批制图系统");
	lpMainFrame->UpdateWindow();
	return (retCode) ;
}


AcRx::AppRetCode CYDsysApp::On_kUnloadAppMsg (void *pkt) 
{
	AcRx::AppRetCode retCode =AcRxArxApp::On_kUnloadAppMsg (pkt) ;

	if(NULL != g_pConnection)
	{
		g_pConnection->Release();g_pConnection = NULL;
	}
	if(NULL != g_pExchangeData)
	{
		g_pExchangeData->Release(); g_pExchangeData = NULL;
	}

	CPowerConfig::Release();
	if(NULL != m_pCmdReactor)
	{
		delete m_pCmdReactor;
		m_pCmdReactor = NULL;
	}

	deleteAcRxClass(CDistPersistentObjReactor::desc());

	if(m_pDlgInfo != NULL)
	{
		delete m_pDlgInfo; m_pDlgInfo = NULL;
	}


	return (retCode) ;
}

void CYDsysApp::RegisterServerComponents () 
{
	//
}


//////////////////////////////////////////// 命令函数 ////////////////////////////////////////////////
BOOL CYDsysApp::IsRoleHavePower(CString strCmd)
{
	int nLen = m_strPowerArray.GetCount();
	///临时
	if (nLen == 0)
	{
		return TRUE;
	}
	BOOL bFound = FALSE;
	for(int i=0; i<nLen; i++)
	{
		CString strTemp = m_strPowerArray.GetAt(i);
		if(strCmd.CompareNoCase(strTemp) == 0)
		{
			bFound = TRUE; break;
		}
	}
	if(bFound == FALSE)
		acutPrintf("\n该命令当前角色不能使用!!!");
	return bFound;
}


void CYDsysApp::shYDsysDISTChk()
{
	if(IsRoleHavePower("DISTChk")==FALSE) return;

	//设置过滤器
	ads_name ssName;
	char strLyName[260]={0};memset(strLyName,0,sizeof(char)*260);
	strcpy(strLyName,m_strCadLyNameDK.GetBuffer(0));
	struct resbuf filter;
	filter.restype=AcDb::kDxfLayerName;
	filter.resval.rstring = strLyName;
	filter.rbnext=NULL;

	CDistSelSet tempSel;
	if(CDistSelSet::kDistCanceled == tempSel.UserSelect(&filter))
	{
		acutPrintf("\n操作被取消！"); return;
	}

	AcDbObjectIdArray ObjIdArray;
	tempSel.AsArray(ObjIdArray);

	if(ObjIdArray.isEmpty())
	{
		acutPrintf("\n没有选中实体!"); return;
	}

	CDkDataCheck tempCheck;
	int nCount = ObjIdArray.length();
	for(int i=0; i<nCount; i++)
	{
		AcDbObjectId tempObjId = ObjIdArray.at(i);
		if(FALSE == tempCheck.CheckEntity(tempObjId)) return;
	}

	for(i=0;i<nCount-1;i++)
	{
		AcDbObjectId startId = ObjIdArray.at(i);
		for(int j=i+1; j<nCount; j++)
		{
			AcDbObjectId endId = ObjIdArray.at(j);
			if(tempCheck.TwoPolygonIsIntersect(startId,endId) != TRUE) return;
		}
	}	

	AfxMessageBox("蓝线地块自身检测通过！");
}


void CYDsysApp::shYDsysDISTLX()  
{
	if(IsRoleHavePower("DISTLX")==FALSE) return;
	gCmdCreateLXBound(m_strCadLyNameDK,5,2,m_strSdeLyNameDK);
}

void CYDsysApp::shYDsysDISTLXSX()
{
	if(IsRoleHavePower("DISTLXSX")==FALSE) return;
	gTestInput(m_strCadLyNameDK,m_strCadLyNameXM,m_strSdeLyNameDK,m_strSdeLyNameXM,m_strWebServiceURL,m_strDapProcessId);
}

void CYDsysApp::shYDsysDISTXMXX()
{
	if(IsRoleHavePower("DISTXMXX")==FALSE) return;
	gCmdSearchXMXX(m_strCadLyNameDK,m_strSdeLyNameDK);
}

void CYDsysApp::shYDsysDISTLXZB()  
{
	if(IsRoleHavePower("DISTLXZB")==FALSE) return;
	int nType=1;
	//交互选择操作类型
	CDistUiPrIntDef prIntType("\n选择类型 (1)选择标注 (2)连续标注 (3)删除标注  默认",NULL,CDistUiPrInt::kRange,1); 
	prIntType.SetRange(1,3);
	if(prIntType.Go()!=CDistUiPrBase::kOk) return;
	nType = prIntType.GetValue();

	CString strCadLyName;
	strCadLyName.Format("%s坐标标注",m_strCadLyNameDK);
	AcDbObjectId resultId;
	CDistEntityTool tempTool;
	tempTool.CreateLayer(resultId,strCadLyName.GetBuffer(0));

	if(nType==1)
		gCmdCreatLabel(m_strCadLyNameDK);
	else if(nType==2)
		gCmdCreatLabelAll(m_strCadLyNameDK);
	else
		gCmdDeleteLabel(m_strCadLyNameDK);
}


void CYDsysApp::shYDsysDISTReadSDE()
{
	if(IsRoleHavePower("DISTReadSDE")==FALSE) return;
	//检查目录树（预防性检测）
	char** strLyNames = NULL;
	int* pPower = NULL;
	int nNum = 0;
	if(1!=DistGetTreeLyPower(&strLyNames,&pPower,&nNum)) return;
	if(nNum == 0) return;

	CStringArray strArray;
	for(int i=0; i<nNum; i++)
		strArray.Add(strLyNames[i]);
	
	gCmdReadSdeLys(strArray,/*NULL,*/m_strSdeLyNameDK,m_strPROJECTID/*,m_strXZQH*/);
}


void CYDsysApp::shYDsysDISTLookInfo()
{
	if(IsRoleHavePower("DISTLookInfo")==FALSE) return;

	//交互选择操作类型
	CDistUiPrIntDef prIntType("\n选择类型 (1)查看实体属性 (2)选择项目相关地块  默认",NULL,CDistUiPrInt::kRange,1); 
	prIntType.SetRange(1,2);
	if(prIntType.Go()!=CDistUiPrBase::kOk) return;
	int nType = prIntType.GetValue();
	
	if(nType == 1)
	{
		CDlgReadInfo dlg;
		dlg.DoModal();
	}
	else
	{
		CDistEntityTool tempTool;
		tempTool.SendCommandToAutoCAD("LOOKDK ");
	}
}



//void CYDsysApp::shYDsysDISTFtpDownload()
//{
//	if(IsRoleHavePower("DISTFtpDownload")==FALSE) return;
//
//	DistDownLoadFromFTP(CBaseCfgInfo::m_BaseFtpInfo.strServer.GetBuffer(0),
//		                CBaseCfgInfo::m_BaseFtpInfo.strUserName.GetBuffer(0),
//						CBaseCfgInfo::m_BaseFtpInfo.strPassword.GetBuffer(0),
//						CBaseCfgInfo::m_BaseFtpInfo.nPort,
//						m_strPROJECTID.GetBuffer(0));
//}


//void CYDsysApp::shYDsysDISTSaveBackDwg()
//{
//	if(IsRoleHavePower("DISTSaveBackDwg")==FALSE) return;
//
//	AcDbDatabase* pCurDB = acdbHostApplicationServices()->workingDatabase();
//	char* strFileName = NULL;
//	pCurDB->getFilename(strFileName);
//	CString strF;
//	strF.Format("你确认要保存当前草图[%s]吗?",strFileName);
//	if(AfxMessageBox(strF,MB_YESNO) == IDYES)
//	{
//		gCmdSaveBackDwg(m_pCmdReactor);
//	}
//}

//void CYDsysApp::shYDsysDISTPATHSET()
//{
//	if(IsRoleHavePower("DISTPATHSET")==FALSE) return;
//
//	CDlgPathSet dlg;
//	dlg.DoModal();
//}

//void CYDsysApp::shYDsysDISTPower()  //设置超级权限
//{
//	if(IsRoleHavePower("DISTPOWER")==FALSE) return;
//	g_bSupperPower = !g_bSupperPower;
//	if(g_bSupperPower)
//		acutPrintf("\n超级权限已被打开！");
//	else
//		acutPrintf("\n超级权限已被关闭！");
//}

//void CYDsysApp::shYDsysDISTOpenBackDwg()
//{
//	if(IsRoleHavePower("DISTOpenBackDwg")==FALSE) return;
//	 gCmdOpenBackDwg();
//}

void CYDsysApp::shYDsysDISTGWS() //DISTGetWorkState
{
	if (!g_bIsInitial)
		return;

	//获取并设置图层树钩选状态
	DistSetTreeCheckState(gGetTreeCheckStateInSDE(g_pConnection, m_strPROJECTID.GetBuffer(0)));
	m_strPROJECTID.ReleaseBuffer();

	////获取查询状态并自动查询
	//CStringArray* strArraySDELayerNames = new CStringArray(); 
	//CStringArray* strArrayMINXs = new CStringArray();
	//CStringArray* strArrayMAXXs = new CStringArray();
	//CStringArray* strArrayMINYs = new CStringArray();
	//CStringArray* strArrayMAXYs = new CStringArray();

	//if (gGetQueryLayerInSDE(g_pConnection, m_strPROJECTID.GetBuffer(0),
	//						strArraySDELayerNames, strArrayMINXs, strArrayMAXXs, strArrayMINYs, strArrayMAXYs))
	//{
	//	m_strPROJECTID.ReleaseBuffer();
	//	for (int i = 0; i < strArraySDELayerNames->GetCount(); i++)
	//		gCmdReadSdeLayerByRect(strArraySDELayerNames->GetAt(i),
	//							strArrayMINXs->GetAt(i),strArrayMAXXs->GetAt(i),
	//							strArrayMINYs->GetAt(i),strArrayMAXYs->GetAt(i),
	//							m_strPROJECTID);
	//}
	//else
	//{
	//	m_strPROJECTID.ReleaseBuffer();
	//}

	//获取视口并定位
	CDistEntityTool tempTool;
	CStringArray* strArraySDELayerNames = new CStringArray(); 
	CStringArray* strArrayMINXs = new CStringArray();
	CStringArray* strArrayMAXXs = new CStringArray();
	CStringArray* strArrayMINYs = new CStringArray();
	CStringArray* strArrayMAXYs = new CStringArray();

	if (gGetQueryLayerInSDE(g_pConnection, m_strPROJECTID.GetBuffer(0),
							strArraySDELayerNames, strArrayMINXs, strArrayMAXXs, strArrayMINYs, strArrayMAXYs))
	{
		m_strPROJECTID.ReleaseBuffer();

		// 定位
		double dminX=0,dminY=0,dmaxX=0,dmaxY=0;
		for (int i = 0; i < strArraySDELayerNames->GetCount(); i++)
		{
			if (strArraySDELayerNames->GetAt(i) == "View")
			{
				dminX=atof(strArrayMINXs->GetAt(0));
				dminY=atof(strArrayMINYs->GetAt(0));
				dmaxX=atof(strArrayMAXXs->GetAt(0));
				dmaxY=atof(strArrayMAXYs->GetAt(0));

				break;
			}
		}

		if (dminX==0 && dminY==0 && dmaxX==0 && dmaxY==0)
		{
			tempTool.SendCommandToAutoCAD("_ZOOM E ");
		}
		else
		{
			AcGePoint2d CenterPt;
			double dHeight,dWidth;
			//定位
			CenterPt.x = (dmaxX+dminX)/2; CenterPt.y = (dmaxY+dminY)/2;
			dHeight = dmaxY-dminY; dWidth =dmaxX-dminX;
			tempTool.DyZoom(CenterPt,dHeight,dWidth);

			//double p1[3],p2[3];
			//p1[0] = dminX;
			//p1[1] = dminY;
			//p1[2] = 0;
			//p2[0] = dmaxX;
			//p2[1] = dmaxY;
			//p2[2] = 0;
			//struct resbuf * rb_cmdecho_Off = acutBuildList(RTSHORT,0,0);
			//struct resbuf * rb_cmdecho_On = acutBuildList(RTSHORT,1,0);
			//acedSetVar("cmdecho", rb_cmdecho_Off);
			//acedCommand(RTSTR, "ZOOM", RTSTR, "w",  RT3DPOINT, p1, RT3DPOINT, p2 ,0);
			//acedSetVar("cmdecho", rb_cmdecho_On);
			//acutRelRb(rb_cmdecho_Off);
			//acutRelRb(rb_cmdecho_On);

			//tempTool.SendCommandToAutoCAD("_u ");
		}
	}
	else
	{
		m_strPROJECTID.ReleaseBuffer();

		tempTool.SendCommandToAutoCAD("_ZOOM E ");
	}

	g_bIsInitial = FALSE;
}

void CYDsysApp::shYDsysDISTSaveWorkDWG()
{
	ProcessCommandFromDap("GIVECOMMAND:CADCONTAINER_SAVE");
}

//////////////////////////////////////////// 辅助函数 ////////////////////////////////////////////////

//string message = "GIVEDATA ap|CADTYPE:预选|WEB_HTTP:" + Common.CurStatic.WebServiceUrl + "XMWebService.asmx?WSDL" + "|";
// 处理从dap传输过来的项目信息
void CYDsysApp::ProcessDataFromDap(CString strData)
{
	OutputDebugString(strData);
	int nTempLen = strData.GetLength();
	if(strData.GetAt(nTempLen-1) != '|')  strData.SetAt(nTempLen-1,'|');

	int nPos = strData.Find("USER_ID:");  //用户ID
	if (nPos >= 0)
	{
		nPos = nPos +  + strlen("USER_ID:");
		int nTemp=strData.Find('|',nPos);
		if(nTemp > nPos)
		{
			m_strDapUserId = strData.Mid(nPos, nTemp-nPos); m_strDapUserId.Trim();
			OutputDebugString(m_strDapUserId);
		}
	}

	nPos = strData.Find("PROJECT_ID:");   //项目编号
	if (nPos >=0 )
	{
		nPos = nPos + strlen("PROJECT_ID:");
		int nTemp=strData.Find('|',nPos);
		if(nTemp > nPos)
		{
			m_strPROJECTID = strData.Mid(nPos, nTemp-nPos);  m_strPROJECTID.Trim(); 
			OutputDebugString(m_strPROJECTID);
		}
	}

	nPos = strData.Find("FILE_FULLNAME"); //打开文件地址
	if (nPos >= 0)
	{
		nPos = nPos + strlen("FILE_FULLNAME:");

		int nTemp=strData.Find('|',nPos);
		if(nTemp > nPos)
		{
			m_strWorkFile = strData.Mid(nPos, nTemp-nPos);
			m_strWorkFile.Trim();

			OutputDebugString(m_strWorkFile);
		}
	}

	nPos = strData.Find("SDE_CONN:");
	if (nPos >= 0)
	{
		nPos = nPos + strlen("SDE_CONN:"); //SDE连接信息

		int nTemp=strData.Find('|',nPos);
		if(nTemp > nPos)
		{
			m_strSDEConnectionXML = strData.Mid(nPos, nTemp-nPos); m_strWebServiceURL.Trim();
			OutputDebugString(m_strSDEConnectionXML);
		}
	}

	nPos = strData.Find("WEB_HTTP:");
	if (nPos >= 0)
	{
		nPos = nPos + strlen("WEB_HTTP:"); //WebServices地址

		int nTemp=strData.Find('|',nPos);
		if(nTemp > nPos)
		{
			m_strWebServiceURL = strData.Mid(nPos, nTemp-nPos); m_strWebServiceURL.Trim();
			OutputDebugString(m_strWebServiceURL);
		}
	}

	nPos = strData.Find("ROLE_ID:");  //角色ID
	if (nPos >= 0)
	{
		nPos = nPos + strlen("ROLE_ID:");

		int nTemp=strData.Find('|',nPos);
		if(nTemp > nPos)
		{
			m_strRoleId = strData.Mid(nPos, nTemp-nPos);  m_strRoleId.Trim();
			OutputDebugString(m_strRoleId);
		}
	}

	nPos = strData.Find("PROCESS_ID:");  //流程ID
	if (nPos >= 0)
	{
		nPos = nPos + strlen("PROCESS_ID:"); 

		int nTemp=strData.Find('|',nPos);
		if(nTemp > nPos)
		{
			m_strDapProcessId = strData.Mid(nPos, nTemp-nPos); m_strDapProcessId.Trim();
			OutputDebugString(m_strDapProcessId);
		}
	}

	nPos = strData.Find("PRE_PROJECT_ID:");		   // 上阶段流程ID(业务系统要求添加)
	if (nPos >= 0)								   
	{
		nPos = nPos + strlen("PRE_PROJECT_ID:"); 
		int nTemp=strData.Find('|',nPos);
		if(nTemp > nPos)
		{
			m_strPreProjectId = strData.Mid(nPos, nTemp-nPos); m_strPreProjectId.Trim();
			OutputDebugString(m_strPreProjectId);
		}
	}

	///临时
	//nPos = strData.Find("RECT:"); // 行政区划名称(控制分局的显示范围)
	//if (nPos >= 0)
	//{
	//	nPos = nPos + strlen("RECT:"); 
	//	int nTemp=strData.Find('|',nPos);
	//	if(nTemp > nPos)
	//	{
	//		m_strXZQH = strData.Mid(nPos, nTemp-nPos);  m_strXZQH.Trim();
	//		OutputDebugString(m_strXZQH);
	//	}
	//}

	nPos = strData.Find("CADTYPE:");  //项目类型（"选址","用地","红线","预选","储备"）
	if (nPos >= 0)
	{
		nPos = nPos + strlen("CADTYPE:"); 
		int nTemp=strData.Find('|',nPos);
		if(nTemp > nPos)
		{
			m_strXMLX = strData.Mid(nPos, nTemp-nPos); m_strXMLX.Trim();
			OutputDebugString(m_strXMLX);
		}
	}
}

//获取当前视口
void /*CCadView::*/GetCurrentView(AcDbViewTableRecord& viewTR)
{
	double height;
	double width;
	struct resbuf var;
	struct resbuf WCS, UCS, DCS;
	WCS.restype = RTSHORT;
	WCS.resval.rint = 0;
	UCS.restype = RTSHORT;
	UCS.resval.rint = 1;
	DCS.restype = RTSHORT;
	DCS.resval.rint = 2;
    
	acedGetVar("VIEWMODE", &var);
	viewTR.setPerspectiveEnabled(var.resval.rint & 1);
	viewTR.setFrontClipEnabled((var.resval.rint & 2) != 0);
	viewTR.setBackClipEnabled((var.resval.rint & 4) != 0);
	viewTR.setFrontClipAtEye(!(var.resval.rint & 16));
    
	acedGetVar("BACKZ", &var);
	viewTR.setBackClipDistance(var.resval.rreal);
    
	acedGetVar("VIEWCTR", &var);
	acedTrans(var.resval.rpoint, &UCS, &DCS, NULL, var.resval.rpoint);
	viewTR.setCenterPoint(AcGePoint2d(var.resval.rpoint[X], var.resval.rpoint[Y]));
    
	acedGetVar("FRONTZ", &var);
	viewTR.setFrontClipDistance(var.resval.rreal);
    
	acedGetVar("LENSLENGTH", &var);
	viewTR.setLensLength(var.resval.rreal);
    
	acedGetVar("TARGET", &var);
	acedTrans(var.resval.rpoint, &UCS, &WCS, NULL, var.resval.rpoint);
	viewTR.setTarget(AcGePoint3d(var.resval.rpoint[X], var.resval.rpoint[Y], var.resval.rpoint[Z]));
    
	acedGetVar("VIEWDIR", &var);
	acedTrans(var.resval.rpoint, &UCS, &WCS, TRUE, var.resval.rpoint);
	viewTR.setViewDirection(AcGeVector3d(var.resval.rpoint[X], var.resval.rpoint[Y], var.resval.rpoint[Z]));

	acedGetVar("VIEWSIZE", &var);
	viewTR.setHeight(var.resval.rreal);

	acedGetVar("VSMIN", &var);
	acedTrans (var.resval.rpoint, &UCS, &DCS, NULL, var.resval.rpoint);
	width = var.resval.rpoint[X];
	height = var.resval.rpoint[Y];

	acedGetVar("VSMAX", &var);
	acedTrans (var.resval.rpoint, &UCS, &DCS, NULL, var.resval.rpoint);
	width = fabs(var.resval.rpoint[X] - width);
	height = fabs(var.resval.rpoint[Y] - height);
	
	viewTR.setWidth(width * viewTR.height() / height);
}
// 处理从dap发送过来的cad命令
void CYDsysApp::ProcessCommandFromDap(CString strData)
{
	int nPos = 0;
	CString strCommand = strData.Right(strData.GetLength()-strlen("GIVECOMMAND:"));

	if(strCommand.Find("APPLICATION_EXIT")>=0)  //程序将要退出
	{
		if(NULL != g_pConnection)
		{
			g_pConnection->Release();g_pConnection = NULL;

		}
		if(NULL != g_pExchangeData)
		{
			g_pExchangeData->Release(); g_pExchangeData = NULL;
		}

		CMDIFrameWnd* lpMainFrame = acedGetAcadFrame();
		::SendMessage(lpMainFrame->m_hWnd,WM_CLOSE,0,0);
		return;
	}

	if(strCommand.Find("CADCONTAINER_HIDE")>=0
		|| strCommand.Find("CADCONTAINER_SAVE")>=0)  //退出CAD表单,CAD隐藏 //保存并上传DWG文件
	{
		AcApDocument* pDoc = acDocManager->curDocument();
		if (m_strWorkFile.CompareNoCase(pDoc->fileName()) != 0)
		{
			AcApDocumentIterator* pIter = acDocManager->newAcApDocumentIterator();
			if(NULL != pIter)
			{
				for (; !pIter->done(); pIter->step()) 
				{
					AcApDocument* pDoc = pIter->document();
					if (m_strWorkFile.CompareNoCase(pDoc->fileName()) == 0)
					{
						acDocManager->setCurDocument(pDoc);
						break;
					}
				}
				delete pIter; pIter=NULL;
			}
		}

		if (m_strWorkFile.CompareNoCase(pDoc->fileName()) == 0)
		{
			if (strCommand.Find("CADCONTAINER_SAVE")>=0)
			{
				//保存视口
				double dminX=0,dminY=0,dmaxX=0,dmaxY=0;
				//AcDbViewportTable* pViewportTable;
				//if (pDoc->database()->getViewportTable(pViewportTable, AcDb::kForRead)
				//	== Acad::eOk)
				//{
				//	// Find the first viewport and open it for read. 
				//	AcDbViewportTableRecord *pRecord;
				//	if (pViewportTable->getAt(
				//		"*ACTIVE", pRecord, 
				//		AcDb::kForRead) == Acad::eOk)
				//	{
						AcDbViewTableRecord *pView = new AcDbViewTableRecord();
						GetCurrentView(*pView);

						AcGePoint2d CenterPt = pView->centerPoint();
						double Width = pView->width();
						double Height = pView->height();

						dminX = CenterPt.x - ( Width / 2.0 );
						dmaxX = CenterPt.x + ( Width / 2.0 );
						dminY = CenterPt.y - ( Height / 2.0 );
						dmaxY = CenterPt.y + ( Height / 2.0 );

						pView->close();
						delete pView;
				//		
				//		pRecord->close(); 
				//	} 
				//	pViewportTable->close();
				//}

				gSaveQueryLayerToSDE(g_pConnection, m_strPROJECTID.GetBuffer(0), 
										"View",	&dminX, &dmaxX, &dminY, &dmaxY);
				m_strPROJECTID.ReleaseBuffer();
			}

			m_pCmdReactor->m_bSave = TRUE; //打开命令开关

			////客户选择是否保存
			//CDistEntityTool tempTool;
			//tempTool.SendCommandToAutoCAD("_qsave ");
			//Acad::ErrorStatus st;
			////AcApDocument *pDoc=acDocManager->curDocument();
			////st=acDocManager->lockDocument(pDoc);
			////if(st==Acad::eOk)
			////{
			////	//acDocManager->curDocument()->database()->save();
			////	CString tmpFileName = m_strWorkFile;
			////	tmpFileName.Replace("work.dwg", "tmp.dwg");
			////	acDocManager->curDocument()->database()->saveAs(tmpFileName);
			////	acDocManager->curDocument()->database()->saveAs(m_strWorkFile);
			////	acDocManager->unlockDocument(pDoc);
			//	st = acDocManager->closeDocument(pDoc);
			//	delete pDoc; pDoc = NULL;
			////}
			//CDistEntityTool tempTool;
			////if(AfxMessageBox("是否保存对图形所作的修改?",MB_YESNO)!=IDYES)
			////{
			////	tempTool.SendCommandToAutoCAD("(command \"Close\") ");
			////	tempTool.SendCommandToAutoCAD("N ");
			////}
			////else
			//{
			//	tempTool.SendCommandToAutoCAD("(command \"Save\" \"\") ");
			//	CString tmpFileName = m_strWorkFile;
			//	tmpFileName.Replace("work.dwg", "tmp.dwg");
			//	tmpFileName.Replace("\\", "\\\\");
			//	tempTool.SendCommandToAutoCAD("(command \"Save\" \"" + tmpFileName + "\") ");
			//	tempTool.SendCommandToAutoCAD("(command \"Close\") ");
			//}
			AcDbDatabase* pCurDb = pDoc->database();
			AcDbDatabase *pOutDb = new AcDbDatabase();
			if(pOutDb != NULL)
			{
				Acad::ErrorStatus es;
				es=acDocManager->lockDocument(pDoc);
				if(es==Acad::eOk)
				{
					es = pCurDb->wblock(pOutDb);
					if(es == Acad::eOk)
					{
						CString tmpFileName = m_strWorkFile;
						tmpFileName.Replace("work.dwg", "tmp.dwg");
						pOutDb->saveAs(tmpFileName);

						delete pOutDb; pOutDb = NULL;
					}

					pCurDb->saveAs(m_strWorkFile);

					acDocManager->unlockDocument(pDoc);

					if (strCommand.Find("CADCONTAINER_HIDE")>=0)
					{
						es = acDocManager->closeDocument(pDoc);
					}
				}
			}

			DWORD dwThreadId; 
			CreateThread(NULL,0,ThreadFunc,m_pCmdReactor,0,&dwThreadId);

			if (strCommand.Find("CADCONTAINER_SAVE")>=0)
			{
				//向DAP发送消息,回传工作DWG
				HWND hCadWnd = adsw_acadMainWnd();
				int nCadWnd = (int)hCadWnd;

				CString message = "GIVECOMMAND:UPLOADWORKDWG";

				COPYDATASTRUCT cds;
				cds.dwData = 0;
				cds.cbData = message.GetLength() + 1;
				cds.lpData = (void*)message.GetBuffer(0);
				::SendMessage(g_DAPHwnd, WM_COPYDATA, (WPARAM)nCadWnd, (LPARAM)&cds);
				message.ReleaseBuffer();
			}

			if (strCommand.Find("CADCONTAINER_HIDE")>=0)
			{
				//向DAP发送消息,删除文件
				HWND hCadWnd = adsw_acadMainWnd();
				int nCadWnd = (int)hCadWnd;

				CString message = "GIVECOMMAND:CLEANLOCALWORKDWG";
				message = "GIVECOMMAND:CLEANLOCALWORKDWG";

				COPYDATASTRUCT cds;
				cds.dwData = 0;
				cds.cbData = message.GetLength() + 1;
				cds.lpData = (void*)message.GetBuffer(0);
				::SendMessage(g_DAPHwnd, WM_COPYDATA, (WPARAM)nCadWnd, (LPARAM)&cds);
				message.ReleaseBuffer();
			}
		}

		return;
	}

	strCommand = strCommand + " ";
	CDistEntityTool tempTool;
	tempTool.SendCommandToAutoCAD(strCommand);
}


DWORD WINAPI CYDsysApp::ThreadFunc(LPVOID threadNum)
{
	DWORD dwCount=GetTickCount(); 
	while(1)
	{
		if(GetTickCount()-dwCount>=5000) 
		{
			//推迟执行的代码
			break;
		}
		Sleep(1);
	}
	return 1;
}

void ReduceString(CString strValue,CStringArray& strValueArray)
{
	strValueArray.RemoveAll();
	if(strValue.IsEmpty()) return;
	int nFound = 0;

	while((nFound=strValue.Find(','))>0)
	{
		CString strTemp = strValue.Left(nFound);
		if(nFound>0)
		{
			strValueArray.Add(strTemp);
			if(strValue.GetLength()-nFound-1 <= 0) break;
			strValue = strValue.Right(strValue.GetLength()-nFound-1);
		}
	}
}

//BOOL newDwg(CString strdwtFileName)
//{
//	Acad::ErrorStatus st;
//	AcApDocument *pDoc=acDocManager->curDocument();
//	st=acDocManager->lockDocument(pDoc);
//	if(st!=Acad::eOk)
//	{
//		acutPrintf("锁定文档失败");
//		return FALSE;
//	}
//	st = acDocManager->appContextNewDocument(strdwtFileName);//新建一个DWG
//	if(st!=Acad::eOk)
//	{
//		acutPrintf("新建文档失败");
//		return FALSE;
//	}
// 
//	acDocManager->unlockDocument(pDoc);
//
//	return TRUE;
//}

void CYDsysApp::InitSystem()
{
	//1.打开工作文件
	CDistEntityTool tempTool;
	tempTool.OpenDwgFile(m_strWorkFile);
	//if (m_strWorkFile.Right(strlen("草图.dwt")) == "草图.dwt")
	//{
	//	//newDwg(m_strWorkFile);

	//	m_strWorkFile.Replace("草图.dwt", "work.dwg");

	//	if (gIsFileExists(m_strWorkFile))
	//		CFile::Remove(m_strWorkFile);

	//	Acad::ErrorStatus st;
	//	AcApDocument *pDoc=acDocManager->curDocument();
	//	st=acDocManager->lockDocument(pDoc);
	//	if(st==Acad::eOk)
	//	{
	//		acDocManager->curDocument()->database()->saveAs(m_strWorkFile);
	//		acDocManager->unlockDocument(pDoc);
	//	}
	//	//AcDbDatabase *pDb = new AcDbDatabase();
	//	//if(pDb != NULL)
	//	//{
	//	//	if(Acad::eOk == acdbHostApplicationServices()->workingDatabase()->wblock(pDb))
	//	//	{
	//	//		if(Acad::eOk == pDb->saveAs(m_strWorkFile))
	//	//		{
	//	//		}
	//	//	}
	//	//	delete pDb;
	//	//}
	//}
	////else
	////{
	////	tempTool.OpenDwgFile(m_strWorkFile);
	////}
	
	//tempTool.SendCommandToAutoCAD("_ZOOM E ");

	//每个文件都保存工作文件后需要更改！！！
	CString tmpFileName = m_strWorkFile;
	if (tmpFileName.Find("work.dwg") > 0)
	{
		tmpFileName.Replace("work.dwg", "tmp.dwg");
		if (gIsFileExists(tmpFileName))
			CFile::Remove(tmpFileName);
	}

	//2.获取SDE登陆配置信息（服务器，实例，数据库，用户名，密码）
	//if(FALSE == CBaseCfgInfo::ReadConnectInfo(m_strWebServiceURL))
	if(FALSE == CBaseCfgInfo::ReadConnectInfoByXML(m_strSDEConnectionXML))
	{
		//OutputDebugString("CBaseCfgInfo::ReadConnectInfo 获取SDE登陆配置信息失败!");
		OutputDebugString("CBaseCfgInfo::ReadConnectInfoByXML 获取SDE登陆配置信息失败!");
	}
	else
	{
		//2.1.创建数据库连接对象和CAD与SDE互换对象
		if(NULL == g_pConnection)
		{
			g_pConnection = CreateSDEConnectObjcet();
			if(NULL == g_pConnection) 
			{
				OutputDebugString("CreateSDEConnectObjcet 创建SDE连接对象失败!"); return ;
			}
			long rc = -1;
			if((rc = g_pConnection->Connect(CBaseCfgInfo::m_BaseSdeInfo.strServer,
											CBaseCfgInfo::m_BaseSdeInfo.strInstance,
											CBaseCfgInfo::m_BaseSdeInfo.strDatabase,
											CBaseCfgInfo::m_BaseSdeInfo.strUserName,
											CBaseCfgInfo::m_BaseSdeInfo.strPassword)) != 1)
			{
				OutputDebugString("Connect 空间数据库连接失败");
				g_pConnection->Release(); g_pConnection = NULL; return ;
			}
		}

		if(NULL == g_pExchangeData)
		{
			g_pExchangeData = CreateExchangeObjcet();
			if(NULL == g_pExchangeData)
			{
				OutputDebugString("CreateSDEConnectObjcet 创建SDE与CAD互换类对象失败!");
				return ;
			}
		}

		//2.2.创建图层目录树
		DistCreateTree(CBaseCfgInfo::m_BaseSdeInfo.strServer.GetBuffer(0),
					   CBaseCfgInfo::m_BaseSdeInfo.strInstance.GetBuffer(0),
					   CBaseCfgInfo::m_BaseSdeInfo.strDatabase.GetBuffer(0),
					   CBaseCfgInfo::m_BaseSdeInfo.strUserName.GetBuffer(0),
					   CBaseCfgInfo::m_BaseSdeInfo.strPassword.GetBuffer(0));
	}

	////3.读 FTP 连接信息
	//if(FALSE == CBaseCfgInfo::ReadFtpInfo(m_strWebServiceURL))
	//{
	//	OutputDebugString("CBaseCfgInfo::ReadFtpInfo 获取FTP基本信息失败!");
	//}

	//4.读取项目基本信息（访问表TBPROJECT_BASE，具体参考项目信息结构）
	if(!m_strPROJECTID.IsEmpty())
	{
		if(FALSE == CBaseCfgInfo::ReadProjectBaseInfo(m_strPROJECTID,m_strWebServiceURL))
		{
			OutputDebugString("CBaseCfgInfo::ReadProjectBaseInfo 获取项目基本信息失败!");
		}
		else
		{
			OutputDebugString("m_strXMLX = CBaseCfgInfo::m_basePrjInfo.strCADTYPE");
			m_strXMLX = CBaseCfgInfo::m_basePrjInfo.strCADTYPE;
			OutputDebugString(m_strXMLX);
		}
	}

	//5.根据用户ID，获取用户详细信息（访问表TBUSER）
	if(FALSE == CBaseCfgInfo::ReadUserInfo(m_strDapUserId,m_strWebServiceURL))
	{
		OutputDebugString("CBaseCfgInfo::ReadUserInfo 获取当前用户信息失败!");
	}

	//5.1.显示功能树和Cad命令窗口
	//CDistEntityTool tempTool;
	tempTool.ShowCommandLineWindow(TRUE);	

	//临时
	////5.2.根据角色ID获取工具条权限控制信息
	//if(NULL != g_pConnection)
	//{
	//	IDistCommand* pCmd = CreateSDECommandObjcet();
	//	if(pCmd == NULL)
	//	{
	//		OutputDebugString("CreateSDECommandObjcet 创建SDE命令对象失败!");  return ;
	//	}
	//	pCmd->SetConnectObj(g_pConnection);

	//	char strSQL[255]={0};
	//	sprintf(strSQL,"F:POWER,T:TB_TOOLBAR_POWER,W:ROLEID='%s',##",m_strRoleId);
	//	IDistRecordSet* pRcdSet = NULL;
	//	if(1 != pCmd->SelectData(strSQL,&pRcdSet))
	//	{
	//		OutputDebugString("获取当前角色工具条权限属性值失败!");
	//		pCmd->Release(); pCmd = NULL; ///临时/*return;*/
	//	}
	//	else
	//	{
	//		CString strTempPower;
	//		if(pRcdSet->BatchRead() == 1)
	//		{
	//			strTempPower = (char*)(*pRcdSet)[0];
	//			OutputDebugString(strTempPower);
	//			ReduceString(strTempPower,m_strPowerArray);
	//		}
	//		if(pRcdSet != NULL)
	//			pRcdSet->Release();
	//		pCmd->Release();
	//	}
	//}

	////5.3.重建工具条
	//if(m_strPowerArray.GetCount()>0)
	//	ReCreateToolBar(&m_strPowerArray);
	//else
	//	ReCreateToolBar(NULL);

	////5.4.获取当前访问SDE图层的读写权限（访问表：TBRIGHT_FORM）
	//CPowerConfig::Release();
	//if(TRUE == CPowerConfig::GetCurUserRight(m_strWebServiceURL,m_strRoleId,m_strPROJECTID))
	//{
	//	//设定权限
	//	int nCount = CPowerConfig::m_PowerArray.GetCount();
	//	if(nCount>0)
	//	{
	//		int* pTreeId = new int[nCount];
	//		int* pPower  = new int[nCount];
	//		for(int k=0; k<nCount; k++)
	//		{
	//			POWERCONFIG temp = CPowerConfig::m_PowerArray.GetAt(k) ;
	//			pTreeId[k] = atoi(temp.strTreeID);
	//			pPower[k] = atoi(temp.strSDEPower);
	//		}
	//		acutPrintf("\n");
	//		DistSetTreeLyPowerAndReCreate(pTreeId,pPower,&nCount);
	//		delete [] pTreeId;
	//		delete [] pPower;
	//	}
	//}
	SendMessage(acedGetAcadFrame()->m_hWnd,WM_SIZE,0,0);

	//6.根据SDE层名称,获取对应的CAD层名称
	if(NULL != g_pConnection)
	{
		char strSQL[255]={0};
		sprintf(strSQL,"F:CADLYNAME,T:CADMAPTOSDE,W:SDELYNAME='%s',##",m_strSdeLyNameDK);
		OutputDebugString(strSQL);

		IDistCommand *pCommand = CreateSDECommandObjcet();
		if(NULL == pCommand) return;
		pCommand->SetConnectObj(g_pConnection);
		IDistRecordSet* pRcdSet = NULL;
		if(1 != pCommand->SelectData(strSQL,&pRcdSet))
		{
			OutputDebugString("Read CADMAPTOSDE Table failed!");
			pCommand->Release(); return;
		}

		if(pRcdSet->BatchRead() == 1)
		{
			char strTempValue[512]={0};
			pRcdSet->GetValueAsString(strTempValue,0);
			m_strCadLyNameDK = strTempValue;
			OutputDebugString(m_strCadLyNameDK);
		}
		if(pRcdSet != NULL)
		{
			pRcdSet->Release(); pRcdSet = NULL;
		}
	

		sprintf(strSQL,"F:CADLYNAME,T:CADMAPTOSDE,W:SDELYNAME='%s',##",m_strSdeLyNameXM);
		OutputDebugString(strSQL);

		if(1 != pCommand->SelectData(strSQL,&pRcdSet))
		{
			OutputDebugString("Read CADMAPTOSDE Table failed!");
			pCommand->Release(); return;
		}

		if(pRcdSet->BatchRead()==1)
		{
			m_strCadLyNameXM = (char*)(*pRcdSet)[0];
			OutputDebugString(m_strCadLyNameXM);
		}
		if(pRcdSet != NULL)
		{
			pRcdSet->Release(); pRcdSet = NULL;
		}

		pCommand->Release();
	}

	//7.根据项目类型获取相应的CAD层，SDE层名称(固定的名称,不要随意修改)
	m_strXMLX.Trim();
	if(m_strXMLX.CompareNoCase("选址")==0) 
	{
		// 临时
		//m_strSdeLyNameDK = "XMGHSDE.SP_XZLX_ZB_POLYGON";
		//m_strSdeLyNameXM = "XMGHSDE.SP_XZLX_ZB_PT";
		m_strSdeLyNameDK = "SDE.SP_XZLX_ZB_POLYGON";
		m_strSdeLyNameXM = "SDE.SP_XZLX_ZB_PT";
	}
	else if(m_strXMLX.CompareNoCase("用地")==0)
	{
		// 临时
		//m_strSdeLyNameDK = "XMGHSDE.SP_YDLX_ZB_POLYGON";
		//m_strSdeLyNameXM = "XMGHSDE.SP_YDLX_ZB_PT";
		m_strSdeLyNameDK = "SDE.SP_YDLX_ZB_POLYGON";
		m_strSdeLyNameXM = "SDE.SP_YDLX_ZB_PT";
	}
	//else if(m_strXMLX.CompareNoCase("红线")==0)
	//{
	//	m_strSdeLyNameDK = "XMGHSDE.SP_HXLX_ZB_POLYGON";
	//	m_strSdeLyNameXM = "XMGHSDE.SP_HXLX_ZB_PT";
	//}
	//else if(m_strXMLX.CompareNoCase("预选")==0)
	//{
	//	m_strSdeLyNameDK = "XMGHSDE.SP_YXLX_ZB_POLYGON";
	//	m_strSdeLyNameXM = "XMGHSDE.SP_YXLX_ZB_PT";
	//}
	//else if(m_strXMLX.CompareNoCase("储备")==0)
	//{
	//	m_strSdeLyNameDK = "XMGHSDE.SP_CBLX_ZB_POLYGON";
	//	m_strSdeLyNameXM = "XMGHSDE.SP_CBLX_ZB_PT";
	//}
	//else if(m_strXMLX.CompareNoCase("方案*用地")==0)
	//{
	//	m_strSdeLyNameDK = "XMGHSDE.SP_YDLX_ZB_POLYGON";
	//	m_strSdeLyNameXM = "XMGHSDE.SP_YDLX_ZB_PT";
	//}
	else
	{
		OutputDebugString("用地类型有误!"); return;
	}

	acutPrintf("\n\n");
}

// 所有的消息映射函数
LRESULT CALLBACK CYDsysApp::DistProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{

	if ( message == (WM_COPYDATA))
	{
		// 对参数lParam解析，得到传输的数据
		COPYDATASTRUCT* cds = (COPYDATASTRUCT*)lParam;
		CString str = (LPCTSTR)cds->lpData;
		
		// 处理从dap传输过来的项目信息
		if (str.Left(strlen("GIVEDATA:dap")).CompareNoCase("GIVEDATA:dap") == 0)
		{
			g_DAPHwnd = (HWND)wParam;
			ProcessDataFromDap(str);

			// 显示Cad命令窗口
			CDistEntityTool tempTool;
			tempTool.ShowCommandLineWindow(TRUE);

			// 获取系统和项目相关信息
			InitSystem();

			g_bIsInitial = TRUE;

			//获取工作状态
			//shYDsysDISTGWS();
			tempTool.SendCommandToAutoCAD("DISTGWS ");

			// 定位
			//AcGePoint2d CenterPt;
			//CenterPt.x = (501000+430000)/2;
			//CenterPt.y = (2766000+2695000)/2;
			//tempTool.DyZoom(CenterPt,2766000-2695000,501000-430000);
			///临时
			//CenterPt.x = (68000+111900)/2;
			//CenterPt.y = (53000+83500)/2;
			//tempTool.DyZoom(CenterPt,83500-53000,111900-68000);
		}
		// 处理从dap发送过来的cad命令
		else if (str.Left(strlen("GIVECOMMAND:")).CompareNoCase("GIVECOMMAND:") == 0)
		{
			ProcessCommandFromDap(str);
		}

		return 0;
	}

	if(message == WM_CLOSE)
	{
		AcApDocumentIterator* pIter = acDocManager->newAcApDocumentIterator();
		if(NULL != pIter)
		{
			for (; !pIter->done(); pIter->step()) 
			{
				AcApDocument* pDoc = pIter->document();
				AcDbDatabase *pDB  = pDoc->database();
				acdbSetDbmod(pDB,0);
			}
			delete pIter; pIter=NULL;
		}
	}


	// 传回原来的处理函数
	if( m_OldProc != NULL)
	{
		return CallWindowProc(m_OldProc, hWnd, message, wParam, lParam);
	}
	else
	{
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}




