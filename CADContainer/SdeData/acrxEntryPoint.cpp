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

//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CSdeDataApp : public AcRxArxApp {

public:
	CSdeDataApp () : AcRxArxApp () {}

	virtual AcRx::AppRetCode On_kInitAppMsg (void *pkt) {
		AcRx::AppRetCode retCode =AcRxArxApp::On_kInitAppMsg (pkt) ;
		// TODO: Add your initialization code here

		CDistDatabaseReactor::rxInit();

		return (retCode) ;
	}

	virtual AcRx::AppRetCode On_kUnloadAppMsg (void *pkt) {
		AcRx::AppRetCode retCode =AcRxArxApp::On_kUnloadAppMsg (pkt) ;
		// TODO: Add your code here

		deleteAcRxClass(CDistDatabaseReactor::desc());
		CDistDatabaseReactor::Release();

		return (retCode) ;
	}

	virtual void RegisterServerComponents () {
	}


	// ----- shSdeData.XData command (do not rename)
	static void shSdeDataXData(void)
	{
		// Add your code for command shSdeData.XData here
		AcDbObjectId objId;
		ads_name ent;
		ads_point pt;
		if (RTNORM != acedEntSel("\nѡ��Ķ���:", ent, pt))
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
		acutPrintf("�ֶ�����Ϊ��%d\n", ctemp.GetRecordCount());
		for (int i = 0; i < strFldNameArray.GetSize(); i++)
		{
			acutPrintf("�ֶ�����%s		�ֶ�ֵ��%s\n", strFldNameArray.GetAt(i), strFldValueArray.GetAt(i));
		}
	}
	static void shSdeDataTestA(void)
	{
		IDistConnection* pConnect = CreateSDEConnectObjcet();
		if(NULL == pConnect) return ;

		if(1 != pConnect->Connect("chenyu","5151","xmghsde","xmghsde","xmghsde"))
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
		if(1 != pEx->ReadSymbolInfo(pConnect,"XMGHSDE.SYMBOLTABLE","XMGHSDE.CADMAPTOSDE","XMGHSDE.GHLXDK_Polygon"))
		{
			acutPrintf("\n Read Symbol info error!");
			pConnect->Release();
			pEx->Release();
			return;
		}

		//ȡ�þ��η�Χ��ʵ������
		/*ads_point  startpt,endpt;    
		int nRet = acedGetPoint(NULL, "\n��ָ�����ε����½�:",startpt);
		if (nRet != RTNORM) return ;
		nRet = acedGetCorner(startpt, "\n��ָ�����ε����Ͻ�:",endpt);
		if (nRet != RTNORM) return;

		AcDbObjectIdArray ObjArray;
		DIST_STRUCT_SELECTWAY Filter;
		Filter.nEntityType = 4;
		Filter.nSelectType = kSM_SC;
		Filter.ptArray.append(AcGePoint3d(startpt[0],startpt[1],0));
		Filter.ptArray.append(AcGePoint3d(endpt[0],endpt[1],0));*/

		AcDbObjectIdArray ObjArray;
		pEx->ReadSdeDBToCAD(&ObjArray,NULL,NULL,FALSE,TRUE);

		pEx->Release();
		pConnect->Release();
	}

	static void shSdeDataTestB(void)
	{
		IDistConnection* pConnect = CreateSDEConnectObjcet();
		if(NULL == pConnect) return ;

		if(1 != pConnect->Connect("chenyu","5151","xmghsde","xmghsde","xmghsde"))
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
		if(1 != pEx->ReadSymbolInfo(pConnect,"XMGHSDE.SYMBOLTABLE","XMGHSDE.CADMAPTOSDE","XMGHSDE.GHLXDK_Polygon"))
		{
			acutPrintf("\n Read Symbol info error!");
			pConnect->Release();
			pEx->Release();
			return;
		}

		//ȡ�þ��η�Χ��ʵ������
		/*ads_point  startpt,endpt;    
		int nRet = acedGetPoint(NULL, "\n��ָ�����ε����½�:",startpt);
		if (nRet != RTNORM) return ;
		nRet = acedGetCorner(startpt, "\n��ָ�����ε����Ͻ�:",endpt);
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

ACED_ARXCOMMAND_ENTRY_AUTO(CSdeDataApp, shSdeData, XData, XData, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CSdeDataApp, shSdeData, TestA, TestA, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CSdeDataApp, shSdeData, TestB, TestB, ACRX_CMD_TRANSPARENT, NULL)