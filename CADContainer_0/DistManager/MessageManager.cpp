#include "StdAfx.h"
#include "messagemanager.h"
#include "DistEntityTool.h"
#include "Basecfginfo.h"

#include "..\SdeData\DistBaseInterface.h"
#include "..\SdeData\DistExchangeInterface.h"

extern HWND	g_DAPHwnd = NULL;           //����DAP�����崰�ں���ָ��

extern IDistConnection*    g_pConnection;         //SDE���ݿ����Ӷ���ָ��
extern IDistExchangeData*  g_pExchangeData;       //SDE<->CAD���ݽ�������ָ��

WNDPROC MessageManager::m_OldProc = NULL;       //����CAD�����崰�ں���ָ��

extern long __cdecl acdbSetDbmod(class AcDbDatabase *,long);

MessageManager::MessageManager(void)
{
	CMDIFrameWnd* lpMainFrame = acedGetAcadFrame();
	HWND hFrame = lpMainFrame->m_hWnd;

	//���Ĵ�����Ϣ������
	m_OldProc = (WNDPROC)SetWindowLong(hFrame, GWL_WNDPROC, (LONG)DistProc);

	//�����������
	HANDLE hSHPlan = CreateEvent(NULL, TRUE, FALSE,"SH-YDCSP-3AC01994-FD16-4F4F-9BA7-A5F27AE8CC76");
	HANDLE hEvent  = CreateEvent(NULL, TRUE, FALSE,"hust_{62B6D001-7766-514f-D8BB-58549A777A7A}");
	SetEvent(hEvent);
	CloseHandle(hEvent);

	//���ñ���
	lpMainFrame->SetWindowText("�õ�������ͼϵͳ");
	lpMainFrame->UpdateWindow();
}

MessageManager::~MessageManager(void)
{
}

//string message = "GIVEDATA ap|CADTYPE:Ԥѡ|WEB_HTTP:" + Common.CurStatic.WebServiceUrl + "XMWebService.asmx?WSDL" + "|";
// �����dap�����������Ŀ��Ϣ
void MessageManager::ProcessDataFromDap(CString strData)
{
	OutputDebugString(strData);
	int nTempLen = strData.GetLength();
	if(strData.GetAt(nTempLen-1) != '|')  strData.SetAt(nTempLen-1,'|');

	int nPos = strData.Find("USER_ID:");  //�û�ID
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

	nPos = strData.Find("PROJECT_ID:");   //��Ŀ���
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
		nPos = nPos + strlen("WEB_HTTP:"); //WebServices��ַ

		int nTemp=strData.Find('|',nPos);
		if(nTemp > nPos)
		{
			m_strWebServiceURL = strData.Mid(nPos, nTemp-nPos); m_strWebServiceURL.Trim();
			OutputDebugString(m_strWebServiceURL);
		}
	}

	nPos = strData.Find("ROLE_ID:");  //��ɫID
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

	nPos = strData.Find("PROCESS_ID:");  //����ID
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

	nPos = strData.Find("PRE_PROJECT_ID:");		   // �Ͻ׶�����ID(ҵ��ϵͳҪ�����)
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

	nPos = strData.Find("RECT:"); // ������������(���Ʒ־ֵ���ʾ��Χ)
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

	nPos = strData.Find("CADTYPE:");  //��Ŀ���ͣ�"ѡַ","�õ�","����","Ԥѡ","����"��
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

// �����dap���͹�����cad����
void MessageManager::ProcessCommandFromDap(CString strData)
{
	int nPos = 0;
	CString strCommand = strData.Right(strData.GetLength()-strlen("GIVECOMMAND:"));

	if(strCommand.Find("APPLICATION_EXIT")>=0)  //����Ҫ�˳�
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

// ���е���Ϣӳ�亯��
LRESULT CALLBACK MessageManager::DistProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{

	if ( message == (WM_COPYDATA))
	{
		// �Բ���lParam�������õ����������
		COPYDATASTRUCT* cds = (COPYDATASTRUCT*)lParam;
		CString str = (LPCTSTR)cds->lpData;

		
		
		// �����dap�����������Ŀ��Ϣ
		if (str.Left(strlen("GIVEDATA:dap")).CompareNoCase("GIVEDATA:dap") == 0)
		{
			g_DAPHwnd = (HWND)wParam;
			ProcessDataFromDap(str);

			// ��ʾCad�����
			CDistEntityTool tempTool;
			tempTool.ShowCommandLineWindow(TRUE);

			// ��ȡϵͳ����Ŀ�����Ϣ
			InitSystem();

			// ��λ
			AcGePoint2d CenterPt;
			CenterPt.x = (501000+430000)/2;
			CenterPt.y = (2766000+2695000)/2;
			tempTool.DyZoom(CenterPt,2766000-2695000,501000-430000);

		}
		// �����dap���͹�����cad����
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


	// ����ԭ���Ĵ�����
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
	//1.��ȡSDE��½������Ϣ����������ʵ�������ݿ⣬�û��������룩
	if(FALSE == CBaseCfgInfo::ReadConnectInfo(m_strWebServiceURL))
	{
		OutputDebugString("CBaseCfgInfo::ReadConnectInfo ��ȡSDE��½������Ϣʧ��!");
	}
	else
	{
		//����ͼ��Ŀ¼��
		DistCreateTree(CBaseCfgInfo::m_BaseSdeInfo.strServer.GetBuffer(0),
					   CBaseCfgInfo::m_BaseSdeInfo.strInstance.GetBuffer(0),
					   CBaseCfgInfo::m_BaseSdeInfo.strDatabase.GetBuffer(0),
					   CBaseCfgInfo::m_BaseSdeInfo.strUserName.GetBuffer(0),
					   CBaseCfgInfo::m_BaseSdeInfo.strPassword.GetBuffer(0));
	}

	//2.�� FTP ������Ϣ
	if(FALSE == CBaseCfgInfo::ReadFtpInfo(m_strWebServiceURL))
	{
		OutputDebugString("CBaseCfgInfo::ReadFtpInfo ��ȡFTP������Ϣʧ��!");
	}

	//3.��ȡ��Ŀ������Ϣ�����ʱ�TBPROJECT_BASE������ο���Ŀ��Ϣ�ṹ��

	if(!m_strPROJECTID.IsEmpty())
	{
		if(FALSE == CBaseCfgInfo::ReadProjectBaseInfo(m_strPROJECTID,m_strWebServiceURL))
		{
			OutputDebugString("CBaseCfgInfo::ReadProjectBaseInfo ��ȡ��Ŀ������Ϣʧ��!");
		}
		else
		{
			OutputDebugString("m_strXMLX = CBaseCfgInfo::m_basePrjInfo.strCADTYPE");
			m_strXMLX = CBaseCfgInfo::m_basePrjInfo.strCADTYPE;
			OutputDebugString(m_strXMLX);
		}
	}


	//4.�����û�ID����ȡ�û���ϸ��Ϣ�����ʱ�TBUSER��
	if(FALSE == CBaseCfgInfo::ReadUserInfo(m_strDapUserId,m_strWebServiceURL))
	{
		OutputDebugString("CBaseCfgInfo::ReadUserInfo ��ȡ��ǰ�û���Ϣʧ��!");
	}

	//5.��ȡ��ǰ����SDEͼ��Ķ�дȨ�ޣ����ʱ�TBRIGHT_FORM��
	CPowerConfig::Release();
	if(TRUE == CPowerConfig::GetCurUserRight(m_strWebServiceURL,m_strRoleId,m_strPROJECTID))
	{
		//�趨Ȩ��
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
	


	//6.��ʾ��������Cad�����
	CDistEntityTool tempTool;
	tempTool.ShowCommandLineWindow(TRUE);

	//7.�������ݿ����Ӷ����CAD��SDE��������
	if(NULL == g_pConnection)
	{
		g_pConnection = CreateSDEConnectObjcet();
		if(NULL == g_pConnection) 
		{
			OutputDebugString("CreateSDEConnectObjcet ����SDE���Ӷ���ʧ��!"); return ;
		}
		long rc = -1;
		if((rc = g_pConnection->Connect(CBaseCfgInfo::m_BaseSdeInfo.strServer,
			                            CBaseCfgInfo::m_BaseSdeInfo.strInstance,
									    CBaseCfgInfo::m_BaseSdeInfo.strDatabase,
									    CBaseCfgInfo::m_BaseSdeInfo.strUserName,
									    CBaseCfgInfo::m_BaseSdeInfo.strPassword)) != 1)
		{
			OutputDebugString("Connect �ռ����ݿ�����ʧ��");
			g_pConnection->Release(); g_pConnection = NULL; return ;
		}
	}

	//���ݽ�ɫID��ȡ������Ȩ�޿�����Ϣ
	if(NULL != g_pConnection)
	{
		IDistCommand* pCmd = CreateSDECommandObjcet();
		if(pCmd == NULL)
		{
			OutputDebugString("CreateSDECommandObjcet ����SDE�������ʧ��!");  return ;
		}
		pCmd->SetConnectObj(g_pConnection);

		char strSQL[255]={0};
		sprintf(strSQL,"F:POWER,T:TB_TOOLBAR_POWER,W:ROLEID='%s',##",m_strRoleId);
		IDistRecordSet* pRcdSet = NULL;
		if(1 != pCmd->SelectData(strSQL,&pRcdSet))
		{
			OutputDebugString("��ȡ��ǰ��ɫ������Ȩ������ֵʧ��!");
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
			OutputDebugString("CreateSDEConnectObjcet ����SDE��CAD���������ʧ��!");
			return ;
		}
	}

	//10.������Ŀ���ͻ�ȡ��Ӧ��CAD�㣬SDE������(�̶�������,��Ҫ�����޸�)
	m_strXMLX.Trim();
	if(m_strXMLX.CompareNoCase("ѡַ")==0) 
	{
		m_strSdeLyNameDK = "XMGHSDE.SP_XZLX_ZB_POLYGON";
		m_strSdeLyNameXM = "XMGHSDE.SP_XZLX_ZB_PT";
	}
	else if(m_strXMLX.CompareNoCase("�õ�")==0)
	{
		m_strSdeLyNameDK = "XMGHSDE.SP_YDLX_ZB_POLYGON";
		m_strSdeLyNameXM = "XMGHSDE.SP_YDLX_ZB_PT";
	}
	else if(m_strXMLX.CompareNoCase("����")==0)
	{
		m_strSdeLyNameDK = "XMGHSDE.SP_HXLX_ZB_POLYGON";
		m_strSdeLyNameXM = "XMGHSDE.SP_HXLX_ZB_PT";
	}
	else if(m_strXMLX.CompareNoCase("Ԥѡ")==0)
	{
		m_strSdeLyNameDK = "XMGHSDE.SP_YXLX_ZB_POLYGON";
		m_strSdeLyNameXM = "XMGHSDE.SP_YXLX_ZB_PT";
	}
	else if(m_strXMLX.CompareNoCase("����")==0)
	{
		m_strSdeLyNameDK = "XMGHSDE.SP_CBLX_ZB_POLYGON";
		m_strSdeLyNameXM = "XMGHSDE.SP_CBLX_ZB_PT";
	}
	else if(m_strXMLX.CompareNoCase("����*�õ�")==0)
	{
		m_strSdeLyNameDK = "XMGHSDE.SP_YDLX_ZB_POLYGON";
		m_strSdeLyNameXM = "XMGHSDE.SP_YDLX_ZB_PT";
	}
	else
	{
		OutputDebugString("�õ���������!"); return;
	}

	if(m_strPowerArray.GetCount()>0)
		ReCreateToolBar(&m_strPowerArray);
	else
		ReCreateToolBar(NULL);


	//11.����SDE������,��ȡ��Ӧ��CAD������
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