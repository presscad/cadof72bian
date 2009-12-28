// (C) Copyright 2002-2003 by Autodesk, Inc. 
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted, 
// provided that the above copyright notice appears in all copies and 
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting 
// documentation.
//
// AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC. 
// DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//
// Use, duplication, or disclosure by the U.S. Government is subject to 
// restrictions set forth in FAR 52.227-19 (Commercial Computer
// Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
// (Rights in Technical Data and Computer Software), as applicable.
//

//-----------------------------------------------------------------------------
//----- acrxEntryPoint.h
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "DistDatabaseReactor.h"
#include "DistBaseInterface.h"
#include "DistExchangeInterface.h"
#include "DistXData.h"

//-----------------------------------------------------------------------------
#define szRDS _RXST("sh")


char** g_pDkInfoArray = NULL;
int    g_nDkCount = 0;

void InitArx()
{
	g_pDkInfoArray = NULL;
	g_nDkCount = 0;
}

void UnLoadArx()
{
	if(NULL != g_pDkInfoArray)
	{
		for(int i=0; i<g_nDkCount; i++)
		{
			delete[] g_pDkInfoArray[i];
		}
		delete g_pDkInfoArray;
		g_pDkInfoArray = NULL;
	}
}


//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
//##ModelId=46F373F2037C
class CSdeDataApp : public AcRxArxApp {

public:
	//##ModelId=46F373F2038A
	CSdeDataApp () : AcRxArxApp () {}

	//##ModelId=46F373F2038B
	virtual AcRx::AppRetCode On_kInitAppMsg (void *pkt) {
		AcRx::AppRetCode retCode =AcRxArxApp::On_kInitAppMsg (pkt) ;
		// TODO: Add your initialization code here

		CDistDatabaseReactor::rxInit();

		InitArx();

		return (retCode) ;
	}

	//##ModelId=46F373F2038E
	virtual AcRx::AppRetCode On_kUnloadAppMsg (void *pkt) {
		AcRx::AppRetCode retCode =AcRxArxApp::On_kUnloadAppMsg (pkt) ;
		// TODO: Add your code here

		deleteAcRxClass(CDistDatabaseReactor::desc());
		CDistDatabaseReactor::Release();

		UnLoadArx();

		return (retCode) ;
	}

	//##ModelId=46F373F20391
	virtual void RegisterServerComponents () {
	}


	// ----- shSdeData.XData command (do not rename)
	//##ModelId=46F373F20399
	static void shSdeDataXData(void)
	{
		// Add your code for command shSdeData.XData here
		AcDbObjectId objId;
		ads_name ent;
		ads_point pt;
		if (RTNORM != acedEntSel("\n选择的对象:", ent, pt))
		{
			return;
		}
		acdbGetObjectId(objId, ent);

		AcDbEntity *pEnt = NULL;
		if (Acad::eOk != acdbOpenObject(pEnt, objId, AcDb::kForRead))
		{
			return;
		}
		pEnt->close();

		CString sLayer = pEnt->layer();
		CString sAppName = "DIST_" + sLayer;

		CDistXData ctemp(objId, sAppName);
		CStringArray strFldNameArray, strFldValueArray;
		ctemp.Select(strFldNameArray, strFldValueArray);
		acutPrintf("字段数量为：%d\n", ctemp.GetRecordCount());
		for (int i = 0; i < strFldNameArray.GetSize(); i++)
		{
			acutPrintf("字段名：%s		字段值：%s\n", strFldNameArray.GetAt(i), strFldValueArray.GetAt(i));
		}
	}
	//##ModelId=46F373F2039C
	static void shSdeDataTestA(void)
	{
		IDistConnection* pConnect = CreateSDEConnectObjcet();
		if(NULL == pConnect) return ;

		if(1 != pConnect->Connect("dbserver01","5151","xmghsde","xmghsde","xmghsde"))
		//if(1 != pConnect->Connect("dist-wanghui","5151","xmghsde","xmghsde","xmghsde"))
		//if(1 != pConnect->Connect("dist-wanghui","5151","sde","sde","sde"))
		{
			acutPrintf("\nConnect Database error!");
			pConnect->Release();
			return;
		}	
		
		IDistExchangeData* pEx= CreateExchangeObjcet();
		if(NULL == pEx)
		{
			pConnect->Release();
			return;
		}

		CStringArray strArray;
		strArray.Add("PROJECT_CODE");strArray.Add("LICENCE_CODE");
		strArray.Add("REG_TIME");strArray.Add("PROJECT_NAME");
		strArray.Add("BUILD_UNIT");strArray.Add("USELAND_TYPEID");
		strArray.Add("JBRY");strArray.Add("DKBH");
		strArray.Add("YDLX");strArray.Add("YDMJ");strArray.Add("ZG");      
		strArray.Add("XK");strArray.Add("X");strArray.Add("Y");strArray.Add("BZ"); 
		//strArray.Add("编码");
		if(1 != pEx->ReadSymbolInfo(pConnect,"XMGHSDE.SYMBOLTABLE",
			                                 "XMGHSDE.CADMAPTOSDE",
											 "XMGHSDE.SP_XZLX_ZB_POLYGON","DIST",&strArray))
		{
			acutPrintf("\n Read Symbol info error!");
			pConnect->Release();
			pEx->Release();
			return;
		}

		//取得矩形范围的实体数据
		/*
		ads_point  startpt,endpt;    
		int nRet = acedGetPoint(NULL, "\n请指定矩形的左下角:",startpt);
		if (nRet != RTNORM) return ;
		nRet = acedGetCorner(startpt, "\n请指定矩形的右上角:",endpt);
		if (nRet != RTNORM) return;

		DIST_STRUCT_SELECTWAY Filter;
		Filter.nEntityType = 4;
		Filter.nSelectType = kSM_SC; // //kSM_AI;//kSM_II_OR_ET;
		Filter.ptArray.append(AcGePoint3d(startpt[0],startpt[1],0));
		Filter.ptArray.append(AcGePoint3d(endpt[0],endpt[1],0));
*/
		AcDbObjectIdArray ObjArray;
		pEx->ReadSdeDBToCAD(&ObjArray,NULL,NULL,TRUE,FALSE);//&Filter

		pEx->Release();
		pConnect->Release();
	}

	//##ModelId=46F373F2039F
	static void shSdeDataTestB(void)
	{
		IDistConnection* pConnect = CreateSDEConnectObjcet();
		if(NULL == pConnect) return ;

		if(1 != pConnect->Connect("chenyu","5151","xmghsde","xmghsde","xmghsde"))
		//if(1 != pConnect->Connect("dist-wanghui","5151","sde","sde","sde"))
		{
			acutPrintf("\nConnect Database error!");
			pConnect->Release();
			return;
		}	

		IDistExchangeData* pEx= CreateExchangeObjcet();
		if(NULL == pEx)
		{
			pConnect->Release();
			return;
		}

		CStringArray strArray;//XMGHSDE.GHLXDK_Polygon  XMGHSDE.GHLXDK_Polygon_X
		strArray.Add("PROJECT_ID");
		//strArray.Add("编码");
		if(1 != pEx->ReadSymbolInfo(pConnect,"XMGHSDE.SYMBOLTABLE","XMGHSDE.CADMAPTOSDE","XMGHSDE.GHLXDK_Polygon","DIST",&strArray))
		{
			acutPrintf("\n Read Symbol info error!");
			pConnect->Release();
			pEx->Release();
			return;
		}

		//取得矩形范围的实体数据
		/*ads_point  startpt,endpt;    
		int nRet = acedGetPoint(NULL, "\n请指定矩形的左下角:",startpt);
		if (nRet != RTNORM) return ;
		nRet = acedGetCorner(startpt, "\n请指定矩形的右上角:",endpt);
		if (nRet != RTNORM) return;

		AcDbObjectIdArray ObjArray;
		DIST_STRUCT_SELECTWAY Filter;
		Filter.nEntityType = 4;
		Filter.nSelectType = kSM_SC;
		Filter.ptArray.append(AcGePoint3d(startpt[0],startpt[1],0));
		Filter.ptArray.append(AcGePoint3d(endpt[0],endpt[1],0));*/

		pEx->SaveEntityToSdeDB(NULL,TRUE,TRUE);

		pEx->Release();
		pConnect->Release();
	}
} ;

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CSdeDataApp)

//ACED_ARXCOMMAND_ENTRY_AUTO(CSdeDataApp, shSdeData, XData, XData, ACRX_CMD_TRANSPARENT, NULL)
//ACED_ARXCOMMAND_ENTRY_AUTO(CSdeDataApp, shSdeData, TestA, TestA, ACRX_CMD_TRANSPARENT, NULL)
//ACED_ARXCOMMAND_ENTRY_AUTO(CSdeDataApp, shSdeData, TestB, TestB, ACRX_CMD_TRANSPARENT, NULL)
