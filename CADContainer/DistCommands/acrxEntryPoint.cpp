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
#include "DistLayerTree_Ext.h"
//#include "DistToolBar_Ext.h"
//#include "DistClipPlot_Ext.h"
#include "DataDefine.h"
#include "DlgReadInfo.h"
#include "CommonFunc.h"
#include "SpcoordLabel.h"
#include "DistXData.h"
#include "DistEntityTool.h"
#include "DistSelSet.h"

#include "TKSelDlg.h"
#include "Dlg_TKYongDi.h"
#include "Dlg_TKGongCheng.h"

#include "DlgEditDesc.h"
#include "DlgEditLog.h"
#include "DlgHistory.h"
#include "DlgViewLog.h"

#include "DistCommandManagerReactor.h"

#include "DistReduceEntity.h"

#include "CommandDefine.h" // CAD命令对照表

#include "DistUiPrompts.h"

#include "..\SdeData\DistExchangeInterface.h"
#include "..\SdeData\DistBaseInterface.h"

#include "..\DistMenuManager\DistMenuManager_Ext.h"

#include "AcDocOperation.h"

#include "DataContainer.h"
// 数据库反应器
#include "SHDatabaseReactor.h"
#include <set>
#include "ConfigReader.h"

extern long __cdecl acdbSetDbmod(class AcDbDatabase *,long);

//-----------------------------------------------------------------------------
#define szRDS _RXST("")

HWND Glb_hDapForm = NULL; // 父窗体

enum DXT_MODE
{
	MODE_SDE,
	MODE_FTP
};

//DXT_MODE Glb_dxtMode = MODE_FTP;
DXT_MODE Glb_dxtMode = MODE_SDE;
BOOL Glb_bMultiConnection = FALSE;
CString Glb_strDXTPath = "基础地形图\\";

WNDPROC Glb_OldMainWndProc = NULL; // CAD主窗口 窗口函数

HWND Glb_hMDIClient = NULL;
WNDPROC Glb_OldMDIClientProc = NULL; // MDI客户区 窗口函数

IDistConnection*    Glb_pLocalConnection = NULL;         // 分局 SDE数据库连接对象指针
IDistExchangeData*  Glb_pLocalExchangeData = NULL;       // 分局 SDE<->CAD数据交换对象指针

IDistConnection*    Glb_pConnection = NULL;         // SDE数据库连接对象指针
IDistExchangeData*  Glb_pExchangeData = NULL;       // SDE<->CAD数据交换对象指针


DIST_STRUCT_SELECTWAY Glb_Filter; //定义空间搜索条件

MapString2ContentTree Glb_mapTree; // 树结构对照表

MapString2ContentTree Glb_mapHistoryTree; // 六线历史数据
CStringArray Glb_arrayHistoryLayer; // 历史图层

MapLong2Power Glb_mapPower; // 图层权限对照表
MapString2Power Glb_commandPower; // CAD命令权限对照表
MapLong2Long Glb_mapCheck; // 图层勾选状态

MapString2String Glb_mapFormItem;

POWER Glb_nFormPower; // 整个表单的权限

CStringArray Glb_layerArray; // 从SDE中调出来的图层列表

ST_SDE_CONNECTION Glb_SdeInfo; // SDE连接信息
ST_SDE_CONNECTION Glb_LocalSdeInfo; // 分局SDE连接信息
//ST_FTP_CONNECTION Glb_FtpInfo; // FTP连接信息
//ST_FTP_CONNECTION Glb_DocFtpInfo; // 档案系统FTP连接信息

ST_BIZZINFO Glb_BizzInfo; // 业务信息
ST_LAYERINFO Glb_LayerInfo; // 图层信息

MapString2String Glb_BizzInfoEx;// 业务信息

BOOL g_bAutoReadSdeData = FALSE;

//CString strAnnotionPolyLineFA = "辅助线方案"; // 辅助线方案图层
//CString strAnnotionTextFA = "文本注记方案"; // 文本注记方案图层
//CString strAnnotionPolygonFA = "辅助面方案"; // 辅助面方案图层
//CString strOtherAnnotionPolyLineFA = "其它线方案";
//
//CString strAnnotionPolyLineDA = "辅助线档案"; // 辅助线档案图层
//CString strAnnotionTextDA = "文本注记档案"; // 文本注记档案图层
//CString strAnnotionPolygonDA = "辅助面档案"; // 辅助面档案图层
//CString strOtherAnnotionPolyLineDA = "其它线档案";
//
//CString strXZHXFA = "选址红线方案"; // 选址红线方案图层
//CString strYDHXFA = "用地红线方案"; // 用地红线方案图层
//
//CString strXZHXDA = "选址红线档案";
//CString strYDHXDA = "用地红线档案";

//CString strXZZYDFA = "选址总用地方案";
//CString strXZJYDFA = "选址净用地方案";
//CString strXZDZYDFA = "选址代征用地方案";
//CString strXZZYDDA = "选址总用地档案";
//CString strXZJYDDA = "选址净用地档案";
//CString strXZDZYDDA = "选址代征用地档案";
//
//CString strYDZYDFA = "用地总用地方案";
//CString strYDJYDFA = "用地净用地方案";
//CString strYDDZYDFA = "用地代征用地方案";
//CString strYDZYDDA = "用地总用地档案";
//CString strYDJYDDA = "用地净用地档案";
//CString strYDDZYDDA = "用地代征用地档案";

//CString strHX_TEMP = "草图范围";
//CString strAnnotionPolyLine_TEMP = "草图辅助线";
//CString strAnnotionText_TEMP = "草图辅助文本";
//CString strAnnotionPolygon_TEMP = "草图辅助面";

CString strXMHX = "项目红线"; // 项目红线图层

CString strXMFWX = "项目范围线"; // 项目范围线图层

BOOL IsSaveXMFWX = FALSE;

CString strDLHX = "道路红线"; // 道路红线图层
CString strCSLX = "河道蓝线"; // 城市蓝线图层
CString strCSHX = "城市黄线"; // 城市黄线图层
CString strCSZX = "城市紫线"; // 城市紫线图层
CString strCSLVX = "城市绿线"; // 城市绿线图层
CString strCSCX = "城市橙线"; // 城市橙线图层
CString strDLZXX = "道路中心线"; // 道路中心线
CString strHDZXX = "河道中心线"; // 河道中心线

BOOL bInitEnvironment = FALSE;

typedef BOOL (__stdcall * OnUploadPltFile)(LPCTSTR lpDapPath, LPCTSTR lpBzCode, LPCTSTR lpPltFile, LPCTSTR lpUserName, LPCTSTR lpOwner);

CStringArray gongshi_array;

SHDatabaseReactor * dbReactor;

extern CDataContainer dataContainer;

///////////////////////////////////////////////////////////////////////////////////////////////

CString DistCommands_GetCurArxAppPath(CString strArxName)
{
	TCHAR exeFullPath[MAX_PATH]; 
	CString strPath; 
	GetModuleFileName(GetModuleHandle(strArxName),exeFullPath,MAX_PATH); 
	strPath=(CString)exeFullPath; 
	int position=strPath.ReverseFind('\\'); 
	strPath=strPath.Left(position+1); 
	return strPath;
}

BOOL DistCommands_ReadCfgLyNameArray(CStringArray& strLyNameArray,CString strTag)
{
	//读文件
	CFile file;
	CString strFName;
	strLyNameArray.RemoveAll();
	strFName.Format("%sCheckLayer.ini",DistCommands_GetCurArxAppPath("shDistCommands.arx"));
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


//地块重叠交叉检测
BOOL DistCommands_CheckCrossOrContain(/*CString strCurXMBH, */CString& strSrcLayerName, CString& strDestLayerName)
{
	IDistConnection* pConnection = Glb_pConnection;

	if(pConnection == NULL)
	{
		acutPrintf("\nSDE 数据库 没有连接！"); return FALSE;
	}

	CString strProjectId = DocVars.docData().GetProjectId();

	if (strProjectId.IsEmpty())
	{
		AfxMessageBox("当前文档不支持该操作");
		return FALSE;
	}

	CStringArray strArrayCheckLy;
	DistCommands_ReadCfgLyNameArray(strArrayCheckLy,"[叠加交叉检测图层]=");
	int nCheck = strArrayCheckLy.GetCount();
	if(nCheck == 0) return TRUE; //没有配置图层，就默认为通过

	//1.指定需要检测的CAD层名
	CStringArray strArrayCAD;
	//strArrayCAD.Add(strXZHXFA); // 选址红线方案
	//strArrayCAD.Add(strXZHXDA); // 选址红线档案
	//strArrayCAD.Add(strYDHXFA); // 用地红线方案
	//strArrayCAD.Add(strYDHXDA); // 用地红线档案
	
	//strArrayCAD.Add(strXZZYDFA); // 选址总用地方案
	//strArrayCAD.Add(strXZJYDFA); // 选址净用地方案
	//strArrayCAD.Add(strXZDZYDFA); // 选址代征用地方案
	//strArrayCAD.Add(strXZZYDDA); // 选址总用地档案
	//strArrayCAD.Add(strXZJYDDA); // 选址净用地档案
	//strArrayCAD.Add(strXZDZYDDA); // 选址代征用地档案

	//strArrayCAD.Add(strYDZYDFA); // 用地总用地方案
	//strArrayCAD.Add(strYDJYDFA); // 用地净用地方案
	//strArrayCAD.Add(strYDDZYDFA); // 用地代征用地方案
	//strArrayCAD.Add(strYDZYDDA); // 用地总用地档案
	//strArrayCAD.Add(strYDJYDDA); // 用地净用地档案
	//strArrayCAD.Add(strYDDZYDDA); // 用地代征用地档案
	strArrayCAD.Add(strXMHX);
	strArrayCAD.Add(strXMFWX);

	//2.根据图层，依次查询与当前项目有关的CAD实体
	for(int k=0; k<strArrayCAD.GetCount(); k++)
	{
		CString strTempCAD = strArrayCAD.GetAt(k);
		STB_CONTENT_TREE xxContentTree = Glb_mapTree[strTempCAD];
		CString strTempSDE = xxContentTree.strSdeLyName;

		if (strTempCAD.IsEmpty() || strTempSDE.IsEmpty()) 
			continue;

		//设置过虑器
		struct resbuf filter;
		filter.restype=AcDb::kDxfLayerName;
		filter.resval.rstring = strTempCAD.GetBuffer(0);
		filter.rbnext=NULL;

		//选择实体
		CDistSelSet tempSet;
		if(CDistSelSet::kDistSelected != tempSet.AllSelect(&filter)) continue;
		AcDbObjectIdArray ObjIdArray;
		tempSet.AsArray(ObjIdArray);
		int nLen = ObjIdArray.length();
		if(nLen == 0) continue;

		//选取当前项目实体
		char strRegName[512]={0};
		sprintf(strRegName,"DIST_%s",strTempSDE.GetBuffer(0));
		for(int m=0; m<nLen; m++)
		{
			AcDbObjectId tempId = ObjIdArray.at(m);
			CDistXData tempXData(tempId,strRegName);
			//CString strTempXMBH;
			CString strTempXMID; //使用项目ID
			if(tempXData.GetRecordCount()>0)  //扩展属性存在，即修改
			{
				//tempXData.Select("XMBH",strTempXMBH); tempXData.Close();
				//strTempXMBH.Trim();
				//if(strTempXMBH.CompareNoCase(strCurXMBH)!=0)  continue;
				tempXData.Select("XMID",strTempXMID); tempXData.Close();
				strTempXMID.Trim();
				if(strTempXMID.CompareNoCase(strProjectId)!=0)  continue;
			}
			else
			{
				tempXData.Close();
			}

			AcDbPolyline* pline = NULL;
			if(Acad::eOk != acdbOpenObject(pline,tempId,AcDb::kForRead)) continue;
			if(!pline->isClosed())
			{
				pline->close(); continue;
			}

			CDistEntityTool tempTool;
			AcGePoint3dArray ptArray;
			tempTool.GetPolylinePts(pline,ptArray);
			pline->close();

			int nPtCount = ptArray.length();
			if(nPtCount < 4) continue;

			IDistPoint* pts = new IDistPoint[nPtCount];
			for(int nn=0; nn<nPtCount; nn++)
			{
				pts[nn].x = ptArray[nn].x;
				pts[nn].y = ptArray[nn].y;
			}

			for(nn =0; nn<nCheck; nn++)
			{
				CString strCheckSde = strArrayCheckLy.GetAt(nn);
				if (strCheckSde.CompareNoCase(strTempSDE) == 0) // 不检测自身图层
					break;

				char strTempSQL[100]={0};
				sprintf(strTempSQL,"F:OBJECTID,T:%s,##",strCheckSde);
				if(1!=gCheckCrossOrContain(pConnection,strCheckSde.GetBuffer(0),strTempSQL,pts,nPtCount))
				{
					MapString2ContentTree::iterator _iter = Glb_mapTree.begin();
					while(_iter != Glb_mapTree.end())
					{
						CString strTempCadLayerName = _iter->first;
						STB_CONTENT_TREE xxTempContentTree  = _iter->second;

						if (xxTempContentTree.strSdeLyName.CompareNoCase(strCheckSde) == 0)
						{
							strSrcLayerName = strTempCAD;
							strDestLayerName = strTempCadLayerName;
							break;
						}

						_iter++;
					}

					if ((strSrcLayerName.IsEmpty()) || (strDestLayerName.IsEmpty()))
					{
						strSrcLayerName = strTempCAD;
						strDestLayerName = strCheckSde;
					}

					delete [] pts;
					return FALSE;
				}
			}

			delete [] pts; 
		}
	}	
	return TRUE;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void EnumHistoryLayer(long nId, ArrayLayerTree& arrayTree)
{
	for (int i=0; i<arrayTree.GetCount(); i++)
	{
		STB_CONTENT_TREE xxContentTree = arrayTree.GetAt(i);
		if (xxContentTree.nOwnerId == nId)
		{
			if(xxContentTree.strSdeLyName.IsEmpty() || xxContentTree.strSdeLyName.Find("SDE图层")>=0) 
			{
				EnumHistoryLayer(xxContentTree.nId, arrayTree);
			}
			else
			{
				Glb_mapHistoryTree[xxContentTree.strNodeName] = xxContentTree;
			}
		}
	}
}

void GenerateXmlTree(MSXML2::IXMLDOMDocumentPtr lpDocument, MSXML2::IXMLDOMElementPtr lpElement, long nId, ArrayLayerTree& arrayTree)
{
	for (int i=0; i<arrayTree.GetCount(); i++)
	{
		STB_CONTENT_TREE xxContentTree = arrayTree.GetAt(i);
		if (xxContentTree.nOwnerId == nId)
		{
			/*************** 判断图层是否可见 ************************/

			POWER power = Glb_mapPower[xxContentTree.nId];

			// 沿用整个表单的权限
			if ((Glb_nFormPower.nPower >= 0) && (Glb_nFormPower.nPrior > 0))
			{
				if (Glb_nFormPower.nPrior > power.nPrior)
				{
					power.nPower = Glb_nFormPower.nPower;
					power.nPrior = Glb_nFormPower.nPrior;
				}
			}

			BOOL bIsVisible = TRUE;
			if ((power.nPower >= 0) && (power.nPrior > 0))
			{
				bIsVisible = (power.nPower != 0);
			}

			if (!bIsVisible)
			{
//				if(!xxContentTree.strSdeLyName.IsEmpty() && xxContentTree.strSdeLyName.Find("SDE图层")<0) 
				{
					// 存储图层map信息
					Glb_mapTree[xxContentTree.strNodeName] = xxContentTree;
				}

				continue;
			}

			/*************** 判断图层是否可见 ************************/

			// 数据类型：命令或数据
			MSXML2::IXMLDOMElementPtr lpNode = NULL;

			if(xxContentTree.strSdeLyName.IsEmpty() || xxContentTree.strSdeLyName.Find("SDE图层")>=0) 
			{
				if (xxContentTree.strNodeName.Find("六线历史") >= 0)
				{
					EnumHistoryLayer(xxContentTree.nId, arrayTree);
//					continue;
				}
				lpNode = lpDocument->createElement("category");
				lpNode->setAttribute("name", _variant_t(xxContentTree.strNodeName));

				long nCheck = Glb_mapCheck[xxContentTree.nId];

				lpNode->setAttribute("checked", _variant_t(nCheck));

				lpElement->appendChild(lpNode);

				// 存储图层map信息
				Glb_mapTree[xxContentTree.strNodeName] = xxContentTree;

				GenerateXmlTree(lpDocument, lpNode, xxContentTree.nId, arrayTree);
			}
			else
			{
				lpNode = lpDocument->createElement("item");
				lpNode->setAttribute("name", _variant_t(xxContentTree.strNodeName));

				long nCheck = Glb_mapCheck[xxContentTree.nId];

				lpNode->setAttribute("checked", _variant_t(nCheck));

				lpElement->appendChild(lpNode);

				// 存储图层map信息
				Glb_mapTree[xxContentTree.strNodeName] = xxContentTree;
			}
		}
	}
}

// 删除其他项目的实体
static long DeleteExternEntities(CStringArray& layerArray)
{
	CDistEntityTool tempTool;
	for (int i=0; i<layerArray.GetCount(); i++)
	{
		CString strCadLayerName = layerArray.GetAt(i);

		int nlen = strlen(strCadLayerName)+1;

		ads_name ents;
		struct resbuf *rb;
		rb=acutNewRb(AcDb::kDxfLayerName);
		rb->restype=AcDb::kDxfLayerName;
		rb->resval.rstring = (char*) malloc(nlen*sizeof(char));
		strcpy(rb->resval.rstring, strCadLayerName);
		rb->rbnext=NULL;
		acedSSGet("X",NULL,NULL,rb,ents);
		long entNums=0;
		acedSSLength(ents,&entNums);

		for (int j=0; j < entNums; j ++)
		{
			AcDbObjectId  objId;
			ads_name ent;
			acedSSName(ents, j, ent);
			acdbGetObjectId(objId, ent);

			STB_CONTENT_TREE xxContentTree = Glb_mapTree[strCadLayerName];
			CString strSdeLayerName = xxContentTree.strSdeLyName;

			char strXDataName[1024]={0};
			sprintf(strXDataName,"DIST_%s",strSdeLayerName);
			CDistXData tempXdata(objId,strXDataName);

			//CString strXMBH;
			//tempXdata.Select("XMBH", strXMBH);
			CString strXMID; // 使用项目ID
			tempXdata.Select("XMID", strXMID);
			int nCount = tempXdata.GetRecordCount();
			tempXdata.Close();

			//strXMBH.Trim();
			strXMID.Trim();

			//if ((strXMBH.CompareNoCase(Glb_BizzInfo.strBizzCode) != 0 ) && (nCount > 0) )
			if ((strXMID.CompareNoCase(DocVars.docData().GetProjectId()) != 0 ) && (nCount > 0) )
			{
				// 删除实体
				AcDbEntity * pEnt = NULL;
				if (ErrorStatus::eOk == acdbOpenObject(pEnt, objId, AcDb::kForWrite))
				{
					pEnt->erase();
					pEnt->close();                
				}
			}
		}

		acedSSFree(ents);
		acutRelRb(rb);
	}

	return 1;
}

// 删除其他项目的实体
static long DeleteExternEntities()
{
	CStringArray layerArray;

	//layerArray.Add(strAnnotionPolyLineFA);
	//layerArray.Add(strAnnotionTextFA);
	//layerArray.Add(strAnnotionPolygonFA);
	//layerArray.Add(strAnnotionPolyLineDA);
	//layerArray.Add(strAnnotionTextDA);
	//layerArray.Add(strAnnotionPolygonDA);

	//layerArray.Add(strOtherAnnotionPolyLineFA);
	//layerArray.Add(strOtherAnnotionPolyLineDA);

	//layerArray.Add(strXZHXFA);
	//layerArray.Add(strYDHXFA);
	//layerArray.Add(strXZHXDA);
	//layerArray.Add(strYDHXDA);
	
	//layerArray.Add(strXZZYDFA);
	//layerArray.Add(strXZJYDFA);
	//layerArray.Add(strXZDZYDFA);
	//layerArray.Add(strXZZYDDA);
	//layerArray.Add(strXZJYDDA);
	//layerArray.Add(strXZDZYDDA);

	//layerArray.Add(strYDZYDFA);
	//layerArray.Add(strYDJYDFA);
	//layerArray.Add(strYDDZYDFA);
	//layerArray.Add(strYDZYDDA);
	//layerArray.Add(strYDJYDDA);
	//layerArray.Add(strYDDZYDDA);
	
	//layerArray.Add(strHX_TEMP);
	//layerArray.Add(strAnnotionPolyLine_TEMP);
	//layerArray.Add(strAnnotionText_TEMP);
	//layerArray.Add(strAnnotionPolygon_TEMP);

	layerArray.Add(strXMHX);

	return DeleteExternEntities(layerArray);
}

// 按照CAD图层名读取空间数据(六线历史图层)
static long ReadHistoryCadLayer(LPCTSTR lpCadLayer, LPCTSTR lpWhere, DIST_STRUCT_SELECTWAY& filter)
{
	long nRet = -1;

	STB_CONTENT_TREE xxContentTree = Glb_mapHistoryTree[lpCadLayer];

	IDistConnection* pConnection = Glb_pConnection;
	IDistExchangeData* pExchangeData = Glb_pExchangeData;

	if (Glb_bMultiConnection)
	{
		pConnection = xxContentTree.bIsLocalLayer ? Glb_pLocalConnection : Glb_pConnection;
		pExchangeData = xxContentTree.bIsLocalLayer ? Glb_pLocalExchangeData : Glb_pExchangeData;
	}

	if (xxContentTree.strSdeLyName.IsEmpty() || xxContentTree.strSdeLyName.Find("SDE图层")>=0) 
	{
		return nRet;
	}

	acutPrintf("\n加载图层 %s...\n", lpCadLayer);
    OutputDebugString("---- to invoke ReadSdeLayer ----");
	nRet = ReadSdeLayer(pConnection, pExchangeData, xxContentTree.strSdeLyName, &filter, lpWhere, TRUE);

	return nRet;
}

// 按照CAD图层名读取空间数据
static long ReadCadLayer(LPCTSTR lpCadLayer, DIST_STRUCT_SELECTWAY& filter, LPCTSTR lpWhere = NULL)
{
	long nRet = -1;

	STB_CONTENT_TREE xxContentTree = Glb_mapTree[lpCadLayer];

	IDistConnection* pConnection = Glb_pConnection;
	IDistExchangeData* pExchangeData = Glb_pExchangeData;
	
	if (Glb_bMultiConnection)
	{
		pConnection = xxContentTree.bIsLocalLayer ? Glb_pLocalConnection : Glb_pConnection;
		pExchangeData = xxContentTree.bIsLocalLayer ? Glb_pLocalExchangeData : Glb_pExchangeData;
	}

	if (xxContentTree.strSdeLyName.IsEmpty() || xxContentTree.strSdeLyName.Find("SDE图层")>=0) 
	{
		return nRet;
	}
		
	acutPrintf("\n加载图层 %s...\n", lpCadLayer);

	POWER power = Glb_mapPower[xxContentTree.nId];

	// 沿用整个表单的权限
	if ((Glb_nFormPower.nPower >= 0) && (Glb_nFormPower.nPrior >= 0))
	{
		if (Glb_nFormPower.nPrior > power.nPrior)
		{
			power.nPower = Glb_nFormPower.nPower;
			power.nPrior = Glb_nFormPower.nPrior;
		}
	}

	BOOL bReadOnly = TRUE;
	if ((power.nPower >= 0) && (power.nPrior >= 0))
	{
		bReadOnly = (power.nPower == 1);
	}

	// 针对临时图层需要进行过滤
	CString strSdeLyName = xxContentTree.strSdeLyName;

	if (strXMHX.CompareNoCase(lpCadLayer) == 0) //项目红线
	{
		if (NULL == lpWhere)
		{
			CString strProjectId = DocVars.docData().GetProjectId();
			if (atoi(strProjectId) > 0)
			{
				CString strWhere;
				//strWhere.Format("XMBH!='%s'", Glb_BizzInfo.strBizzCode);
				strWhere.Format("XMID!=%s", DocVars.docData().GetProjectId()); // 使用项目ID

				OutputDebugStringX("ReadCadLayer::ReadSdeLayer::%s, %s", xxContentTree.strSdeLyName, strWhere);
				nRet = ReadSdeLayer(pConnection, pExchangeData, xxContentTree.strSdeLyName, &filter, strWhere, FALSE);
				//strWhere.Format("XMBH='%s'", Glb_BizzInfo.strBizzCode);
				strWhere.Format("XMID=%s", DocVars.docData().GetProjectId()); // 使用项目ID

				OutputDebugStringX("ReadCadLayer::ReadSdeLayer::%s, %s", xxContentTree.strSdeLyName, strWhere);
				nRet = ReadSdeLayer(pConnection, pExchangeData, xxContentTree.strSdeLyName, NULL, strWhere, FALSE, FALSE);
			}
			else
			{
				OutputDebugStringX("ReadCadLayer::ReadSdeLayer::%s, %s", xxContentTree.strSdeLyName, "");
				nRet = ReadSdeLayer(pConnection, pExchangeData, xxContentTree.strSdeLyName, &filter, NULL, FALSE);
			}
		}
		else
		{
			OutputDebugStringX("ReadCadLayer::ReadSdeLayer::%s, %s", xxContentTree.strSdeLyName, lpWhere);
			nRet = ReadSdeLayer(pConnection, pExchangeData, xxContentTree.strSdeLyName, NULL, lpWhere, FALSE);
		}
	}
	else if (strXMFWX.CompareNoCase(lpCadLayer) == 0)
	{
		if (NULL == lpWhere)
		{
			OutputDebugStringX("ReadCadLayer::ReadSdeLayer::%s, %s", xxContentTree.strSdeLyName, "");
			nRet = ReadSdeLayer(pConnection, pExchangeData, xxContentTree.strSdeLyName, &filter, NULL, FALSE);
		}
		else // 根据条件进行查询
		{
			OutputDebugStringX("ReadCadLayer::ReadSdeLayer::%s, %s", xxContentTree.strSdeLyName, lpWhere);
			nRet = ReadSdeLayer(pConnection, pExchangeData, xxContentTree.strSdeLyName, NULL, lpWhere, FALSE);
		}
	}
/*	else if (
		//项目范围线
		strXMFWX.CompareNoCase(lpCadLayer) == 0
		)
	{
		CString strProjectId = DocVars.docData().GetProjectId();
		if (atoi(strProjectId) > 0)
		{
			CString strWhere;
			//strWhere.Format("XMBH!='%s'", Glb_BizzInfo.strBizzCode);
			strWhere.Format("XMID!=%s", DocVars.docData().GetProjectId()); // 使用项目ID
			nRet = ReadSdeLayer(pConnection, pExchangeData, xxContentTree.strSdeLyName, &filter, strWhere, FALSE);
			//strWhere.Format("XMBH='%s'", Glb_BizzInfo.strBizzCode);
			strWhere.Format("XMID=%s", DocVars.docData().GetProjectId()); // 使用项目ID
			nRet = ReadSdeLayer(pConnection, pExchangeData, xxContentTree.strSdeLyName, NULL, strWhere, FALSE, FALSE);
		}
		else
		{
			nRet = ReadSdeLayer(pConnection, pExchangeData, xxContentTree.strSdeLyName, &filter, NULL, FALSE);
		}
	}
*/	else
	{
		OutputDebugStringX("ReadCadLayer::ReadSdeLayer::%s, %s", xxContentTree.strSdeLyName, "");
		nRet = ReadSdeLayer(pConnection, pExchangeData, xxContentTree.strSdeLyName, &filter, NULL, FALSE);
	}

	Glb_layerArray.Add(lpCadLayer);

	// 打上此图层已经读取过的标记
	xxContentTree.bIsReadBefore = TRUE;
	Glb_mapTree[lpCadLayer] = xxContentTree;

	return nRet;
}

// 按照CAD图层名保存空间数据
static void SaveCadLayer(LPCTSTR lpCadLayer)
{
	STB_CONTENT_TREE xxContentTree = Glb_mapTree[lpCadLayer];

	IDistConnection* pConnection = Glb_pConnection;
	IDistExchangeData* pExchangeData = Glb_pExchangeData;

	if (Glb_bMultiConnection)
	{
		pConnection = xxContentTree.bIsLocalLayer ? Glb_pLocalConnection : Glb_pConnection;
		pExchangeData = xxContentTree.bIsLocalLayer ? Glb_pLocalExchangeData : Glb_pExchangeData;
	}

//	double dArea = 0.0; // 总用地面积 / 净用地面积 / 代征用地面积

	if (!xxContentTree.strSdeLyName.IsEmpty())
	{

		POWER power = Glb_mapPower[xxContentTree.nId];

		// 沿用整个表单的权限
		if ((Glb_nFormPower.nPower >= 0) && (Glb_nFormPower.nPrior >= 0))
		{
			if (Glb_nFormPower.nPrior > power.nPrior)
			{
				power.nPower = Glb_nFormPower.nPower;
				power.nPrior = Glb_nFormPower.nPrior;
			}
		}

		BOOL bReadOnly = TRUE;
		if ((power.nPower >= 0) && (power.nPrior >= 0))
		{
			bReadOnly = (power.nPower != 2);
		}

		if ((strXMHX.CompareNoCase(lpCadLayer) != 0 && strXMFWX.CompareNoCase(lpCadLayer) != 0)) //硬代码
			return;
/*
		if (strXMHX.CompareNoCase(lpCadLayer) != 0 && 
			strXMFWX.CompareNoCase(lpCadLayer) != 0 &&
			strDLHX.CompareNoCase(lpCadLayer) != 0 && 
			strCSLX.CompareNoCase(lpCadLayer) != 0 && 
			strCSHX.CompareNoCase(lpCadLayer) != 0 && 
			strCSZX.CompareNoCase(lpCadLayer) != 0 && 
			strCSLVX.CompareNoCase(lpCadLayer) != 0 && 
			strCSCX.CompareNoCase(lpCadLayer) != 0 ) //硬代码
		{
			return;
		}
*/

		AcDbObjectIdArray objIdArray;

		CString strSdeLayer = xxContentTree.strSdeLyName;
		// 保存前自动添加项目属性
		if (
			(strXMHX.CompareNoCase(lpCadLayer) == 0) ||
			(strXMFWX.CompareNoCase(lpCadLayer) == 0)
			)
		{
			//设置图层过滤器
			struct resbuf *filter=NULL;
			filter=acutNewRb(AcDb::kDxfLayerName);
			filter->restype=AcDb::kDxfLayerName;
			filter->resval.rstring = (char*) malloc(strlen(lpCadLayer)*sizeof(char)+1);
			strcpy(filter->resval.rstring,lpCadLayer);
			filter->rbnext=NULL;

			ads_name ssName;
			acedSSGet(_T("_A"), NULL, NULL, filter, ssName);

			acutRelRb(filter);

			long sslen = 0;
			acedSSLength(ssName, &sslen);

			for (int i=0L; i<sslen; i++) 
			{
				ads_name ent;
				AcDbObjectId objId;

				if (acedSSName(ssName, i, ent) != RTNORM)
				{
					continue;
				}

				if (acdbGetObjectId(objId, ent) != Acad::eOk)
				{
					acedSSFree(ent);
					continue;
				}

				acedSSFree(ent);


				CString strSdeLayerName = xxContentTree.strSdeLyName;

				char strXDataName[1024]={0};
				sprintf(strXDataName,"DIST_%s",strSdeLayerName);
				CDistXData tempXdata(objId,strXDataName);

				CString strXMID; // 使用项目ID
				tempXdata.Select("XMID", strXMID);

				if ((strXMID.CompareNoCase(DocVars.docData().GetProjectId()) != 0 ) && (tempXdata.GetRecordCount() > 0) )
				{
					continue;
				}

				MapString2String::iterator iter = Glb_BizzInfoEx.begin();
				while (iter != Glb_BizzInfoEx.end()) 
				{
					CString strName = iter->first;
					CString strValue = iter->second;

					CString strTable = strName.Left(strName.Find("→"));
					CString strField = strName.Right(strName.GetLength()-strName.Find("→")-strlen("→"));

					if (strTable.CompareNoCase(lpCadLayer) == 0)
					{
						tempXdata.Add(strField, strValue);
					}

					iter++;
				}

				tempXdata.Update();
				tempXdata.Close();

				objIdArray.append(objId);
			}

			acedSSFree(ssName);
		}

		OutputDebugStringX("正在保存图层%s", xxContentTree.strSdeLyName);
//		SaveSdeLayer(pConnection, pExchangeData, xxContentTree.strSdeLyName, NULL,/*&Glb_Filter,*/ false);
		SaveSdeLayer(pConnection, pExchangeData, xxContentTree.strSdeLyName, objIdArray, DocVars.docData().GetProjectId(), false);

	}
}

// 打开关闭图层（如果图层不存在，从sde库中读取）
void ShowLayer(CString strCadLayer, BOOL bShow, LPCTSTR lpWhere)
{
	LockDocument _Lock;

	Acad::ErrorStatus es = Acad::eOk ;

	AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();
	AcDbLayerTable *pLyTable = NULL;
	AcDbLayerTableRecord *pLyRcd = NULL;

	es = pDB->getSymbolTable(pLyTable,AcDb::kForRead);            //获得当前数据库中层表指针
	if ( es != Acad::eOk ) 
	{
		acutPrintf("\n获取LayerTable的时候发生错误，请稍后再试。\n");
		return;
	}

	STB_CONTENT_TREE xxContentTree = Glb_mapTree[strCadLayer];

	if ( pLyTable->has(strCadLayer) && xxContentTree.bIsReadBefore)
	{
		es= pLyTable->getAt(strCadLayer, pLyRcd, AcDb::kForWrite); // 在pLyRcd打开strLyName指定的图层并且返回指针到被打开的pLyRcd图层
		if( es != Acad::eOk ) 
		{
			acutPrintf("\n打开图层 %s 的时候发生错误，请稍后再试。\n", strCadLayer);
			pLyTable->close();
			return;
		}

		bool bSetIsOff = bShow ? Adesk::kFalse : Adesk::kTrue;
		if (pLyRcd->isOff() != bSetIsOff) // 查询图层开关状态，kFalse为开
		{
			if (Adesk::kTrue == bSetIsOff)
				acutPrintf("\n关闭图层 %s...\n", strCadLayer);
			else
				acutPrintf("\n打开图层 %s...\n", strCadLayer);

			pLyRcd->setIsOff(bSetIsOff); // 改变图层可见状态
		}

		pLyRcd->close();
		pLyTable->close();
	}
	else if (bShow) // 如果找不到当前图层,需要到sde库中读取
	{
		if (pLyTable->has(strCadLayer))
		{
			es= pLyTable->getAt(strCadLayer, pLyRcd, AcDb::kForWrite); // 在pLyRcd打开strLyName指定的图层并且返回指针到被打开的pLyRcd图层
			if( es != Acad::eOk ) 
			{
				acutPrintf("\n打开图层 %s 的时候发生错误，请稍后再试。\n", strCadLayer);
				pLyTable->close();
				return;
			}

			pLyRcd->setIsOff(Adesk::kFalse); // 改变图层可见状态
			pLyRcd->close();
		}
		pLyTable->close();

		ReadCadLayer(strCadLayer, Glb_Filter, lpWhere);
	}
	else
	{
		pLyTable->close();
	}
}

// 连接SDE数据库
void CreateLocalSdeConnection(LPCTSTR lpSdeServer, LPCTSTR lpSdeService, LPCTSTR lpSdeDatabase, LPCTSTR lpSdeUserName, LPCTSTR lpSdePassword)
{
	if(Glb_pLocalConnection != NULL)
	{
		Glb_pLocalConnection->Disconnect();
		Glb_pLocalConnection->Release();
		Glb_pLocalConnection = NULL;
	}

	Glb_pLocalConnection = CreateSDEConnectObjcet();
	if(NULL == Glb_pLocalConnection) 
	{
		OutputDebugStringX("创建SDE连接对象失败");
		return ;
	}
	long rc = -1;
	if((rc = Glb_pLocalConnection->Connect(lpSdeServer, lpSdeService, lpSdeDatabase, lpSdeUserName, lpSdePassword)) != 1)
	{
		Glb_pLocalConnection->Release();
		Glb_pLocalConnection = NULL;

		OutputDebugStringX("SDE数据库连接失败,错误原因：%s ",ReturnErrorCodeInfo(rc));
		return ;
	}
	if (NULL == Glb_pLocalExchangeData)
	{
		Glb_pLocalExchangeData = CreateExchangeObjcet();
		if(NULL == Glb_pLocalExchangeData)
		{
			OutputDebugStringX("CreateSDEConnectObjcet 创建SDE与CAD互换类对象");
			return ;
		}
	}
}

// 连接SDE数据库
void CreateSdeConnection(LPCTSTR lpSdeServer, LPCTSTR lpSdeService, LPCTSTR lpSdeDatabase, LPCTSTR lpSdeUserName, LPCTSTR lpSdePassword)
{
	if(Glb_pConnection != NULL)
	{
		Glb_pConnection->Disconnect();
		Glb_pConnection->Release();
		Glb_pConnection = NULL;
	}

	Glb_pConnection = CreateSDEConnectObjcet();
	if(NULL == Glb_pConnection) 
	{
		OutputDebugStringX("创建SDE连接对象失败");
		return ;
	}
	long rc = -1;
	if((rc = Glb_pConnection->Connect(lpSdeServer, lpSdeService, lpSdeDatabase, lpSdeUserName, lpSdePassword)) != 1)
	{
		Glb_pConnection->Release();
		Glb_pConnection = NULL;

		OutputDebugStringX("SDE数据库连接失败,错误原因：%s ",ReturnErrorCodeInfo(rc));
		return ;
	}
	if (NULL == Glb_pExchangeData)
	{
		Glb_pExchangeData = CreateExchangeObjcet();
		if(NULL == Glb_pExchangeData)
		{
			OutputDebugStringX("CreateSDEConnectObjcet 创建SDE与CAD互换类对象");
			return ;
		}
	}
}

// Modify
//// 加载工具条
//void LoadDistToolBar()
//{
//	// 卸载工具条
//	Sys_UnLoadDistToolBar();
//
//	// 得到arx所在路径
//	TCHAR lpPath[MAX_PATH] = {0};
//	GetModuleFileName(GetModuleHandle("shDistCommands.arx"), lpPath, MAX_PATH);
//	PathRemoveFileSpec(lpPath);
//	PathAddBackslash(lpPath);
//
//	CString strXmlFile = lpPath;
//	strXmlFile += "toolbar.xml";
//
//	HRESULT hr = S_OK;
//	MSXML2::IXMLDOMDocumentPtr lpDocument;
//	hr = lpDocument.CreateInstance(__uuidof(DOMDocument));
//
//	if ( FAILED(hr) ) 
//		_com_raise_error(hr);
//
//	// 装载XML字符串
//	if (!lpDocument->load(_variant_t(strXmlFile)))
//	{
//		OutputDebugString("LoadXML failed!");
//
//		return;
//	}
//
//	MSXML2::IXMLDOMElementPtr lpDocElement = NULL;
//	lpDocument->get_documentElement(&lpDocElement);
//
//	// toolbar list
//	MSXML2::IXMLDOMNodeListPtr lpToolbarList = lpDocElement->getElementsByTagName(_bstr_t("toolbar"));
//
//	MSXML2::IXMLDOMNodePtr lpToolbar = NULL;
//
//	for ( ; (lpToolbar = lpToolbarList->nextNode()) != NULL ; )
//	{
//		MSXML2::IXMLDOMNodePtr lpCaptionNode = lpToolbar->attributes->getNamedItem("caption");
//		CString strCaption = (char*)lpCaptionNode->text;
//
//		OutputDebugString(strCaption);
//
//		// button list
//		MSXML2::IXMLDOMNodeListPtr lpButtonList = lpToolbar->GetchildNodes();
//
//		MSXML2::IXMLDOMNodePtr lpButton = NULL;
//
//		for ( ; (lpButton = lpButtonList->nextNode()) != NULL ; )
//		{
//			MSXML2::IXMLDOMNodePtr lpToolTipNode = lpButton->attributes->getNamedItem("tooltip");
//			CString strToolTip = (char*)lpToolTipNode->text;
//
//			MSXML2::IXMLDOMNodePtr lpCommandNode = lpButton->attributes->getNamedItem("command");
//			CString strCommand = (char*)lpCommandNode->text;
//			strCommand += " ";
//
//			MSXML2::IXMLDOMNodePtr lpIconNode = lpButton->attributes->getNamedItem("icon");
//			CString strIcon = (char*)lpIconNode->text;
//
//			OutputDebugStringX("tooltip=%s, command=%s, icon=%s", strToolTip, strCommand, strIcon);
//
//			CString strIconFile = lpPath;
//			strIconFile += strIcon;
//
//			if (!PathFileExists(strIconFile))
//			{
//				strIconFile = lpPath;
//				strIconFile += "dist.ico";
//			}
//
//			if (!PathFileExists(strIconFile)) continue;
//
////			hr = tlbr->AddToolbarButton(index, CComBSTR(strToolTip), CComBSTR(strToolTip), CComBSTR(strCommand), vtFalse, &button);
//		}
//	}
//
//	// 加载工具条
//	Sys_LoadDistToolBar("");
//}

//// 读取市局图层列表
//void LoadExCadLayer()
//{
//	TCHAR lpReturnedString[1024] = {0};
//
//	TCHAR lpCadProjectFileName[MAX_PATH] = {0};
//	GetModuleFileName(GetModuleHandle("shDistCommands.arx"), lpCadProjectFileName, MAX_PATH); 
//	PathRemoveFileSpec(lpCadProjectFileName);
//
//	PathAppend(lpCadProjectFileName, "CadProject.ini");
//
//	GetPrivateProfileSection("市局图层", lpReturnedString, 1024, lpCadProjectFileName);
//
//	int i=0;
//	CString strLayerName = lpReturnedString;
//	while (!strLayerName.IsEmpty())
//	{
//		Glb_mapTree[strLayerName].bIsLocalLayer = FALSE;
//
//		while (lpReturnedString[i++] != '\0')
//		{
//		}
//		strLayerName = lpReturnedString+i;
//	}
//
//}

//BOOL LoadFormItemText(MapString2String& attributes)
//{
//	// 得到arx所在路径
//	TCHAR lpPath[MAX_PATH] = {0};
//	GetModuleFileName(GetModuleHandle("shDistCommands.arx"), lpPath, MAX_PATH);
//	PathRemoveFileSpec(lpPath);
//	PathAddBackslash(lpPath);
//
//	CString strXmlFile = lpPath;
//	strXmlFile += "CadProject.xml";
//
//	HRESULT hr = S_OK;
//	MSXML2::IXMLDOMDocumentPtr lpDocument;
//	hr = lpDocument.CreateInstance(__uuidof(DOMDocument));
//
//	if ( FAILED(hr) ) 
//		_com_raise_error(hr);
//
//	// 装载XML字符串
//	if (!lpDocument->load(_variant_t(strXmlFile)))
//	{
//		OutputDebugString("LoadXML failed!");
//		return FALSE;
//	}
//
//	MSXML2::IXMLDOMNodeListPtr lpNodeList = lpDocument->getElementsByTagName(_bstr_t("bizz"));
//	if (NULL == lpNodeList)
//	{
//		OutputDebugString("NULL == lpNodeList");
//		return FALSE;
//	}
//
//	MSXML2::IXMLDOMNodePtr lpBizzNode = NULL;
//	for ( ; (lpBizzNode = lpNodeList->nextNode()) != NULL ; )
//	{
//		MSXML2::IXMLDOMNodePtr lpChildNode = lpBizzNode->attributes->getNamedItem("id");
//		CString strBizzId = (char*)lpChildNode->text;
//
//		OutputDebugStringX("strBizzId :: %s", strBizzId);
//		OutputDebugStringX("curBizzId :: %s", Glb_BizzInfo.strBzIdId);
//
//		if (Glb_BizzInfo.strBzIdId.CompareNoCase(strBizzId) == 0)
//		{
//			OutputDebugStringX("getElementsByTagName :: %s", strBizzId);
//			break;
//		}
//	}
//
//	if (NULL == lpBizzNode)
//	{
//		OutputDebugString("NULL == lpBizzNode");
//		return FALSE;
//	}
//
//	// 解析参数列表
//	MSXML2::IXMLDOMNodeListPtr lpChildNodeList = lpBizzNode->GetchildNodes();
//
//	MSXML2::IXMLDOMNodePtr lpParam = NULL;
//
//	CString strCommand = "GetFormItemText";
//	CStringArray strParamArray;
//
//	CStringArray arrayLabel;
//
//	for ( ; (lpParam = lpChildNodeList->nextNode()) != NULL ; )
//	{
//		MSXML2::IXMLDOMNodePtr lpNameNode = lpParam->attributes->getNamedItem("name");
//		CString strName = (char*)lpNameNode->text;
//
//		strName.TrimLeft();		strName.TrimRight();
//
//		OutputDebugStringX("name::::%s", strName);
//
//		MSXML2::IXMLDOMNodePtr lpIdNode = lpParam->attributes->getNamedItem("id");
//		CString strId = (char*)lpIdNode->text;
//
//		strId.TrimLeft();		strId.TrimRight();
//
//		OutputDebugStringX("id::::%s", strId);
//
//		if (strId.Find("{") < 0)
//			strId = "{" + strId + "}";
//
//		arrayLabel.Add(strName);
//
//		CString strParam;
//		strParam.Format("%s=%s", strName, strId);
//		strParamArray.Add(strParam);
//
//	}
//
//	SendCopyDataMessage(Glb_hDapForm, strCommand, strParamArray);
//
//	for (int i=0; i<arrayLabel.GetCount(); i++)
//	{
//		CString strLabel = arrayLabel.GetAt(i);
//
//		TCHAR lpValue[100] = {0};
//		GetPrivateProfileString("图框", strLabel, "", lpValue, 100, "BizzInfo.ini");
//
//		CString strValue = lpValue;
//		if (!strValue.IsEmpty())
//		{
//			attributes[strLabel] = strValue;
//		}
//	}
//
//	return TRUE;
//}

// 从SDE库中读取图层树
void LoadLayerTree()
{
	IDistConnection* pConnection = Glb_pConnection;

	IDistCommand *pCommand = CreateSDECommandObjcet();
	if(NULL == pCommand) return;
	pCommand->SetConnectObj(pConnection);

	TCHAR lpSQL[256] = {0};
	sprintf(lpSQL, "F:SELFID,OWNERID,NODENAME,SDELYNAME,POSORDER,T:TB_CONTENT_TREE,O:order by posorder,##");
	IDistRecordSet *pRcdSet = NULL;
	if (pCommand->SelectData(lpSQL, &pRcdSet) != 1)
	{
		pCommand->Release(); 

		if(pRcdSet != NULL) 
		{
			pRcdSet->Release();
		}
		return;
	}

	ArrayLayerTree arrayTree;
	while (pRcdSet->BatchRead() == 1)
	{
		STB_CONTENT_TREE xxContentTree;
		xxContentTree.nId          = *(int*)(*pRcdSet)[0];
		xxContentTree.nOwnerId     = *(int*)(*pRcdSet)[1];
		xxContentTree.strNodeName  = (char*)(*pRcdSet)[2];
		xxContentTree.strSdeLyName = (char*)(*pRcdSet)[3];
		xxContentTree.strPosOrder  = (char*)(*pRcdSet)[4];

		arrayTree.Add(xxContentTree);
	}

	pRcdSet->Release();
	pCommand->Release();

	MSXML2::IXMLDOMDocumentPtr lpDocument;
	long hr = lpDocument.CreateInstance(__uuidof(DOMDocument));

	if ( FAILED(hr) ) 
		_com_raise_error(hr);

	MSXML2::IXMLDOMProcessingInstructionPtr lpInstruction = lpDocument->createProcessingInstruction("xml","version='1.0' encoding='GB2312'");
	lpDocument->appendChild(lpInstruction);

	MSXML2::IXMLDOMElementPtr lpRoot = lpDocument->createElement(LPCTSTR("root"));

	lpDocument->appendChild(lpRoot);

	// 清空图层对照数据
	Glb_mapTree.clear();

	GenerateXmlTree(lpDocument, lpRoot, -1, arrayTree);

	CString strProjectId = DocVars.docData().GetProjectId(); // 六线调整为0,电子展板为-1

	Sys_LoadLayerTree(lpDocument->xml, atoi(strProjectId));

/*	CStringArray array;
	array.Add("审批红线图层");
	array.Add("项目红线");
	array.Add("项目范围线");
	Sys_SetCheckedLayers(array);
*/
	OutputDebugString("LoadLayerTree");
	OutputDebugString(lpDocument->xml);
}

void ClearScreenOn()
{
	SendCommandToAutoCAD("CLEANSCREENON ");
//	acedCommand(RTSTR, "CLEANSCREENON",0);
}

void LoadAffixFile(LPCTSTR lpFile)
{
	//if(CheckTheDWGFileHasInsert(lpFile)) 
	//{
	//	OutputDebugStringX("CheckTheDWGFileHasInsert(%s)", lpFile);
	//	return;
	//}

	TCHAR lpTempPath[MAX_PATH] = {0};
	strcpy(lpTempPath, lpFile);

	PathRemoveFileSpec(lpTempPath);
	LPTSTR lpTempFile = PathFindFileName(lpFile);

	AcGePoint3d pt(0,0,0);
	AcGeScale3d scale(1,1,1);

	//查找地形图并将其插入当前图面
	AcDbObjectId objectId = FindDWGAndInsertAsBlock(lpTempPath, lpTempFile, pt, scale, 0, -1, "0");

	//if(objectId == NULL)
	//{
	//	acutPrintf("\n%s图形文件插入失败！\n",lpTempFile);
	//	return;
	//}
	//else
	//{
	//	DocVars.docData().m_mapBlockIds.SetAt(lpTempFile, objectId);
	//}

	// 通过cadcontainer实现剩余功能（清除源文件）
	CString strCommand = "CLEANLOCALWORKDWG";
	CStringArray strParamArray;

	CString strParam;
	strParam.Format("filename=%s", lpFile);
	strParamArray.Add(strParam);

	SendCopyDataMessage(Glb_hDapForm, strCommand, strParamArray);

//	_activeCurrentCadView();

	CString strProjectId = DocVars.docData().GetProjectId();
	if (strProjectId.CompareNoCase("-1") == 0) // 电子展板
	{
		SendCommandToAutoCAD("CLEANSCREENON ");
	}
	else
	{
		SendCommandToAutoCAD("CLEANSCREENOFF ");
	}

	if (g_bAutoReadSdeData)
	{
		g_bAutoReadSdeData = FALSE;
//		SendCommandToAutoCAD("ReadSdeLayers ");

		// 2009-05-04 //
		// 从项目导航进入后，图层树，不勾选任何图层，也不下载任何数据，如果项目已经有了项目范围线，也不做下载
/*		CString strProjectId = DocVars.docData().GetProjectId();

		CString strWhere;
		if (atoi(strProjectId) > 0)
		{
			strWhere.Format("XMID=%s", DocVars.docData().GetProjectId()); // 使用项目ID
		}

		LockDocument _Lock;
		ReadCadLayer("项目红线", Glb_Filter, strWhere.IsEmpty() ? NULL : (LPCTSTR)strWhere);
		ReadCadLayer("项目范围线", Glb_Filter, strWhere.IsEmpty() ? NULL : (LPCTSTR)strWhere);
*/	
		// 2009-05-04 //
	}

	struct resbuf * rb_cmdecho_Off = acutBuildList(RTSHORT,0,0);
	struct resbuf * rb_cmdecho_On = acutBuildList(RTSHORT,1,0);

	acedSetVar("cmdecho", rb_cmdecho_Off);

	if (Glb_Filter.ptArray.logicalLength() > 0)
	{
		CString strLB, strRT;
		strLB.Format("%f,%f", Glb_Filter.ptArray.at(0).x, Glb_Filter.ptArray.at(0).y);
		strRT.Format("%f,%f", Glb_Filter.ptArray.at(1).x, Glb_Filter.ptArray.at(1).y);
		acedCommand(RTSTR,"_ZOOM",RTSTR,"W",RTSTR, strLB, RTSTR, strRT, 0);

		CString strCmd;
		strCmd.Format("zoom %s %s\n", strLB, strRT);
		Acad::ErrorStatus es = acDocManager->sendStringToExecute( curDoc(), strCmd);
//		setView(Glb_Filter.ptArray.at(0), Glb_Filter.ptArray.at(1), 1.0);
	}
	else
	{
//		acedCommand(RTNONE);
//		acedCommand(RTSTR, "_zoom",RTSTR,"E",0);
//		SendCommandToAutoCAD("ZoomExtents ");
//		fZoomExtents();
		Acad::ErrorStatus es = acDocManager->sendStringToExecute( curDoc(), "zoom e\n");
	}

	acedSetVar("cmdecho", rb_cmdecho_On);
	acutRelRb(rb_cmdecho_Off);
	acutRelRb(rb_cmdecho_On);
}

// 获取红线绘制范围
BOOL GetRangePoints(AcGePoint3d& ptLB, AcGePoint3d& ptRT)
{
	IDistConnection* pConnection = Glb_pConnection;
	IDistExchangeData* pExchangeData = Glb_pExchangeData;

	if (NULL == pExchangeData)
	{
		pExchangeData = CreateExchangeObjcet();
	}

	//CString strSdeLayerName = Glb_LayerInfo.strLayer; //".KGFWTZ_HZ";
	//CString strField = Glb_LayerInfo.strField; //"XMBH";
	//CString strBizzCode = Glb_BizzInfo.strBizzCode;
	//CString strBizzSame = Glb_BizzInfo.strBizzSame;

	CString strProjectId = DocVars.docData().GetProjectId();

	if (strProjectId.IsEmpty())
	{
		AfxMessageBox("当前文档不支持该操作");
		return FALSE;
	}

	STB_CONTENT_TREE xxContentTree = Glb_mapTree[strXMFWX];

	if (xxContentTree.strSdeLyName.IsEmpty())
	{
//		AfxMessageBox("未找到项目范围线对应的SDE图层！", MB_OK);
		return FALSE;
	}

	TCHAR lpSQL[256] = {0};
	//if (strField.CompareNoCase("XMBH") == 0) // 按照项目编号定位
	//	sprintf(lpSQL, "F:OBJECTID,SHAPE,T:%s,W:%s='%s',##", strSdeLayerName, strField, strBizzCode);
	//else // 按照联动编号定位
	//	sprintf(lpSQL, "F:OBJECTID,SHAPE,T:%s,W:%s='%s',##", strSdeLayerName, strField, strBizzSame);
	sprintf(lpSQL, "F:OBJECTID,SHAPE,T:%s,W:XMID='%s',##", xxContentTree.strSdeLyName, strProjectId); // 按照项目ID定位

	AcGePoint3dArray ptArray;
	pExchangeData->GetSearchShapePointArray(pConnection, lpSQL, &ptArray);

	if (ptArray.logicalLength() <= 0)
	{
		return FALSE;
	}

	// 对点数组进行处理
	for(int i=0; i<ptArray.logicalLength(); i++)
	{
		// left
		if (ptLB.x == 0 || ptArray.at(i).x < ptLB.x)
			ptLB.x = ptArray.at(i).x;
		// bottom
		if (ptLB.y == 0 || ptArray.at(i).y < ptLB.y)
			ptLB.y = ptArray.at(i).y;
		// right
		if (ptRT.x == 0 || ptArray.at(i).x > ptRT.x)
			ptRT.x = ptArray.at(i).x;
		// top
		if (ptRT.y == 0 || ptArray.at(i).y > ptRT.y)
			ptRT.y = ptArray.at(i).y;
	}

	return TRUE;
}

// 自定义的窗口函数
LRESULT CALLBACK DistMDIClientProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch( message )
	{
	case WM_CLOSE:
		return 0;
	default:
		break;
	}

	return CallWindowProc( Glb_OldMDIClientProc, hWnd, message, wParam, lParam ); 
}

#include "XdeFile.h"
BOOL ReadSdeParams(LPCTSTR lpXdeFile)
{
	// 打开当前文件
	HANDLE  hFile = CreateFile(lpXdeFile, GENERIC_READ, 0, 
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if(hFile == INVALID_HANDLE_VALUE)
	{
		MessageBox(NULL, _T("打开SDE连接文件失败"), 
			_T("错误"), MB_ICONSTOP);
		return FALSE;
	}

	CXdeFile xdeFile;
	if (!xdeFile.ReadXdeFile(hFile))
	{
		CloseHandle(hFile);
		return FALSE;
	}

	CloseHandle(hFile);

	ST_SDE_DATA xxSdeData = xdeFile.GetSdeData();
	Glb_LocalSdeInfo.strServer = xxSdeData.strServer;
	Glb_LocalSdeInfo.strService = xxSdeData.strService;
	Glb_LocalSdeInfo.strDatabase = xxSdeData.strDatabase;
	Glb_LocalSdeInfo.strUserName = xxSdeData.strUserName;
	Glb_LocalSdeInfo.strPassword = xxSdeData.strPassword;

	return TRUE;
}

// 消息处理
void ProcessCommand(CString& strCommand, MapString2String& parameters)
{
	CString strDebugInfo;
	strDebugInfo.Format("ProcessCommand : %s", strCommand);
	OutputDebugString(strDebugInfo);

	// 显示命令行
	if (strCommand.CompareNoCase("ShowCommandLineWindow") == 0)
	{
		ShowCommandLineWindow(TRUE);
	}
	// 业务数据
	else if (strCommand.CompareNoCase("bizzdata") == 0)
	{
		// 连接SDE数据库
		CreateSdeConnection(Glb_SdeInfo.strServer.GetBuffer(), Glb_SdeInfo.strService.GetBuffer(), Glb_SdeInfo.strDatabase.GetBuffer(), Glb_SdeInfo.strUserName.GetBuffer(), Glb_SdeInfo.strPassword.GetBuffer());

		if (Glb_bMultiConnection)
		{
			// 连接分局SDE数据库
			CreateLocalSdeConnection(Glb_LocalSdeInfo.strServer.GetBuffer(), Glb_LocalSdeInfo.strService.GetBuffer(), Glb_LocalSdeInfo.strDatabase.GetBuffer(), Glb_LocalSdeInfo.strUserName.GetBuffer(), Glb_LocalSdeInfo.strPassword.GetBuffer());
		}

		DocVars.docData().SetProjectId(parameters["projectid"]);

		Glb_SdeInfo.strServer.ReleaseBuffer();
		Glb_SdeInfo.strService.ReleaseBuffer();
		Glb_SdeInfo.strDatabase.ReleaseBuffer();
		Glb_SdeInfo.strUserName.ReleaseBuffer();
		Glb_SdeInfo.strPassword.ReleaseBuffer();

		Glb_LocalSdeInfo.strServer.ReleaseBuffer();
		Glb_LocalSdeInfo.strService.ReleaseBuffer();
		Glb_LocalSdeInfo.strDatabase.ReleaseBuffer();
		Glb_LocalSdeInfo.strUserName.ReleaseBuffer();
		Glb_LocalSdeInfo.strPassword.ReleaseBuffer();

		Glb_BizzInfo.strBizzCode = parameters["bi_code"];
		Glb_BizzInfo.strBizzSame = parameters["bi_same"];
		Glb_BizzInfo.strBizzName = parameters["bi_name"];
		Glb_BizzInfo.strBzOwner = parameters["bi_owner"];
		Glb_BizzInfo.strBzIdId = parameters["bi_BzId"];
		Glb_BizzInfo.strBzId = parameters["bi_BzName"];
		Glb_BizzInfo.strBztmOver = parameters["bi_tmOver"];
		Glb_BizzInfo.strCurTime = parameters["cur_time"];

		Glb_BizzInfo.strUserId = parameters["lg_UserId"];
		Glb_BizzInfo.strLoginUserName = parameters["lg_UserName"];

		OutputDebugStringX("bi_code=%s, bi_same=%s, bi_name=%s, lg_UserName=%s", 
			Glb_BizzInfo.strBizzCode, Glb_BizzInfo.strBizzSame, Glb_BizzInfo.strBizzName, Glb_BizzInfo.strLoginUserName);

		CString strIniFile = "BizzInfo.ini";
		WritePrivateProfileString("BizzInfo","项目编号",Glb_BizzInfo.strBizzCode,strIniFile);
		WritePrivateProfileString("BizzInfo","项目名称",Glb_BizzInfo.strBizzName,strIniFile);
		WritePrivateProfileString("BizzInfo","建设单位",Glb_BizzInfo.strBzOwner,strIniFile);
		WritePrivateProfileString("BizzInfo","业务类型",Glb_BizzInfo.strBzId,strIniFile);
		WritePrivateProfileString("BizzInfo","结束日期",Glb_BizzInfo.strBztmOver,strIniFile);
	}
	// 项目属性（需要写回图层字段中的属性值）
	else if (strCommand.CompareNoCase("bizzdata_ex") == 0)
	{
		Glb_BizzInfoEx.clear();
		Glb_BizzInfoEx = parameters;
	}
	// sde数据(连接参数,图层参数)
	else if (strCommand.CompareNoCase("sdedata") == 0)
	{
		// DAP表单句柄
		Glb_hDapForm = HWND(atol(parameters["parent_hwnd"]));

		// 市局SDE连接参数
		Glb_SdeInfo.strServer = parameters["server"];
		Glb_SdeInfo.strService = parameters["service"];
		Glb_SdeInfo.strDatabase = parameters["database"];
		Glb_SdeInfo.strUserName = parameters["username"];
		Glb_SdeInfo.strPassword = parameters["password"];

		// SDE图层配置
		Glb_LayerInfo.strAnnotionLine = parameters["标注线层"]; // 标注线层
		Glb_LayerInfo.strAnnotionText = parameters["标注文字层"]; // 标注文字层
		Glb_LayerInfo.strLayer = parameters["范围图层"];
		Glb_LayerInfo.strField = parameters["范围图层字段"];
		Glb_LayerInfo.strMapLayer = parameters["地形图"];
		Glb_LayerInfo.strIndexField = parameters["地形图索引字段"];

		if (Glb_LayerInfo.strIndexField.IsEmpty())
			Glb_LayerInfo.strIndexField = "图号";

		OutputDebugStringX("SdeServer=%s, SdeService=%s, SdeDatabase=%s, SdeUserName=%s, SdePassword=%s", 
			Glb_SdeInfo.strServer, Glb_SdeInfo.strService, Glb_SdeInfo.strDatabase, Glb_SdeInfo.strUserName, Glb_SdeInfo.strPassword);

		// 清空图层信息
		Glb_layerArray.RemoveAll();
	}
	// 权限设置
	else if (strCommand.CompareNoCase("SetItemPower") == 0)
	{
		Glb_nFormPower.nPower = -1;
		Glb_nFormPower.nPrior = -1;

		Glb_mapPower.clear();
		Glb_commandPower.clear();

		MapString2String::iterator _iter = parameters.begin();
		while(_iter != parameters.end())
		{
			CString strId = _iter->first;
			CString strPower = _iter->second;

			// "ID:%d=POWER:%d,PRIOR:%d"

			int nPos = strId.Find("ID:") + strlen("ID:");
			long nId = atol(strId.Mid(nPos, strId.GetLength()-nPos));

			POWER power;
			nPos = strPower.Find("POWER:") + strlen("POWER:");
			power.nPower = atol(strPower.Mid(nPos, strPower.Find(',')-nPos));
			nPos = strPower.Find("PRIOR:") + strlen("PRIOR:");
			power.nPrior = atol(strPower.Mid(nPos, strPower.GetLength()-nPos));

			OutputDebugStringX("ID=%d, POWER=%d, PRIOR=%d", nId, power.nPower, power.nPrior);

			if (nId == 88888888) // 所有CAD图层
			{
				Glb_nFormPower.nPower = power.nPower;
				Glb_nFormPower.nPrior = power.nPrior;
			}
			else if (nId > 80000000) // CAD命令
			{
				int nCount = sizeof(Glb_strCommandToId)/sizeof(CString);

				for (int i=0; i<nCount; i+=3)
				{
					CString strId = Glb_strCommandToId[i];
					CString strCommand = Glb_strCommandToId[i+1];

					if (nId == atoi(strId))
						Glb_commandPower[strCommand] = power;
				}
			}
			else // CAD图层
			{
				Glb_mapPower[nId] = power;
			}

			_iter++;
		}
	}
	// 加载图层树
	else if (strCommand.CompareNoCase("LoadLayerTree") == 0)
	{
		CString strProjectId = DocVars.docData().GetProjectId();

		OutputDebugStringX("LoadLayerTree::strProjectId=%s", strProjectId);

		if (strProjectId.CompareNoCase("0") == 0) // 六线调整时ProjectId为0
		{
			Sys_UnLoadDistMenu("规划用图");
			Sys_LoadDistMenu("六线调整");
		}
		else if (strProjectId.CompareNoCase("-1") == 0) // 电子展板
		{
			Sys_UnLoadDistMenu("六线调整");
			Sys_UnLoadDistMenu("规划用图");
		}
		else
		{
			g_bAutoReadSdeData = TRUE;
			Sys_UnLoadDistMenu("六线调整");
			Sys_LoadDistMenu("规划用图");
		}

/*		//屏蔽关闭按钮
		CMenu* pMenu;
		pMenu=acedGetAcadFrame()->GetActiveFrame()->GetSystemMenu(FALSE);
		pMenu->DeleteMenu(SC_CLOSE,MF_BYCOMMAND|MF_GRAYED);
		pMenu->DeleteMenu(SC_NEXTWINDOW,MF_BYCOMMAND|MF_GRAYED);
		pMenu->DeleteMenu(0,MF_BYCOMMAND|MF_GRAYED);
		pMenu->DeleteMenu(0,MF_BYCOMMAND|MF_GRAYED);
*/
		Glb_mapCheck.clear();

		MapString2String::iterator iter = parameters.begin();
		while (iter != parameters.end())
		{
			Glb_mapCheck[atol(iter->first)] = atol(iter->second);

			OutputDebugStringX("Glb_mapCheck[%d] = %d", atol(iter->first), atol(iter->second));

			iter++;
		}
		if (strProjectId.CompareNoCase("-1") == 0) // 电子展板
		{
			return;
		}

		// 加载图层树
		LoadLayerTree();

		if (strProjectId.CompareNoCase("0") == 0) // 六线调整
		{
			Sys_ShowSampToolBar(TRUE);
			return;
		}
		else
		{
			Sys_ShowSampToolBar(FALSE);
		}

		// 2009-05-04 //
		// 从项目导航进入后，图层树，不勾选任何图层，也不下载任何数据，如果项目已经有了项目范围线，也不做下载
/*		CStringArray array;
		array.Add("审批红线图层");
		array.Add("项目红线");
		array.Add("项目范围线");
		Sys_SetCheckedLayers(array);
*/
		// 2009-05-04 //

		AcGePoint3d ptLB, ptRT;
/*
		// 获取红线绘制范围
		if (GetRangePoints(ptLB, ptRT))
		{
			// 定义空间过滤条件
			Glb_Filter.nEntityType = 4;
			Glb_Filter.nSelectType = kSM_AI;
			Glb_Filter.ptArray.setLogicalLength(0);

			Glb_Filter.ptArray.append(AcGePoint3d(ptLB[0], ptLB[1], ptLB[2]));
			Glb_Filter.ptArray.append(AcGePoint3d(ptRT[0], ptRT[1], ptRT[2]));

//			ReadCadLayer("项目红线");
//			ReadCadLayer("项目范围线");
		}
		else
		{
			Glb_Filter.ptArray.setLogicalLength(0);

			acutPrintf("\n没有找到绘制的项目范围\n");
		}
*/
		Glb_Filter.ptArray.setLogicalLength(0);
	}
/*	// 加载工具条
	else if (strCommand.CompareNoCase("LoadDistToolBar") == 0)
	{
		LoadDistToolBar();
	}
//*/	// 读取SDE数据
//	else if (strCommand.CompareNoCase("ReadSdeData") == 0)
//	{
//		if (acDocManager->documentCount() <= 0)
//		{
//			static char pData[] = "acadiso.dwt";
//			if (acDocManager->isApplicationContext()) 
//			{
//				acDocManager->appContextNewDocument((const char *)pData);
//			}
//			else
//			{
//				acutPrintf("\n创建新文档失败!\n");
//			}
//		}
//
//		// 消息处理函数
//		HWND hView = acedGetAcadDwgView()->m_hWnd;
//		if (hView != NULL)
//			hView = GetParent(hView);
//		if (hView != NULL)
//		{
//			LONG proc = GetWindowLong(hView, GWL_WNDPROC);
//			if (proc != (LONG)DistMDIClientProc)
//			{
//				Glb_OldMDIClientProc = (WNDPROC)SetWindowLong(hView, GWL_WNDPROC, (LONG)DistMDIClientProc);
//			}
//		}
//
//		ShowCommandLineWindow(TRUE);
//		SendCommandToAutoCAD("DeleteAllEntity ");
//		SendCommandToAutoCAD("ShowDockTreePane ");
//
//		if ((Glb_pConnection == NULL) || (Glb_pExchangeData == NULL))
//			return;
//
//		if (Glb_dxtMode == MODE_FTP)
//		{
//			SendCommandToAutoCAD("LoadBaseMap ");
//		}
//
//		SendCommandToAutoCAD("ReadSdeLayers ");
//
//		//DocVars.docData().SetProjectCode(Glb_BizzInfo.strBizzCode);
//	}
	// 删除所有数据
	else if (strCommand.CompareNoCase("DeleteAllData") == 0)
	{
		SendCommandToAutoCAD("DeleteAllEntity ");

		DocVars.docData().SetProjectId("");

		DocVars.docData().SetFileId("");

		DocVars.docData().m_mapBlockIds.RemoveAll();

		//是否删除图层，块定义？
	}
	// 显示命令窗口
	else if (strCommand.CompareNoCase("ShowCommandLine") == 0)
	{
		ShowCommandLineWindow(TRUE);
	}
	// 开关图层
	else if (strCommand.CompareNoCase("ShowLayer") == 0)
	{
		CString strLayer = parameters["layer"];
		CString strShow = parameters["show"];

		OutputDebugStringX("layer=%s, show=%s", strLayer, strShow);

		if (Glb_Filter.ptArray.logicalLength() <= 0)
		{
			acutPrintf("\n没有找到绘制的项目范围\n");
			return;
		}

		ShowLayer(strLayer, atoi(strShow), NULL);

		activeCurrentCadView();

//		SendCommandToAutoCAD("redraw ");
	}
	// 调历史图层
	else if (strCommand.CompareNoCase("loadHistoryLayer") == 0)
	{
		CString strWhere = parameters["where"];

		LockDocument _Lock;

		acutPrintf("\n选择数据加载范围：\n");

		// Add your code for command DistCommands._ReadSdeLayersEx here
		ads_point p1, p2;  //交互矩形点坐标获取
		if (acedGetPoint(NULL, "\n>>指定第一个角点：\n", p1) != RTNORM)
			return;

		if (acedGetCorner(p1, "\n>>指定第二个角点：\n", p2) != RTNORM) 
			return;

		if (p1[0] == p2[0] || p1[1] == p2[1]) //矩形框合法性检查
			return;

		double fTemp;
		if (p1[0] > p2[0]) {fTemp=p1[0]; p1[0]=p2[0]; p2[0]=fTemp;}
		if (p1[1] > p2[1]) {fTemp=p1[1]; p1[1]=p2[1]; p2[1]=fTemp;}
		p1[2] = 0; p2[2] = 0;

		//定义空间搜索条件
		DIST_STRUCT_SELECTWAY filter;

		filter.nEntityType = 4;
		filter.nSelectType = kSM_AI;
		filter.ptArray.setLogicalLength(0);

		filter.ptArray.append(AcGePoint3d(p1[0], p1[1], p1[2]));
		filter.ptArray.append(AcGePoint3d(p2[0], p2[1], p2[2]));

		// 读取图层数据
		for (int i=0; i<Glb_arrayHistoryLayer.GetCount();i++)
		{
			OutputDebugString("---- to Invoke ReadHistoryCadLayer ----");
			ReadHistoryCadLayer(Glb_arrayHistoryLayer.GetAt(i), strWhere, filter);
		}

		acutPrintf("\n图层加载完毕\n");
	}
	// 叠加材料清单中的方案图
	else if (strCommand.CompareNoCase("LoadAffixFile") == 0)
	{
		CString strFile = parameters["filename"];

		OutputDebugStringX("filename=%s", strFile);

		if (strFile != "")
		{
			LoadAffixFile(strFile);

			CString strFileId = parameters["fileid"];

			if (DocVars.docData().GetFileId() == "")
			{
				DocVars.docData().SetFileId(strFileId);

//				acedCommand(RTSTR, "_zoom",RTSTR,"E",0);
			}

			if (!strFile.IsEmpty())
			{
				CString strFileName = PathFindFileName(strFile);

				DocVars.docData().SetFileName(strFileName.Right(strFileName.GetLength()-strFileId.GetLength()));
			}
		}
	}
	else if (strCommand.CompareNoCase("OpenDwgFile")==0)
	{
		LockDocument _Lock;

		// 关闭数据库反应器
		if (Glb_pExchangeData != NULL)
			Glb_pExchangeData->SetDbReactorStatues(false);

		// 选择所有实体
		ads_name ents;
		acedSSGet("A",NULL,NULL,NULL,ents);

		long nLen = 0;
		acedSSLength(ents,&nLen);

		for (long i = 0; i < nLen; i++)
		{
			AcDbObjectId  objId;
			ads_name ent;
			acedSSName(ents, i, ent);
			acdbGetObjectId(objId, ent);

			// 删除实体
			AcDbEntity * pEnt = NULL;
			acdbOpenObject(pEnt, objId, AcDb::kForWrite); 
			pEnt->erase();
			pEnt->close();                
		}

		acedSSFree(ents);

		// 关闭数据库反应器
		if (Glb_pExchangeData != NULL)
			Glb_pExchangeData->SetDbReactorStatues(true);

		CString strFile = parameters["filename"];

		OutputDebugStringX("filename=%s", strFile);

		if (strFile != "")
		{
			LoadAffixFile(strFile);

			CString strFileId = parameters["fileid"];

			if (DocVars.docData().GetFileId() == "")
			{
				DocVars.docData().SetFileId(strFileId);

				//				acedCommand(RTSTR, "_zoom",RTSTR,"E",0);
			}

			if (!strFile.IsEmpty())
			{
				CString strFileName = PathFindFileName(strFile);

				DocVars.docData().SetFileName(strFileName.Right(strFileName.GetLength()-strFileId.GetLength()));
			}
		}

/*		
		CString strFile = parameters["filename"];

		OutputDebugStringX("filename=%s", strFile);

		if (!strFile.IsEmpty() && (PathFileExists(strFile)))
		{
			AcApDocument *pDoc = NULL;
			//打开文件，自动关闭只读提示对话框
			XAcDocOp::openAcDocument(strFile , pDoc , FALSE , TRUE);
			acDocManager->activateDocument(pDoc);		
		}
*/
	}
	else if (strCommand.CompareNoCase("CloseAllViews") == 0)
	{
		AcApDocumentIterator* pIter = acDocManager->newAcApDocumentIterator();

		for (; !pIter->done(); pIter->step()) 
		{
			AcApDocument* pDoc = pIter->document();
			AcDbDatabase *pDB  = pDoc->database();
			acdbSetDbmod(pDB,0);
			OutputDebugString("begin acDocManager->closeDocument");
			Acad::ErrorStatus es = acDocManager->closeDocument(pDoc);
			if (es == ErrorStatus::eOk)
			{
				OutputDebugString("acDocManager->closeDocument ok");
			}
			OutputDebugString("end acDocManager->closeDocument");
		}
		delete pIter;
	}
	else if (strCommand.CompareNoCase("SaveLayerState") == 0)
	{
		SendCommandToAutoCAD("SaveLayerState ");
	}
}

// 解析XML中描述的命令及参数
BOOL ParseParameters(CString strXML, CString& strCommand, MapString2String& parameters)
{
	OutputDebugString(strXML);
	// 截去结尾添加的多余信息
	if (strXML.Find("</root>") > 0)
	{
		strXML = strXML.Left(strXML.Find("</root>") + 7);
	}
	else
	{
		OutputDebugString("LoadXML failed!");

		return FALSE;
	}


	MSXML2::IXMLDOMDocumentPtr lpDocument;
	long hr = lpDocument.CreateInstance(__uuidof(DOMDocument));

	if ( FAILED(hr) ) 
		_com_raise_error(hr);

	BSTR bstrText = strXML.AllocSysString();

	// 装载XML字符串
	if (!lpDocument->loadXML(bstrText))
	{
		SysFreeString(bstrText);

		OutputDebugString("LoadXML failed!");

		return FALSE;
	}

	SysFreeString(bstrText);

	MSXML2::IXMLDOMElementPtr lpDocElement = NULL;
	lpDocument->get_documentElement(&lpDocElement);

	// 解析Command名称
	MSXML2::IXMLDOMNodePtr lpCommand = lpDocElement->selectSingleNode(_bstr_t("command"));

	if (NULL == lpCommand)
		return FALSE;

	MSXML2::IXMLDOMNodePtr lpNameNode = lpCommand->attributes->getNamedItem("name");
	strCommand = (char*)lpNameNode->text;

	// 解析参数列表
	MSXML2::IXMLDOMNodeListPtr lpNodeList = lpCommand->GetchildNodes();

	MSXML2::IXMLDOMNodePtr lpParam = NULL;

	for ( ; (lpParam = lpNodeList->nextNode()) != NULL ; )
	{
		MSXML2::IXMLDOMNodePtr lpNameNode = lpParam->attributes->getNamedItem("name");
		CString strName = (char*)lpNameNode->text;

		MSXML2::IXMLDOMNodePtr lpValueNode = lpParam->attributes->getNamedItem("value");
		CString strValue = (char*)lpValueNode->text;

		parameters.insert(MapString2String::value_type(strName, strValue));
	}

	return TRUE;
}

// 获取命令权限值 (0：隐藏 1：只读 2：可写 3：必填)
BOOL CheckCommandPower(CString strCommand)
{
	POWER power = Glb_commandPower[strCommand];

	// power.nPower 权限值 (0：隐藏 1：只读 2：可写 3：必填)
	if ((power.nPrior > 0) && ((power.nPower == 0) || (power.nPower == 1)))
	{
		acutPrintf("\n你没有使用%s命令的权限，请与管理员联系。",strCommand);
		return FALSE;
	}

	return TRUE;
}

DistCommandManagerReactor *pCmdReactor=NULL;

LRESULT CALLBACK DistMainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) ;

//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CDistCommandsApp : public AcRxArxApp {

public:
	CDistCommandsApp () : AcRxArxApp () {}

	virtual AcRx::AppRetCode On_kInitAppMsg (void *pkt) {
		AcRx::AppRetCode retCode =AcRxArxApp::On_kInitAppMsg (pkt) ;
		// TODO: Add your initialization code here
		
		pCmdReactor=new DistCommandManagerReactor;

		// 消息处理函数
		Glb_OldMainWndProc = (WNDPROC)SetWindowLong(acedGetAcadFrame()->m_hWnd, GWL_WNDPROC, (LONG)DistMainWndProc);
		OutputDebugString("CDistCommandsApp::SetWindowLong");

		CMDIFrameWnd *pWnd = acedGetAcadFrame();
		pWnd ->SetWindowText("CadContainer");
		pWnd ->UpdateWindow();

		return (retCode) ;
	}

	virtual AcRx::AppRetCode On_kUnloadAppMsg (void *pkt) 
	{
		AcRx::AppRetCode retCode =AcRxArxApp::On_kUnloadAppMsg (pkt) ;
		// TODO: Add your code here

		SetWindowLong(acedGetAcadFrame()->m_hWnd, GWL_WNDPROC, (LONG)Glb_OldMainWndProc);

//		if (Glb_hMDIClient != NULL && Glb_OldMDIClientProc != NULL)
//			SetWindowLong(Glb_hMDIClient, GWL_WNDPROC, (LONG)Glb_OldMDIClientProc);

		if(Glb_pConnection != NULL)
		{
			Glb_pConnection->Release();
			Glb_pConnection = NULL;
		}
		if(Glb_pExchangeData != NULL)
		{
			Glb_pExchangeData->Release();
			Glb_pExchangeData = NULL;
		}

		if(Glb_pLocalConnection != NULL)
		{
			Glb_pLocalConnection->Release();
			Glb_pLocalConnection = NULL;
		}
		if(Glb_pLocalExchangeData != NULL)
		{
			Glb_pLocalExchangeData->Release();
			Glb_pLocalExchangeData = NULL;
		}

		if(pCmdReactor!=NULL)
		{
			delete pCmdReactor;
			pCmdReactor=NULL;
		}

		return (retCode) ;
	}

	virtual void RegisterServerComponents () {
	}

	static void GetUniqueValues(CStringArray & arrayLayers)
	{
		std::set<CString> strSet;
		for (int i = 0; i < arrayLayers.GetSize(); i ++)
		{
			CString str = arrayLayers[i];
			if (strSet.find(str) != strSet.end())	//如果已存在
			{
				arrayLayers.RemoveAt(i);
				i--;
			}
			else strSet.insert(str);
		}
	}
	// ----- DistCommands._ReadSdeLayers command (do not rename)
	static void DistCommands_ReadSdeLayers(void)
	{
		if (!CheckCommandPower("ReadSdeLayers"))
			return;

		LockDocument _Lock;

		AcGePoint3d leftBottomPoint , rightTopPoint;
		if(!getViewExtent(leftBottomPoint , rightTopPoint))
			return;

		//定义空间搜索条件
//		DIST_STRUCT_SELECTWAY filter;
		Glb_Filter.nEntityType = 4;
		Glb_Filter.nSelectType = kSM_AI;
		Glb_Filter.ptArray.setLogicalLength(0);

		Glb_Filter.ptArray.append(leftBottomPoint);
		Glb_Filter.ptArray.append(rightTopPoint);

		Glb_layerArray.RemoveAll();

		CStringArray arrayLayers;

		acutPrintf("\n获取图层列表...\n");

		// 获取所有打勾的图层
		Sys_GetCheckedLayers(arrayLayers);
		//过滤掉重复的图层,保持顺序不变
		GetUniqueValues(arrayLayers);

		// 读取图层数据
		for (int i=0; i<arrayLayers.GetCount();i++)
		{
			acutPrintf("\n加载图层 %s...\n", arrayLayers.GetAt(i));
			ReadCadLayer(arrayLayers.GetAt(i), Glb_Filter);
		}

		acutPrintf("\n图层加载完毕。\n");

//		setView(Glb_Filter.ptArray.at(0), Glb_Filter.ptArray.at(1), 1.0);
	}

	// ----- DistCommands._ShowObjectAttr command (do not rename)
	static void DistCommands_ShowObjectAttr(void)
	{
		if (!CheckCommandPower("ShowObjectAttr"))
			return;

		LockDocument _Lock;

		CDlgReadInfo dlg;
		dlg.DoModal();
	}

	// ----- DistCommands._SaveSdeLayers command (do not rename)
	static void DistCommands_SaveSdeLayers(void)
	{
		if (!CheckCommandPower("SaveSdeLayers"))
			return;

		CString strProjectId = DocVars.docData().GetProjectId();
		if (atoi(strProjectId) <= 0)
		{
			acutPrintf("\n当前操作不可用\n");
			return;
		}

		IDistExchangeData* pExchangeData = Glb_pExchangeData;
		IDistConnection* pConnection = Glb_pConnection;

		LockDocument _Lock;

		CString strIniFile = "BizzInfo.ini";

		CString strItems[] = 
		{
			"建设地址", "用地性质", "容积率", "建筑密度", "建筑限高", "绿地率", "停车位", "许可证号", "经办人员", 
		};

		int nCount = sizeof(strItems)/sizeof(CString);

		for (int i=0; i<nCount; i++)
		{
			CString strName = strItems[i];

			WritePrivateProfileString("BizzInfo", strName, "", strIniFile);
		}

		acutPrintf("\n保存图层 %s...\n", strXMHX);
		SaveCadLayer(strXMHX);
		acutPrintf("\n保存图层 %s...\n", strXMFWX);
		SaveCadLayer(strXMFWX);
		
		acutPrintf("\n图层保存完毕\n");

		// Modify
		//// 将用地面积传给dap表单
		//SendDataToDapForm(pConnection);
	}

//	// ----- DistCommands._DimAnnotion command (do not rename)
//	static void DistCommands_DimAnnotion(void)
//	{
//		if (!CheckCommandPower("DimAnnotion"))
//			return;
//
//		LockDocument _Lock;
//		// 坐标标注
//
//		AcDbDatabase * pDb = acdbHostApplicationServices()->workingDatabase();
//		if (NULL == pDb)
//			return;
//
//		AcDbLayerTable * pLayerTable;
//		Acad::ErrorStatus es = pDb->getSymbolTable(pLayerTable, AcDb::kForRead);
//		if (es != Acad::eOk)
//			return;
//
//		CString strLineCadLayer = strAnnotionPolyLineFA;
//		CString strTextCadLayer = strAnnotionTextFA;
//
//		// 默认为方案图层，如果方案图层不可写，就默认草图层
//		BOOL bReadOnly = TRUE;
//		STB_CONTENT_TREE xxContentTree = Glb_mapTree[strLineCadLayer];
//		if (!xxContentTree.strSdeLyName.IsEmpty())
//		{
//			POWER power = Glb_mapPower[xxContentTree.nId];
//
//			// 沿用整个表单的权限
//			if ((Glb_nFormPower.nPower >= 0) && (Glb_nFormPower.nPrior >= 0))
//			{
//				if (Glb_nFormPower.nPrior > power.nPrior)
//				{
//					power.nPower = Glb_nFormPower.nPower;
//					power.nPrior = Glb_nFormPower.nPrior;
//				}
//			}
//
//			if ((power.nPower >= 0) && (power.nPrior >= 0))
//			{
//				bReadOnly = (power.nPower == 1);
//			}
//		}
//		if (bReadOnly)
//		{
//			strLineCadLayer = strAnnotionPolyLine_TEMP;
//			strTextCadLayer = strAnnotionText_TEMP;
//		}
//
///*		MapString2ContentTree::iterator iter = Glb_mapTree.begin();
//		while (iter != Glb_mapTree.end())
//		{
//			CString strCadLayerName = iter->first;
//			STB_CONTENT_TREE xxContentTree = iter->second;
//
//			if (xxContentTree.strSdeLyName.CompareNoCase(Glb_LayerInfo.strAnnotionLine) == 0)
//			{
//				strLineCadLayer = strCadLayerName;
//			}
//			else if (xxContentTree.strSdeLyName.CompareNoCase(Glb_LayerInfo.strAnnotionText) == 0)
//			{
//				strTextCadLayer = strCadLayerName;
//			}
//
//			iter++;
//		}
//*/
//		BOOL bIsLineLayerExist = pLayerTable->has(strLineCadLayer);
//		BOOL bIsTextLayerExist = pLayerTable->has(strTextCadLayer);
//	
//		pLayerTable->close();
//
//		// 如果图层不存在,需要从SDE库中读取
//		if (!bIsLineLayerExist)
//		{
////			long lRt = ReadSdeLayer(Glb_LayerInfo.strAnnotionLine, &Glb_Filter);
//			long lRt = ReadCadLayer(strLineCadLayer);
//			if (lRt != 1)
//			{
//#ifdef _DEBUG
//				acutPrintf("\n D无法读取%s图层数据，错误代码为%ld，请检查！", strLineCadLayer, lRt);
//#else
//				acutPrintf("\n 读取%s图层数据失败！\n", strLineCadLayer);
//#endif
//				return;
//			}
//		}
//		if (!bIsTextLayerExist)
//		{
////			long lRt = ReadSdeLayer(Glb_LayerInfo.strAnnotionText, &Glb_Filter);
//			long lRt = ReadCadLayer(strTextCadLayer);
//			if (lRt != 1)
//			{
//#ifdef _DEBUG
//				acutPrintf("\n D无法读取%s图层数据，错误代码为%ld，请检查！", strTextCadLayer, lRt);
//#else
//				acutPrintf("\n 读取%s图层数据失败！\n", strTextCadLayer);
//#endif
//				return;
//			}
//		}
//
//		struct resbuf rb;
//		acedGetVar("clayer", &rb);  //获得当前层名
//
//		CreateHXBZ(strLineCadLayer, strTextCadLayer);
//
//		acedSetVar("clayer", &rb);  //获得当前层名
//	}

	static CString EncodeFileName(CString strSrcFileName)
	{
		CString strDestFileName = strSrcFileName;

		LPTSTR lpDestFile = strDestFileName.GetBuffer(MAX_PATH);

		PathRemoveExtension(lpDestFile);

		int nLength = strlen(lpDestFile);

		int nKey[] = {14,8,-5,-8};
		for (int i=0; i<nLength; i++)
		{
			int j = i % 3;
			lpDestFile[i] = lpDestFile[i] - '0' + 'A' + nKey[j];

			while (lpDestFile[i] < '0')
			{
				lpDestFile[i] += 5;
			}
			while (lpDestFile[i] > '9' && lpDestFile[i] < 'A')
			{
				lpDestFile[i] -= 5;
			}
			while (lpDestFile[i] > 'Z' && lpDestFile[i] < 'a')
			{
				lpDestFile[i] -= 5;
			}
			while (lpDestFile[i] > 'a')
			{
				lpDestFile[i] -= 5;
			}
		}

		PathAddExtension(lpDestFile, ".ac$");

		strDestFileName.ReleaseBuffer();

		return strDestFileName;
	};

	// Modify
//	// ----- DistCommands._LoadBaseMap command (do not rename)
//	static void DistCommands_LoadBaseMap(void)
//	{
//		if (!CheckCommandPower("LoadBaseMap"))
//			return;
//
//		LockDocument _Lock;
//
//		if (Glb_Filter.ptArray.logicalLength() <= 0)
//		{
//			acutPrintf("\n没有找到绘制的项目范围\n");
//			return;
//		}
//
//		// 写SQL查询图幅索引
//		char strSQL[200] = {0};
//		sprintf(strSQL,"F:OBJECTID,%s,T:%s,##",Glb_LayerInfo.strIndexField, Glb_LayerInfo.strMapLayer);
//
//		AcGePoint3d pt(0,0,0);
//		AcGeScale3d scale(1,1,1);
//
//		OutputDebugString("Glb_pExchangeData->GetSearchFieldArray");
//
//		CStringArray array;
//		Glb_pExchangeData->GetSearchFieldArray(Glb_pConnection, strSQL, Glb_LayerInfo.strMapLayer, "shape", &Glb_Filter, array);
//
//		// 连接FTP服务器
//		XFtpConn ftpConn;
//		ftpConn.SetFtpSetting(Glb_FtpInfo.strServer, Glb_FtpInfo.strUserName, Glb_FtpInfo.strPassword, Glb_FtpInfo.nPort);
//		ftpConn.OpenConnect();
//
//		TCHAR lpTempPath[MAX_PATH] = {0};
//		GetTempPath(MAX_PATH, lpTempPath);
//		PathAddBackslash(lpTempPath);
//		CString strRemoteFile,strLocalFile;
//
//		// 图层
//		AcDbObjectId resultId;
//		CDistEntityTool tempTool;
//		tempTool.CreateLayer(resultId,"DIST_DXT",252);
//
////		// 关闭数据库反应器
////		if (Glb_pExchangeData != NULL)
////			Glb_pExchangeData->SetDbReactorStatues(false);
//
//		for (int i=0; i<array.GetCount(); i++)
//		{
//			CString strFile = array.GetAt(i);
//			PathRenameExtension(strFile.GetBuffer(MAX_PATH), ".dwg");
//			strFile.ReleaseBuffer();
////			strFile += ".dwg";
//
//			strRemoteFile = Glb_strDXTPath + strFile;
//			strLocalFile = lpTempPath + strFile;
//
//			if (!PathFileExists(strLocalFile))
//			{
//				CString strEncodeFile = EncodeFileName(strFile);
//				strLocalFile = lpTempPath + strEncodeFile;
//
//				if (!PathFileExists(strLocalFile))
//				{
//					if (ftpConn.FtpDownLoad(strRemoteFile, strLocalFile) != S_OK)
//					{
//						acutPrintf("\n下载地形图%s失败！\n",strFile);
//						continue;
//					}
//				}
//				strFile = strEncodeFile;
//			}
//
////			if(CheckTheDWGFileHasInsert(strLocalFile)) 
////			{
////				OutputDebugStringX("CheckTheDWGFileHasInsert(%s)", strLocalFile);
////				continue;
////			}
//
//			//查找地形图并将其插入当前图面
//			AcDbObjectId objectId = FindDWGAndInsertAsBlock(lpTempPath, strFile, pt, scale, 0, 252, "DIST_DXT");
//
////			if(objectId == NULL)
////			{
////				acutPrintf("\n%s图形文件插入失败！\n",strFile);
////				continue;
////			}
////			else
////			{
////				DocVars.docData().m_mapBlockIds.SetAt(strFile, objectId);
////			}
//		}
//
//		// 断开FTP服务器
//		ftpConn.CloseConnect();
//
////		// 关闭数据库反应器
////		if (Glb_pExchangeData != NULL)
////			Glb_pExchangeData->SetDbReactorStatues(true);
//
//		// 按照项目范围定位
//		if (Glb_Filter.ptArray.logicalLength() <= 0)
//			acedCommand(RTSTR, "_zoom",RTSTR,"E",0);
//		else
//		{
//			CString strLB, strRT;
//			strLB.Format("%f,%f", Glb_Filter.ptArray.at(0).x, Glb_Filter.ptArray.at(0).y);
//			strRT.Format("%f,%f", Glb_Filter.ptArray.at(1).x, Glb_Filter.ptArray.at(1).y);
//			acedCommand(RTSTR,"_ZOOM",RTSTR,"W",RTSTR, strLB, RTSTR, strRT, 0);
//		}
//	}

//	// ----- DistCommands._LoadBaseMapEx command (do not rename)
//	static void DistCommands_LoadBaseMapEx(void)
//	{
//		if (!CheckCommandPower("LoadBaseMapEx"))
//			return;
//
//		LockDocument _Lock;
//
//		ads_point p1, p2;  //交互矩形点坐标获取
//		if (acedGetPoint(NULL, "请指定一个范围，按Esc键取消\n", p1) == RTNORM)
//		{
//			if (acedGetCorner(p1, "请指定输出SDE数据的对角顶点\n", p2) != RTNORM) 
//				return;
//
//			if (p1[0] == p2[0] || p1[1] == p2[1]) //矩形框合法性检查
//			{
//				return;
//			}
//		}
//		else
//			return;
//
//		double fTemp;
//		if (p1[0] > p2[0]) {fTemp=p1[0]; p1[0]=p2[0]; p2[0]=fTemp;}
//		if (p1[1] > p2[1]) {fTemp=p1[1]; p1[1]=p2[1]; p2[1]=fTemp;}
//		p1[2] = 0; p2[2] = 0;
//
///*		AcGePoint3d ptLB, ptRT;
//
//		ptLB.x = (p1[0] < p2[0]) ? p1[0] : p2[0];
//		ptLB.y = (p1[1] < p2[1]) ? p1[1] : p2[1];
//		ptLB.z = 0;
//		ptRT.x = (p1[0] > p2[0]) ? p1[0] : p2[0];
//		ptRT.y = (p1[1] > p2[1]) ? p1[1] : p2[1];
//		ptRT.z = 0;
//*/
//		//定义空间搜索条件
//		DIST_STRUCT_SELECTWAY filter;
//		filter.nEntityType = 4;
//		filter.nSelectType = kSM_AI;
//		filter.ptArray.setLogicalLength(0);
//
//		filter.ptArray.append(AcGePoint3d(p1[0], p1[1], p1[2]));
//		filter.ptArray.append(AcGePoint3d(p2[0], p2[1], p2[2]));
//
//		//写SQL查询CAD和SDE图层对照表
//		char strSQL[200] = {0};
//		sprintf(strSQL,"F:OBJECTID,%s,T:%s,##",Glb_LayerInfo.strIndexField, Glb_LayerInfo.strMapLayer);
//
//		AcGePoint3d pt(0,0,0);
//		AcGeScale3d scale(1,1,1);
//
//		CString strFile;
//
//		CStringArray array;
//		Glb_pExchangeData->GetSearchFieldArray(Glb_pConnection, strSQL, Glb_LayerInfo.strMapLayer, "shape", &filter, array);
//
//		XFtpConn ftpConn;
//		ftpConn.SetFtpSetting(Glb_FtpInfo.strServer, Glb_FtpInfo.strUserName, Glb_FtpInfo.strPassword, Glb_FtpInfo.nPort);
//		ftpConn.OpenConnect();
//
//		TCHAR lpTempPath[MAX_PATH] = {0};
//		GetTempPath(MAX_PATH, lpTempPath);
//		PathAddBackslash(lpTempPath);
//		CString strRemoteFile,strLocalFile;
//
////		// 关闭数据库反应器
////		if (Glb_pExchangeData != NULL)
////			Glb_pExchangeData->SetDbReactorStatues(false);
//
//		for (int i=0; i<array.GetCount(); i++)
//		{
//			strFile = array.GetAt(i);
//			PathRenameExtension(strFile.GetBuffer(MAX_PATH), ".dwg");
//			strFile.ReleaseBuffer();
////			strFile += ".dwg";
//
//			strRemoteFile = Glb_strDXTPath + strFile;
//			strLocalFile = lpTempPath + strFile;
//
//			if (!PathFileExists(strLocalFile))
//			{
//				CString strEncodeFile = EncodeFileName(strFile);
//				strLocalFile = lpTempPath + strEncodeFile;
//
//				if (!PathFileExists(strLocalFile))
//				{
//					if (ftpConn.FtpDownLoad(strRemoteFile, strLocalFile) != S_OK)
//					{
//						acutPrintf("\n下载地形图%s失败！\n",strFile);
//						continue;
//					}
//				}
//				strFile = strEncodeFile;
//			}
//
////			if(CheckTheDWGFileHasInsert(strFile)) 
////			{
////				continue;
////			}
//
//			//查找地形图并将其插入当前图面
//			AcDbObjectId objectId = FindDWGAndInsertAsBlock(lpTempPath, strFile, pt, scale, 0, 252, "DIST_DXT");
//
////			if(objectId == NULL)
////			{
////				acutPrintf("\n%s图形文件插入失败！\n",strFile);
////				continue;
////			}
////			else
////			{
////				DocVars.docData().m_mapBlockIds.SetAt(strFile, objectId);
////			}
//		}
//
//		ftpConn.CloseConnect();
//
////		// 关闭数据库反应器
////		if (Glb_pExchangeData != NULL)
////			Glb_pExchangeData->SetDbReactorStatues(true);
//	}

	// ----- DistCommands._DimText command (do not rename)
	static void DistCommands_DimText(void)
	{
		if (!CheckCommandPower("DimText"))
			return;

		LockDocument _Lock;

		//CString strTextCadLayer = strAnnotionTextFA;
		CString strTextCadLayer = "";

		// 默认为方案图层，如果方案图层不可写，就默认草图层
		BOOL bReadOnly = TRUE;
		STB_CONTENT_TREE xxContentTree = Glb_mapTree[strTextCadLayer];
		if (!xxContentTree.strSdeLyName.IsEmpty())
		{
			POWER power = Glb_mapPower[xxContentTree.nId];

			// 沿用整个表单的权限
			if ((Glb_nFormPower.nPower >= 0) && (Glb_nFormPower.nPrior >= 0))
			{
				if (Glb_nFormPower.nPrior > power.nPrior)
				{
					power.nPower = Glb_nFormPower.nPower;
					power.nPrior = Glb_nFormPower.nPrior;
				}
			}

			if ((power.nPower >= 0) && (power.nPrior >= 0))
			{
				bReadOnly = (power.nPower == 1);
			}
		}
		//if (bReadOnly)
		//{
		//	strTextCadLayer = strAnnotionText_TEMP;
		//}

		// 设置当前图层
		struct resbuf *Value=acutBuildList(RTSTR, strTextCadLayer, 0);
		acedSetVar("clayer", Value);

//		CDistEntityTool tempTool;
//		tempTool.SetCurTextStyle("宋体");
		acedCommand(RTSTR, "text",0);
	}

	// ----- DistCommands._ReadSdeLayersEx command (do not rename)
	static void DistCommands_ReadSdeLayersEx(void) // 拉框调图
	{
		if (!CheckCommandPower("ReadSdeLayersEx"))
			return;

		LockDocument _Lock;

		acutPrintf("\n选择数据加载范围：\n");

		// Add your code for command DistCommands._ReadSdeLayersEx here
		ads_point p1, p2;  //交互矩形点坐标获取
		if (acedGetPoint(NULL, "\n>>指定第一个角点：\n", p1) != RTNORM)
			return;

		if (acedGetCorner(p1, "\n>>指定第二个角点：\n", p2) != RTNORM) 
			return;

		if (p1[0] == p2[0] || p1[1] == p2[1]) //矩形框合法性检查
			return;

		double fTemp;
		if (p1[0] > p2[0]) {fTemp=p1[0]; p1[0]=p2[0]; p2[0]=fTemp;}
		if (p1[1] > p2[1]) {fTemp=p1[1]; p1[1]=p2[1]; p2[1]=fTemp;}
		p1[2] = 0; p2[2] = 0;

		//定义空间搜索条件
		if (Glb_Filter.ptArray.logicalLength() > 0)
		{
			AcGePoint3d ptLB = Glb_Filter.ptArray.at(0);
			AcGePoint3d ptRT = Glb_Filter.ptArray.at(1);

			if (ptLB.x > p1[0]) ptLB.x = p1[0];
			if (ptLB.y > p1[1]) ptLB.y = p1[1];

			if (ptRT.x < p2[0]) ptRT.x = p2[0];
			if (ptRT.y < p2[1]) ptRT.y = p2[1];

			Glb_Filter.nEntityType = 4;
			Glb_Filter.nSelectType = kSM_AI;
			Glb_Filter.ptArray.setLogicalLength(0);

			Glb_Filter.ptArray.append(ptLB);
			Glb_Filter.ptArray.append(ptRT);
		}
		else
		{
			Glb_Filter.nEntityType = 4;
			Glb_Filter.nSelectType = kSM_AI;
			Glb_Filter.ptArray.setLogicalLength(0);

			Glb_Filter.ptArray.append(AcGePoint3d(p1[0], p1[1], p1[2]));
			Glb_Filter.ptArray.append(AcGePoint3d(p2[0], p2[1], p2[2]));
		}

		Glb_layerArray.RemoveAll();

		CStringArray arrayLayers;

		acutPrintf("\n获取图层列表...\n");

		// 获取所有打勾的图层
		Sys_GetCheckedLayers(arrayLayers);

		GetUniqueValues(arrayLayers);

		// 读取图层数据
		for (int i=0; i<arrayLayers.GetCount();i++)
		{
//			acutPrintf("\n加载图层 %s...\n", arrayLayers.GetAt(i));
			ReadCadLayer(arrayLayers.GetAt(i), Glb_Filter);
		}

		acutPrintf("\n图层加载完毕\n");
	}

	// Modify
	//// ----- DistCommands._UploadDwgFile command (do not rename)
	//static void DistCommands_UploadDwgFile(void)
	//{
	//	if (!CheckCommandPower("UploadDwgFile"))
	//		return;

	//	LockDocument _Lock;

	//	// 将文件保存至临时目录下并上传至FTP
	//	TCHAR lpTempPath[MAX_PATH] = {0};
	//	TCHAR lpTempFile[MAX_PATH] = {0};

	//	TCHAR lpDwgFile[MAX_PATH] = {0};
	//	sprintf(lpDwgFile, "%s.dwg", Glb_BizzInfo.strBizzCode);

	//	GetTempPath(MAX_PATH, lpTempPath);
	//	lstrcpy(lpTempFile, lpTempPath);
	//	GetTempFileName(lpTempPath, "$SH", 0, lpTempFile);

	//	AcDbDatabase *pDb;
	//	pDb = acdbHostApplicationServices()->workingDatabase();
	//	Acad::ErrorStatus er = pDb->saveAs(lpTempFile);
	//	if (er != Acad::eOk)
	//	{
	//		acutPrintf("\n 存储当前图出错！\n");
	//		return;
	//	}

	//	// 将文件复制一份，用于上传
	//	TCHAR lpLocalFile[MAX_PATH] = {0};
	//	GetTempFileName(lpTempPath, "$SH", 0, lpLocalFile);
	//	CopyFile(lpTempFile, lpLocalFile, FALSE);

	//	// 通过cadcontainer实现剩余功能（上传dwg文件，update数据库表）
	//	CString strCommand = "UploadDwgFile";
	//	CStringArray strParamArray;

	//	CString strParam;
	//	strParam.Format("file=%s", lpLocalFile);
	//	strParamArray.Add(strParam);

	//	SendCopyDataMessage(Glb_hDapForm, strCommand, strParamArray);
	//}

//	// ----- DistCommands._ExtractPolyLine command (do not rename)
//	static void DistCommands_ExtractPolyLine(void)
//	{
//		if (!CheckCommandPower("ExtractPolyLine"))
//			return;
//
//		LockDocument _Lock;
//
//		//交互选择实体
//		ads_name ssName;
//		struct resbuf *filter=NULL;
//
//		filter=acutNewRb(AcDb::kDxfLayerName);
//		filter->restype=AcDb::kDxfLayerName;
//		filter->resval.rstring = (char*) malloc((strXZHXDA.GetLength()+1)*sizeof(char));
//		strcpy(filter->resval.rstring,strXZHXDA.GetBuffer(0));
//		filter->rbnext=NULL;
//
////		if (acedSSGet(":S", NULL, NULL, filter, ssName) != RTNORM)
//		if (acedSSGet(":S", NULL, NULL, NULL, ssName) != RTNORM)
//		{
//			acutPrintf("\n选择实体有误!");
//			acedSSFree(ssName); acutRelRb(filter);
//			return;
//		}
//		if(filter != NULL) acutRelRb(filter);
//
//		AcDbObjectId tempObjId;
//		ads_name ssTemp;
//		long nNum = 0;
//		int nPos = 0;
//		acedSSLength(ssName,&nNum);
//
//		AcDbObjectIdArray aryIds;
//
//		for(int i=0; i<nNum; i++)
//		{
//			acedSSName(ssName, i, ssTemp);
//			if(Acad::eOk != acdbGetObjectId(tempObjId, ssTemp)) 
//			{
//				continue;
//			}
//			aryIds.append(tempObjId);
//		}
//
//		acedSSFree(ssName);
//
//		AcDbDatabase * pDB = acdbHostApplicationServices()->workingDatabase();
//
//		//打开表
//		AcDbBlockTableRecord* pBlkRec = NULL;
//		Acad::ErrorStatus es = acdbOpenObject(pBlkRec, pDB->currentSpaceId(), AcDb::kForWrite);
//
//		for(int i=0;i<aryIds.length();i++)
//		{
//			AcDbObjectId Id=aryIds.at(i);
//			AcDbEntity* pEnt=NULL;
//			acdbOpenObject(pEnt,Id,AcDb::kForRead);
//			if(pEnt==NULL)
//				continue;
//
//			CString strSrcLayer = pEnt->layer();
//			CString strDestLayer;
//
//			if (strSrcLayer.CompareNoCase(strXZZYDDA) == 0) // "选址总用地档案"
//				strDestLayer = strYDZYDFA; // "用地总用地方案"
//			else if (strSrcLayer.CompareNoCase(strXZJYDDA) == 0) // "选址净用地档案"
//				strDestLayer = strYDJYDFA; // "用地净用地方案"
//			else if (strSrcLayer.CompareNoCase(strXZDZYDDA) == 0) // "选址代征用地档案"
//				strDestLayer = strYDDZYDFA; // "用地代征用地方案"
//			else
//			{
//				pEnt->close();
//				continue;
//			}
//
//			AcDbEntity* pEnt0=(AcDbEntity*)pEnt->clone();
//			pEnt0->setLayer(strDestLayer.GetBuffer());
//			strDestLayer.ReleaseBuffer();
//
//			if (es != Acad::eOk)
//			{
//				acutPrintf("\n 打开模型空间失败！");
//				pEnt->close();
//				break;
//			}
//
//			AcDbObjectId resultId;
//			//添加实体
//			es = pBlkRec->appendAcDbEntity(resultId,pEnt0);
//			if (es != Acad::eOk) 
//			{
//				acutPrintf("\n 添加实体到模型空间失败！");
//				pEnt->close();
//				break;
//			}
//
//			pEnt->close();
//			pEnt0->close();
//
//			// 清空ROWID扩展信息
//			CDistXData tempXdata(resultId, "ROWID_OBJS");
//			if(tempXdata.GetRecordCount() > 0) 
//			{
//				tempXdata.DeleteAllRecord();
//				tempXdata.Close();
//			}
//
//			// 清空选址扩展信息
//			TCHAR strXDataName[128] = {0};
//			sprintf(strXDataName,"DIST_%s",Glb_mapTree[strXZHXDA].strSdeLyName);
//			CDistXData tempXdata_XZ(resultId, strXDataName);
//
//			CStringArray strFldNameArray, strFldValueArray;
//
////			tempXdata_XZ.GetFldNameArray(strFldNameArray);
////			tempXdata_XZ.GetFldValueArray(strFldValueArray);
//
//			tempXdata_XZ.DeleteAllRecord();
//			tempXdata_XZ.Close();
///*
//			// 添加用地扩展信息，并将选址复制到用地
//			sprintf(strXDataName,"DIST_%s",Glb_mapTree[strYDHXFA].strSdeLyName);
//			CDistXData tempXdata_YD(resultId, strXDataName);
//
//			for (int i=0; i<strFldNameArray.GetCount(); i++) 
//			{
//				tempXdata_YD.Add(strFldNameArray.GetAt(i), strFldValueArray.GetAt(i));
//			}
//
//			tempXdata_YD.Modify("OBJECTID", "-1");
//			tempXdata_YD.Modify("XMBH", Glb_BizzInfo.strBizzCode); // 项目编号
//			tempXdata_YD.Modify("XMBH", Glb_BizzInfo.strBizzCode); // 建设单位
//			tempXdata_YD.Modify("XMBH", Glb_BizzInfo.strBizzCode); // 项目名称
//
//			tempXdata_YD.Update();
//			tempXdata_YD.Close();
//*/
//		} 
//
//		pBlkRec->close();
//	}

//	// ----- DistCommands._AddHXAttr command (do not rename)
//	static void DistCommands_AddHXAttr(void)
//	{
//		if (!CheckCommandPower("AddHXAttr"))
//			return;
//
//		LockDocument _Lock;
//
//		//选择红线
//		ads_name ssName;
//		CString strCadLayerName = strXZJYDFA;//"选址红线";
//
//		struct resbuf *filter=NULL;
//		filter=acutNewRb(AcDb::kDxfLayerName);
//		filter->restype=AcDb::kDxfLayerName;
//		filter->resval.rstring = (char*) malloc(strlen(strCadLayerName)*sizeof(char)+1);
//		strcpy(filter->resval.rstring, strCadLayerName);
//		filter->rbnext=NULL;
//
//		if (acedSSGet(":S", NULL, NULL, filter, ssName) != RTNORM)
//		{
//			acedSSFree(ssName); acutRelRb(filter);return;
//		}
//		acutRelRb(filter);
//
//		AcDbObjectId tempObjId;
//		ads_name ssTemp;
//		acedSSName(ssName, 0, ssTemp);
//		acdbGetObjectId(tempObjId, ssTemp);
//		acedSSFree(ssName);
//
//		AcDbEntity* pEnt = NULL;
//		if(Acad::eOk != acdbOpenObject(pEnt,tempObjId,AcDb::kForWrite)) return;
//		pEnt->close();
//
//		CString strSdeLayerName = Glb_mapTree[strCadLayerName].strSdeLyName;
//
//		char strXDataName[1024]={0};
//		sprintf(strXDataName,"DIST_%s",strSdeLayerName);
//		CDistXData tempXdata(tempObjId,strXDataName);
//		if(tempXdata.GetRecordCount() != 0) 
//		{
//			AfxMessageBox("该实体红线属性已经存在！");
//			return;
//		}
//
//		tempXdata.Add("XMBH", Glb_BizzInfo.strBizzCode); // 项目编号
//		tempXdata.Add("JSDW",Glb_BizzInfo.strBzOwner); // 建设单位
//		tempXdata.Add("XMMC", Glb_BizzInfo.strBizzName); // 项目名称
//		tempXdata.Add("DZDAH",Glb_BizzInfo.strBizzSame); // 电子档案号
//		tempXdata.Add("SPSJ",Glb_BizzInfo.strCurTime); // 审批时间
////		tempXdata.Add("ZKZH",strUnit); // 选址意见书编号
////		tempXdata.Add("JZMD",strAddr); // 建筑密度
////		tempXdata.Add("JZXG",strAddr); // 建筑限高
//
//		tempXdata.Update();
//		tempXdata.Close();
//
//		AfxMessageBox("添加属性成功!");
//	}

//	// ----- DistCommands._LoadJpg command (do not rename)
//	static void DistCommands_LoadJpg(void)
//	{
//		if (!CheckCommandPower("LoadJpg"))
//			return;
//
//		LockDocument _Lock;
//
//		ads_point p1, p2;  //交互矩形点坐标获取
//		if (acedGetPoint(NULL, "请指定一个范围，按Esc键取消\n", p1) == RTNORM)
//		{
//			if (acedGetCorner(p1, "请指定输出SDE数据的对角顶点\n", p2) != RTNORM) 
//				return;
//
//			if (p1[0] == p2[0] || p1[1] == p2[1]) //矩形框合法性检查
//			{
//				return;
//			}
//		}
//		else
//			return;
//
//		AcGePoint3d ptLB, ptRT;
//
//		ptLB.x = (p1[0] < p2[0]) ? p1[0] : p2[0];
//		ptLB.y = (p1[1] < p2[1]) ? p1[1] : p2[1];
//		ptLB.z = 0;
//		ptRT.x = (p1[0] > p2[0]) ? p1[0] : p2[0];
//		ptRT.y = (p1[1] > p2[1]) ? p1[1] : p2[1];
//		ptRT.z = 0;
//
//		//定义空间搜索条件
//		DIST_STRUCT_SELECTWAY filter;
//		filter.nEntityType = 4;
//		filter.nSelectType = kSM_AI;
//		filter.ptArray.setLogicalLength(0);
//
//		filter.ptArray.append(ptLB);
//		filter.ptArray.append(ptRT);
//
//		//写SQL查询CAD和SDE图层对照表
//		char strSQL[200] = {0};
//		sprintf(strSQL,"F:OBJECTID,图号,T:%s,##",Glb_LayerInfo.strMapLayer);
//
//		AcGePoint3d pt(0,0,0);
//		AcGeScale3d scale(1,1,1);
//
//		CString strFile;
//
//		CStringArray array;
//		Glb_pExchangeData->GetSearchFieldArray(Glb_pConnection, strSQL, Glb_LayerInfo.strMapLayer, "shape", &filter, array);
//
//		TCHAR lpTempPath[MAX_PATH] = {0};
//		TCHAR lpTempFile[MAX_PATH] = {0};
//		lstrcpy(lpTempPath, "c:\\temp\\");
//		lstrcpy(lpTempFile, "c:\\temp\\");
//
//		if (!PathFileExists(lpTempPath))
//			CreateDirectory(lpTempPath, NULL);
////		GetTempPath(MAX_PATH, lpTempPath);
//
////		GetLongPathName(lpTempPath, lpTempFile, MAX_PATH);
////		lstrcpyn(lpTempPath, lpTempFile, MAX_PATH);
//
//		XFtpConn ftpConn;
//		ftpConn.SetFtpSetting(Glb_FtpInfo.strServer, Glb_FtpInfo.strUserName, Glb_FtpInfo.strPassword, Glb_FtpInfo.nPort);
//		ftpConn.OpenConnect();
//
//		for (int i=0; i<array.GetCount(); i++)
//		{
//			strFile = array.GetAt(i);
//			strFile += ".jpg";
//
//			PathAppend(lpTempFile, strFile.GetBuffer());
//			strFile.ReleaseBuffer();
//
//			CString strRemoteFile = "影像图\\"+strFile;
//			if (!PathFileExists(lpTempFile))
//			{
//				// 下载文件
//				if (ftpConn.FtpDownLoad(strRemoteFile, lpTempFile) != S_OK)
//				{
//					acutPrintf("\n下载影像图%s失败", strFile);
//					continue;
//				}
//
///*				CFTPTransferDlg dlg;
//				dlg.m_sServer = Glb_FtpInfo.strServer;
//				dlg.m_sUserName = Glb_FtpInfo.strUserName;
//				dlg.m_sPassword = Glb_FtpInfo.strPassword;
//				dlg.m_nPort = Glb_FtpInfo.nPort;
//				dlg.m_sRemoteFile = strRemoteFile;
//				dlg.m_sLocalFile = lpTempFile;
//				dlg.m_sFileName = strFile;
//				dlg.m_bDownload = TRUE;
//				dlg.m_bBinary = TRUE;
//
//				if (dlg.DoModal() != IDOK)
//				{
//					return;
//				}
//				if (!PathFileExists(lpTempFile))
//				{
//					acutPrintf("\n下载影像图%s失败", strFile);
//					continue;
//				}
//*/			}
//
//			int nStartPos = strFile.Find('-');
//			int nEndPos = strFile.Find('.');
//			pt.y = atof(strFile.Left(nStartPos))*10;
//			pt.x = atof(strFile.Mid(nStartPos+1, nEndPos-nStartPos-1))*10;
//
//			//查找删格图并插入当前图面
//			AcGePoint2dArray pts;
//			pts.append(AcGePoint2d(pt.x, pt.y));
//			pts.append(AcGePoint2d(pt.x+1000, pt.y+1000));
//
//			AcDbObjectId objectId = FindTifAndInsertToCurrent(lpTempPath, strFile, pt, scale, 0, -1, pts);
//			if(objectId == NULL)
//			{
//				acutPrintf("\n%s图形文件插入失败！\n",strFile);
//				continue;
//			}
//		}
//
//		ftpConn.CloseConnect();
//	}

//	// ----- DistCommands._Archive command (do not rename)
//	static void DistCommands_Archive(void)
//	{
//		if (!CheckCommandPower("Archive"))
//			return;
//
//		LockDocument _Lock;
//
//		if (MessageBox(acedGetAcadFrame()->m_hWnd, "是否要进行方案转档案操作？", "提示", MB_YESNO) == IDNO)
//		{
//			return;
//		}
//
////		DistCommands_SaveSdeLayers();
//
//		CStringArray arrayFA;
//		CStringArray arrayDA;
//
//		arrayFA.Add(strXZZYDFA);
//		arrayFA.Add(strXZJYDFA);
//		arrayFA.Add(strXZDZYDFA);
//		arrayFA.Add(strYDZYDFA);
//		arrayFA.Add(strYDJYDFA);
//		arrayFA.Add(strYDDZYDFA);
//
//		arrayFA.Add(strXZHXFA);
//		arrayFA.Add(strYDHXFA);
//
//		arrayFA.Add(strAnnotionPolyLineFA);
//		arrayFA.Add(strAnnotionTextFA);
//		arrayFA.Add(strAnnotionPolygonFA);
//		arrayFA.Add(strOtherAnnotionPolyLineFA);
//		
//		arrayDA.Add(strXZZYDDA);
//		arrayDA.Add(strXZJYDDA);
//		arrayDA.Add(strXZDZYDDA);
//		arrayDA.Add(strYDZYDDA);
//		arrayDA.Add(strYDJYDDA);
//		arrayDA.Add(strYDDZYDDA);
//
//		arrayFA.Add(strXZHXDA);
//		arrayFA.Add(strYDHXDA);
//
//		arrayDA.Add(strAnnotionPolyLineDA);
//		arrayDA.Add(strAnnotionTextDA);
//		arrayDA.Add(strAnnotionPolygonDA);
//		arrayDA.Add(strOtherAnnotionPolyLineDA);
//
//		// 读取图层数据
//		for (int i=0; i<arrayFA.GetCount();i++)
//		{
//			acutPrintf("\n保存图层 %s...\n", arrayFA.GetAt(i));
//			SaveCadLayer(arrayFA.GetAt(i));
//		}
//
//		CString strConnectString;
//		strConnectString.Format("%s,%s,%s,%s,%s,", Glb_SdeInfo.strServer, Glb_SdeInfo.strService, Glb_SdeInfo.strDatabase,Glb_SdeInfo.strUserName,Glb_SdeInfo.strPassword);
//
//		int nRes = 0;
////		nRes = gBeginTrans("192.168.1.66,5151,xmghsde,xmghsde,xmghsde,");
//		nRes = gBeginTrans(strConnectString.GetBuffer());
//		strConnectString.ReleaseBuffer();
//
//		CString strWhere;
//		strWhere.Format("XMBH='%s'", Glb_BizzInfo.strBizzCode);
//
////		nRes = gCopyRecordFromOneToOther("XMGHSDE.SP_XZLX_DA_POLYGON","XMGHSDE.SP_YDLX_DA_POLYGON","PROJECT_ID='10000309'");
//		CString strSrcLayer, strDestLayer;
//		for (int i=0; i<arrayFA.GetCount(); i++)
//		{
//			strSrcLayer = Glb_mapTree[arrayFA.GetAt(i)].strSdeLyName;
//			strDestLayer = Glb_mapTree[arrayDA.GetAt(i)].strSdeLyName;
//
//			if (strSrcLayer.IsEmpty() || strDestLayer.IsEmpty())
//				continue;
//
//			acutPrintf("\n归档图层 %s 至 %s...\n", strSrcLayer, strDestLayer);
//
//			LPSTR lpSrcLayer = strSrcLayer.GetBuffer();
//			LPSTR lpDestLayer = strDestLayer.GetBuffer();
//			LPSTR lpWhere = strWhere.GetBuffer();
//			nRes = gCopyRecordFromOneToOther(lpSrcLayer, lpDestLayer, lpWhere);
//
//			if (nRes == 1 || nRes == 2)
//			{
//				nRes = gDeleteRecord(lpSrcLayer, lpWhere);
//			}
//
//			strSrcLayer.ReleaseBuffer();
//			strDestLayer.ReleaseBuffer();
//			strWhere.ReleaseBuffer();
//
//			if(nRes != 1)
//			{
//				gRollbackTrans();
//				MessageBox(acedGetAcadFrame()->m_hWnd, "图层归档操作失败!", "提示", MB_OK);
//				return;
//			}
//		}
//
//		gCommitTrans();
//
//		MessageBox(acedGetAcadFrame()->m_hWnd, "图层归档操作成功!", "提示", MB_OK);
//
////		DistCommands_ReadSdeLayers();  //重新调图
//
//		// 读取图层数据
//		for (i=0; i<arrayFA.GetCount();i++)
//		{
////			acutPrintf("\n加载图层 %s...\n", arrayFA.GetAt(i));
//			ReadCadLayer(arrayFA.GetAt(i));
//		}
//		for (i=0; i<arrayDA.GetCount();i++)
//		{
////			acutPrintf("\n加载图层 %s...\n", arrayDA.GetAt(i));
//			ReadCadLayer(arrayDA.GetAt(i));
//		}
//	}

	// ----- DistCommands._DeleteAllEntity command (do not rename)
	static void DistCommands_DeleteAllEntity(void)
	{
		if (!CheckCommandPower("DeleteAllEntity"))
			return;

		LockDocument _Lock;

		// 关闭数据库反应器
		if (Glb_pExchangeData != NULL)
			Glb_pExchangeData->SetDbReactorStatues(false);

		// 选择所有实体
		ads_name ents;
		acedSSGet("A",NULL,NULL,NULL,ents);

		long nLen = 0;
		acedSSLength(ents,&nLen);

		for (long i = 0; i < nLen; i++)
		{
			AcDbObjectId  objId;
			ads_name ent;
			acedSSName(ents, i, ent);
			acdbGetObjectId(objId, ent);

			// 删除实体
			AcDbEntity * pEnt = NULL;
			acdbOpenObject(pEnt, objId, AcDb::kForWrite); 
			pEnt->erase();
			pEnt->close();                
		}

		acedSSFree(ents);

		// 关闭数据库反应器
		if (Glb_pExchangeData != NULL)
			Glb_pExchangeData->SetDbReactorStatues(true);
	}

	// ----- DistCommands._DeleteInsertDwg command (do not rename)
	static void DistCommands_DeleteInsertDwg(void)
	{
		if (!CheckCommandPower("DeleteInsertDwg"))
			return;

		LockDocument _Lock;

		// 删除除了sde图层和0层,"DIST_DXT","DIST_TK"之外的所有图层
		AcDbLayerTable* pLayerTable = NULL;

		AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();
		Acad::ErrorStatus es = pDb->getLayerTable(pLayerTable, AcDb::kForRead);

		if (es != Acad::eOk)
			return;

		AcDbLayerTableIterator *pLayerIterator = NULL;

		if (pLayerTable->newIterator(pLayerIterator) != Acad::eOk)
		{
			pLayerTable->close();
			return;
		}

		CStringArray array;
		for (; !pLayerIterator->done(); pLayerIterator->step())
		{
			AcDbLayerTableRecord *pLayerTableRecord = NULL;

			if (Acad::eOk == pLayerIterator->getRecord(pLayerTableRecord, AcDb::kForWrite))
			{
				char* pStrName;
				es = pLayerTableRecord->getName(pStrName);

				if (es != Acad::eOk)
				{
					pLayerTableRecord->close();
					continue;
				}

				if ((strcmpi(pStrName, "0") == 0) || 
					(strcmpi(pStrName, "DIST_DXT") == 0) || // 地形图
					(strcmpi(pStrName, "DIST_TK") == 0)) // 图框
				{
					pLayerTableRecord->close();
					continue;
				}

				STB_CONTENT_TREE xxContentTree = Glb_mapTree[pStrName];

				if (!xxContentTree.strSdeLyName.IsEmpty())
				{
					pLayerTableRecord->close();
					continue;
				}

				array.Add(pStrName);
			}

			pLayerTableRecord->close();
		}

		delete pLayerIterator;

		pLayerTable->close();

		for (int i=0; i< array.GetCount(); i++)
		{
			CString strLayerName = array.GetAt(i);
			CDistEntityTool tempTool;
			tempTool.DeleteAllEntityInLayer(strLayerName.GetBuffer(), pDb);
			strLayerName.ReleaseBuffer();
		}
	}

	// ----- DistCommands._LoadLayerTree command (do not rename)
	static void DistCommands_LoadLayerTree(void)
	{
		if (!CheckCommandPower("LoadLayerTree"))
			return;

		LockDocument _Lock;

		// 加载目录树
		CString strCommand = "LoadLayerTree";
		CStringArray strParamArray;
		strParamArray.RemoveAll();
		SendCopyDataMessage(acedGetAcadFrame()->m_hWnd, strCommand, strParamArray);
	}

	// ----- DistCommands._ShowDockTreePane command (do not rename)
	static void DistCommands_ShowDockTreePane(void)
	{
		if (!CheckCommandPower("ShowDockTreePane"))
			return;

		CString strProjectId = DocVars.docData().GetProjectId(); // 六线调整为0,电子展板为-1

		Sys_ShowDockTreePane(atoi(strProjectId));
	}

	// Modify
//	// ----- DistCommands._DISTDYTK command (do not rename)
//	static void DistCommands_DISTDYTK(void)
//	{
//		IDistExchangeData* pExchangeData = Glb_pExchangeData;
//
//		if (!CheckCommandPower("DISTDYTK"))
//			return;
//
//		// 关闭数据库反应器
//		if (pExchangeData != NULL)
//			pExchangeData->SetDbReactorStatues(false);
//
////		SendCommandToAutoCAD("DISTDYTK_ ");
//		DeleteExternEntities();
//		Sys_PlotEx();
//
//		// 关闭数据库反应器
//		if (pExchangeData != NULL)
//			pExchangeData->SetDbReactorStatues(true);
//	}

	//// ----- DistCommands._DistBoundary command (do not rename)
	//static void DistCommands_DistBoundary(void) // 提取代征用地
	//{
	//	ads_name first,last;
	//	AcDbObjectId firstId,lastId;
	//	acdbEntLast(first);
	//	acedCommand(RTSTR, "boundary", 0);
	//	acdbEntLast(last);
	//	acdbGetObjectId(firstId,first);
	//	acdbGetObjectId(lastId,last);
	//	if(firstId!=lastId)
	//	{

	//	}
	//}

	//static void ReadSdeData()
	//{
	//	IDistExchangeData* pExchangeData = Glb_pExchangeData;
	//	IDistConnection* pConnection = Glb_pConnection;

	//	// 读sde数据
	//	/*		总用地面积=
	//	净用地面积=
	//	代征用地面积=
	//	容积率=
	//	建筑密度=
	//	建筑限高=
	//	绿地率=
	//	停车位=
	//	*/
	//	CString strLayer;
	//	if (Glb_BizzInfo.strBzId.Find("选址") >= 0)
	//		strLayer = strXZJYDDA;
	//	else
	//		strLayer = strYDJYDDA;
	//	STB_CONTENT_TREE xxContentTree = Glb_mapTree[strLayer];
	//	if (xxContentTree.strSdeLyName.IsEmpty())
	//		return;

	//	IDistCommand *pCommand = CreateSDECommandObjcet();
	//	if(NULL == pCommand) return;
	//	pCommand->SetConnectObj(pConnection);

	//	///////////////////////////////////////////////////////////////
	//	CString strJYDMJ, strRJL, strJZMD, strJZXG, strLDL, strTCW;
	//	TCHAR lpSQL[256] = {0}; // 净用地面积，容积率，建筑密度，建筑限高，绿地率, 停车位
	//	sprintf(lpSQL, "F:%s,T:%s,W:XMBH='%s',##", "JYDMJ,RJL,JZMD,JZXG,LDL,TCW", xxContentTree.strSdeLyName, Glb_BizzInfo.strBizzCode);

	//	IDistRecordSet *pRcdSet = NULL;
	//	if (pCommand->SelectData(lpSQL, &pRcdSet) != 1)
	//	{
	//		if(pRcdSet != NULL) 
	//		{
	//			pRcdSet->Release();
	//			pRcdSet = NULL;
	//		}
	//		pCommand->Release();
	//		return;
	//	}
	//	else if (pRcdSet->GetRecordCount() <= 0)
	//	{
	//		if(pRcdSet != NULL) 
	//		{
	//			pRcdSet->Release();
	//			pRcdSet = NULL;
	//		}

	//		if (Glb_BizzInfo.strBzId.Find("选址") >= 0)
	//			strLayer = strXZJYDFA;
	//		else
	//			strLayer = strYDJYDFA;

	//		xxContentTree.strSdeLyName.Empty();
	//		xxContentTree = Glb_mapTree[strLayer];
	//		if (xxContentTree.strSdeLyName.IsEmpty())
	//		{
	//			pCommand->Release();
	//			return;
	//		}

	//		sprintf(lpSQL, "F:%s,T:%s,W:XMBH='%s',##", "JYDMJ,RJL,JZMD,JZXG,LDL,TCW", xxContentTree.strSdeLyName, Glb_BizzInfo.strBizzCode);

	//		if (pCommand->SelectData(lpSQL, &pRcdSet) == 1)
	//		{
	//			double dVal = 0.0;
	//			while (pRcdSet->BatchRead() == 1)
	//			{
	//				dVal += *(double*)(*pRcdSet)[0];
	//				strJYDMJ.Format("%0.3f", dVal);
	//				strRJL = (char*)(*pRcdSet)[1];
	//				strJZMD = (char*)(*pRcdSet)[2];
	//				strJZXG = (char*)(*pRcdSet)[3];
	//				strLDL = (char*)(*pRcdSet)[4];
	//				strTCW = (char*)(*pRcdSet)[5];
	//			}
	//		}
	//	}
	//	else if (pRcdSet->BatchRead() == 1)
	//	{
	//		double dVal = 0.0;
	//		while (pRcdSet->BatchRead() == 1)
	//		{
	//			dVal += *(double*)(*pRcdSet)[0];
	//			strJYDMJ.Format("%0.3f", dVal);
	//			strRJL = (char*)(*pRcdSet)[1];
	//			strJZMD = (char*)(*pRcdSet)[2];
	//			strJZXG = (char*)(*pRcdSet)[3];
	//			strLDL = (char*)(*pRcdSet)[4];
	//			strTCW = (char*)(*pRcdSet)[5];
	//		}
	//	}

	//	pRcdSet->Release();

	//	///////////////////////////////////////////////////////////////
	//	if (Glb_BizzInfo.strBzId.Find("选址") >= 0)
	//		strLayer = strXZZYDDA;
	//	else
	//		strLayer = strYDZYDDA;

	//	xxContentTree.strSdeLyName.Empty();
	//	xxContentTree = Glb_mapTree[strLayer];
	//	if (xxContentTree.strSdeLyName.IsEmpty())
	//	{
	//		pCommand->Release();
	//		return;
	//	}

	//	CString strZYDMJ; // 总用地面积
	//	memset(lpSQL, 0, sizeof(lpSQL)); 
	//	sprintf(lpSQL, "F:%s,T:%s,W:XMBH='%s',##", "YDZMJ", xxContentTree.strSdeLyName, Glb_BizzInfo.strBizzCode);

	//	pRcdSet = NULL;
	//	if (pCommand->SelectData(lpSQL, &pRcdSet) != 1)
	//	{
	//		if(pRcdSet != NULL) 
	//		{
	//			pRcdSet->Release();
	//			pRcdSet = NULL;
	//		}
	//		pCommand->Release();
	//		return;
	//	}
	//	else if (pRcdSet->GetRecordCount() <= 0)
	//	{
	//		if(pRcdSet != NULL) 
	//		{
	//			pRcdSet->Release();
	//			pRcdSet = NULL;
	//		}

	//		if (Glb_BizzInfo.strBzId.Find("选址") >= 0)
	//			strLayer = strXZZYDFA;
	//		else
	//			strLayer = strYDZYDFA;

	//		xxContentTree.strSdeLyName.Empty();
	//		xxContentTree = Glb_mapTree[strLayer];
	//		if (xxContentTree.strSdeLyName.IsEmpty())
	//		{
	//			pCommand->Release();
	//			return;
	//		}

	//		sprintf(lpSQL, "F:%s,T:%s,W:XMBH='%s',##", "YDZMJ", xxContentTree.strSdeLyName, Glb_BizzInfo.strBizzCode);

	//		if (pCommand->SelectData(lpSQL, &pRcdSet) == 1)
	//		{
	//			double dVal = 0.0;
	//			while (pRcdSet->BatchRead() == 1)
	//			{
	//				dVal += *(double*)(*pRcdSet)[0];
	//				strZYDMJ.Format("%0.3f", dVal);
	//			}
	//		}
	//	}
	//	else if (pRcdSet->BatchRead() == 1)
	//	{
	//		double dVal = 0.0;
	//		while (pRcdSet->BatchRead() == 1)
	//		{
	//			dVal += *(double*)(*pRcdSet)[0];
	//			strZYDMJ.Format("%0.3f", dVal);
	//		}
	//	}

	//	pRcdSet->Release();

	//	///////////////////////////////////////////////////////////////
	//	if (Glb_BizzInfo.strBzId.Find("选址") >= 0)
	//		strLayer = strXZDZYDDA;
	//	else
	//		strLayer = strYDDZYDDA;

	//	xxContentTree.strSdeLyName.Empty();
	//	xxContentTree = Glb_mapTree[strLayer];
	//	if (xxContentTree.strSdeLyName.IsEmpty())
	//	{
	//		pCommand->Release();
	//		return;
	//	}

	//	CString strDZYDMJ; // 代征用地面积
	//	memset(lpSQL, 0, sizeof(lpSQL)); 
	//	sprintf(lpSQL, "F:%s,T:%s,W:XMBH='%s',##", "DZYDMJ", xxContentTree.strSdeLyName, Glb_BizzInfo.strBizzCode);

	//	pRcdSet = NULL;
	//	if (pCommand->SelectData(lpSQL, &pRcdSet) != 1)
	//	{
	//		if(pRcdSet != NULL) 
	//		{
	//			pRcdSet->Release();
	//			pRcdSet = NULL;
	//		}
	//		pCommand->Release();
	//		return;
	//	}
	//	else if (pRcdSet->GetRecordCount() == 0)
	//	{
	//		if(pRcdSet != NULL) 
	//		{
	//			pRcdSet->Release();
	//			pRcdSet = NULL;
	//		}

	//		if (Glb_BizzInfo.strBzId.Find("选址") >= 0)
	//			strLayer = strXZDZYDFA;
	//		else
	//			strLayer = strYDDZYDFA;

	//		xxContentTree.strSdeLyName.Empty();
	//		xxContentTree = Glb_mapTree[strLayer];
	//		if (xxContentTree.strSdeLyName.IsEmpty())
	//		{
	//			pCommand->Release();
	//			return;
	//		}

	//		sprintf(lpSQL, "F:%s,T:%s,W:XMBH='%s',##", "DZYDMJ", xxContentTree.strSdeLyName, Glb_BizzInfo.strBizzCode);

	//		if (pCommand->SelectData(lpSQL, &pRcdSet) == 1)
	//		{
	//			double dVal = 0.0;
	//			if (pRcdSet->BatchRead() == 1)
	//			{
	//				dVal += *(double*)(*pRcdSet)[0];
	//				strDZYDMJ.Format("%0.3f", dVal);
	//			}
	//		}
	//	}
	//	else if (pRcdSet->BatchRead())
	//	{
	//		double dVal = 0.0;
	//		if (pRcdSet->BatchRead() == 1)
	//		{
	//			dVal += *(double*)(*pRcdSet)[0];
	//			strDZYDMJ.Format("%0.3f", dVal);
	//		}
	//	}

	//	pRcdSet->Release();

	//	pCommand->Release();

	//	///////////////////// 写ini文件
	//	TCHAR lpFileName[MAX_PATH] = {0};
	//	GetModuleFileName(GetModuleHandle("shDistCommands.arx"), lpFileName, MAX_PATH); 

	//	PathRemoveFileSpec(lpFileName);
	//	PathRemoveFileSpec(lpFileName);
	//	PathAppend(lpFileName, "sys");
	//	if (!PathFileExists(lpFileName))
	//		CreateDirectory(lpFileName, NULL);

	//	PathAppend(lpFileName, "SoftConfig.ini");

	//	WritePrivateProfileString("System", "总用地面积", "", lpFileName);
	//	WritePrivateProfileString("System", "净用地面积", "", lpFileName);
	//	WritePrivateProfileString("System", "代征用地面积", "", lpFileName);
	//	WritePrivateProfileString("System", "容积率", "", lpFileName);
	//	WritePrivateProfileString("System", "建筑密度", "", lpFileName);
	//	WritePrivateProfileString("System", "建筑限高", "", lpFileName);
	//	WritePrivateProfileString("System", "绿地率", "", lpFileName);
	//	WritePrivateProfileString("System", "停车位", "", lpFileName);

	//	WritePrivateProfileString("System", "总用地面积", strZYDMJ, lpFileName);
	//	WritePrivateProfileString("System", "净用地面积", strJYDMJ, lpFileName);
	//	WritePrivateProfileString("System", "代征用地面积", strDZYDMJ, lpFileName);
	//	WritePrivateProfileString("System", "容积率", strRJL, lpFileName);
	//	WritePrivateProfileString("System", "建筑密度", strJZMD, lpFileName);
	//	WritePrivateProfileString("System", "建筑限高", strJZXG, lpFileName);
	//	WritePrivateProfileString("System", "绿地率", strLDL, lpFileName);
	//	WritePrivateProfileString("System", "停车位", strTCW, lpFileName);
	//}

	//// ----- DistCommands._ReadSdeData command (do not rename)
	//static void DistCommands_ReadSdeData(void)
	//{
	//	ReadSdeData();

	//	// 调用 经济指标表 命令
	//	SendCommandToAutoCAD("JJZBB ");
	//}

	//// ----- DistCommands._gongshi command (do not rename)
	//static void DistCommands_gongshi(void) // 公示
	//{
	//	if (!CheckCommandPower("gongshi"))
	//		return;

	//	if (gongshi_array.GetCount() <= 0)
	//		return;

	//	CDlgGongShi2 dlg;

	//	if (dlg.DoModal() != IDOK)
	//		return;

	//	LockDocument _Lock;

	//	// 将文件保存至临时目录下并上传至FTP
	//	TCHAR lpTempPath[MAX_PATH] = {0};
	//	TCHAR lpTempFile[MAX_PATH] = {0};

	//	TCHAR lpDwgFile[MAX_PATH] = {0};
	//	sprintf(lpDwgFile, "%s.dwg", Glb_BizzInfo.strBizzCode);

	//	GetTempPath(MAX_PATH, lpTempPath);
	//	lstrcpy(lpTempFile, lpTempPath);
	//	GetTempFileName(lpTempPath, "$SH", 0, lpTempFile);

	//	AcDbDatabase *pDb;
	//	pDb = acdbHostApplicationServices()->workingDatabase();
	//	Acad::ErrorStatus er = pDb->saveAs(lpTempFile);
	//	if (er != Acad::eOk)
	//	{
	//		acutPrintf("\n 存储当前图出错！\n");
	//		return;
	//	}

	//	// 将文件复制一份，用于上传
	//	TCHAR lpLocalFile[MAX_PATH] = {0};
	//	GetTempFileName(lpTempPath, "$SH", 0, lpLocalFile);
	//	CopyFile(lpTempFile, lpLocalFile, FALSE);

	//	// 通过cadcontainer实现剩余功能（上传dwg文件，update数据库表）
	//	CString strCommand = "gongshi";
	//	CStringArray strParamArray;

	//	CString strParam;
	//	strParam.Format("file=%s%s", lpLocalFile, ";");
	//	strParamArray.Add(strParam);
	//	strParam.Format("审批人=%s", dlg.m_strReceiver);
	//	strParamArray.Add(strParam);
	//	strParam.Format("外部文件公示=%d", 0);
	//	strParamArray.Add(strParam);

	//	strParamArray.Append(gongshi_array);

	//	SendCopyDataMessage(Glb_hDapForm, strCommand, strParamArray);

	//	gongshi_array.RemoveAll();
	//}

//	// ----- DistCommands._gongshi_TK command (do not rename)
//	static void DistCommands_gongshi_TK(void)
//	{
//		IDistExchangeData* pExchangeData = Glb_pExchangeData;
//		IDistConnection* pConnection = Glb_pConnection;
//
////		CAcModuleResourceOverride res;
//	
//		CDlgGongShi3 dlg3;
//		if (dlg3.DoModal() != IDOK)
//			return;
//
//		// 创建公示编号
//		TCHAR lpCadProjectFileName[MAX_PATH] = {0};
//		GetModuleFileName(GetModuleHandle("shDistCommands.arx"), lpCadProjectFileName, MAX_PATH); 
//		PathRemoveFileSpec(lpCadProjectFileName);
//
//		PathAppend(lpCadProjectFileName, "CadProject.ini");
//
//		TCHAR lpRule[100] = {0};
//		GetPrivateProfileString("公示", "编号规则", "%4y", lpRule, 100, lpCadProjectFileName);
//
//		TCHAR lpPhone[200] = {0};
//		GetPrivateProfileString("公示", "联系电话", "0576-;0576-;0576-", lpPhone, 200, lpCadProjectFileName);
//
//		TCHAR lpAddress[200] = {0};
//		GetPrivateProfileString("公示", "联系地址", "台州市建设规划局;台州市建设规划局;台州市建设规划局", lpAddress, 200, lpCadProjectFileName);
//
//		TCHAR lpWebSite[200] = {0};
//		GetPrivateProfileString("公示", "网站", "台州市建设规划局;台州市建设规划局;台州市建设规划局", lpWebSite, 200, lpCadProjectFileName);
//
//		CString strCommand = "GeneratePublicizeCode";
//		CStringArray strParamArray;
//
//		CString strParam;
//		strParam.Format("编号规则=%s", lpRule);
//		strParamArray.Add(strParam);
//		strParam.Format("公示类型=%d", dlg3.m_nClass);
//		strParamArray.Add(strParam);
//		strParam.Format("公示栏目=%s", dlg3.m_strGSLM);
//		strParamArray.Add(strParam);
//
//		// 发送消息给dap，生成公示编号
//		SendCopyDataMessage(Glb_hDapForm, strCommand, strParamArray);
//
//		Sleep(200);
//		
//		CDlgGongShi dlg;
//
//		dlg.m_strBHGZ = lpRule; // 编号规则
//		dlg.m_strLXDH = lpPhone; // 联系电话
//		dlg.m_strLXDZ = lpAddress; // 联系地址
//		dlg.m_strWebSite = lpWebSite; // 网站
//		dlg.m_nClass = dlg3.m_nClass;
//		dlg.m_nGSLM = dlg3.m_nGSLM;
//		dlg.m_strBJBH = Glb_BizzInfo.strBizzSame; // 报建编号
//		dlg.m_strXMMC = Glb_BizzInfo.strBizzName; // 项目名称
//		dlg.m_strJSDW = Glb_BizzInfo.strBzOwner; // 建设单位
//		dlg.m_strYDXZ = ""; // 用地位置
//		dlg.m_strGCXZ = ""; // 工程性质
//
//		// 如果是不同意后退回的需要读取上次保存的值
//		TCHAR lpTemp[100] = {0};
//		GetPrivateProfileString("公示", "是否退回", "0", lpTemp, 100, "BizzInfo.ini");
//		if (strcmpi(lpTemp, "1") == 0)
//		{
//			lpTemp[0] = '\0';
//			GetPrivateProfileString("公示", "总用地面积", "", lpTemp, 100, "BizzInfo.ini");
//			dlg.m_strYDZMJ = lpTemp; dlg.m_strYDZMJ.Trim();
//			lpTemp[0] = '\0';
//			GetPrivateProfileString("公示", "净用地面积", "", lpTemp, 100, "BizzInfo.ini");
//			dlg.m_strYDJMJ = lpTemp; dlg.m_strYDJMJ.Trim();
//			lpTemp[0] = '\0';
//			GetPrivateProfileString("公示", "代征用地面积", "", lpTemp, 100, "BizzInfo.ini");
//			dlg.m_strYDDZMJ = lpTemp; dlg.m_strYDDZMJ.Trim();
//			lpTemp[0] = '\0';
//			GetPrivateProfileString("公示", "容积率", "", lpTemp, 100, "BizzInfo.ini");
//			dlg.m_strRJL = lpTemp; dlg.m_strRJL.Trim();
//			lpTemp[0] = '\0';
//			GetPrivateProfileString("公示", "建筑密度", "", lpTemp, 100, "BizzInfo.ini");
//			dlg.m_strJZMD = lpTemp; dlg.m_strJZMD.Trim();
//			lpTemp[0] = '\0';
//			GetPrivateProfileString("公示", "建筑规模", "", lpTemp, 100, "BizzInfo.ini");
//			dlg.m_strJZGM = lpTemp; dlg.m_strJZGM.Trim();
//			lpTemp[0] = '\0';
//			GetPrivateProfileString("公示", "建筑总高", "", lpTemp, 100, "BizzInfo.ini");
//			dlg.m_strJZZG = lpTemp; dlg.m_strJZZG.Trim();
//			lpTemp[0] = '\0';
//			GetPrivateProfileString("公示", "绿地率", "", lpTemp, 100, "BizzInfo.ini");
//			dlg.m_strLDL = lpTemp; dlg.m_strLDL.Trim();
//			lpTemp[0] = '\0';
//			GetPrivateProfileString("公示", "用地位置", "", lpTemp, 100, "BizzInfo.ini");
//			dlg.m_strYDXZ = lpTemp; dlg.m_strYDXZ.Trim();
//			lpTemp[0] = '\0';
//			GetPrivateProfileString("公示", "工程性质", "", lpTemp, 100, "BizzInfo.ini");
//			dlg.m_strGCXZ = lpTemp; dlg.m_strGCXZ.Trim();
//		}
//		else
//		{
//			// 读取用地面积、容积率等信息
//			ReadSdeData();
//
////			Sleep(200);
//
//			///////////////////// 读ini文件
//			TCHAR lpFileName[MAX_PATH] = {0};
//			GetModuleFileName(GetModuleHandle("shDistCommands.arx"), lpFileName, MAX_PATH); 
//
//			PathRemoveFileSpec(lpFileName);
//			PathRemoveFileSpec(lpFileName);
//			PathAppend(lpFileName, "\\sys\\SoftConfig.ini");
//
//			lpTemp[0] = '\0';
//			GetPrivateProfileString("System", "总用地面积", "", lpTemp, 100, lpFileName);
//			dlg.m_strYDZMJ = lpTemp; dlg.m_strYDZMJ.Trim();
//			lpTemp[0] = '\0';
//			GetPrivateProfileString("System", "净用地面积", "", lpTemp, 100, lpFileName);
//			dlg.m_strYDJMJ = lpTemp; dlg.m_strYDJMJ.Trim();
//			lpTemp[0] = '\0';
//			GetPrivateProfileString("System", "代征用地面积", "", lpTemp, 100, lpFileName);
//			dlg.m_strYDDZMJ = lpTemp; dlg.m_strYDDZMJ.Trim();
//			lpTemp[0] = '\0';
//			GetPrivateProfileString("System", "容积率", "", lpTemp, 100, lpFileName);
//			dlg.m_strRJL = lpTemp; dlg.m_strRJL.Trim();
//			lpTemp[0] = '\0';
//			GetPrivateProfileString("System", "建筑密度", "", lpTemp, 100, lpFileName);
//			dlg.m_strJZMD = lpTemp; dlg.m_strJZMD.Trim();
//			lpTemp[0] = '\0';
//			GetPrivateProfileString("System", "绿地率", "", lpTemp, 100, lpFileName);
//			dlg.m_strLDL = lpTemp; dlg.m_strLDL.Trim();
//		}
//
//		lpTemp[0] = '\0';
//		GetPrivateProfileString("公示", "公示编号", "", lpTemp, 100, "BizzInfo.ini");
//		dlg.m_strGSBH = lpTemp;	dlg.m_strGSBH.Trim();
//
//		// Add your code for command DistCommands._gongshi here
//		if (dlg.DoModal() == IDOK)
//		{
//			TCHAR lpFile[100] = {0};
//			GetPrivateProfileString("公示", dlg.m_strGSLX+dlg.m_strGSLM, "", lpFile, 100, lpCadProjectFileName);
//
//			if (lpFile[0] == '\0')
//			{
//				CString strMessage;
//				strMessage.Format("无法找到 %s %s 的模板文件，请检查CadProject.ini文件的配置。", dlg.m_strGSLX, dlg.m_strGSLM);
//				AfxMessageBox(strMessage);
//				return;
//			}
//			CString strDwgFile;
//			strDwgFile.Format("%s%s%s",DistCommands_GetCurArxAppPath("shDistCommands.arx"), "公示模板\\", lpFile);
//
//			CStringArray strTagArray, strTextArray;
//			strTagArray.Add("项目名称");
//			strTagArray.Add("建设单位");
//			strTagArray.Add("用地位置");
//			strTagArray.Add("工程性质");
//			strTagArray.Add("公告期限");
//			strTagArray.Add("日期");
//			strTagArray.Add("地块总面积");
//			strTagArray.Add("地块净面积");
//			strTagArray.Add("代征道路绿化面积");
//			strTagArray.Add("容积率");
//			strTagArray.Add("建筑密度");
//			strTagArray.Add("建筑规模");
//			strTagArray.Add("建筑总高");
//			strTagArray.Add("绿地率");
//			strTagArray.Add("公示编号");
//			strTagArray.Add("联系电话");
//			strTagArray.Add("联系地址");
//			strTagArray.Add("网站");
//
//			CString strParam;
//
//			strTextArray.Add(dlg.m_strXMMC); // 项目名称
//			strTextArray.Add(dlg.m_strJSDW); // 建设单位
//			strTextArray.Add(dlg.m_strYDXZ); // 用地位置
//			strTextArray.Add(dlg.m_strGCXZ); // 工程性质
//
//			// 公告期限
////			strParam.Format("%d年%d月%d日-%d年%d月%d日", dlg.m_tmStart.wYear, dlg.m_tmStart.wMonth, dlg.m_tmStart.wDay, dlg.m_tmEnd.wYear, dlg.m_tmEnd.wMonth, dlg.m_tmEnd.wDay);
////			strParam.Format("%d 天", dlg.m_nLimit);
//			strParam.Format("%s年%s月%s日-%s年%s月%s日", "    ", "  ", "  ", "    ", "  ", "  ");
//			strTextArray.Add(strParam);
//			
//			// 日期
//			lpTemp[0] = '\0';
//			GetPrivateProfileString("公示", "当前时间", "", lpTemp, 100, "BizzInfo.ini");
//			strParam = lpTemp;
//			strTextArray.Add(strParam);
//
//			strTextArray.Add(dlg.m_strYDZMJ); // 地块总面积
//			strTextArray.Add(dlg.m_strYDJMJ); // 地块净面积
//			strTextArray.Add(dlg.m_strYDDZMJ); // 代征道路绿化面积
//			strTextArray.Add(dlg.m_strRJL); // 容积率
//			strTextArray.Add(dlg.m_strJZMD); // 建筑密度
//			strTextArray.Add(dlg.m_strJZGM); // 建筑规模
//			strTextArray.Add(dlg.m_strJZZG); // 建筑总高
//			strTextArray.Add(dlg.m_strLDL); // 绿地率
//			strTextArray.Add(dlg.m_strGSBH); // 公示编号
//			strTextArray.Add(dlg.m_strLXDH); // 联系电话
//			strTextArray.Add(dlg.m_strLXDZ); // 联系地址
//			strTextArray.Add(dlg.m_strWebSite); // 网站
//
//			// 关闭数据库反应器
//			if (pExchangeData != NULL)
//				pExchangeData->SetDbReactorStatues(false);
//
//			DeleteExternEntities();
//			BOOL bRet = Sys_PlotEx(strDwgFile, strTagArray, strTextArray);
//
//			// 关闭数据库反应器
//			if (pExchangeData != NULL)
//				pExchangeData->SetDbReactorStatues(true);
//
//			if (!bRet)
//				return;
//
//			gongshi_array.RemoveAll();
//
//			strParam.Format("编号规则=%s", dlg.m_strBHGZ);
//			gongshi_array.Add(strParam);
//
//			strParam.Format("项目名称=%s", dlg.m_strXMMC);
//			gongshi_array.Add(strParam);
//			strParam.Format("建设单位=%s", dlg.m_strJSDW);
//			gongshi_array.Add(strParam);
//			strParam.Format("用地位置=%s", dlg.m_strYDXZ);
//			gongshi_array.Add(strParam);
//			strParam.Format("工程性质=%s", dlg.m_strGCXZ);
//			gongshi_array.Add(strParam);
//			strParam.Format("公示类型=%d", dlg.m_nClass);
//			gongshi_array.Add(strParam);
//			strParam.Format("公示栏目=%s", dlg.m_strGSLM);
//			gongshi_array.Add(strParam);
//			strParam.Format("公示形式=%s", dlg.m_strGSXX);
//			gongshi_array.Add(strParam);
//			strParam.Format("地块总面积=%s", dlg.m_strYDZMJ);
//			gongshi_array.Add(strParam);
//			strParam.Format("地块净面积=%s", dlg.m_strYDJMJ);
//			gongshi_array.Add(strParam);
//			strParam.Format("代征道路绿化面积=%s", dlg.m_strYDDZMJ);
//			gongshi_array.Add(strParam);
//			strParam.Format("容积率=%s", dlg.m_strRJL);
//			gongshi_array.Add(strParam);
//			strParam.Format("建筑密度=%s", dlg.m_strJZMD);
//			gongshi_array.Add(strParam);
//			strParam.Format("建筑规模=%s", dlg.m_strJZGM);
//			gongshi_array.Add(strParam);
//			strParam.Format("建筑总高=%s", dlg.m_strJZZG);
//			gongshi_array.Add(strParam);
//			strParam.Format("绿地率=%s", dlg.m_strLDL);
//			gongshi_array.Add(strParam);
//			strParam.Format("公示编号=%s", dlg.m_strGSBH);
//			gongshi_array.Add(strParam);
//			strParam.Format("流水号=%s", dlg.m_strLSH);
//			gongshi_array.Add(strParam);
//			// 公告期限
//			strParam.Format("公示开始时间=%02d-%02d-%04d", dlg.m_tmStart.wDay, dlg.m_tmStart.wMonth, dlg.m_tmStart.wYear);
//			gongshi_array.Add(strParam);
//			strParam.Format("公示结束时间=%02d-%02d-%04d", dlg.m_tmStart.wDay, dlg.m_tmStart.wMonth, dlg.m_tmStart.wYear);
//			gongshi_array.Add(strParam);
//
//			strParam.Format("公示期限=%d", dlg.m_nLimit);
//			gongshi_array.Add(strParam);
//		}
//	}

//	// ----- DistCommands._GongShiEx command
//	static void DistCommands_GongShiEx(void)
//	{
//		if (!CheckCommandPower("gongshi"))
//			return;
//
//		// 创建公示编号
//		TCHAR lpCadProjectFileName[MAX_PATH] = {0};
//		GetModuleFileName(GetModuleHandle("shDistCommands.arx"), lpCadProjectFileName, MAX_PATH); 
//		PathRemoveFileSpec(lpCadProjectFileName);
//
//		PathAppend(lpCadProjectFileName, "CadProject.ini");
//
//		TCHAR lpRule[100] = {0};
//		GetPrivateProfileString("公示", "编号规则", "%4y", lpRule, 100, lpCadProjectFileName);
//
//		CString strCommand = "GeneratePublicizeCode";
//		CStringArray strParamArray;
//
//		CString strParam;
//		strParam.Format("编号规则=%s", lpRule);
//		strParamArray.Add(strParam);
//		strParam.Format("公示类型=%d", 0);
//		strParamArray.Add(strParam);
//		strParam.Format("公示栏目=%s", "");
//		strParamArray.Add(strParam);
//
//		// 发送消息给dap，生成公示编号
//		SendCopyDataMessage(Glb_hDapForm, strCommand, strParamArray);
//
//		CDlgGongShi dlg;
//		dlg.m_bExt = TRUE;
//
//		dlg.m_strBHGZ = lpRule; // 编号规则
//
//		TCHAR lpTemp[100] = {0};
//		GetPrivateProfileString("公示", "公示编号", "", lpTemp, 100, "BizzInfo.ini");
//		dlg.m_strGSBH = lpTemp;	dlg.m_strGSBH.Trim();
//
//		if (dlg.DoModal() != IDOK)
//			return;
//
//		gongshi_array.RemoveAll();
//
//		strParam.Format("编号规则=%s", dlg.m_strBHGZ);
//		gongshi_array.Add(strParam);
//
//		strParam.Format("项目名称=%s", dlg.m_strXMMC);
//		gongshi_array.Add(strParam);
//		strParam.Format("建设单位=%s", dlg.m_strJSDW);
//		gongshi_array.Add(strParam);
//		strParam.Format("用地位置=%s", dlg.m_strYDXZ);
//		gongshi_array.Add(strParam);
//		strParam.Format("工程性质=%s", dlg.m_strGCXZ);
//		gongshi_array.Add(strParam);
//		strParam.Format("公示类型=%d", dlg.m_nClass);
//		gongshi_array.Add(strParam);
//		strParam.Format("公示栏目=%s", dlg.m_strGSLM);
//		gongshi_array.Add(strParam);
//		strParam.Format("公示形式=%s", dlg.m_strGSXX);
//		gongshi_array.Add(strParam);
//		strParam.Format("地块总面积=%s", dlg.m_strYDZMJ);
//		gongshi_array.Add(strParam);
//		strParam.Format("地块净面积=%s", dlg.m_strYDJMJ);
//		gongshi_array.Add(strParam);
//		strParam.Format("代征道路绿化面积=%s", dlg.m_strYDDZMJ);
//		gongshi_array.Add(strParam);
//		strParam.Format("容积率=%s", dlg.m_strRJL);
//		gongshi_array.Add(strParam);
//		strParam.Format("建筑密度=%s", dlg.m_strJZMD);
//		gongshi_array.Add(strParam);
//		strParam.Format("建筑规模=%s", dlg.m_strJZGM);
//		gongshi_array.Add(strParam);
//		strParam.Format("建筑总高=%s", dlg.m_strJZZG);
//		gongshi_array.Add(strParam);
//		strParam.Format("绿地率=%s", dlg.m_strLDL);
//		gongshi_array.Add(strParam);
//		strParam.Format("公示编号=%s", dlg.m_strGSBH);
//		gongshi_array.Add(strParam);
//		strParam.Format("流水号=%s", dlg.m_strLSH);
//		gongshi_array.Add(strParam);
//
//		// 公告期限
//		strParam.Format("公示开始时间=%02d-%02d-%04d", dlg.m_tmStart.wDay, dlg.m_tmStart.wMonth, dlg.m_tmStart.wYear);
//		gongshi_array.Add(strParam);
//		strParam.Format("公示结束时间=%02d-%02d-%04d", dlg.m_tmStart.wDay, dlg.m_tmStart.wMonth, dlg.m_tmStart.wYear);
//		gongshi_array.Add(strParam);
//
//		strParam.Format("公示期限=%d", dlg.m_nLimit);
//		gongshi_array.Add(strParam);
//
//		CDlgGongShi2 dlg2;
//
//		if (dlg2.DoModal() != IDOK)
//			return;
//
////		// 将文件保存至临时目录下并上传至FTP
////		TCHAR lpTempPath[MAX_PATH] = {0};
////		TCHAR lpTempFile[MAX_PATH] = {0};
//
////		GetTempPath(MAX_PATH, lpTempPath);
////		GetTempFileName(lpTempPath, "$SH", 0, lpTempFile);
////		CopyFile(dlg.m_strFileName, lpTempFile, FALSE);
//
//		CString strFileList = "";
//		for (int i = 0; i < dlg.m_strFileArray.GetCount(); i++)
//		{
//			strFileList += dlg.m_strFileArray.GetAt(i);
//			strFileList += ";";
//		}
//
//		// 通过cadcontainer实现剩余功能（上传dwg文件，update数据库表）
//		strCommand = "gongshi";
//		strParamArray.RemoveAll();
//
//		strParam.Format("file=%s", strFileList);
//		strParamArray.Add(strParam);
//		strParam.Format("审批人=%s", dlg2.m_strReceiver);
//		strParamArray.Add(strParam);
//		strParam.Format("外部文件公示=%d", 1);
//		strParamArray.Add(strParam);
//
//		strParamArray.Append(gongshi_array);
//
//		SendCopyDataMessage(Glb_hDapForm, strCommand, strParamArray);
//
//		gongshi_array.RemoveAll();
//	}

	//// ----- DistCommands._RetrivePublicize command (do not rename)
	//static void DistCommands_RetrivePublicize(void)
	//{
	//	// 取回公示
	//	CString strCommand = "RetrivePublicize";
	//	CStringArray strParamArray;

	//	SendCopyDataMessage(Glb_hDapForm, strCommand, strParamArray);
	//}

	//// ----- DistCommands._TuKuang command (do not rename)
	//static void DistCommands_TuKuang(void)
	//{
	//	IDistExchangeData* pExchangeData = Glb_pExchangeData;

	//	if (!CheckCommandPower("DISTDYTK"))
	//		return;


	//	CTKSelDlg dlg;
	//	if (dlg.DoModal() != IDOK)
	//		return;

	//	CString strTKName = dlg.m_strTKName;

	//	CString strDwgFile;
	//	strDwgFile.Format("%s%s%s%s",DistCommands_GetCurArxAppPath("shDistCommands.arx"), "图框模板\\", strTKName, ".dwg");

	//	CStringArray arrayTag, arrayValue;

	//	if (strTKName.CompareNoCase("用地") == 0)
	//	{
	//		CTKYongDiDlg dlg;
	//		if (dlg.DoModal() != IDOK)
	//			return;

	//		arrayTag.Append(dlg.arrayTag);
	//		arrayValue.Append(dlg.arrayValue);
	//	}
	//	else if (strTKName.CompareNoCase("工程") == 0)
	//	{
	//		CTKGongChengDlg dlg;
	//		if (dlg.DoModal() != IDOK)
	//			return;

	//		arrayTag.Append(dlg.arrayTag);
	//		arrayValue.Append(dlg.arrayValue);
	//	}
	//	else
	//		 return;

	//	// 关闭数据库反应器
	//	if (pExchangeData != NULL)
	//		pExchangeData->SetDbReactorStatues(false);

	//	Sys_PlotEx_NN(strDwgFile, arrayTag, arrayValue);

	//	// 关闭数据库反应器
	//	if (pExchangeData != NULL)
	//		pExchangeData->SetDbReactorStatues(true);
	//}

	// ----- DistCommands._DeleteXData command (do not rename)
	static void DistCommands_DeleteXData(void)
	{
		//交互选择实体
//		BeginEditorCommand();

		ads_name ssName;
		struct resbuf *filter=NULL;
		if (acedSSGet(":S", NULL, NULL, filter, ssName) != RTNORM)
		{
			acutPrintf("\n选择实体有误!");
			acedSSFree(ssName); acutRelRb(filter);
//			CompleteEditorCommand();
			return;
		}
		if(filter != NULL) acutRelRb(filter);

		AcDbObjectId tempObjId;
		ads_name ssTemp;
		long nNum = 0;
		int nPos = 0;
		acedSSLength(ssName,&nNum);
		CString strSdeName,strId,strRowId;

		for(int i=0; i<nNum; i++)
		{
			acedSSName(ssName, i, ssTemp);
			if(Acad::eOk != acdbGetObjectId(tempObjId, ssTemp)) continue;
			strId.Format("%d",tempObjId.asOldId());
			CDistXData tempXData(tempObjId,"ROWID_OBJS");
			tempXData.DeleteAllRecord();
			tempXData.Close();
		}

		acedSSFree(ssName);

//		CompleteEditorCommand();
	}

	// ----- DistCommands._SaveLayerState command (do not rename)
	static void DistCommands_SaveLayerState(void)
	{
		// Add your code for command DistCommands._SaveLayerState here
		CStringArray arrayLayers;

		// 获取所有打勾的图层
		Sys_GetCheckedLayers(arrayLayers);

		acutPrintf("\n保存图层状态...\n");

		CString strCommand = "SaveLayerState";
		CStringArray arrayParams;

		// 读取图层数据
		for (int i=0; i<arrayLayers.GetCount();i++)
		{
			STB_CONTENT_TREE xxContentTree = Glb_mapTree[arrayLayers.GetAt(i)];
			if (xxContentTree.nId > 0)
			{
				OutputDebugStringX("\nSaveLayerState::%s\n", xxContentTree.strNodeName);
				CString strParam;
				strParam.Format("%d=%d", xxContentTree.nId, 1);
				arrayParams.Add(strParam);
			}
		}

		SendCopyDataMessage(Glb_hDapForm, strCommand, arrayParams);

		acutPrintf("\n保存成功。\n");
	}

	// ----- DistCommands._OnExportSdeData command (do not rename)
	static void DistCommands_OnExportSdeData(void)
	{
		// Add your code for command DistCommands._OnExportSdeData here

		if ((Glb_pConnection == NULL) || (Glb_pExchangeData == NULL))
			return;

		// 1.清空图面
//		SendCommandToAutoCAD("DeleteAllEntity ");

		DistCommands_DeleteAllEntity();

		// 2.重新调图
//		SendCommandToAutoCAD("ReadSdeLayers ");

		DistCommands_ReadSdeLayers();

		// 3.保存当前图面到文件
		TCHAR lpTempFile[MAX_PATH] = {0};

		GetTempPath(MAX_PATH, lpTempFile);
		GetTempFileName(lpTempFile, "$SH", 0, lpTempFile);

		AcDbDatabase *pDb;
		pDb = acdbHostApplicationServices()->workingDatabase();
		Acad::ErrorStatus er = pDb->saveAs(lpTempFile);
		if (er != Acad::eOk)
		{
			acutPrintf("\n 存储当前图出错！\n");
			return;
		}

		TCHAR lpDwgFile[MAX_PATH] = {0};
		sprintf(lpDwgFile, "%s.dwg", Glb_BizzInfo.strBizzCode);
		CFileDialog fileDlg(false, "*.dwg", lpDwgFile, OFN_LONGNAMES | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST,
			"dwg文件 (*.dwg)|*.dwg||", acedGetAcadFrame());
		
		if (IDOK == fileDlg.DoModal())
		{
			CString strFileName = fileDlg.GetPathName();
			CopyFile(lpTempFile, strFileName, FALSE);
		}
	}

	//画项目范围线
	// ----- DistCommands._DrawXMFWX command (do not rename)
	static void DistCommands_DrawXMFWX(void)
	{
		//int i,k;                    //循环控制
		//int Ret;                    //函数返回值
		//long RowId;                 //返回SDE库的Id
		BOOL IsEdFwLine=FALSE;      //是否更新范围红线
		//AcGePoint3d pt1,pt2,pt3,pt4;//点
		//AcGePoint3dArray FwPts;     //范围点集
		//AcDbObjectId FwLineId;      //范围线的Id
		CString TemStr;             //临时字符串
		CString SqlStrs;             //SQL查询语句
		//CString CurLayer;           //当前处理的图层
		//CStringArray XDataVals;     //扩展数据
		//AcDbEntity* pEnt;           //AutoCAD实体
		CDistEntityTool tempTool;
		
		long lRt = 0;
		IDistConnection* pConnection = Glb_pConnection;
		IDistExchangeData* pExchangeData = Glb_pExchangeData;

		BOOL bReadOnly = TRUE;

		CString strProjectId = DocVars.docData().GetProjectId();

		if (strProjectId.IsEmpty())
		{
			AfxMessageBox("当前文档不支持该操作");
			return;
		}

		STB_CONTENT_TREE xxContentTree = Glb_mapTree[strXMFWX];

		if (xxContentTree.strSdeLyName.IsEmpty())
		{
			AfxMessageBox("未找到项目范围线对应的SDE图层！", MB_OK);
			return;
		}

		//CString strAppNme = "DIST_";
		//strAppNme += xxContentTree.strSdeLyName;
		//lRt = pExchangeData->ReadSymbolInfo(pConnection, NULL, NULL, xxContentTree.strSdeLyName, strAppNme, NULL);
		//if (lRt != 1)
		//{
		//	acutPrintf("\n%s",ReturnErrorCodeInfo(lRt)); return;
		//}

		CString strWhere;
		strWhere.Format("XMID=%s", strProjectId); // 使用项目ID
		ShowLayer(strXMFWX, true, strWhere);

		AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();
		if (tempTool.SetCurLayer(strXMFWX, pDB))
		{
			acutPrintf("\n当前图层切换为 %s\n", strXMFWX);
		}
		else
		{
			acutPrintf("\n未能找到图层 %s\n", strXMFWX);
			return;
		}
		///////////////////////////////////////////////////////////////

		IDistCommand *pCommand = CreateSDECommandObjcet();
		if(NULL == pCommand) return;
		pCommand->SetConnectObj(pConnection);

		TemStr="F:OBJECTID,XMID,SHAPE,";
		SqlStrs.Format("T:%s,W:XMID=%s,##", xxContentTree.strSdeLyName, strProjectId);
		SqlStrs=TemStr+SqlStrs;

		IDistRecordSet *pRcdSet = NULL;
		if (pCommand->SelectData(SqlStrs, &pRcdSet) != 1)
		{
			if(pRcdSet != NULL) 
			{
				pRcdSet->Release();
				pRcdSet = NULL;
			}
			pCommand->Release();
			return;
		}
		else if (pRcdSet->GetRecordCount() > 0)
		{
			lRt=AfxMessageBox("当前项目范围红线已经存在,您是否要替换当前的范围红线",MB_YESNO);
			if(lRt!=6) return;
			IsEdFwLine=TRUE;
		}
		
		////交互矩形点坐标获取
		////k=pGeFun->SetOsMode(0);
		//if(acedGetPoint(NULL,"\n 请确定范围的的第一角点",asDblArray(pt1))!=RTNORM)
		//{
		//	//pGeFun->SetOsMode(k);
		//	return;
		//}
		//if(acedGetCorner(asDblArray(pt1),
		//	"\n 请再确定范围的第二角点",asDblArray(pt2))!=RTNORM)
		//{
		//	//pGeFun->SetOsMode(k);
		//	return;
		//}
		////pGeFun->SetOsMode(k);
		////矩形框合法性检查
		//if(pt1==pt2)
		//{
		//	AfxMessageBox("请指定正确的范围区域");
		//	return;
		//}
		////pGeFun->LockLayer(pGeFun->mFwHxLayerDef.SdeToCad.CadLyName,FALSE);
		//pt3.x=min(pt1.x,pt2.x);pt3.y=min(pt1.y,pt2.y);
		//pt4.x=max(pt1.x,pt2.x);pt4.y=max(pt1.y,pt2.y);
		//pt1.x=pt4.x;pt1.y=pt3.y;
		//pt2.x=pt3.x;pt2.y=pt4.y;
		//FwPts.append(pt3);
		//FwPts.append(pt1);
		//FwPts.append(pt4);
		//FwPts.append(pt2);
		//tempTool.CreatePolyline(FwLineId,FwPts,strXMFWX,TRUE);
		////acdbOpenAcDbEntity(pEnt,FwLineId,AcDb::kForWrite);
		////pEnt->setLinetypeScale(100);
		////pEnt->recordGraphicsModified();
		////pEnt->downgradeOpen();
		////pEnt->draw();
		////pEnt->close();

		//清除项目范围线图层上所有实体
//		tempTool.DeleteAllEntityInLayer(strXMFWX.GetBuffer(), acdbCurDwg());
//		strXMFWX.ReleaseBuffer();

		AcDbObjectIdArray IdArray;
		if(tempTool.SelectEntityInLayer(IdArray, strXMFWX)>0) //取得所以实体
		{
			AcDbObjectId eId;
			AcDbEntity *pE;

			for(int i=0;i<IdArray.logicalLength();i++)
			{
				eId=IdArray.at(i);

				char strXDataName[1024]={0};
				sprintf(strXDataName,"DIST_%s", xxContentTree.strSdeLyName);
				CDistXData tempXdata(eId,strXDataName);

				CString strXMID; // 使用项目ID
				tempXdata.Select("XMID", strXMID);
				int nCount = tempXdata.GetRecordCount();
				tempXdata.Close();

				strXMID.Trim();

				if ((strXMID.CompareNoCase(DocVars.docData().GetProjectId()) == 0 ) && (nCount > 0) )
				{
					// 删除实体
					AcDbEntity * pEnt = NULL;
					if (ErrorStatus::eOk == acdbOpenObject(pEnt, eId, AcDb::kForWrite))
					{
						pEnt->erase();
						pEnt->close();                
					}
				}
			}

			IdArray.setLogicalLength(0);
		}

		AcDbObjectId objId;
		if (!gCmdCreateBound(strXMFWX, "项目范围线", objId))
			return;

//		acedUpdateDisplay();
/*
		TCHAR lpSql[256] = {0};
		sprintf(lpSql, "T:%s,W:XMID=%s,##", xxContentTree.strSdeLyName, DocVars.docData().GetProjectId());
		pCommand->DeleteData(lpSql);
*/
		AcDbEntity * pEnt=NULL; 
		AcDbPolyline* pLine=NULL; 
		if(Acad::eOk != acdbOpenObject(pEnt, objId,AcDb::kForWrite))
		{
			return;
		}

		if(pEnt->isKindOf(AcDbPolyline::desc())==false)
		{
			pEnt->close(); return;
		}

		pLine = (AcDbPolyline*) pEnt;

		AcGePoint3dArray ptArray;
		tempTool.GetPolylinePts(pLine, ptArray);

		AcGePoint3d ptLB, ptRT;
		// 对点数组进行处理
		for(int i=0; i<ptArray.logicalLength(); i++)
		{
			// left
			if (ptLB.x == 0 || ptArray.at(i).x < ptLB.x)
				ptLB.x = ptArray.at(i).x;
			// bottom
			if (ptLB.y == 0 || ptArray.at(i).y < ptLB.y)
				ptLB.y = ptArray.at(i).y;
			// right
			if (ptRT.x == 0 || ptArray.at(i).x > ptRT.x)
				ptRT.x = ptArray.at(i).x;
			// top
			if (ptRT.y == 0 || ptArray.at(i).y > ptRT.y)
				ptRT.y = ptArray.at(i).y;
		}

		pLine->close();

		if (!SaveXMFWX(xxContentTree.strNodeName, xxContentTree.strSdeLyName, objId))
			return;

		// 定义空间过滤条件
		DIST_STRUCT_SELECTWAY filter;
		filter.nEntityType = 4;
		filter.nSelectType = kSM_AI;
		filter.ptArray.setLogicalLength(0);

		filter.ptArray.append(AcGePoint3d(ptLB[0]-100, ptLB[1]-100, ptLB[2]));
		filter.ptArray.append(AcGePoint3d(ptRT[0]+100, ptRT[1]+100, ptRT[2]));

		AddRaletionProjects(xxContentTree.strSdeLyName, "XMID", filter);

		return;

		lRt=AfxMessageBox("保存到数据库",MB_YESNO);
		if(lRt!=6)
		{
			//acdbOpenAcDbEntity(pEnt,FwLineId,AcDb::kForWrite);
			//pEnt->erase();
			//pEnt->close();
			//pEnt=NULL;
			////pGeFun->LockLayer(pGeFun->mFwHxLayerDef.SdeToCad.CadLyName,TRUE);

			//清除项目范围线图层上所有实体
			tempTool.DeleteAllEntityInLayer(strXMFWX.GetBuffer(), acdbCurDwg());
			strXMFWX.ReleaseBuffer();
			return;
		}
		//pGeFun->mFwHxLayerDef.LefDwnPt=pt3;
		//pGeFun->mFwHxLayerDef.RigUpPt=pt4;
		if(IsEdFwLine)
		{
			//原来已经存在范围线了
			//TemStr.Format("%d",pGeFun->mFwHxLayerDef.RowId);
			//XDataVals.Add("OBJECTID:"+TemStr);//RowId
			//XDataVals.Add("SDELYBJ:ITEMSELF");//该实体所在的SDE图层的标记
			//TemStr="PROJECT_ID:"+pGeFun->mProjectId;
			//XDataVals.Add(TemStr);            //项目ID
			//Ret=pGeFun->SetRedXData(XDataVals,FwLineId,"ENTITY_STYLE");
			//if(Ret!=0)
			//{
			//	//pGeFun->LockLayer(pGeFun->mFwHxLayerDef.SdeToCad.CadLyName,TRUE);
			//	return;//异常退出
			//}
			//Ret=pGeFun->CalFwRedXData(XDataVals);
			//if(Ret!=0)
			//{
			//	//pGeFun->LockLayer(pGeFun->mFwHxLayerDef.SdeToCad.CadLyName,TRUE);
			//	return;//异常退出
			//}
			//Ret=pGeFun->SetRedXData(XDataVals,FwLineId);//设置完全扩展数据
			//if(Ret!=0)
			//{
			//	//pGeFun->LockLayer(pGeFun->mFwHxLayerDef.SdeToCad.CadLyName,TRUE);
			//	return;//异常退出
			//}
   //   //负载工作线的属性数据
   //   for(i=0;i<pGeFun->pLyTrGlInfs[0].SySdeCadCfgs.length();i++)
   //   {
   //     Ret=pGeFun->CalWorkRedXData(XDataVals,i);//工作主线
   //     if(Ret!=0) continue;
   //     TemStr.Format("%d",i);
   //     TemStr="WORKLINE"+TemStr;
   //     Ret=pGeFun->SetRedXData(XDataVals,FwLineId,TemStr);
   //   }
   //   k=pGeFun->mDimSdeLayers.GetSize()/2;
   //   for(i=0;i<k;i++)
   //   {
   //     Ret=pGeFun->CalDimRedXData(XDataVals,i);//工作辅助线[标注]
   //     if(Ret!=0) continue;
   //     TemStr.Format("%d",i);
   //     TemStr="DIM"+TemStr;
   //     Ret=pGeFun->SetRedXData(XDataVals,FwLineId,TemStr);
   //   }
			//pGeFun->mCurSdeLayer=pGeFun->mFwHxLayerDef.SdeToCad.SdeLyName;
			//Ret=pGeFun->EditRecordToSDE(FwLineId,pGeFun->mFwHxLayerDef.RowId,2,
			//	pGeFun->mFwHxLayerDef.SdeToCad.SdeLyName,
			//	pGeFun->mFwHxLayerDef.SdeToCad.SdeLyType);
			//if(Ret!=0)
			//{
			//	//pGeFun->LockLayer(pGeFun->mFwHxLayerDef.SdeToCad.CadLyName,TRUE);
			//	return;
			//}
			//if(acdbOpenAcDbEntity(pEnt,pGeFun->mFwHxLayerDef.CadId,AcDb::kForWrite)==Acad::eOk)
			//{
			//	pEnt->erase();
			//	pEnt->close();
			//}
			//pGeFun->mFwHxLayerDef.CadId=FwLineId;

			SqlStrs.Format("T:%s,W:XMID=%s,##", xxContentTree.strSdeLyName, strProjectId);
			pCommand->DeleteData(SqlStrs.GetBuffer());

			SqlStrs.ReleaseBuffer();

		}

		//XDataVals.Add("OBJECTID:0");      //临时的RowId[目前还没有数据]
		//XDataVals.Add("SDELYBJ:ITEMSELF");//该实体所在的SDE图层的标记
		//TemStr="PROJECT_ID:"+pGeFun->mProjectId;
		//XDataVals.Add(TemStr);            //项目ID
		//pGeFun->SetRedXData(XDataVals,FwLineId,"ENTITY_STYLE");
		//Ret=pGeFun->CalFwRedXData(XDataVals);//设置完全扩展数据
		//if(Ret!=0)
		//{
		//	//pGeFun->LockLayer(pGeFun->mFwHxLayerDef.SdeToCad.CadLyName,TRUE);
  //  OutputDebugString("画范围红线pGeFun->CalFwRedXData失败");
		//	return;//异常退出
		//}
		//Ret=pGeFun->SetRedXData(XDataVals,FwLineId);//设置完全扩展数据
		//if(Ret!=0)
		//{
		//	//pGeFun->LockLayer(pGeFun->mFwHxLayerDef.SdeToCad.CadLyName,TRUE);
  //  OutputDebugString("画范围红线pGeFun->SetRedXData失败");
		//	return;//异常退出
		//}
  //  //负载工作线的属性数据
  //  for(i=0;i<pGeFun->pLyTrGlInfs[0].SySdeCadCfgs.length();i++)
  //  {
  //  Ret=pGeFun->CalWorkRedXData(XDataVals,i);//工作主线
  //  if(Ret!=0) continue;
  //  TemStr.Format("%d",i);
  //  TemStr="WORKLINE"+TemStr;
  //  Ret=pGeFun->SetRedXData(XDataVals,FwLineId,TemStr);
  //  }
  //  k=pGeFun->mDimSdeLayers.GetSize()/2;
  //  for(i=0;i<k;i++)
  //  {
  //  Ret=pGeFun->CalDimRedXData(XDataVals,i);//工作辅助线[标注]
  //  if(Ret!=0) continue;
  //  TemStr.Format("%d",i);
  //  TemStr="DIM"+TemStr;
  //  Ret=pGeFun->SetRedXData(XDataVals,FwLineId,TemStr);
  //  }
		//Ret=pGeFun->AddRecordToSDE(RowId,pGeFun->mFwHxLayerDef.SdeToCad.SdeLyName,
		//	FwLineId,pGeFun->mFwHxLayerDef.SdeToCad.SdeLyType);
		//if(Ret!=0)
		//{
		//	//pGeFun->LockLayer(pGeFun->mFwHxLayerDef.SdeToCad.CadLyName,TRUE);
  //  OutputDebugString("画范围红线pGeFun->AddRecordToSDE失败");
		//	return;
		//}
		////校正扩展数据
		//TemStr.Format("%d",RowId);
		//TemStr="OBJECTID:"+TemStr;
		//XDataVals[0]=TemStr;
		//Ret=pGeFun->SetRedXData(XDataVals,FwLineId,"ENTITY_STYLE");
		//if(Ret!=0)
		//{
		//	//pGeFun->LockLayer(pGeFun->mFwHxLayerDef.SdeToCad.CadLyName,TRUE);
  //  OutputDebugString("画范围红线pGeFun->SetRedXData失败");
		//	return;//异常退出
		//}
		//pGeFun->mFwHxLayerDef.IsHasFanWei=TRUE;
		//pGeFun->mFwHxLayerDef.RowId=RowId;
		//pGeFun->mFwHxLayerDef.CadId=FwLineId;

		////pGeFun->LockLayer(pGeFun->mFwHxLayerDef.SdeToCad.CadLyName,TRUE);
		//Ret=pGeFun->DelAllReadEnts();//自动进行全面删除处理
		//if(Ret!=0) OutputDebugString("在清理的过程中发生了错误");

		IsSaveXMFWX = TRUE;
		SaveCadLayer(strXMFWX.GetBuffer());
		strXMFWX.ReleaseBuffer();
		IsSaveXMFWX = FALSE;

		acutPrintf("\n 保存成功");

//		AcGePoint3d ptLB, ptRT;
		// 获取红线绘制范围
		if (!GetRangePoints(ptLB, ptRT))
		{
			Glb_Filter.ptArray.setLogicalLength(0);

			acutPrintf("\n没有找到绘制的项目范围\n");
//			MessageBox(acedGetAcadFrame()->m_hWnd, "没有找到绘制的项目范围", "错误提示", MB_OK);
			return;
		}

		//查询相关项目
		CStringArray arrProjctIDs;
		arrProjctIDs.RemoveAll();
		Glb_Filter.ptArray.setLogicalLength(0);

		Glb_Filter.ptArray.append(AcGePoint3d(ptLB[0]-100, ptLB[1]-100, ptLB[2])); //100米缓冲内
		Glb_Filter.ptArray.append(AcGePoint3d(ptRT[0]+100, ptRT[1]+100, ptRT[2])); //100米缓冲内
		
		strWhere.Format("XMID!=%s", DocVars.docData().GetProjectId()); // 使用项目ID
		lRt = ReadSdeLayer(pConnection, pExchangeData, xxContentTree.strSdeLyName, NULL, strWhere, bReadOnly, FALSE);

		int nlen = strlen(strXMFWX)+1;

		ads_name ents;
		struct resbuf *rb;
		rb=acutNewRb(AcDb::kDxfLayerName);
		rb->restype=AcDb::kDxfLayerName;
		rb->resval.rstring = (char*) malloc(nlen*sizeof(char));
		strcpy(rb->resval.rstring, strXMFWX);
		rb->rbnext=NULL;
		acedSSGet("X",NULL,NULL,rb,ents);
		long entNums=0;
		acedSSLength(ents,&entNums);

		for (int j=0; j < entNums; j ++)
		{
			AcDbObjectId  objId;
			ads_name ent;
			acedSSName(ents, j, ent);
			acdbGetObjectId(objId, ent);

			char strXDataName[1024]={0};
			sprintf(strXDataName,"DIST_%s", xxContentTree.strSdeLyName);
			CDistXData tempXdata(objId,strXDataName);

			//CString strXMBH;
			//tempXdata.Select("XMBH", strXMBH);
			CString strXMID; // 使用项目ID
			tempXdata.Select("XMID", strXMID);
			int nCount = tempXdata.GetRecordCount();
			tempXdata.Close();

			//strXMBH.Trim();
			strXMID.Trim();

			//if ((strXMBH.CompareNoCase(Glb_BizzInfo.strBizzCode) != 0 ) && (nCount > 0) )
			if ((strXMID.CompareNoCase(DocVars.docData().GetProjectId()) != 0 ) && (nCount > 0) )
			{
				// 记录项目ID
				arrProjctIDs.Add(strXMID);

				// 删除实体
				AcDbEntity * pEnt = NULL;
				if (ErrorStatus::eOk == acdbOpenObject(pEnt, objId, AcDb::kForWrite))
				{
					pEnt->erase();
					pEnt->close();                
				}
			}
		}

		acedSSFree(ents);
		acutRelRb(rb);

		// 添加相关项目
		// 通过cadcontainer添加
		CString strCommand = "AddRaletionProjects";
		CStringArray strParamArray;

		for (int j=0; j < arrProjctIDs.GetCount(); j ++)
		{
			CString strParam;
			strParam.Format("ProjectID%d=%s", j, arrProjctIDs.GetAt(j));
			strParamArray.Add(strParam);
		}

		SendCopyDataMessage(Glb_hDapForm, strCommand, strParamArray);

		Glb_Filter.ptArray.setLogicalLength(0);

		Glb_Filter.ptArray.append(AcGePoint3d(ptLB[0], ptLB[1], ptLB[2]));
		Glb_Filter.ptArray.append(AcGePoint3d(ptRT[0], ptRT[1], ptRT[2]));
	}

	static void AddRaletionProjects(LPCTSTR lpSdeLayerName, LPCTSTR lpFieldName, DIST_STRUCT_SELECTWAY filter)
	{
		//写SQL查询CAD和SDE图层对照表
		char strSQL[200] = {0};
		sprintf(strSQL,"F:OBJECTID,%s,T:%s,##", lpFieldName, lpSdeLayerName);

		CStringArray arrProjctIDs;
		Glb_pExchangeData->GetSearchFieldArray(Glb_pConnection, strSQL, lpSdeLayerName, "shape", &filter, arrProjctIDs);

		// 添加相关项目
		// 通过cadcontainer添加
		CString strCommand = "AddRaletionProjects";
		CStringArray strParamArray;

		for (int j=0; j < arrProjctIDs.GetCount(); j++)
		{
			CString strProjectId = arrProjctIDs.GetAt(j);
			if (strProjectId.IsEmpty() || atoi(strProjectId) <= 0 || (strProjectId.CompareNoCase(DocVars.docData().GetProjectId()) == 0))
				continue;

			CString strParam;
			strParam.Format("ProjectID%d=%s", j, arrProjctIDs.GetAt(j));
			strParamArray.Add(strParam);
		}

		SendCopyDataMessage(Glb_hDapForm, strCommand, strParamArray);

		return;
/*		CStringArray arrProjctIDs;

		CString strWhere;
		strWhere.Format("XMID!=%s", DocVars.docData().GetProjectId()); // 使用项目ID
		long lRt = ReadSdeLayer(Glb_pConnection, Glb_pExchangeData, lpSdeLayerName, NULL, strWhere, TRUE, FALSE);

		int nlen = strlen(strXMFWX)+1;

		ads_name ents;
		struct resbuf *rb;
		rb=acutNewRb(AcDb::kDxfLayerName);
		rb->restype=AcDb::kDxfLayerName;
		rb->resval.rstring = (char*) malloc(nlen*sizeof(char));
		strcpy(rb->resval.rstring, strXMFWX);
		rb->rbnext=NULL;
		acedSSGet("X",NULL,NULL,rb,ents);
		long entNums=0;
		acedSSLength(ents,&entNums);

		for (int j=0; j < entNums; j ++)
		{
			AcDbObjectId  objId;
			ads_name ent;
			acedSSName(ents, j, ent);
			acdbGetObjectId(objId, ent);

			char strXDataName[1024]={0};
			sprintf(strXDataName,"DIST_%s", lpSdeLayerName);
			CDistXData tempXdata(objId,strXDataName);

			//CString strXMBH;
			//tempXdata.Select("XMBH", strXMBH);
			CString strXMID; // 使用项目ID
			tempXdata.Select("XMID", strXMID);
			int nCount = tempXdata.GetRecordCount();
			tempXdata.Close();

			//strXMBH.Trim();
			strXMID.Trim();

			//if ((strXMBH.CompareNoCase(Glb_BizzInfo.strBizzCode) != 0 ) && (nCount > 0) )
			if ((!strXMID.IsEmpty()) && (strXMID.CompareNoCase(DocVars.docData().GetProjectId()) != 0 ) && (nCount > 0) )
			{
				// 记录项目ID
				arrProjctIDs.Add(strXMID);

				// 删除实体
				AcDbEntity * pEnt = NULL;
				if (ErrorStatus::eOk == acdbOpenObject(pEnt, objId, AcDb::kForWrite))
				{
					pEnt->erase();
					pEnt->close();                
				}
			}
		}

		acedSSFree(ents);
		acutRelRb(rb);

		// 添加相关项目
		// 通过cadcontainer添加
		CString strCommand = "AddRaletionProjects";
		CStringArray strParamArray;

		for (int j=0; j < arrProjctIDs.GetCount(); j ++)
		{
			CString strParam;
			strParam.Format("ProjectID%d=%s", j, arrProjctIDs.GetAt(j));
			strParamArray.Add(strParam);
		}

		SendCopyDataMessage(Glb_hDapForm, strCommand, strParamArray);
*/	}

	//画项目红线
	// ----- DistCommands._DrawXMHX command (do not rename)
	static void DistCommands_DrawXMHX(void)
	{
		//long lRt = 0;
		//IDistConnection* pConnection = Glb_pConnection;
		//IDistExchangeData* pExchangeData = Glb_pExchangeData;

		//STB_CONTENT_TREE xxContentTree = Glb_mapTree[strXMHX];

		//if (xxContentTree.strSdeLyName.IsEmpty())
		//{
		//	AfxMessageBox("未找到项目红线对应的SDE图层！", MB_OK);
		//	return;
		//}

		//CString strAppNme = "DIST_";
		//strAppNme += xxContentTree.strSdeLyName;
		//lRt = pExchangeData->ReadSymbolInfo(pConnection, NULL, NULL, xxContentTree.strSdeLyName, strAppNme, NULL);
		//if (lRt != 1)
		//{
		//	acutPrintf("\n%s",ReturnErrorCodeInfo(lRt)); return;
		//}

		AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();
		//AcDbLayerTable * pLyTable = NULL;

		//ErrorStatus es = pDB->getSymbolTable(pLyTable, AcDb::kForRead);
		//if (es != ErrorStatus::eOk)
		//{
		//	acutPrintf("\n获取LayerTable失败，请稍后再试。\n");
		//	return;
		//}

		//if (!pLyTable->has(strXMHX))
		//{
		//	acutPrintf("\n没找到需要定位的红线图层 %s ,请检查配置后重试。\n", strXMHX);
		//	pLyTable->close();
		//	return;
		//}

		//pLyTable->close();


		CString strProjectId = DocVars.docData().GetProjectId();

		if (strProjectId.IsEmpty())
		{
			AfxMessageBox("当前文档不支持该操作");
			return;
		}

		CDistEntityTool tempTool;
		if (tempTool.SetCurLayer(strXMHX, pDB))
		{
			//acutPrintf("\n当前图层切换为 %s\n", strXMHX);
		}
		else
		{
			//acutPrintf("\n未能找到图层 %s\n", strXMHX);

			CString strWhere;
			strWhere.Format("XMID=%s", strProjectId); // 使用项目ID

			ShowLayer(strXMHX, true, strWhere);

			//清除图层上所有实体
//			tempTool.DeleteAllEntityInLayer(strXMHX.GetBuffer(), acdbCurDwg());
//			strXMHX.ReleaseBuffer();
		}

//		gCmdCreateBound(strXMHX, "项目红线");

		AcDbObjectIdArray objIds;

		if ((!gCmdCreatePolyLine(strXMHX, "项目红线", objIds, FALSE, TRUE)) ||
			(objIds.logicalLength() <= 0))
			return;

		STB_CONTENT_TREE xxContentTree = Glb_mapTree[strXMHX];

		if (!SaveXMHX(xxContentTree.strNodeName, xxContentTree.strSdeLyName, objIds))
			return;
	}

	//画道路红线
	// ----- DistCommands._DrawDLHX command (do not rename)
	static void DistCommands_DrawDLHX(void)
	{
		// 开始编辑
		Sys_StartEdit();

		AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();

		IDistConnection* pConnection = Glb_pConnection;
		IDistExchangeData* pExchangeData = Glb_pExchangeData;
		CString strLayer = strDLHX;
			
		if ((DocVars.docData().GetProjectId().CompareNoCase("0") != 0))
		{
			strLayer += "_审批";
		}

		CDistEntityTool tempTool;
		if (tempTool.SetCurLayer(strLayer, pDB))
		{
			//acutPrintf("\n当前图层切换为 %s\n", strDLHX);
		}
		else
		{
			//acutPrintf("\n未能找到图层 %s\n", strDLHX);

			int nColorIndex;
			CString strLineType;
			AcDb::LineWeight dLineWeight = AcDb::kLnWtByLwDefault;

			pExchangeData->ReadCadLayerInfo(pConnection, strDLHX, nColorIndex, strLineType, dLineWeight);
			AcDbObjectId tempId;
			tempTool.CreateLayer(tempId, strLayer, nColorIndex, strLineType); 
		}

//		gCmdCreateBound(strDLHX, "道路红线");
		AcDbObjectIdArray objIds;
		if (!gCmdCreatePolyLine(strLayer, "道路红线", objIds, FALSE) ||
			(objIds.logicalLength() <= 0))
			return;


		STB_CONTENT_TREE xxContentTree = Glb_mapTree[strLayer];

		if (!SaveXMHX(xxContentTree.strNodeName, xxContentTree.strSdeLyName, objIds))
			return;
	}

	//画城市蓝线
	// ----- DistCommands._DrawCSLX command (do not rename)
	static void DistCommands_DrawCSLX(void)
	{
		// 开始编辑
		Sys_StartEdit();

		AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();

		IDistConnection* pConnection = Glb_pConnection;
		IDistExchangeData* pExchangeData = Glb_pExchangeData;
		CString strLayer = strCSLX;

		if ((DocVars.docData().GetProjectId().CompareNoCase("0") != 0))
		{
			strLayer += "_审批";
		}

		CDistEntityTool tempTool;
		if (tempTool.SetCurLayer(strLayer, pDB))
		{
			//acutPrintf("\n当前图层切换为 %s\n", strCSLX);
		}
		else
		{
			//acutPrintf("\n未能找到图层 %s\n", strCSLX);

			int nColorIndex;
			CString strLineType;
			AcDb::LineWeight dLineWeight = AcDb::kLnWtByLwDefault;

			pExchangeData->ReadCadLayerInfo(pConnection, strCSLX, nColorIndex, strLineType, dLineWeight);
			AcDbObjectId tempId;
			tempTool.CreateLayer(tempId, strLayer, nColorIndex, strLineType); 
		}

//		gCmdCreateBound(strCSLX, "城市蓝线");
		AcDbObjectIdArray objIds;
		if (!gCmdCreatePolyLine(strLayer, "城市蓝线", objIds, FALSE) ||
			(objIds.logicalLength() <= 0))
			return;

		STB_CONTENT_TREE xxContentTree = Glb_mapTree[strLayer];

		if (!SaveXMHX(xxContentTree.strNodeName, xxContentTree.strSdeLyName, objIds))
			return;
	}

	//画城市黄线
	// ----- DistCommands._DrawCSHX command (do not rename)
	static void DistCommands_DrawCSHX(void)
	{
		// 开始编辑
		Sys_StartEdit();

		AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();

		IDistConnection* pConnection = Glb_pConnection;
		IDistExchangeData* pExchangeData = Glb_pExchangeData;
		CString strLayer = strCSHX;

		if ((DocVars.docData().GetProjectId().CompareNoCase("0") != 0))
		{
			strLayer += "_审批";
		}

		CDistEntityTool tempTool;
		if (tempTool.SetCurLayer(strLayer, pDB))
		{
			//acutPrintf("\n当前图层切换为 %s\n", strCSHX);
		}
		else
		{
			//acutPrintf("\n未能找到图层 %s\n", strCSHX);

			int nColorIndex;
			CString strLineType;
			AcDb::LineWeight dLineWeight = AcDb::kLnWtByLwDefault;

			pExchangeData->ReadCadLayerInfo(pConnection, strCSHX, nColorIndex, strLineType, dLineWeight);
			AcDbObjectId tempId;
			tempTool.CreateLayer(tempId, strLayer, nColorIndex, strLineType); 
		}

//		gCmdCreateBound(strCSHX, "城市黄线");
		AcDbObjectIdArray objIds;
		if (!gCmdCreatePolyLine(strLayer, "城市黄线", objIds, FALSE) ||
			(objIds.logicalLength() <= 0))
			return;

		STB_CONTENT_TREE xxContentTree = Glb_mapTree[strLayer];

		if (!SaveXMHX(xxContentTree.strNodeName, xxContentTree.strSdeLyName, objIds))
			return;
	}

	//画城市绿线
	// ----- DistCommands._DrawCSLVX command (do not rename)
	static void DistCommands_DrawCSLVX(void)
	{
		// 开始编辑
		Sys_StartEdit();

		AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();

		IDistConnection* pConnection = Glb_pConnection;
		IDistExchangeData* pExchangeData = Glb_pExchangeData;
		CString strLayer = strCSLVX;

		if ((DocVars.docData().GetProjectId().CompareNoCase("0") != 0))
		{
			strLayer += "_审批";
		}

		CDistEntityTool tempTool;
		if (tempTool.SetCurLayer(strLayer, pDB))
		{
			//acutPrintf("\n当前图层切换为 %s\n", strCSLVX);
		}
		else
		{
			//acutPrintf("\n未能找到图层 %s\n", strCSLVX);

			int nColorIndex;
			CString strLineType;
			AcDb::LineWeight dLineWeight = AcDb::kLnWtByLwDefault;

			pExchangeData->ReadCadLayerInfo(pConnection, strCSLVX, nColorIndex, strLineType, dLineWeight);
			AcDbObjectId tempId;
			tempTool.CreateLayer(tempId, strLayer, nColorIndex, strLineType); 
		}

//		gCmdCreateBound(strCSLVX, "城市绿线");
		AcDbObjectIdArray objIds;
		if (!gCmdCreatePolyLine(strLayer, "城市绿线", objIds, FALSE) ||
			(objIds.logicalLength() <= 0))
			return;

		STB_CONTENT_TREE xxContentTree = Glb_mapTree[strLayer];

		if (!SaveXMHX(xxContentTree.strNodeName, xxContentTree.strSdeLyName, objIds))
			return;
	}

	//画城市紫线
	// ----- DistCommands._DrawCSZX command (do not rename)
	static void DistCommands_DrawCSZX(void)
	{
		// 开始编辑
		Sys_StartEdit();

		AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();

		IDistConnection* pConnection = Glb_pConnection;
		IDistExchangeData* pExchangeData = Glb_pExchangeData;
		CString strLayer = strCSZX;

		if ((DocVars.docData().GetProjectId().CompareNoCase("0") != 0))
		{
			strLayer += "_审批";
		}

		CDistEntityTool tempTool;
		if (tempTool.SetCurLayer(strLayer, pDB))
		{
			//acutPrintf("\n当前图层切换为 %s\n", strCSZX);
		}
		else
		{
			//acutPrintf("\n未能找到图层 %s\n", strCSZX);

			int nColorIndex;
			CString strLineType;
			AcDb::LineWeight dLineWeight = AcDb::kLnWtByLwDefault;

			pExchangeData->ReadCadLayerInfo(pConnection, strCSZX, nColorIndex, strLineType, dLineWeight);
			AcDbObjectId tempId;
			tempTool.CreateLayer(tempId, strLayer, nColorIndex, strLineType); 
		}

//		gCmdCreateBound(strCSZX, "城市紫线");
		AcDbObjectIdArray objIds;
		if (!gCmdCreatePolyLine(strLayer, "城市紫线", objIds, FALSE) ||
			(objIds.logicalLength() <= 0))
			return;

		STB_CONTENT_TREE xxContentTree = Glb_mapTree[strLayer];

		if (!SaveXMHX(xxContentTree.strNodeName, xxContentTree.strSdeLyName, objIds))
			return;
	}

	//画城市橙线
	// ----- DistCommands._DrawCSCX command (do not rename)
	static void DistCommands_DrawCSCX(void)
	{
		// 开始编辑
		Sys_StartEdit();

		AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();

		IDistConnection* pConnection = Glb_pConnection;
		IDistExchangeData* pExchangeData = Glb_pExchangeData;
		CString strLayer = strCSCX;

		if ((DocVars.docData().GetProjectId().CompareNoCase("0") != 0))
		{
			strLayer += "_审批";
		}

		CDistEntityTool tempTool;
		if (tempTool.SetCurLayer(strLayer, pDB))
		{
			//acutPrintf("\n当前图层切换为 %s\n", strCSCX);
		}
		else
		{
			//acutPrintf("\n未能找到图层 %s\n", strCSCX);
			int nColorIndex;
			CString strLineType;
			AcDb::LineWeight dLineWeight = AcDb::kLnWtByLwDefault;

			pExchangeData->ReadCadLayerInfo(pConnection, strCSCX, nColorIndex, strLineType, dLineWeight);
			AcDbObjectId tempId;
			tempTool.CreateLayer(tempId, strLayer, nColorIndex, strLineType); 
		}

//		gCmdCreateBound(strCSCX, "城市橙线");
		AcDbObjectIdArray objIds;
		if (!gCmdCreatePolyLine(strLayer, "城市橙线", objIds, FALSE) ||
			(objIds.logicalLength() <= 0))
			return;

		STB_CONTENT_TREE xxContentTree = Glb_mapTree[strLayer];

		if (!SaveXMHX(xxContentTree.strNodeName, xxContentTree.strSdeLyName, objIds))
			return;
	}

	// ----- DistCommands._DrawDLZXX command (do not rename)
	static void DistCommands_DrawDLZXX(void)
	{
		// 开始编辑
		Sys_StartEdit();

		AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();

		IDistConnection* pConnection = Glb_pConnection;
		IDistExchangeData* pExchangeData = Glb_pExchangeData;
		CString strLayer = strDLZXX;

		if ((DocVars.docData().GetProjectId().CompareNoCase("0") != 0))
		{
			strLayer += "_审批";
		}

		CDistEntityTool tempTool;
		if (tempTool.SetCurLayer(strLayer, pDB))
		{
			//acutPrintf("\n当前图层切换为 %s\n", strCSCX);
		}
		else
		{
			//acutPrintf("\n未能找到图层 %s\n", strCSCX);
			int nColorIndex;
			CString strLineType;
			AcDb::LineWeight dLineWeight = AcDb::kLnWtByLwDefault;

			pExchangeData->ReadCadLayerInfo(pConnection, strCSCX, nColorIndex, strLineType, dLineWeight);
			AcDbObjectId tempId;
			tempTool.CreateLayer(tempId, strLayer, nColorIndex, strLineType); 
		}

		//		gCmdCreateBound(strCSCX, "道路中心线");
		AcDbObjectIdArray objIds;
		if (!gCmdCreatePolyLine(strLayer, "道路中心线", objIds, FALSE) ||
			(objIds.logicalLength() <= 0))
			return;

		STB_CONTENT_TREE xxContentTree = Glb_mapTree[strLayer];

		if (!SaveXMHX(xxContentTree.strNodeName, xxContentTree.strSdeLyName, objIds))
			return;
	}

	// ----- DistCommands._DrawHDZXX command (do not rename)
	static void DistCommands_DrawHDZXX(void)
	{
		// 开始编辑
		Sys_StartEdit();

		AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();

		IDistConnection* pConnection = Glb_pConnection;
		IDistExchangeData* pExchangeData = Glb_pExchangeData;
		CString strLayer = strHDZXX;

		if ((DocVars.docData().GetProjectId().CompareNoCase("0") != 0))
		{
			strLayer += "_审批";
		}

		CDistEntityTool tempTool;
		if (tempTool.SetCurLayer(strLayer, pDB))
		{
			//acutPrintf("\n当前图层切换为 %s\n", strCSCX);
		}
		else
		{
			//acutPrintf("\n未能找到图层 %s\n", strCSCX);
			int nColorIndex;
			CString strLineType;
			AcDb::LineWeight dLineWeight = AcDb::kLnWtByLwDefault;

			pExchangeData->ReadCadLayerInfo(pConnection, strCSCX, nColorIndex, strLineType, dLineWeight);
			AcDbObjectId tempId;
			tempTool.CreateLayer(tempId, strLayer, nColorIndex, strLineType); 
		}

		//		gCmdCreateBound(strCSCX, "河道中心线");
		AcDbObjectIdArray objIds;
		if (!gCmdCreatePolyLine(strLayer, "河道中心线", objIds, FALSE) ||
			(objIds.logicalLength() <= 0))
			return;

		STB_CONTENT_TREE xxContentTree = Glb_mapTree[strLayer];

		if (!SaveXMHX(xxContentTree.strNodeName, xxContentTree.strSdeLyName, objIds))
			return;
	}

	//查询相关项目
	// ----- DistCommands._DistSelProject command (do not rename)
	static void DistCommands_DistSelProject(void)
	{

	}

	//查询相关项目
	// ----- DistCommands._LoadMenu command (do not rename)
	static void DistCommands_LoadMenu(void)
	{
		Sys_LoadDistMenu("规划用图");
	}

	//查询相关项目
	// ----- DistCommands._LoadMenu2 command (do not rename)
	static void DistCommands_LoadMenu2(void)
	{
		Sys_LoadDistMenu("六线调整");
	}

	//查询相关项目
	// ----- DistCommands._UnLoadMenu command (do not rename)
	static void DistCommands_UnLoadMenu(void)
	{
		Sys_UnLoadDistMenu("规划用图");
	}
	// ----- DistCommands._UnLoadMenu2 command (do not rename)
	static void DistCommands_UnLoadMenu2(void)
	{
		Sys_UnLoadDistMenu("六线调整");
	}
	// ----- DistCommands._ZoomExtents command (do not rename)
	static void DistCommands_ZoomExtents(void)
	{
		fZoomExtents();
	}

	static void DistCommands_SaveLX(void)
	{
		IDistConnection* pConnection = Glb_pConnection;
		IDistExchangeData* pExchangeData = Glb_pExchangeData;

		CString strPrompt = "\n选择类型 (1)修改六线 (2)删除六线 ";
		CDistUiPrIntDef prIntType(strPrompt,NULL,CDistUiPrInt::kRange,1); 
		prIntType.SetRange(1,2);
		if(prIntType.Go()!=CDistUiPrBase::kOk)
		{
			return;
		}

		int nType = prIntType.GetValue();

		const resbuf* filter = NULL;
		ads_name ss;
		int result = acedSSGet("_", NULL, NULL, filter, ss);

		if (RTNORM == result)
		{
			AcDbObjectIdArray objIds;

			long sslen;
			if (acedSSLength(ss, &sslen) != RTNORM) {
				acutPrintf(_T("\nInvalid selection set."));
				sslen = 0;
			}

			int ret;
			AcDbObjectId objId;
			ads_name ent;

			for (long i=0L; i<sslen; i++) 
			{
				ret = acedSSName(ss, i, ent);
				if (ret != RTNORM)
				{
					acutPrintf(_T("\nInvalid selection set."));
					break;
				}

				Acad::ErrorStatus es = acdbGetObjectId(objId, ent);
				if (es != Acad::eOk)
				{
					acedSSFree(ent);
					continue;
				}
				objIds.append(objId);
//				acutPrintf("\nObjectId=%d", objId.asOldId());

				acedSSFree(ent);
			}

			acedSSFree(ss);

			CString layers[] = 
			{
				"道路红线","道路中心线","道路设施线","道路注记","铁路中心线","铁路防护线","铁路设施线","铁路注记","红线辅助线",

				"河道蓝线","河道中心线","河道防护线","蓝线辅助线","河道注记",

				"城市黄线","黄线辅助线","黄线注记",

				"城市紫线","紫线辅助线","紫线注记",

				"城市绿线","绿线辅助线","绿线注记",

				"城市橙线","橙线辅助线","橙线注记",
			};

			int nCount = sizeof(layers)/sizeof(CString);

			CString strCadLayer, strSdeLayer;

			for (int i=0; i<nCount; i++)
			{
				AcDbObjectIdArray tempObjIds;

				strCadLayer = layers[i];
				STB_CONTENT_TREE xxContentTree = Glb_mapTree[strCadLayer];
				strSdeLayer = xxContentTree.strSdeLyName;
				if (strSdeLayer.IsEmpty())
				{
					continue;
				}

				for (int j=0; j<objIds.logicalLength(); j++)
				{
					AcDbObjectId tempObjId = objIds.at(j);

					AcDbEntity * pEnt = NULL;
					if (ErrorStatus::eOk == acdbOpenObject(pEnt, objId, AcDb::kForRead))
					{
						if (strCadLayer.CompareNoCase(pEnt->layer()) == 0)
						{
							tempObjIds.append(tempObjId);
						}
						pEnt->close();            
					}
				}

				if (tempObjIds.logicalLength() > 0)
				{
					if (nType == 1)
					{
						CMapStringToString ObjectIdMaps;
						for (int k=0; k<tempObjIds.logicalLength(); k++)
						{
							AcDbObjectId tempObjId = tempObjIds.at(k);

							CDistXData tempXData(tempObjId, "ROWID_OBJS");
							CString strObjectId;
							if(tempXData.GetRecordCount()>0)  //扩展属性存在，即修改
							{
								tempXData.Select("OBJECTID", strObjectId);
								if (ObjectIdMaps[strObjectId] == "1") // 如果已经存在（对于线段打断会出现这种情况）
								{
									tempXData.DeleteAllRecord();
									tempXData.Close();
								}
								else
								{
									ObjectIdMaps[strObjectId] = "1";
									tempXData.Close();
								}
							}
						}

						SaveSdeLayer(pConnection, pExchangeData, strSdeLayer, tempObjIds, false);
					}
					else
						DeleSdeData(pConnection, pExchangeData, strSdeLayer, tempObjIds, true);
				}
			}
		}
		else
		{
			acedSSFree(ss);
			return;
		}
	}

	// ----- DistCommands.StartEdit command (do not rename)
	static void DistCommandsStartEdit(void) // 开始编辑
	{
		// Add your code for command DistCommands.StartEdit here
		dataContainer.m_mapNewData.RemoveAll();
		dataContainer.m_mapAttrEditData.RemoveAll();
		dataContainer.m_mapShapeEditData.RemoveAll();
		dataContainer.m_mapDelData.RemoveAll();

		AcApDocument *pDoc = acDocManager->curDocument();
		acDocManager->lockDocument(pDoc);

		//启动反应器 
		dbReactor = new SHDatabaseReactor();
		acdbHostApplicationServices()->workingDatabase()->addReactor(dbReactor);
		acDocManager->unlockDocument(pDoc);
	}

	static void printxxx(CString strType, CMapCADIdToAttrData& m_mapData)
	{
		CAttrDataClass rAttrData;
		long rCADId = -1;

		POSITION pos = m_mapData.GetStartPosition();

		while(pos)
		{
			m_mapData.GetNextAssoc(pos,rCADId,rAttrData);

			CString strErr;

			acutPrintf("\n%s::%d\n", strType, rCADId);
		}
	}
	// ----- DistCommands.EndEdit command (do not rename)
	static void DistCommandsEndEdit(void) // 结束编辑
	{
		CDlgEditDesc dlg;
		dlg.DoModal();

		int nEditRowId = dlg.m_nEditRowId;

		if (nEditRowId < 0)
		{
			acdbHostApplicationServices()->workingDatabase()->removeReactor(dbReactor);
			dataContainer.m_mapNewData.RemoveAll();
			dataContainer.m_mapAttrEditData.RemoveAll();
			dataContainer.m_mapShapeEditData.RemoveAll();
			dataContainer.m_mapDelData.RemoveAll();
			return;
		}

		LockDocument lock;

		// Add your code for command DistCommands.EndEdit here
		printxxx("Append", dataContainer.m_mapNewData);
		printxxx("Modify", dataContainer.m_mapAttrEditData);
		printxxx("Modify", dataContainer.m_mapShapeEditData);
		printxxx("Delete", dataContainer.m_mapDelData);

		IDistConnection* pConnection = Glb_pConnection;
		IDistExchangeData* pExchangeData = Glb_pExchangeData;

		acdbHostApplicationServices()->workingDatabase()->removeReactor(dbReactor);

		CAttrDataClass rAttrData;
		long rCADId = -1;

		// 新增对象，需要清空扩展属性（线段打断或者复制的情况下扩展属性是重的）
		POSITION pos = dataContainer.m_mapNewData.GetStartPosition();

		while(pos)
		{
			dataContainer.m_mapNewData.GetNextAssoc(pos,rCADId,rAttrData);

			AcDbObjectId tmpObjId;

			tmpObjId.setFromOldId(rCADId);

			CDistXData tempXData(tmpObjId, "ROWID_OBJS");
			CString strObjectId;
			if(tempXData.GetRecordCount()>0)  // 删除扩展属性
			{
				tempXData.DeleteAllRecord();
				tempXData.Close();
			}
			tempXData.Close();
		}

		CString layers[] = 
		{
			"道路红线","道路中心线","道路设施线","道路注记","铁路中心线","铁路防护线","铁路设施线","铁路注记","红线辅助线",

			"河道蓝线","河道中心线","河道防护线","蓝线辅助线","河道注记",

			"城市黄线","黄线辅助线","黄线注记",

			"城市紫线","紫线辅助线","紫线注记",

			"城市绿线","绿线辅助线","绿线注记",

			"城市橙线","橙线辅助线","橙线注记",
		};

		int nCount = sizeof(layers)/sizeof(CString);

		CString strCadLayer, strSdeLayer;

		for (int i=0; i<nCount; i++)
		{
			AcDbObjectIdArray newObjIds;
			AcDbObjectIdArray modifyObjIds;

			AcDbObjectIdArray objIds;
			CUIntArray deleteRowIds;
			CUIntArray historyRowIds;

			strCadLayer = layers[i];
			STB_CONTENT_TREE xxContentTree = Glb_mapTree[strCadLayer];
			strSdeLayer = xxContentTree.strSdeLyName;
			if (strSdeLayer.IsEmpty())
			{
				continue;
			}

			// 新增
			POSITION pos = dataContainer.m_mapNewData.GetStartPosition();
			while(pos)
			{
				dataContainer.m_mapNewData.GetNextAssoc(pos,rCADId,rAttrData);

				AcDbObjectId tmpObjId;

				tmpObjId.setFromOldId(rCADId);

				AcDbEntity * pEnt = NULL;
				if (ErrorStatus::eOk == acdbOpenObject(pEnt, tmpObjId, AcDb::kForRead))
				{
					if (strCadLayer.CompareNoCase(pEnt->layer()) == 0)
					{
						newObjIds.append(tmpObjId);
						//objIds.append(tmpObjId);
						dataContainer.m_mapNewData.RemoveKey(rCADId);
					}
					pEnt->close();            
				}
			}

			// 修改
			pos = dataContainer.m_mapShapeEditData.GetStartPosition();
			while(pos)
			{
				dataContainer.m_mapShapeEditData.GetNextAssoc(pos,rCADId,rAttrData);

				AcDbObjectId tmpObjId;

				tmpObjId.setFromOldId(rCADId);

				AcDbEntity * pEnt = NULL;
				if (ErrorStatus::eOk == acdbOpenObject(pEnt, tmpObjId, AcDb::kForRead))
				{
					if (strCadLayer.CompareNoCase(pEnt->layer()) == 0)
					{
						modifyObjIds.append(tmpObjId);
						historyRowIds.Add(rAttrData.m_fid);
						//objIds.append(tmpObjId);
						dataContainer.m_mapShapeEditData.RemoveKey(rCADId);
					}
					pEnt->close();            
				}
			}

			// 删除
			pos = dataContainer.m_mapDelData.GetStartPosition();
			while(pos)
			{
				dataContainer.m_mapDelData.GetNextAssoc(pos,rCADId,rAttrData);

				AcDbObjectId tmpObjId;

				tmpObjId.setFromOldId(rCADId);

				if (strCadLayer.CompareNoCase(rAttrData.m_strCADLayer) == 0)
				{
					deleteRowIds.Add(rAttrData.m_fid);
					historyRowIds.Add(rAttrData.m_fid);
					dataContainer.m_mapDelData.RemoveKey(rCADId);
				}
			}

			if (historyRowIds.GetCount() > 0)
			{
				// 保存修改历史
				CString strHistoryLayer = strSdeLayer + "_LS";
				CopyRecordsToHistory(pConnection, historyRowIds, strSdeLayer, strHistoryLayer, Glb_BizzInfo.strLoginUserName, nEditRowId);
			}

			if (newObjIds.logicalLength() > 0) // 新增
			{
				SaveSdeLayerInsert(pConnection, pExchangeData, strSdeLayer, newObjIds, false);
			}
			if (modifyObjIds.logicalLength() > 0) // 修改
			{
				SaveSdeLayerModify(pConnection, pExchangeData, strSdeLayer, modifyObjIds, false);
			}
			if (deleteRowIds.GetCount() > 0) // 删除
			{
				IDistCommand* pCmd = CreateSDECommandObjcet();
				pCmd->SetConnectObj(pConnection);

				for (int i=0; i<deleteRowIds.GetCount(); i++) 
				{
					int nRowId = deleteRowIds.GetAt(i);

					if (nRowId <=0) continue;

					TCHAR strSQL[128] = {0};
					sprintf(strSQL,"T:%s,W:%s=%d,##",strSdeLayer,"OBJECTID",nRowId);
					pCmd->DeleteData(strSQL);
				}
			}
		}

		dataContainer.m_mapNewData.RemoveAll();
		dataContainer.m_mapAttrEditData.RemoveAll();
		dataContainer.m_mapShapeEditData.RemoveAll();
		dataContainer.m_mapDelData.RemoveAll();
	}

	// ----- DistCommands.CancelEdit command (do not rename)
	static void DistCommandsCancelEdit(void) // 取消编辑
	{
		acdbHostApplicationServices()->workingDatabase()->removeReactor(dbReactor);

		dataContainer.m_mapNewData.RemoveAll();
		dataContainer.m_mapAttrEditData.RemoveAll();
		dataContainer.m_mapShapeEditData.RemoveAll();
		dataContainer.m_mapDelData.RemoveAll();
	}

	// ----- DistCommands._LoadHistoryLayer command (do not rename)
	static void DistCommands_LoadHistoryLayer(void)
	{
		// Add your code for command DistCommands._LoadHistoryLayer here
		CDlgHistory dlg;
		dlg.DoModal();
	}

	static void SSToIds(ads_name ssName,AcDbObjectIdArray& ObjIds)
	{
		AcDbObjectId ObjId;
		ads_name EntName;
		long nLength=0;
		acedSSLength(ssName,&nLength);

		for(int nLen=0; nLen<nLength; nLen++)
		{
			acedSSName(ssName,nLen,EntName);
			acdbGetObjectId(ObjId,EntName);
			ObjIds.append(ObjId);
		}
	}

	// ----- DistCommands._ViewLog command (do not rename)
	static void DistCommands_ViewLog(void)
	{
		// Add your code for command DistCommands._ViewLog here
		AcApDocument * pDoc = curDoc();

		if (NULL == pDoc)
			return;

		//锁定文档
		LockDocument lock; 

		AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();

		if (NULL == pDB)
			return;

		// 暂时不支持多选
		AcDbObjectIdArray EntIds;

		ads_name ssName;
		if(acedSSGet(NULL,NULL,NULL,NULL,ssName)!=RTNORM)
			return;

		SSToIds(ssName,EntIds);
		acedSSFree(ssName);

		// 根据选择实体获取图层名列表
		if (EntIds.logicalLength() > 1)
		{
			AfxMessageBox("不支持多选。");
			return;
		}

		CDlgViewLog dlg;
		dlg.m_ObjId = EntIds[0];
		dlg.DoModal();
	}

	// ----- DistCommands._debug command (do not rename)
	static void DistCommands_debug(void)
	{
		//test store
		//CJn_CommandPower cmdpwr;
		//CString strLocation = "c:\\cmdPower.cfg";
		//CFile myFile(strLocation,CFile::modeCreate | CFile::modeWrite);
		//CArchive ar(&myFile,CArchive::store);
		//cmdpwr.Serialize(ar);
		//ar.Close();
		//myFile.Close();
		
		////test read
		//CFile myFile(strLocation,CFile::modeRead);
		//CArchive ar(&myFile,CArchive::load);
		//CJn_CommandPower cmdpwr;
		//cmdpwr.Serialize(ar);
		//ar.Close();
		//myFile.Close();
		//
		// Add your code for command DistCommands._debug here

		Glb_SdeInfo.strServer.ReleaseBuffer();
		Glb_SdeInfo.strService.ReleaseBuffer();
		Glb_SdeInfo.strDatabase.ReleaseBuffer();
		Glb_SdeInfo.strUserName.ReleaseBuffer();
		Glb_SdeInfo.strPassword.ReleaseBuffer();

		Glb_LocalSdeInfo.strServer.ReleaseBuffer();
		Glb_LocalSdeInfo.strService.ReleaseBuffer();
		Glb_LocalSdeInfo.strDatabase.ReleaseBuffer();
		Glb_LocalSdeInfo.strUserName.ReleaseBuffer();
		Glb_LocalSdeInfo.strPassword.ReleaseBuffer();

		Glb_BizzInfo.strLoginUserName = "开发管理员";
		Glb_BizzInfo.strUserId = "621";
		Glb_BizzInfo.strBizzCode = "项目编号";
		Glb_BizzInfo.strBizzName = "项目名称";
		Glb_BizzInfo.strBizzName = "建设单位";

/*
		Glb_BizzInfo.strBizzCode = parameters["bi_code"];
		Glb_BizzInfo.strBizzSame = parameters["bi_same"];
		Glb_BizzInfo.strBizzName = parameters["bi_name"];
		Glb_BizzInfo.strBzOwner = parameters["bi_owner"];
		Glb_BizzInfo.strBzIdId = parameters["bi_BzId"];
		Glb_BizzInfo.strBzId = parameters["bi_BzName"];
		Glb_BizzInfo.strBztmOver = parameters["bi_tmOver"];
		Glb_BizzInfo.strCurTime = parameters["cur_time"];

		Glb_BizzInfo.strUserId = parameters["lg_UserId"];
		Glb_BizzInfo.strLoginUserName = parameters["lg_UserName"];

		OutputDebugStringX("bi_code=%s, bi_same=%s, bi_name=%s, lg_UserName=%s", 
			Glb_BizzInfo.strBizzCode, Glb_BizzInfo.strBizzSame, Glb_BizzInfo.strBizzName, Glb_BizzInfo.strLoginUserName);

		CString strIniFile = "BizzInfo.ini";
		WritePrivateProfileString("BizzInfo","项目编号",Glb_BizzInfo.strBizzCode,strIniFile);
		WritePrivateProfileString("BizzInfo","项目名称",Glb_BizzInfo.strBizzName,strIniFile);
		WritePrivateProfileString("BizzInfo","建设单位",Glb_BizzInfo.strBzOwner,strIniFile);
		WritePrivateProfileString("BizzInfo","业务类型",Glb_BizzInfo.strBzId,strIniFile);
		WritePrivateProfileString("BizzInfo","结束日期",Glb_BizzInfo.strBztmOver,strIniFile);
*/

		// 市局SDE连接参数
		Glb_SdeInfo.strServer = "192.168.1.23"; //parameters["server"];
		Glb_SdeInfo.strService = "5151"; //parameters["service"];
		Glb_SdeInfo.strDatabase = ""; //parameters["database"];
		Glb_SdeInfo.strUserName = "csdata"; //parameters["username"];
		Glb_SdeInfo.strPassword = "csdata"; //parameters["password"];

		// 连接SDE数据库
		CreateSdeConnection(Glb_SdeInfo.strServer.GetBuffer(), Glb_SdeInfo.strService.GetBuffer(), Glb_SdeInfo.strDatabase.GetBuffer(), Glb_SdeInfo.strUserName.GetBuffer(), Glb_SdeInfo.strPassword.GetBuffer());

		// SDE图层配置
	/*	Glb_LayerInfo.strAnnotionLine = parameters["标注线层"]; // 标注线层
		Glb_LayerInfo.strAnnotionText = parameters["标注文字层"]; // 标注文字层
		Glb_LayerInfo.strLayer = parameters["范围图层"];
		Glb_LayerInfo.strField = parameters["范围图层字段"];
		Glb_LayerInfo.strMapLayer = parameters["地形图"];
		Glb_LayerInfo.strIndexField = parameters["地形图索引字段"];

		if (Glb_LayerInfo.strIndexField.IsEmpty())
			Glb_LayerInfo.strIndexField = "图号";
*/
		// 清空图层信息
		Glb_layerArray.RemoveAll();

		CString strProjectId = "0";
		DocVars.docData().SetProjectId(strProjectId);

		if (strProjectId.CompareNoCase("0") == 0) // 六线调整时ProjectId为0
		{
			Sys_UnLoadDistMenu("规划用图");
			Sys_LoadDistMenu("六线调整");
		}
		else if (strProjectId.CompareNoCase("-1") == 0) // 电子展板
		{
			Sys_UnLoadDistMenu("六线调整");
			Sys_UnLoadDistMenu("规划用图");
		}
		else
		{
			g_bAutoReadSdeData = TRUE;
			Sys_UnLoadDistMenu("六线调整");
			Sys_LoadDistMenu("规划用图");
		}

		Glb_mapCheck.clear();

		if (strProjectId.CompareNoCase("-1") == 0) // 电子展板
		{
			return;
		}

		// 加载图层树
		LoadLayerTree();

		if (strProjectId.CompareNoCase("0") == 0) // 六线调整
		{
			Sys_ShowSampToolBar(TRUE);
			return;
		}
		else
		{
			Sys_ShowSampToolBar(FALSE);
		}		
	}
} ;

// 自定义的窗口函数
LRESULT CALLBACK DistMainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch( message )
	{
	case WM_COPYDATA:
		{
			COPYDATASTRUCT * cds = (COPYDATASTRUCT*)lParam;

			if (cds->dwData == 1) // 系统命令
			{
				break;
			}
			else if (cds->dwData == 2) // 自定义命令
			{
				CString strCommand = (LPCTSTR)cds->lpData;
				if (strCommand.CompareNoCase("开始编辑") == 0)
				{
					acutPrintf("\n开始编辑。\n");
					CDistCommandsApp::DistCommandsStartEdit();
				}
				else if (strCommand.CompareNoCase("结束编辑") == 0)
				{
					acutPrintf("\n结束编辑。\n");
					CDistCommandsApp::DistCommandsEndEdit();
				}
				else if (strCommand.CompareNoCase("取消编辑") == 0)
				{
					acutPrintf("\n取消编辑。\n");
					CDistCommandsApp::DistCommandsCancelEdit();
				}
				break;
			}
			else // 自定义消息
			{
			}

			CString strXML = (LPCTSTR)cds->lpData;

			OutputDebugString("WM_COPYDATA");

			CString strCommand;
			MapString2String parameters;

			// 分解参数
			if (!ParseParameters(strXML, strCommand, parameters))
				return 0;

			// 处理命令
			ProcessCommand(strCommand, parameters);
		}
		break;
	case WM_CLOSE:
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

			if ((Glb_hDapForm != NULL) && IsWindow(Glb_hDapForm))
			{
				CString strCommand = "CloseTab";
				CStringArray strParamArray;

				SendCopyDataMessage(Glb_hDapForm, strCommand, strParamArray);
			}
		}
	default:
		break;
	}

	return CallWindowProc( Glb_OldMainWndProc, hWnd, message, wParam, lParam ); 
}

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CDistCommandsApp)

ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, _ReadSdeLayers, ReadSdeLayers, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, _SaveSdeLayers, SaveSdeLayers, ACRX_CMD_TRANSPARENT, NULL)
//ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, _DimAnnotion, DimAnnotion, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, _ShowObjectAttr, ShowObjectAttr, ACRX_CMD_TRANSPARENT, NULL)
//ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, _LoadBaseMap, LoadBaseMap, ACRX_CMD_TRANSPARENT, NULL)
//ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, _LoadBaseMapEx, LoadBaseMapEx, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, _DimText, DimText, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, _ReadSdeLayersEx, ReadSdeLayersEx, ACRX_CMD_TRANSPARENT, NULL)
//ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, _UploadDwgFile, UploadDwgFile, ACRX_CMD_TRANSPARENT, NULL)
//ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, _ExtractPolyLine, ExtractPolyLine, ACRX_CMD_TRANSPARENT, NULL)
//ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, _AddHXAttr, AddHXAttr, ACRX_CMD_TRANSPARENT, NULL)
//ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, _LoadJpg, LoadJpg, ACRX_CMD_TRANSPARENT, NULL)
//ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, _Archive, Archive, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, _DeleteAllEntity, DeleteAllEntity, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, _DeleteInsertDwg, DeleteInsertDwg, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, _LoadLayerTree, LoadLayerTree, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, _ShowDockTreePane, ShowDockTreePane, ACRX_CMD_TRANSPARENT, NULL)
//ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, _DISTDYTK, DISTDYTK, ACRX_CMD_TRANSPARENT, NULL)
//ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, _DistBoundary, DistBoundary, ACRX_CMD_TRANSPARENT, NULL)
//ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, _ReadSdeData, ReadSdeData, ACRX_CMD_TRANSPARENT, NULL)
//ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, _gongshi, gongshi, ACRX_CMD_TRANSPARENT, NULL)
//ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, _gongshi_TK, gongshi_TK, ACRX_CMD_TRANSPARENT, NULL)
//ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, _GongShiEx, GongShiEx, ACRX_CMD_TRANSPARENT, NULL)
//ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, _RetrivePublicize, RetrivePublicize, ACRX_CMD_TRANSPARENT, NULL)
//ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, _TuKuang, TuKuang, ACRX_CMD_TRANSPARENT, NULL) // 只针对南宁项目
ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, _DeleteXData, DeleteXData, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, _SaveLayerState, SaveLayerState, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, _OnExportSdeData, OnExportSdeData, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, _DrawXMHX, DrawXMHX, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, _DrawXMFWX, DrawXMFWX, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, _DrawDLHX, DrawDLHX, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, _DrawCSLX, DrawCSLX, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, _DrawCSHX, DrawCSHX, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, _DrawCSLVX, DrawCSLVX, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, _DrawCSZX, DrawCSZX, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, _DrawCSCX, DrawCSCX, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, _DistSelProject, DistSelProject, ACRX_CMD_TRANSPARENT, NULL)
//ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, _SaveLX, SaveLX, ACRX_CMD_TRANSPARENT, NULL)

ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, _LoadMenu, LoadMenu, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, _UnLoadMenu, UnLoadMenu, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, _LoadMenu2, LoadMenu2, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, _UnLoadMenu2, UnLoadMenu2, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, _ZoomExtents, ZoomExtents, ACRX_CMD_TRANSPARENT, NULL)
//ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, StartEdit, StartEdit, ACRX_CMD_TRANSPARENT, NULL)
//ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, EndEdit, EndEdit, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, _DrawDLZXX, DrawDLZXX, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, _DrawHDZXX, DrawHDZXX, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, _LoadHistoryLayer, LoadHistoryLayer, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, _ViewLog, ViewLog, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, _debug, debug, ACRX_CMD_TRANSPARENT, NULL)
