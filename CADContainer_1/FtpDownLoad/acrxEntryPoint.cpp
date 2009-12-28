
#include "StdAfx.h"

#include "LayerManager.h"

#include "DlgFTPTransfer.h"
//-----------------------------------------------------------------------------
#define szRDS _RXST("sh")

__declspec(dllexport) int WINAPI DistDownLoadFromFTP(char* strServer,char* strUserName,char* strPassword,int nPort,char* strPrjId);

CLayerManager* pLayerManager = NULL;
//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
//##ModelId=46F3742A02A1
class CFtpDownLoadApp : public AcRxArxApp {

public:
	//##ModelId=46F3742A02A3
	CFtpDownLoadApp () : AcRxArxApp () {}

	//##ModelId=46F3742A02A4
	virtual AcRx::AppRetCode On_kInitAppMsg (void *pkt) {
		AcRx::AppRetCode retCode =AcRxArxApp::On_kInitAppMsg (pkt) ;
		// TODO: Add your initialization code here

		AfxSocketInit();

		if (pLayerManager == NULL)
		{
			//第一次调用命令时
			//转换资源
			CAcModuleResourceOverride _UseThisRes;

			pLayerManager = new CLayerManager;
			
			CWnd *pWnd = acedGetAcadFrame();
			pLayerManager->Create(pWnd, "数慧材料袋");
			pLayerManager->ShowDockPane(FALSE);
		}

		return (retCode) ;
	}

	//##ModelId=46F3742A02B0
	virtual AcRx::AppRetCode On_kUnloadAppMsg (void *pkt) {
		AcRx::AppRetCode retCode =AcRxArxApp::On_kUnloadAppMsg (pkt) ;
		// TODO: Add your code here

		if(NULL != pLayerManager)
			delete pLayerManager;

		return (retCode) ;
	}


	//##ModelId=46F3742A02B3
	virtual void RegisterServerComponents () {
	}

} ;


int WINAPI DistDownLoadFromFTP(char* strServer,char* strUserName,char* strPassword,int nPort,char* strPrjId)
{
	
	pLayerManager->mpChildDlg->Init(strServer,strUserName,strPassword,nPort,strPrjId);
	pLayerManager->ShowDockPane(TRUE);
	pLayerManager->mpChildDlg->ShowWindow(SW_NORMAL);
	pLayerManager->mpChildDlg->ConnectFtp();
	return 1;
}


//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CFtpDownLoadApp)

