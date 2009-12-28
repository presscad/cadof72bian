
#include "StdAfx.h"
#include "DistClipEntity.h"
#include "DistEntityTool.h"
#include "DistSelSet.h"
//-----------------------------------------------------------------------------
#define szRDS _RXST("sh")


void PlotByPtMaxAndMin(AcGePoint3d ptMin,AcGePoint3d ptMax)
{
	//获取当前数据库指针
	AcDbDatabase *pCurDb=acdbHostApplicationServices()->workingDatabase();

	//获取表记录
	AcDbBlockTable *pBlockTable=NULL;
	AcDbBlockTableRecord *pBlkTblRcd=NULL;
	if(Acad::eOk != pCurDb->getBlockTable(pBlockTable,AcDb::kForRead)) return;
	if(Acad::eOk != pBlockTable->getAt(ACDB_MODEL_SPACE,pBlkTblRcd,AcDb::kForRead))
	{
		pBlockTable->close(); return;	
	}
	pBlockTable->close();

	//获取样式ID
	AcDbObjectId LaoutId=pBlkTblRcd->getLayoutId();
	pBlkTblRcd->close();


	char* plotCfgchar = NULL;
	AcDbLayout *pSrcLayout = NULL;
	if(acdbOpenAcDbObject((AcDbObject*&)pSrcLayout,LaoutId,AcDb::kForWrite)!=Acad::eOk) return;    

	Acad::ErrorStatus es;
	AcDbPlotSettingsValidator *pPltValid = acdbHostApplicationServices()->plotSettingsValidator(); 

	if((es = pPltValid->setPlotWindowArea (pSrcLayout,0,0,100,100)) != Acad::eOk)
	{
		pSrcLayout->close(); return;
	}

	if((es =pPltValid->setPlotType (pSrcLayout, AcDbPlotSettings::PlotType::kWindow))!=Acad::eOk)
	{
		pSrcLayout->close(); return; 
	}
	if((es =pSrcLayout->getPlotCfgName(plotCfgchar)) != Acad::eOk)
	{
		pSrcLayout->close(); return;
	}

	pPltValid->refreshLists(pSrcLayout); 
	pSrcLayout->close();   


	if(acdbOpenAcDbObject((AcDbObject*&)pSrcLayout,LaoutId,AcDb::kForWrite)!=Acad::eOk) return; 
	if((es = pPltValid->setPlotWindowArea (pSrcLayout,ptMin.x,ptMin.y,ptMax.x,ptMax.y)) != Acad::eOk)
	{
		pSrcLayout->close(); return;
	}  
	if((es = pPltValid->setPlotType (pSrcLayout, AcDbPlotSettings::PlotType::kWindow)) != Acad::eOk)
	{
		pSrcLayout->close(); return;	
	}
	if((es = pSrcLayout->getPlotCfgName(plotCfgchar)) != Acad::eOk)
	{
		pSrcLayout->close(); return;	
	}

	pPltValid->refreshLists(pSrcLayout); 
	pSrcLayout->close();   
	CString plotCfgName(plotCfgchar);        //模型Model
	if(plotCfgName.Find("无")>=0)
	{
		acutPrintf("\n请设置打印样式,并置为当前!%s\n",plotCfgchar);return;
	}
	acedCommand(RTSTR,"_plot",RTSTR,"N",RTSTR,"模型",RTSTR,"",RTSTR,plotCfgName,RTSTR,"N",RTSTR,"N",RTSTR,"Y",0);
}

//##ModelId=46F3742002C1
class CDistClipPlotApp : public AcRxArxApp {

public:
	//##ModelId=46F3742002C3
	CDistClipPlotApp () : AcRxArxApp () {}

	//##ModelId=46F3742002C4
	virtual AcRx::AppRetCode On_kInitAppMsg (void *pkt) {
		AcRx::AppRetCode retCode =AcRxArxApp::On_kInitAppMsg (pkt) ;
		// TODO: Add your initialization code here

		return (retCode) ;
	}

	//##ModelId=46F3742002D0
	virtual AcRx::AppRetCode On_kUnloadAppMsg (void *pkt) {
		AcRx::AppRetCode retCode =AcRxArxApp::On_kUnloadAppMsg (pkt) ;
		// TODO: Add your code here

		return (retCode) ;
	}

	//##ModelId=46F3742002D3
	virtual void RegisterServerComponents () {
	}

	//##ModelId=46F3742002D5
	static void shYDsysDISTDYOUT()
	{
		/*
		acedCommand(RTSTR,"_pagesetup",0);

		AcGePoint3d ptMin;
		AcGePoint3d ptMax; 
		if(acedGetPoint(NULL,"\n指定起点:",asDblArray(ptMin))!=RTNORM)
		{ 
			return;
		}
		if(acedGetCorner(asDblArray(ptMin),"\n指定对角点:",asDblArray(ptMax))!=RTNORM)
		{ 
			return;
		}
		if(ptMin.x>ptMax.x)
		{
			double dX=ptMin.x;
			ptMin.x=ptMax.x;
			ptMax.x=dX;
		}
		if(ptMin.y>ptMax.y)
		{
			double dY=ptMin.y;
			ptMin.y=ptMax.y;
			ptMax.y=dY;
		}

		PlotByPtMaxAndMin(ptMin,ptMax); 
		*/


		CDistClipEntity::PlotByPaperSpace();
	}

	//##ModelId=46F3742002D7
	static void shYDsysDISTDYTK()
	{
		CDistClipEntity::PlotEx();
	}

	static void shYDsysXDataDel()
	{
		ads_name ssName;
		if(RTNORM == acedSSGet(":S", NULL, NULL, NULL, ssName))
		{
			AcDbObjectId tempObjId;
			ads_name ssTemp;
			long nLen=0;
			if(RTNORM != acedSSLength(ssName,&nLen)) return;
			for(int j=0; j<nLen; j++)
			{
				if(RTNORM == acedSSName(ssName,j,ssTemp)) continue;
				if(Acad::eOk != acdbGetObjectId(tempObjId, ssTemp)) continue;
				AcDbEntity* pEnt = NULL;
				if(Acad::eOk == acdbOpenObject(pEnt,tempObjId,AcDb::kForWrite))
				{
					
					resbuf* pHead = pEnt->xData(NULL);
					if(pHead == NULL)
					{
						pEnt->close(); continue;
					}
					resbuf* pNext =pHead->rbnext;
					if(pNext == NULL)
					{
						acutRelRb(pHead); pEnt->close(); continue;
					}
					while(pNext != NULL)
					{
						acutRelRb(pHead);
						pHead = pNext;
						pNext = pNext->rbnext;
					}
					pEnt->setXData(NULL);
					pEnt->close();
				}
			}
			acedSSFree(ssName);
		}
	}

	static void shYDsysLOOKDK()
	{
		CDistClipEntity::SelectDkAndHighLight();
	}

	//static void shYDsysDISTCT()
	//{
	//	CDistClipEntity::ReadDwgCT();
	//}
} ;

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CDistClipPlotApp)
ACED_ARXCOMMAND_ENTRY_AUTO(CDistClipPlotApp, shYDsys, DISTDYTK, DISTDYTK, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CDistClipPlotApp, shYDsys, DISTDYOUT,DISTDYOUT, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CDistClipPlotApp, shYDsys, XDataDel,XDataDel, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CDistClipPlotApp, shYDsys, LOOKDK,LOOKDK, ACRX_CMD_TRANSPARENT, NULL)
//ACED_ARXCOMMAND_ENTRY_AUTO(CDistClipPlotApp, shYDsys, DISTCT,DISTCT, ACRX_CMD_TRANSPARENT, NULL)
//ACED_ARXCOMMAND_ENTRY_AUTO(CDistClipPlotApp, shYDsys, Test,Test, ACRX_CMD_TRANSPARENT, NULL)
