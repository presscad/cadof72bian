#include "StdAfx.h"
#include "messagemanager.h"
#include "DistEntityTool.h"
#include "Basecfginfo.h"

#include "..\SdeData\DistBaseInterface.h"
#include "..\SdeData\DistExchangeInterface.h"

extern HWND	g_DAPHwnd = NULL;           //保存DAP主窗体窗口函数指针

extern IDistConnection*    g_pConnection;         //SDE数据库连接对象指针
extern IDistExchangeData*  g_pExchangeData;       //SDE<->CAD数据交换对象指针

WNDPROC MessageManager::m_OldProc = NULL;       //保存CAD主窗体窗口函数指针

extern long __cdecl acdbSetDbmod(class AcDbDatabase *,long);

MessageManager::MessageManager(void)
{
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
}

MessageManager::~MessageManager(void)
{
}

//string message = "GIVEDATA ap|CADTYPE:预选|WEB_HTTP:" + Common.CurStatic.WebServiceUrl + "XMWebService.asmx?WSDL" + "|";
// 处理从dap传输过来的项目信息
void MessageManager::ProcessDataFromDap(CString strData)
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

	nPos = strData.Find("RECT:"); // 行政区划名称(控制分局的显示范围)
	if (nPos >= 0)
	{
		nPos = nPos + strlen("RECT:"); 
		int nTemp=strData.Find('|',nPos);
		if(nTemp > nPos)
		{
			m_strXZQH = strData.Mid(nPos, nTemp-nPos);  m_strXZQH.Trim();
			OutputDebugString(m_strXZQH);
		}
	}

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

// 处理从dap发送过来的cad命令
void MessageManager::ProcessCommandFromDap(CString strData)
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
	strCommand = strCommand + " ";
	CDistEntityTool tempTool;
	tempTool.SendCommandToAutoCAD(strCommand);
}

// 所有的消息映射函数
LRESULT CALLBACK MessageManager::DistProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
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

			// 定位
			AcGePoint2d CenterPt;
			CenterPt.x = (501000+430000)/2;
			CenterPt.y = (2766000+2695000)/2;
			tempTool.DyZoom(CenterPt,2766000-2695000,501000-430000);

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

void MessageManager::InitSystem()
{
	//1.获取SDE登陆配置信息（服务器，实例，数据库，用户名，密码）
	if(FALSE == CBaseCfgInfo::ReadConnectInfo(m_strWebServiceURL))
	{
		OutputDebugString("CBaseCfgInfo::ReadConnectInfo 获取SDE登陆配置信息失败!");
	}
	else
	{
		//创建图层目录树
		DistCreateTree(CBaseCfgInfo::m_BaseSdeInfo.strServer.GetBuffer(0),
					   CBaseCfgInfo::m_BaseSdeInfo.strInstance.GetBuffer(0),
					   CBaseCfgInfo::m_BaseSdeInfo.strDatabase.GetBuffer(0),
					   CBaseCfgInfo::m_BaseSdeInfo.strUserName.GetBuffer(0),
					   CBaseCfgInfo::m_BaseSdeInfo.strPassword.GetBuffer(0));
	}

	//2.读 FTP 连接信息
	if(FALSE == CBaseCfgInfo::ReadFtpInfo(m_strWebServiceURL))
	{
		OutputDebugString("CBaseCfgInfo::ReadFtpInfo 获取FTP基本信息失败!");
	}

	//3.读取项目基本信息（访问表TBPROJECT_BASE，具体参考项目信息结构）

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


	//4.根据用户ID，获取用户详细信息（访问表TBUSER）
	if(FALSE == CBaseCfgInfo::ReadUserInfo(m_strDapUserId,m_strWebServiceURL))
	{
		OutputDebugString("CBaseCfgInfo::ReadUserInfo 获取当前用户信息失败!");
	}

	//5.获取当前访问SDE图层的读写权限（访问表：TBRIGHT_FORM）
	CPowerConfig::Release();
	if(TRUE == CPowerConfig::GetCurUserRight(m_strWebServiceURL,m_strRoleId,m_strPROJECTID))
	{
		//设定权限
		int nCount = CPowerConfig::m_PowerArray.GetCount();
		if(nCount>0)
		{
			int* pTreeId = new int[nCount];
			int* pPower  = new int[nCount];
			for(int k=0; k<nCount; k++)
			{
				POWERCONFIG temp = CPowerConfig::m_PowerArray.GetAt(k) ;
				pTreeId[k] = atoi(temp.strTreeID);
				pPower[k] = atoi(temp.strSDEPower);
			}
			acutPrintf("\n");
			DistSetTreeLyPowerAndReCreate(pTreeId,pPower,&nCount);
			delete [] pTreeId;
			delete [] pPower;
		}
	}
	SendMessage(acedGetAcadFrame()->m_hWnd,WM_SIZE,0,0);
	


	//6.显示功能树和Cad命令窗口
	CDistEntityTool tempTool;
	tempTool.ShowCommandLineWindow(TRUE);

	//7.创建数据库连接对象和CAD与SDE互换对象
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

	//根据角色ID获取工具条权限控制信息
	if(NULL != g_pConnection)
	{
		IDistCommand* pCmd = CreateSDECommandObjcet();
		if(pCmd == NULL)
		{
			OutputDebugString("CreateSDECommandObjcet 创建SDE命令对象失败!");  return ;
		}
		pCmd->SetConnectObj(g_pConnection);

		char strSQL[255]={0};
		sprintf(strSQL,"F:POWER,T:TB_TOOLBAR_POWER,W:ROLEID='%s',##",m_strRoleId);
		IDistRecordSet* pRcdSet = NULL;
		if(1 != pCmd->SelectData(strSQL,&pRcdSet))
		{
			OutputDebugString("获取当前角色工具条权限属性值失败!");
			pCmd->Release(); pCmd = NULL; return;
		}

		CString strTempPower;
		if(pRcdSet->BatchRead() == 1)
		{
			strTempPower = (char*)(*pRcdSet)[0];
			OutputDebugString(strTempPower);
			ReduceString(strTempPower,m_strPowerArray);
		}
		if(pRcdSet != NULL)
			pRcdSet->Release();
		pCmd->Release();
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

	//10.根据项目类型获取相应的CAD层，SDE层名称(固定的名称,不要随意修改)
	m_strXMLX.Trim();
	if(m_strXMLX.CompareNoCase("选址")==0) 
	{
		m_strSdeLyNameDK = "XMGHSDE.SP_XZLX_ZB_POLYGON";
		m_strSdeLyNameXM = "XMGHSDE.SP_XZLX_ZB_PT";
	}
	else if(m_strXMLX.CompareNoCase("用地")==0)
	{
		m_strSdeLyNameDK = "XMGHSDE.SP_YDLX_ZB_POLYGON";
		m_strSdeLyNameXM = "XMGHSDE.SP_YDLX_ZB_PT";
	}
	else if(m_strXMLX.CompareNoCase("红线")==0)
	{
		m_strSdeLyNameDK = "XMGHSDE.SP_HXLX_ZB_POLYGON";
		m_strSdeLyNameXM = "XMGHSDE.SP_HXLX_ZB_PT";
	}
	else if(m_strXMLX.CompareNoCase("预选")==0)
	{
		m_strSdeLyNameDK = "XMGHSDE.SP_YXLX_ZB_POLYGON";
		m_strSdeLyNameXM = "XMGHSDE.SP_YXLX_ZB_PT";
	}
	else if(m_strXMLX.CompareNoCase("储备")==0)
	{
		m_strSdeLyNameDK = "XMGHSDE.SP_CBLX_ZB_POLYGON";
		m_strSdeLyNameXM = "XMGHSDE.SP_CBLX_ZB_PT";
	}
	else if(m_strXMLX.CompareNoCase("方案*用地")==0)
	{
		m_strSdeLyNameDK = "XMGHSDE.SP_YDLX_ZB_POLYGON";
		m_strSdeLyNameXM = "XMGHSDE.SP_YDLX_ZB_PT";
	}
	else
	{
		OutputDebugString("用地类型有误!"); return;
	}

	if(m_strPowerArray.GetCount()>0)
		ReCreateToolBar(&m_strPowerArray);
	else
		ReCreateToolBar(NULL);


	//11.根据SDE层名称,获取对应的CAD层名称
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

	acutPrintf("\n\n");
}