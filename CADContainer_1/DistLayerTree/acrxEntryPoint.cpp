#include "StdAfx.h"
#include "LayerManager.h"
//#include "sorttab.h"
#include "DistDbDocReactor.h"
//-----------------------------------------------------------------------------
#define szRDS _RXST("sh")


//adskMyDbReactor	*pDbReactor = NULL; // Pointer to database reactor
//adskDocReactor *pDocReactor = NULL; 
//const int kBufferLength = 800;
//const int kLayerNameLength = 200;
void ads_regen();
//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint

//Adesk::UInt8 giDrawOrderCtl = 0;

CLayerManager* pLayerManager=NULL;

//创建图层树
__declspec(dllexport) int WINAPI DistCreateTree(char* strServerName,char* strInstanceName,
			                                    char* strDBName,char* strUserName,char* strPassword);

//设置权限
__declspec(dllexport) int WINAPI DistSetTreeLyPowerAndReCreate(int* pIdArray,int* pPowerArray,int* pNum);


//获取SDE图层名称和权限值
__declspec(dllexport) int WINAPI DistGetTreeLyPower(char*** strSdeLyNameArray,int** pPowerArray,int* pNum);

//获取SDE图层名称和权限值
__declspec(dllexport) int WINAPI DistDeleteSdeLys();

//获取图层树钩选状态
__declspec(dllexport) CString WINAPI DistGetTreeCheckState();

//设置图层树钩选状态
__declspec(dllexport) int WINAPI DistSetTreeCheckState(CStringArray* strArrayCheck);

//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
//##ModelId=46F374240040
class CDistLayerTreeApp : public AcRxArxApp {

public:
	//##ModelId=46F374240042
	CDistLayerTreeApp () : AcRxArxApp () {}

	//##ModelId=46F374240043
	virtual AcRx::AppRetCode On_kInitAppMsg (void *pkt) {
		AcRx::AppRetCode retCode =AcRxArxApp::On_kInitAppMsg (pkt) ;
		// TODO: Add your initialization code here
		
		if (pLayerManager == NULL)
		{
			//第一次调用命令时
			//转换资源
			CAcModuleResourceOverride _UseThisRes;

			pLayerManager = new CLayerManager;

			CWnd *pWnd = acedGetAcadFrame();
			pLayerManager->Create(pWnd, "数慧图层控制器");

			pLayerManager->ShowDockPane(FALSE);
		}
		//attachReactor();

		return (retCode) ;
	}

	//##ModelId=46F374240050
	virtual AcRx::AppRetCode On_kUnloadAppMsg (void *pkt) {
		AcRx::AppRetCode retCode =AcRxArxApp::On_kUnloadAppMsg (pkt) ;
		// TODO: Add your code here
		if(pLayerManager != NULL)
			delete pLayerManager;

		//clearReactor();
		return (retCode) ;
	}

	//##ModelId=46F374240053
	virtual void RegisterServerComponents () 
	{
	}

/*
	static void attachReactor()
	{
		if (pDbReactor == NULL) 
		{
			pDbReactor = new adskMyDbReactor();
			AcDbDatabase* pDb= acdbHostApplicationServices()->workingDatabase();
			//add db reactor
			pDb->addReactor(pDbReactor); 
			//add doc reactor
			if(pDocReactor == NULL)
			{
				pDocReactor = new adskDocReactor();
				pDocReactor->Attach();
			}
			//set drawOrderCtl
			giDrawOrderCtl = pDb->drawOrderCtl();
			pDb->setDrawOrderCtl(0);
			AcDbSortentsTable *pSortTab = NULL;
			if(getOrCreateSortEntsDictionary(pSortTab))
			{
				acutPrintf("\nDrawOrderByLayer is enabled.");
			}	
			pSortTab->close();
			pSortTab = NULL;
		}
	}


	//This function remove db and doc reactors
	static void clearReactor()
	{
		if(pDbReactor)
		{
			AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();
			if (pDb) 
			{
				//remove Db reactor
				pDb->removeReactor(pDbReactor);
				acutPrintf("\nDrawOrderBylayer is disabled.");
				delete pDbReactor;
				pDbReactor = NULL;

				//set to original DrawOrderCtl
				pDb->setDrawOrderCtl(giDrawOrderCtl);
				//remove doc reactor
			}
		}else
		{
			acutPrintf("\nDrawOrderByLayer is already disabled.");
		}

		if(pDocReactor)
		{
			pDocReactor->Detach();
			delete pDocReactor;
			pDocReactor = NULL;
		}
	}
*/
	
	//##ModelId=46F374240055
	static void shDISTSHOWTREE()
	{
		if (pLayerManager == NULL)
		{
			//第一次调用命令时
			//转换资源
			CAcModuleResourceOverride _UseThisRes;

			pLayerManager = new CLayerManager;

			CWnd *pWnd = acedGetAcadFrame();
			pLayerManager->Create(pWnd, "数慧图层控制器");

			pLayerManager->ShowDockPane(TRUE);
		}
		else
			pLayerManager->ShowDockPane(TRUE);
	}

	static void shDISTSAVETREE()
	{
		if(pLayerManager == NULL) return;
		pLayerManager->mpChildDlg->OnClose();
	}

	static void  shShowAllSelLayer()
	{
		if(pLayerManager == NULL) return;
		pLayerManager->mpChildDlg->OnBnClickedBtnOpen();
	}

	static void  shCloseSelLayer()
	{
		if(pLayerManager == NULL) return;
		pLayerManager->mpChildDlg->OnCloseAllLayer();
	}
} ;

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CDistLayerTreeApp)
ACED_ARXCOMMAND_ENTRY_AUTO(CDistLayerTreeApp, sh, DISTSHOWTREE,DISTSHOWTREE, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CDistLayerTreeApp, sh, DISTSAVETREE,DISTSAVETREE, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CDistLayerTreeApp, sh, ShowAllSelLayer,ShowAllSelLayer, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CDistLayerTreeApp, sh, CloseSelLayer,CloseSelLayer, ACRX_CMD_TRANSPARENT, NULL)


int WINAPI DistCreateTree(char* strServerName,char* strInstanceName,
                          char* strDBName,char* strUserName,char* strPassword)
{
	if(NULL == pLayerManager) return 0;
	if(NULL == pLayerManager->mpChildDlg) return 0;

	return pLayerManager->mpChildDlg->CreateTree(strServerName,strInstanceName,
		                                         strDBName,strUserName,strPassword);
}


int WINAPI DistSetTreeLyPowerAndReCreate(int* pIdArray,int* pPowerArray,int* pNum)
{
	if(NULL == pLayerManager) return 0;
	if(NULL == pLayerManager->mpChildDlg) return 0;

	return pLayerManager->mpChildDlg->SetTreeLyPowerAndReCreate(pIdArray,pPowerArray,pNum);

}

int WINAPI DistGetTreeLyPower(char*** strSdeLyNameArray,int** pPowerArray,int* pNum)
{
	if(NULL == pLayerManager) return 0;
	if(NULL == pLayerManager->mpChildDlg) return 0;

	return pLayerManager->mpChildDlg->GetTreeLyPower(strSdeLyNameArray,pPowerArray,pNum);
}

int WINAPI DistDeleteSdeLys()
{
	if(NULL == pLayerManager) return 0;
	if(NULL == pLayerManager->mpChildDlg) return 0;
	pLayerManager->mpChildDlg->OnBnClickedBtnDelete();
	return 1;
}

CString WINAPI DistGetTreeCheckState()
{
	if(NULL == pLayerManager) return "";
	if(NULL == pLayerManager->mpChildDlg) return "";

	return pLayerManager->mpChildDlg->GetCheckState();
}

int WINAPI DistSetTreeCheckState(CStringArray* strArrayCheck)
{
	if(NULL == pLayerManager) return 0;
	if(NULL == pLayerManager->mpChildDlg) return 0;

	return pLayerManager->mpChildDlg->SetCheckState(strArrayCheck);
}













