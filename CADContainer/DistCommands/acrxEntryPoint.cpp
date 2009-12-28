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

#include "CommandDefine.h" // CAD������ձ�

#include "DistUiPrompts.h"

#include "..\SdeData\DistExchangeInterface.h"
#include "..\SdeData\DistBaseInterface.h"

#include "..\DistMenuManager\DistMenuManager_Ext.h"

#include "AcDocOperation.h"

#include "DataContainer.h"
// ���ݿⷴӦ��
#include "SHDatabaseReactor.h"
#include <set>
#include "ConfigReader.h"

extern long __cdecl acdbSetDbmod(class AcDbDatabase *,long);

//-----------------------------------------------------------------------------
#define szRDS _RXST("")

HWND Glb_hDapForm = NULL; // ������

enum DXT_MODE
{
	MODE_SDE,
	MODE_FTP
};

//DXT_MODE Glb_dxtMode = MODE_FTP;
DXT_MODE Glb_dxtMode = MODE_SDE;
BOOL Glb_bMultiConnection = FALSE;
CString Glb_strDXTPath = "��������ͼ\\";

WNDPROC Glb_OldMainWndProc = NULL; // CAD������ ���ں���

HWND Glb_hMDIClient = NULL;
WNDPROC Glb_OldMDIClientProc = NULL; // MDI�ͻ��� ���ں���

IDistConnection*    Glb_pLocalConnection = NULL;         // �־� SDE���ݿ����Ӷ���ָ��
IDistExchangeData*  Glb_pLocalExchangeData = NULL;       // �־� SDE<->CAD���ݽ�������ָ��

IDistConnection*    Glb_pConnection = NULL;         // SDE���ݿ����Ӷ���ָ��
IDistExchangeData*  Glb_pExchangeData = NULL;       // SDE<->CAD���ݽ�������ָ��


DIST_STRUCT_SELECTWAY Glb_Filter; //����ռ���������

MapString2ContentTree Glb_mapTree; // ���ṹ���ձ�

MapString2ContentTree Glb_mapHistoryTree; // ������ʷ����
CStringArray Glb_arrayHistoryLayer; // ��ʷͼ��

MapLong2Power Glb_mapPower; // ͼ��Ȩ�޶��ձ�
MapString2Power Glb_commandPower; // CAD����Ȩ�޶��ձ�
MapLong2Long Glb_mapCheck; // ͼ�㹴ѡ״̬

MapString2String Glb_mapFormItem;

POWER Glb_nFormPower; // ��������Ȩ��

CStringArray Glb_layerArray; // ��SDE�е�������ͼ���б�

ST_SDE_CONNECTION Glb_SdeInfo; // SDE������Ϣ
ST_SDE_CONNECTION Glb_LocalSdeInfo; // �־�SDE������Ϣ
//ST_FTP_CONNECTION Glb_FtpInfo; // FTP������Ϣ
//ST_FTP_CONNECTION Glb_DocFtpInfo; // ����ϵͳFTP������Ϣ

ST_BIZZINFO Glb_BizzInfo; // ҵ����Ϣ
ST_LAYERINFO Glb_LayerInfo; // ͼ����Ϣ

MapString2String Glb_BizzInfoEx;// ҵ����Ϣ

BOOL g_bAutoReadSdeData = FALSE;

//CString strAnnotionPolyLineFA = "�����߷���"; // �����߷���ͼ��
//CString strAnnotionTextFA = "�ı�ע�Ƿ���"; // �ı�ע�Ƿ���ͼ��
//CString strAnnotionPolygonFA = "�����淽��"; // �����淽��ͼ��
//CString strOtherAnnotionPolyLineFA = "�����߷���";
//
//CString strAnnotionPolyLineDA = "�����ߵ���"; // �����ߵ���ͼ��
//CString strAnnotionTextDA = "�ı�ע�ǵ���"; // �ı�ע�ǵ���ͼ��
//CString strAnnotionPolygonDA = "�����浵��"; // �����浵��ͼ��
//CString strOtherAnnotionPolyLineDA = "�����ߵ���";
//
//CString strXZHXFA = "ѡַ���߷���"; // ѡַ���߷���ͼ��
//CString strYDHXFA = "�õغ��߷���"; // �õغ��߷���ͼ��
//
//CString strXZHXDA = "ѡַ���ߵ���";
//CString strYDHXDA = "�õغ��ߵ���";

//CString strXZZYDFA = "ѡַ���õط���";
//CString strXZJYDFA = "ѡַ���õط���";
//CString strXZDZYDFA = "ѡַ�����õط���";
//CString strXZZYDDA = "ѡַ���õص���";
//CString strXZJYDDA = "ѡַ���õص���";
//CString strXZDZYDDA = "ѡַ�����õص���";
//
//CString strYDZYDFA = "�õ����õط���";
//CString strYDJYDFA = "�õؾ��õط���";
//CString strYDDZYDFA = "�õش����õط���";
//CString strYDZYDDA = "�õ����õص���";
//CString strYDJYDDA = "�õؾ��õص���";
//CString strYDDZYDDA = "�õش����õص���";

//CString strHX_TEMP = "��ͼ��Χ";
//CString strAnnotionPolyLine_TEMP = "��ͼ������";
//CString strAnnotionText_TEMP = "��ͼ�����ı�";
//CString strAnnotionPolygon_TEMP = "��ͼ������";

CString strXMHX = "��Ŀ����"; // ��Ŀ����ͼ��

CString strXMFWX = "��Ŀ��Χ��"; // ��Ŀ��Χ��ͼ��

BOOL IsSaveXMFWX = FALSE;

CString strDLHX = "��·����"; // ��·����ͼ��
CString strCSLX = "�ӵ�����"; // ��������ͼ��
CString strCSHX = "���л���"; // ���л���ͼ��
CString strCSZX = "��������"; // ��������ͼ��
CString strCSLVX = "��������"; // ��������ͼ��
CString strCSCX = "���г���"; // ���г���ͼ��
CString strDLZXX = "��·������"; // ��·������
CString strHDZXX = "�ӵ�������"; // �ӵ�������

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
	//���ļ�
	CFile file;
	CString strFName;
	strLyNameArray.RemoveAll();
	strFName.Format("%sCheckLayer.ini",DistCommands_GetCurArxAppPath("shDistCommands.arx"));
	if(file.Open(strFName,CFile::modeRead)==FALSE) return FALSE; //���ļ�ʧ��,ֱ���˳�
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

	//��ȡ��Ϣ
	int nPos = 0;	
	char chTag[3]={0x0D,0x0A,0};
	int nTagLen = strTag.GetLength();
	while((nPos=strValue.Find(strTag))>=0)
	{
		int nPosDA = strValue.Find(chTag,nPos); //Ѱ�һس�����λ��
		CString strTemp= strValue.Mid(nPos+nTagLen,nPosDA-nPos-nTagLen); 
		strTemp.TrimLeft(); strTemp.TrimRight();
		strLyNameArray.Add(strTemp);
		strValue = strValue.Right(strValue.GetLength()-nPosDA);
	}

	return TRUE;
}


//�ؿ��ص�������
BOOL DistCommands_CheckCrossOrContain(/*CString strCurXMBH, */CString& strSrcLayerName, CString& strDestLayerName)
{
	IDistConnection* pConnection = Glb_pConnection;

	if(pConnection == NULL)
	{
		acutPrintf("\nSDE ���ݿ� û�����ӣ�"); return FALSE;
	}

	CString strProjectId = DocVars.docData().GetProjectId();

	if (strProjectId.IsEmpty())
	{
		AfxMessageBox("��ǰ�ĵ���֧�ָò���");
		return FALSE;
	}

	CStringArray strArrayCheckLy;
	DistCommands_ReadCfgLyNameArray(strArrayCheckLy,"[���ӽ�����ͼ��]=");
	int nCheck = strArrayCheckLy.GetCount();
	if(nCheck == 0) return TRUE; //û������ͼ�㣬��Ĭ��Ϊͨ��

	//1.ָ����Ҫ����CAD����
	CStringArray strArrayCAD;
	//strArrayCAD.Add(strXZHXFA); // ѡַ���߷���
	//strArrayCAD.Add(strXZHXDA); // ѡַ���ߵ���
	//strArrayCAD.Add(strYDHXFA); // �õغ��߷���
	//strArrayCAD.Add(strYDHXDA); // �õغ��ߵ���
	
	//strArrayCAD.Add(strXZZYDFA); // ѡַ���õط���
	//strArrayCAD.Add(strXZJYDFA); // ѡַ���õط���
	//strArrayCAD.Add(strXZDZYDFA); // ѡַ�����õط���
	//strArrayCAD.Add(strXZZYDDA); // ѡַ���õص���
	//strArrayCAD.Add(strXZJYDDA); // ѡַ���õص���
	//strArrayCAD.Add(strXZDZYDDA); // ѡַ�����õص���

	//strArrayCAD.Add(strYDZYDFA); // �õ����õط���
	//strArrayCAD.Add(strYDJYDFA); // �õؾ��õط���
	//strArrayCAD.Add(strYDDZYDFA); // �õش����õط���
	//strArrayCAD.Add(strYDZYDDA); // �õ����õص���
	//strArrayCAD.Add(strYDJYDDA); // �õؾ��õص���
	//strArrayCAD.Add(strYDDZYDDA); // �õش����õص���
	strArrayCAD.Add(strXMHX);
	strArrayCAD.Add(strXMFWX);

	//2.����ͼ�㣬���β�ѯ�뵱ǰ��Ŀ�йص�CADʵ��
	for(int k=0; k<strArrayCAD.GetCount(); k++)
	{
		CString strTempCAD = strArrayCAD.GetAt(k);
		STB_CONTENT_TREE xxContentTree = Glb_mapTree[strTempCAD];
		CString strTempSDE = xxContentTree.strSdeLyName;

		if (strTempCAD.IsEmpty() || strTempSDE.IsEmpty()) 
			continue;

		//���ù�����
		struct resbuf filter;
		filter.restype=AcDb::kDxfLayerName;
		filter.resval.rstring = strTempCAD.GetBuffer(0);
		filter.rbnext=NULL;

		//ѡ��ʵ��
		CDistSelSet tempSet;
		if(CDistSelSet::kDistSelected != tempSet.AllSelect(&filter)) continue;
		AcDbObjectIdArray ObjIdArray;
		tempSet.AsArray(ObjIdArray);
		int nLen = ObjIdArray.length();
		if(nLen == 0) continue;

		//ѡȡ��ǰ��Ŀʵ��
		char strRegName[512]={0};
		sprintf(strRegName,"DIST_%s",strTempSDE.GetBuffer(0));
		for(int m=0; m<nLen; m++)
		{
			AcDbObjectId tempId = ObjIdArray.at(m);
			CDistXData tempXData(tempId,strRegName);
			//CString strTempXMBH;
			CString strTempXMID; //ʹ����ĿID
			if(tempXData.GetRecordCount()>0)  //��չ���Դ��ڣ����޸�
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
				if (strCheckSde.CompareNoCase(strTempSDE) == 0) // ���������ͼ��
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
			if(xxContentTree.strSdeLyName.IsEmpty() || xxContentTree.strSdeLyName.Find("SDEͼ��")>=0) 
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
			/*************** �ж�ͼ���Ƿ�ɼ� ************************/

			POWER power = Glb_mapPower[xxContentTree.nId];

			// ������������Ȩ��
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
//				if(!xxContentTree.strSdeLyName.IsEmpty() && xxContentTree.strSdeLyName.Find("SDEͼ��")<0) 
				{
					// �洢ͼ��map��Ϣ
					Glb_mapTree[xxContentTree.strNodeName] = xxContentTree;
				}

				continue;
			}

			/*************** �ж�ͼ���Ƿ�ɼ� ************************/

			// �������ͣ����������
			MSXML2::IXMLDOMElementPtr lpNode = NULL;

			if(xxContentTree.strSdeLyName.IsEmpty() || xxContentTree.strSdeLyName.Find("SDEͼ��")>=0) 
			{
				if (xxContentTree.strNodeName.Find("������ʷ") >= 0)
				{
					EnumHistoryLayer(xxContentTree.nId, arrayTree);
//					continue;
				}
				lpNode = lpDocument->createElement("category");
				lpNode->setAttribute("name", _variant_t(xxContentTree.strNodeName));

				long nCheck = Glb_mapCheck[xxContentTree.nId];

				lpNode->setAttribute("checked", _variant_t(nCheck));

				lpElement->appendChild(lpNode);

				// �洢ͼ��map��Ϣ
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

				// �洢ͼ��map��Ϣ
				Glb_mapTree[xxContentTree.strNodeName] = xxContentTree;
			}
		}
	}
}

// ɾ��������Ŀ��ʵ��
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
			CString strXMID; // ʹ����ĿID
			tempXdata.Select("XMID", strXMID);
			int nCount = tempXdata.GetRecordCount();
			tempXdata.Close();

			//strXMBH.Trim();
			strXMID.Trim();

			//if ((strXMBH.CompareNoCase(Glb_BizzInfo.strBizzCode) != 0 ) && (nCount > 0) )
			if ((strXMID.CompareNoCase(DocVars.docData().GetProjectId()) != 0 ) && (nCount > 0) )
			{
				// ɾ��ʵ��
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

// ɾ��������Ŀ��ʵ��
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

// ����CADͼ������ȡ�ռ�����(������ʷͼ��)
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

	if (xxContentTree.strSdeLyName.IsEmpty() || xxContentTree.strSdeLyName.Find("SDEͼ��")>=0) 
	{
		return nRet;
	}

	acutPrintf("\n����ͼ�� %s...\n", lpCadLayer);
    OutputDebugString("---- to invoke ReadSdeLayer ----");
	nRet = ReadSdeLayer(pConnection, pExchangeData, xxContentTree.strSdeLyName, &filter, lpWhere, TRUE);

	return nRet;
}

// ����CADͼ������ȡ�ռ�����
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

	if (xxContentTree.strSdeLyName.IsEmpty() || xxContentTree.strSdeLyName.Find("SDEͼ��")>=0) 
	{
		return nRet;
	}
		
	acutPrintf("\n����ͼ�� %s...\n", lpCadLayer);

	POWER power = Glb_mapPower[xxContentTree.nId];

	// ������������Ȩ��
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

	// �����ʱͼ����Ҫ���й���
	CString strSdeLyName = xxContentTree.strSdeLyName;

	if (strXMHX.CompareNoCase(lpCadLayer) == 0) //��Ŀ����
	{
		if (NULL == lpWhere)
		{
			CString strProjectId = DocVars.docData().GetProjectId();
			if (atoi(strProjectId) > 0)
			{
				CString strWhere;
				//strWhere.Format("XMBH!='%s'", Glb_BizzInfo.strBizzCode);
				strWhere.Format("XMID!=%s", DocVars.docData().GetProjectId()); // ʹ����ĿID

				OutputDebugStringX("ReadCadLayer::ReadSdeLayer::%s, %s", xxContentTree.strSdeLyName, strWhere);
				nRet = ReadSdeLayer(pConnection, pExchangeData, xxContentTree.strSdeLyName, &filter, strWhere, FALSE);
				//strWhere.Format("XMBH='%s'", Glb_BizzInfo.strBizzCode);
				strWhere.Format("XMID=%s", DocVars.docData().GetProjectId()); // ʹ����ĿID

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
		else // �����������в�ѯ
		{
			OutputDebugStringX("ReadCadLayer::ReadSdeLayer::%s, %s", xxContentTree.strSdeLyName, lpWhere);
			nRet = ReadSdeLayer(pConnection, pExchangeData, xxContentTree.strSdeLyName, NULL, lpWhere, FALSE);
		}
	}
/*	else if (
		//��Ŀ��Χ��
		strXMFWX.CompareNoCase(lpCadLayer) == 0
		)
	{
		CString strProjectId = DocVars.docData().GetProjectId();
		if (atoi(strProjectId) > 0)
		{
			CString strWhere;
			//strWhere.Format("XMBH!='%s'", Glb_BizzInfo.strBizzCode);
			strWhere.Format("XMID!=%s", DocVars.docData().GetProjectId()); // ʹ����ĿID
			nRet = ReadSdeLayer(pConnection, pExchangeData, xxContentTree.strSdeLyName, &filter, strWhere, FALSE);
			//strWhere.Format("XMBH='%s'", Glb_BizzInfo.strBizzCode);
			strWhere.Format("XMID=%s", DocVars.docData().GetProjectId()); // ʹ����ĿID
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

	// ���ϴ�ͼ���Ѿ���ȡ���ı��
	xxContentTree.bIsReadBefore = TRUE;
	Glb_mapTree[lpCadLayer] = xxContentTree;

	return nRet;
}

// ����CADͼ��������ռ�����
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

//	double dArea = 0.0; // ���õ���� / ���õ���� / �����õ����

	if (!xxContentTree.strSdeLyName.IsEmpty())
	{

		POWER power = Glb_mapPower[xxContentTree.nId];

		// ������������Ȩ��
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

		if ((strXMHX.CompareNoCase(lpCadLayer) != 0 && strXMFWX.CompareNoCase(lpCadLayer) != 0)) //Ӳ����
			return;
/*
		if (strXMHX.CompareNoCase(lpCadLayer) != 0 && 
			strXMFWX.CompareNoCase(lpCadLayer) != 0 &&
			strDLHX.CompareNoCase(lpCadLayer) != 0 && 
			strCSLX.CompareNoCase(lpCadLayer) != 0 && 
			strCSHX.CompareNoCase(lpCadLayer) != 0 && 
			strCSZX.CompareNoCase(lpCadLayer) != 0 && 
			strCSLVX.CompareNoCase(lpCadLayer) != 0 && 
			strCSCX.CompareNoCase(lpCadLayer) != 0 ) //Ӳ����
		{
			return;
		}
*/

		AcDbObjectIdArray objIdArray;

		CString strSdeLayer = xxContentTree.strSdeLyName;
		// ����ǰ�Զ������Ŀ����
		if (
			(strXMHX.CompareNoCase(lpCadLayer) == 0) ||
			(strXMFWX.CompareNoCase(lpCadLayer) == 0)
			)
		{
			//����ͼ�������
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

				CString strXMID; // ʹ����ĿID
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

					CString strTable = strName.Left(strName.Find("��"));
					CString strField = strName.Right(strName.GetLength()-strName.Find("��")-strlen("��"));

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

		OutputDebugStringX("���ڱ���ͼ��%s", xxContentTree.strSdeLyName);
//		SaveSdeLayer(pConnection, pExchangeData, xxContentTree.strSdeLyName, NULL,/*&Glb_Filter,*/ false);
		SaveSdeLayer(pConnection, pExchangeData, xxContentTree.strSdeLyName, objIdArray, DocVars.docData().GetProjectId(), false);

	}
}

// �򿪹ر�ͼ�㣨���ͼ�㲻���ڣ���sde���ж�ȡ��
void ShowLayer(CString strCadLayer, BOOL bShow, LPCTSTR lpWhere)
{
	LockDocument _Lock;

	Acad::ErrorStatus es = Acad::eOk ;

	AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();
	AcDbLayerTable *pLyTable = NULL;
	AcDbLayerTableRecord *pLyRcd = NULL;

	es = pDB->getSymbolTable(pLyTable,AcDb::kForRead);            //��õ�ǰ���ݿ��в��ָ��
	if ( es != Acad::eOk ) 
	{
		acutPrintf("\n��ȡLayerTable��ʱ�����������Ժ����ԡ�\n");
		return;
	}

	STB_CONTENT_TREE xxContentTree = Glb_mapTree[strCadLayer];

	if ( pLyTable->has(strCadLayer) && xxContentTree.bIsReadBefore)
	{
		es= pLyTable->getAt(strCadLayer, pLyRcd, AcDb::kForWrite); // ��pLyRcd��strLyNameָ����ͼ�㲢�ҷ���ָ�뵽���򿪵�pLyRcdͼ��
		if( es != Acad::eOk ) 
		{
			acutPrintf("\n��ͼ�� %s ��ʱ�����������Ժ����ԡ�\n", strCadLayer);
			pLyTable->close();
			return;
		}

		bool bSetIsOff = bShow ? Adesk::kFalse : Adesk::kTrue;
		if (pLyRcd->isOff() != bSetIsOff) // ��ѯͼ�㿪��״̬��kFalseΪ��
		{
			if (Adesk::kTrue == bSetIsOff)
				acutPrintf("\n�ر�ͼ�� %s...\n", strCadLayer);
			else
				acutPrintf("\n��ͼ�� %s...\n", strCadLayer);

			pLyRcd->setIsOff(bSetIsOff); // �ı�ͼ��ɼ�״̬
		}

		pLyRcd->close();
		pLyTable->close();
	}
	else if (bShow) // ����Ҳ�����ǰͼ��,��Ҫ��sde���ж�ȡ
	{
		if (pLyTable->has(strCadLayer))
		{
			es= pLyTable->getAt(strCadLayer, pLyRcd, AcDb::kForWrite); // ��pLyRcd��strLyNameָ����ͼ�㲢�ҷ���ָ�뵽���򿪵�pLyRcdͼ��
			if( es != Acad::eOk ) 
			{
				acutPrintf("\n��ͼ�� %s ��ʱ�����������Ժ����ԡ�\n", strCadLayer);
				pLyTable->close();
				return;
			}

			pLyRcd->setIsOff(Adesk::kFalse); // �ı�ͼ��ɼ�״̬
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

// ����SDE���ݿ�
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
		OutputDebugStringX("����SDE���Ӷ���ʧ��");
		return ;
	}
	long rc = -1;
	if((rc = Glb_pLocalConnection->Connect(lpSdeServer, lpSdeService, lpSdeDatabase, lpSdeUserName, lpSdePassword)) != 1)
	{
		Glb_pLocalConnection->Release();
		Glb_pLocalConnection = NULL;

		OutputDebugStringX("SDE���ݿ�����ʧ��,����ԭ��%s ",ReturnErrorCodeInfo(rc));
		return ;
	}
	if (NULL == Glb_pLocalExchangeData)
	{
		Glb_pLocalExchangeData = CreateExchangeObjcet();
		if(NULL == Glb_pLocalExchangeData)
		{
			OutputDebugStringX("CreateSDEConnectObjcet ����SDE��CAD���������");
			return ;
		}
	}
}

// ����SDE���ݿ�
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
		OutputDebugStringX("����SDE���Ӷ���ʧ��");
		return ;
	}
	long rc = -1;
	if((rc = Glb_pConnection->Connect(lpSdeServer, lpSdeService, lpSdeDatabase, lpSdeUserName, lpSdePassword)) != 1)
	{
		Glb_pConnection->Release();
		Glb_pConnection = NULL;

		OutputDebugStringX("SDE���ݿ�����ʧ��,����ԭ��%s ",ReturnErrorCodeInfo(rc));
		return ;
	}
	if (NULL == Glb_pExchangeData)
	{
		Glb_pExchangeData = CreateExchangeObjcet();
		if(NULL == Glb_pExchangeData)
		{
			OutputDebugStringX("CreateSDEConnectObjcet ����SDE��CAD���������");
			return ;
		}
	}
}

// Modify
//// ���ع�����
//void LoadDistToolBar()
//{
//	// ж�ع�����
//	Sys_UnLoadDistToolBar();
//
//	// �õ�arx����·��
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
//	// װ��XML�ַ���
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
//	// ���ع�����
//	Sys_LoadDistToolBar("");
//}

//// ��ȡ�о�ͼ���б�
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
//	GetPrivateProfileSection("�о�ͼ��", lpReturnedString, 1024, lpCadProjectFileName);
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
//	// �õ�arx����·��
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
//	// װ��XML�ַ���
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
//	// ���������б�
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
//		GetPrivateProfileString("ͼ��", strLabel, "", lpValue, 100, "BizzInfo.ini");
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

// ��SDE���ж�ȡͼ����
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

	// ���ͼ���������
	Glb_mapTree.clear();

	GenerateXmlTree(lpDocument, lpRoot, -1, arrayTree);

	CString strProjectId = DocVars.docData().GetProjectId(); // ���ߵ���Ϊ0,����չ��Ϊ-1

	Sys_LoadLayerTree(lpDocument->xml, atoi(strProjectId));

/*	CStringArray array;
	array.Add("��������ͼ��");
	array.Add("��Ŀ����");
	array.Add("��Ŀ��Χ��");
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

	//���ҵ���ͼ��������뵱ǰͼ��
	AcDbObjectId objectId = FindDWGAndInsertAsBlock(lpTempPath, lpTempFile, pt, scale, 0, -1, "0");

	//if(objectId == NULL)
	//{
	//	acutPrintf("\n%sͼ���ļ�����ʧ�ܣ�\n",lpTempFile);
	//	return;
	//}
	//else
	//{
	//	DocVars.docData().m_mapBlockIds.SetAt(lpTempFile, objectId);
	//}

	// ͨ��cadcontainerʵ��ʣ�๦�ܣ����Դ�ļ���
	CString strCommand = "CLEANLOCALWORKDWG";
	CStringArray strParamArray;

	CString strParam;
	strParam.Format("filename=%s", lpFile);
	strParamArray.Add(strParam);

	SendCopyDataMessage(Glb_hDapForm, strCommand, strParamArray);

//	_activeCurrentCadView();

	CString strProjectId = DocVars.docData().GetProjectId();
	if (strProjectId.CompareNoCase("-1") == 0) // ����չ��
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
		// ����Ŀ���������ͼ����������ѡ�κ�ͼ�㣬Ҳ�������κ����ݣ������Ŀ�Ѿ�������Ŀ��Χ�ߣ�Ҳ��������
/*		CString strProjectId = DocVars.docData().GetProjectId();

		CString strWhere;
		if (atoi(strProjectId) > 0)
		{
			strWhere.Format("XMID=%s", DocVars.docData().GetProjectId()); // ʹ����ĿID
		}

		LockDocument _Lock;
		ReadCadLayer("��Ŀ����", Glb_Filter, strWhere.IsEmpty() ? NULL : (LPCTSTR)strWhere);
		ReadCadLayer("��Ŀ��Χ��", Glb_Filter, strWhere.IsEmpty() ? NULL : (LPCTSTR)strWhere);
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

// ��ȡ���߻��Ʒ�Χ
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
		AfxMessageBox("��ǰ�ĵ���֧�ָò���");
		return FALSE;
	}

	STB_CONTENT_TREE xxContentTree = Glb_mapTree[strXMFWX];

	if (xxContentTree.strSdeLyName.IsEmpty())
	{
//		AfxMessageBox("δ�ҵ���Ŀ��Χ�߶�Ӧ��SDEͼ�㣡", MB_OK);
		return FALSE;
	}

	TCHAR lpSQL[256] = {0};
	//if (strField.CompareNoCase("XMBH") == 0) // ������Ŀ��Ŷ�λ
	//	sprintf(lpSQL, "F:OBJECTID,SHAPE,T:%s,W:%s='%s',##", strSdeLayerName, strField, strBizzCode);
	//else // ����������Ŷ�λ
	//	sprintf(lpSQL, "F:OBJECTID,SHAPE,T:%s,W:%s='%s',##", strSdeLayerName, strField, strBizzSame);
	sprintf(lpSQL, "F:OBJECTID,SHAPE,T:%s,W:XMID='%s',##", xxContentTree.strSdeLyName, strProjectId); // ������ĿID��λ

	AcGePoint3dArray ptArray;
	pExchangeData->GetSearchShapePointArray(pConnection, lpSQL, &ptArray);

	if (ptArray.logicalLength() <= 0)
	{
		return FALSE;
	}

	// �Ե�������д���
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

// �Զ���Ĵ��ں���
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
	// �򿪵�ǰ�ļ�
	HANDLE  hFile = CreateFile(lpXdeFile, GENERIC_READ, 0, 
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if(hFile == INVALID_HANDLE_VALUE)
	{
		MessageBox(NULL, _T("��SDE�����ļ�ʧ��"), 
			_T("����"), MB_ICONSTOP);
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

// ��Ϣ����
void ProcessCommand(CString& strCommand, MapString2String& parameters)
{
	CString strDebugInfo;
	strDebugInfo.Format("ProcessCommand : %s", strCommand);
	OutputDebugString(strDebugInfo);

	// ��ʾ������
	if (strCommand.CompareNoCase("ShowCommandLineWindow") == 0)
	{
		ShowCommandLineWindow(TRUE);
	}
	// ҵ������
	else if (strCommand.CompareNoCase("bizzdata") == 0)
	{
		// ����SDE���ݿ�
		CreateSdeConnection(Glb_SdeInfo.strServer.GetBuffer(), Glb_SdeInfo.strService.GetBuffer(), Glb_SdeInfo.strDatabase.GetBuffer(), Glb_SdeInfo.strUserName.GetBuffer(), Glb_SdeInfo.strPassword.GetBuffer());

		if (Glb_bMultiConnection)
		{
			// ���ӷ־�SDE���ݿ�
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
		WritePrivateProfileString("BizzInfo","��Ŀ���",Glb_BizzInfo.strBizzCode,strIniFile);
		WritePrivateProfileString("BizzInfo","��Ŀ����",Glb_BizzInfo.strBizzName,strIniFile);
		WritePrivateProfileString("BizzInfo","���赥λ",Glb_BizzInfo.strBzOwner,strIniFile);
		WritePrivateProfileString("BizzInfo","ҵ������",Glb_BizzInfo.strBzId,strIniFile);
		WritePrivateProfileString("BizzInfo","��������",Glb_BizzInfo.strBztmOver,strIniFile);
	}
	// ��Ŀ���ԣ���Ҫд��ͼ���ֶ��е�����ֵ��
	else if (strCommand.CompareNoCase("bizzdata_ex") == 0)
	{
		Glb_BizzInfoEx.clear();
		Glb_BizzInfoEx = parameters;
	}
	// sde����(���Ӳ���,ͼ�����)
	else if (strCommand.CompareNoCase("sdedata") == 0)
	{
		// DAP�����
		Glb_hDapForm = HWND(atol(parameters["parent_hwnd"]));

		// �о�SDE���Ӳ���
		Glb_SdeInfo.strServer = parameters["server"];
		Glb_SdeInfo.strService = parameters["service"];
		Glb_SdeInfo.strDatabase = parameters["database"];
		Glb_SdeInfo.strUserName = parameters["username"];
		Glb_SdeInfo.strPassword = parameters["password"];

		// SDEͼ������
		Glb_LayerInfo.strAnnotionLine = parameters["��ע�߲�"]; // ��ע�߲�
		Glb_LayerInfo.strAnnotionText = parameters["��ע���ֲ�"]; // ��ע���ֲ�
		Glb_LayerInfo.strLayer = parameters["��Χͼ��"];
		Glb_LayerInfo.strField = parameters["��Χͼ���ֶ�"];
		Glb_LayerInfo.strMapLayer = parameters["����ͼ"];
		Glb_LayerInfo.strIndexField = parameters["����ͼ�����ֶ�"];

		if (Glb_LayerInfo.strIndexField.IsEmpty())
			Glb_LayerInfo.strIndexField = "ͼ��";

		OutputDebugStringX("SdeServer=%s, SdeService=%s, SdeDatabase=%s, SdeUserName=%s, SdePassword=%s", 
			Glb_SdeInfo.strServer, Glb_SdeInfo.strService, Glb_SdeInfo.strDatabase, Glb_SdeInfo.strUserName, Glb_SdeInfo.strPassword);

		// ���ͼ����Ϣ
		Glb_layerArray.RemoveAll();
	}
	// Ȩ������
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

			if (nId == 88888888) // ����CADͼ��
			{
				Glb_nFormPower.nPower = power.nPower;
				Glb_nFormPower.nPrior = power.nPrior;
			}
			else if (nId > 80000000) // CAD����
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
			else // CADͼ��
			{
				Glb_mapPower[nId] = power;
			}

			_iter++;
		}
	}
	// ����ͼ����
	else if (strCommand.CompareNoCase("LoadLayerTree") == 0)
	{
		CString strProjectId = DocVars.docData().GetProjectId();

		OutputDebugStringX("LoadLayerTree::strProjectId=%s", strProjectId);

		if (strProjectId.CompareNoCase("0") == 0) // ���ߵ���ʱProjectIdΪ0
		{
			Sys_UnLoadDistMenu("�滮��ͼ");
			Sys_LoadDistMenu("���ߵ���");
		}
		else if (strProjectId.CompareNoCase("-1") == 0) // ����չ��
		{
			Sys_UnLoadDistMenu("���ߵ���");
			Sys_UnLoadDistMenu("�滮��ͼ");
		}
		else
		{
			g_bAutoReadSdeData = TRUE;
			Sys_UnLoadDistMenu("���ߵ���");
			Sys_LoadDistMenu("�滮��ͼ");
		}

/*		//���ιرհ�ť
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
		if (strProjectId.CompareNoCase("-1") == 0) // ����չ��
		{
			return;
		}

		// ����ͼ����
		LoadLayerTree();

		if (strProjectId.CompareNoCase("0") == 0) // ���ߵ���
		{
			Sys_ShowSampToolBar(TRUE);
			return;
		}
		else
		{
			Sys_ShowSampToolBar(FALSE);
		}

		// 2009-05-04 //
		// ����Ŀ���������ͼ����������ѡ�κ�ͼ�㣬Ҳ�������κ����ݣ������Ŀ�Ѿ�������Ŀ��Χ�ߣ�Ҳ��������
/*		CStringArray array;
		array.Add("��������ͼ��");
		array.Add("��Ŀ����");
		array.Add("��Ŀ��Χ��");
		Sys_SetCheckedLayers(array);
*/
		// 2009-05-04 //

		AcGePoint3d ptLB, ptRT;
/*
		// ��ȡ���߻��Ʒ�Χ
		if (GetRangePoints(ptLB, ptRT))
		{
			// ����ռ��������
			Glb_Filter.nEntityType = 4;
			Glb_Filter.nSelectType = kSM_AI;
			Glb_Filter.ptArray.setLogicalLength(0);

			Glb_Filter.ptArray.append(AcGePoint3d(ptLB[0], ptLB[1], ptLB[2]));
			Glb_Filter.ptArray.append(AcGePoint3d(ptRT[0], ptRT[1], ptRT[2]));

//			ReadCadLayer("��Ŀ����");
//			ReadCadLayer("��Ŀ��Χ��");
		}
		else
		{
			Glb_Filter.ptArray.setLogicalLength(0);

			acutPrintf("\nû���ҵ����Ƶ���Ŀ��Χ\n");
		}
*/
		Glb_Filter.ptArray.setLogicalLength(0);
	}
/*	// ���ع�����
	else if (strCommand.CompareNoCase("LoadDistToolBar") == 0)
	{
		LoadDistToolBar();
	}
//*/	// ��ȡSDE����
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
//				acutPrintf("\n�������ĵ�ʧ��!\n");
//			}
//		}
//
//		// ��Ϣ������
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
	// ɾ����������
	else if (strCommand.CompareNoCase("DeleteAllData") == 0)
	{
		SendCommandToAutoCAD("DeleteAllEntity ");

		DocVars.docData().SetProjectId("");

		DocVars.docData().SetFileId("");

		DocVars.docData().m_mapBlockIds.RemoveAll();

		//�Ƿ�ɾ��ͼ�㣬�鶨�壿
	}
	// ��ʾ�����
	else if (strCommand.CompareNoCase("ShowCommandLine") == 0)
	{
		ShowCommandLineWindow(TRUE);
	}
	// ����ͼ��
	else if (strCommand.CompareNoCase("ShowLayer") == 0)
	{
		CString strLayer = parameters["layer"];
		CString strShow = parameters["show"];

		OutputDebugStringX("layer=%s, show=%s", strLayer, strShow);

		if (Glb_Filter.ptArray.logicalLength() <= 0)
		{
			acutPrintf("\nû���ҵ����Ƶ���Ŀ��Χ\n");
			return;
		}

		ShowLayer(strLayer, atoi(strShow), NULL);

		activeCurrentCadView();

//		SendCommandToAutoCAD("redraw ");
	}
	// ����ʷͼ��
	else if (strCommand.CompareNoCase("loadHistoryLayer") == 0)
	{
		CString strWhere = parameters["where"];

		LockDocument _Lock;

		acutPrintf("\nѡ�����ݼ��ط�Χ��\n");

		// Add your code for command DistCommands._ReadSdeLayersEx here
		ads_point p1, p2;  //�������ε������ȡ
		if (acedGetPoint(NULL, "\n>>ָ����һ���ǵ㣺\n", p1) != RTNORM)
			return;

		if (acedGetCorner(p1, "\n>>ָ���ڶ����ǵ㣺\n", p2) != RTNORM) 
			return;

		if (p1[0] == p2[0] || p1[1] == p2[1]) //���ο�Ϸ��Լ��
			return;

		double fTemp;
		if (p1[0] > p2[0]) {fTemp=p1[0]; p1[0]=p2[0]; p2[0]=fTemp;}
		if (p1[1] > p2[1]) {fTemp=p1[1]; p1[1]=p2[1]; p2[1]=fTemp;}
		p1[2] = 0; p2[2] = 0;

		//����ռ���������
		DIST_STRUCT_SELECTWAY filter;

		filter.nEntityType = 4;
		filter.nSelectType = kSM_AI;
		filter.ptArray.setLogicalLength(0);

		filter.ptArray.append(AcGePoint3d(p1[0], p1[1], p1[2]));
		filter.ptArray.append(AcGePoint3d(p2[0], p2[1], p2[2]));

		// ��ȡͼ������
		for (int i=0; i<Glb_arrayHistoryLayer.GetCount();i++)
		{
			OutputDebugString("---- to Invoke ReadHistoryCadLayer ----");
			ReadHistoryCadLayer(Glb_arrayHistoryLayer.GetAt(i), strWhere, filter);
		}

		acutPrintf("\nͼ��������\n");
	}
	// ���Ӳ����嵥�еķ���ͼ
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

		// �ر����ݿⷴӦ��
		if (Glb_pExchangeData != NULL)
			Glb_pExchangeData->SetDbReactorStatues(false);

		// ѡ������ʵ��
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

			// ɾ��ʵ��
			AcDbEntity * pEnt = NULL;
			acdbOpenObject(pEnt, objId, AcDb::kForWrite); 
			pEnt->erase();
			pEnt->close();                
		}

		acedSSFree(ents);

		// �ر����ݿⷴӦ��
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
			//���ļ����Զ��ر�ֻ����ʾ�Ի���
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

// ����XML���������������
BOOL ParseParameters(CString strXML, CString& strCommand, MapString2String& parameters)
{
	OutputDebugString(strXML);
	// ��ȥ��β��ӵĶ�����Ϣ
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

	// װ��XML�ַ���
	if (!lpDocument->loadXML(bstrText))
	{
		SysFreeString(bstrText);

		OutputDebugString("LoadXML failed!");

		return FALSE;
	}

	SysFreeString(bstrText);

	MSXML2::IXMLDOMElementPtr lpDocElement = NULL;
	lpDocument->get_documentElement(&lpDocElement);

	// ����Command����
	MSXML2::IXMLDOMNodePtr lpCommand = lpDocElement->selectSingleNode(_bstr_t("command"));

	if (NULL == lpCommand)
		return FALSE;

	MSXML2::IXMLDOMNodePtr lpNameNode = lpCommand->attributes->getNamedItem("name");
	strCommand = (char*)lpNameNode->text;

	// ���������б�
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

// ��ȡ����Ȩ��ֵ (0������ 1��ֻ�� 2����д 3������)
BOOL CheckCommandPower(CString strCommand)
{
	POWER power = Glb_commandPower[strCommand];

	// power.nPower Ȩ��ֵ (0������ 1��ֻ�� 2����д 3������)
	if ((power.nPrior > 0) && ((power.nPower == 0) || (power.nPower == 1)))
	{
		acutPrintf("\n��û��ʹ��%s�����Ȩ�ޣ��������Ա��ϵ��",strCommand);
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

		// ��Ϣ������
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
			if (strSet.find(str) != strSet.end())	//����Ѵ���
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

		//����ռ���������
//		DIST_STRUCT_SELECTWAY filter;
		Glb_Filter.nEntityType = 4;
		Glb_Filter.nSelectType = kSM_AI;
		Glb_Filter.ptArray.setLogicalLength(0);

		Glb_Filter.ptArray.append(leftBottomPoint);
		Glb_Filter.ptArray.append(rightTopPoint);

		Glb_layerArray.RemoveAll();

		CStringArray arrayLayers;

		acutPrintf("\n��ȡͼ���б�...\n");

		// ��ȡ���д򹴵�ͼ��
		Sys_GetCheckedLayers(arrayLayers);
		//���˵��ظ���ͼ��,����˳�򲻱�
		GetUniqueValues(arrayLayers);

		// ��ȡͼ������
		for (int i=0; i<arrayLayers.GetCount();i++)
		{
			acutPrintf("\n����ͼ�� %s...\n", arrayLayers.GetAt(i));
			ReadCadLayer(arrayLayers.GetAt(i), Glb_Filter);
		}

		acutPrintf("\nͼ�������ϡ�\n");

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
			acutPrintf("\n��ǰ����������\n");
			return;
		}

		IDistExchangeData* pExchangeData = Glb_pExchangeData;
		IDistConnection* pConnection = Glb_pConnection;

		LockDocument _Lock;

		CString strIniFile = "BizzInfo.ini";

		CString strItems[] = 
		{
			"�����ַ", "�õ�����", "�ݻ���", "�����ܶ�", "�����޸�", "�̵���", "ͣ��λ", "���֤��", "������Ա", 
		};

		int nCount = sizeof(strItems)/sizeof(CString);

		for (int i=0; i<nCount; i++)
		{
			CString strName = strItems[i];

			WritePrivateProfileString("BizzInfo", strName, "", strIniFile);
		}

		acutPrintf("\n����ͼ�� %s...\n", strXMHX);
		SaveCadLayer(strXMHX);
		acutPrintf("\n����ͼ�� %s...\n", strXMFWX);
		SaveCadLayer(strXMFWX);
		
		acutPrintf("\nͼ�㱣�����\n");

		// Modify
		//// ���õ��������dap��
		//SendDataToDapForm(pConnection);
	}

//	// ----- DistCommands._DimAnnotion command (do not rename)
//	static void DistCommands_DimAnnotion(void)
//	{
//		if (!CheckCommandPower("DimAnnotion"))
//			return;
//
//		LockDocument _Lock;
//		// �����ע
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
//		// Ĭ��Ϊ����ͼ�㣬�������ͼ�㲻��д����Ĭ�ϲ�ͼ��
//		BOOL bReadOnly = TRUE;
//		STB_CONTENT_TREE xxContentTree = Glb_mapTree[strLineCadLayer];
//		if (!xxContentTree.strSdeLyName.IsEmpty())
//		{
//			POWER power = Glb_mapPower[xxContentTree.nId];
//
//			// ������������Ȩ��
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
//		// ���ͼ�㲻����,��Ҫ��SDE���ж�ȡ
//		if (!bIsLineLayerExist)
//		{
////			long lRt = ReadSdeLayer(Glb_LayerInfo.strAnnotionLine, &Glb_Filter);
//			long lRt = ReadCadLayer(strLineCadLayer);
//			if (lRt != 1)
//			{
//#ifdef _DEBUG
//				acutPrintf("\n D�޷���ȡ%sͼ�����ݣ��������Ϊ%ld�����飡", strLineCadLayer, lRt);
//#else
//				acutPrintf("\n ��ȡ%sͼ������ʧ�ܣ�\n", strLineCadLayer);
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
//				acutPrintf("\n D�޷���ȡ%sͼ�����ݣ��������Ϊ%ld�����飡", strTextCadLayer, lRt);
//#else
//				acutPrintf("\n ��ȡ%sͼ������ʧ�ܣ�\n", strTextCadLayer);
//#endif
//				return;
//			}
//		}
//
//		struct resbuf rb;
//		acedGetVar("clayer", &rb);  //��õ�ǰ����
//
//		CreateHXBZ(strLineCadLayer, strTextCadLayer);
//
//		acedSetVar("clayer", &rb);  //��õ�ǰ����
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
//			acutPrintf("\nû���ҵ����Ƶ���Ŀ��Χ\n");
//			return;
//		}
//
//		// дSQL��ѯͼ������
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
//		// ����FTP������
//		XFtpConn ftpConn;
//		ftpConn.SetFtpSetting(Glb_FtpInfo.strServer, Glb_FtpInfo.strUserName, Glb_FtpInfo.strPassword, Glb_FtpInfo.nPort);
//		ftpConn.OpenConnect();
//
//		TCHAR lpTempPath[MAX_PATH] = {0};
//		GetTempPath(MAX_PATH, lpTempPath);
//		PathAddBackslash(lpTempPath);
//		CString strRemoteFile,strLocalFile;
//
//		// ͼ��
//		AcDbObjectId resultId;
//		CDistEntityTool tempTool;
//		tempTool.CreateLayer(resultId,"DIST_DXT",252);
//
////		// �ر����ݿⷴӦ��
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
//						acutPrintf("\n���ص���ͼ%sʧ�ܣ�\n",strFile);
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
//			//���ҵ���ͼ��������뵱ǰͼ��
//			AcDbObjectId objectId = FindDWGAndInsertAsBlock(lpTempPath, strFile, pt, scale, 0, 252, "DIST_DXT");
//
////			if(objectId == NULL)
////			{
////				acutPrintf("\n%sͼ���ļ�����ʧ�ܣ�\n",strFile);
////				continue;
////			}
////			else
////			{
////				DocVars.docData().m_mapBlockIds.SetAt(strFile, objectId);
////			}
//		}
//
//		// �Ͽ�FTP������
//		ftpConn.CloseConnect();
//
////		// �ر����ݿⷴӦ��
////		if (Glb_pExchangeData != NULL)
////			Glb_pExchangeData->SetDbReactorStatues(true);
//
//		// ������Ŀ��Χ��λ
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
//		ads_point p1, p2;  //�������ε������ȡ
//		if (acedGetPoint(NULL, "��ָ��һ����Χ����Esc��ȡ��\n", p1) == RTNORM)
//		{
//			if (acedGetCorner(p1, "��ָ�����SDE���ݵĶԽǶ���\n", p2) != RTNORM) 
//				return;
//
//			if (p1[0] == p2[0] || p1[1] == p2[1]) //���ο�Ϸ��Լ��
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
//		//����ռ���������
//		DIST_STRUCT_SELECTWAY filter;
//		filter.nEntityType = 4;
//		filter.nSelectType = kSM_AI;
//		filter.ptArray.setLogicalLength(0);
//
//		filter.ptArray.append(AcGePoint3d(p1[0], p1[1], p1[2]));
//		filter.ptArray.append(AcGePoint3d(p2[0], p2[1], p2[2]));
//
//		//дSQL��ѯCAD��SDEͼ����ձ�
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
////		// �ر����ݿⷴӦ��
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
//						acutPrintf("\n���ص���ͼ%sʧ�ܣ�\n",strFile);
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
//			//���ҵ���ͼ��������뵱ǰͼ��
//			AcDbObjectId objectId = FindDWGAndInsertAsBlock(lpTempPath, strFile, pt, scale, 0, 252, "DIST_DXT");
//
////			if(objectId == NULL)
////			{
////				acutPrintf("\n%sͼ���ļ�����ʧ�ܣ�\n",strFile);
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
////		// �ر����ݿⷴӦ��
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

		// Ĭ��Ϊ����ͼ�㣬�������ͼ�㲻��д����Ĭ�ϲ�ͼ��
		BOOL bReadOnly = TRUE;
		STB_CONTENT_TREE xxContentTree = Glb_mapTree[strTextCadLayer];
		if (!xxContentTree.strSdeLyName.IsEmpty())
		{
			POWER power = Glb_mapPower[xxContentTree.nId];

			// ������������Ȩ��
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

		// ���õ�ǰͼ��
		struct resbuf *Value=acutBuildList(RTSTR, strTextCadLayer, 0);
		acedSetVar("clayer", Value);

//		CDistEntityTool tempTool;
//		tempTool.SetCurTextStyle("����");
		acedCommand(RTSTR, "text",0);
	}

	// ----- DistCommands._ReadSdeLayersEx command (do not rename)
	static void DistCommands_ReadSdeLayersEx(void) // �����ͼ
	{
		if (!CheckCommandPower("ReadSdeLayersEx"))
			return;

		LockDocument _Lock;

		acutPrintf("\nѡ�����ݼ��ط�Χ��\n");

		// Add your code for command DistCommands._ReadSdeLayersEx here
		ads_point p1, p2;  //�������ε������ȡ
		if (acedGetPoint(NULL, "\n>>ָ����һ���ǵ㣺\n", p1) != RTNORM)
			return;

		if (acedGetCorner(p1, "\n>>ָ���ڶ����ǵ㣺\n", p2) != RTNORM) 
			return;

		if (p1[0] == p2[0] || p1[1] == p2[1]) //���ο�Ϸ��Լ��
			return;

		double fTemp;
		if (p1[0] > p2[0]) {fTemp=p1[0]; p1[0]=p2[0]; p2[0]=fTemp;}
		if (p1[1] > p2[1]) {fTemp=p1[1]; p1[1]=p2[1]; p2[1]=fTemp;}
		p1[2] = 0; p2[2] = 0;

		//����ռ���������
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

		acutPrintf("\n��ȡͼ���б�...\n");

		// ��ȡ���д򹴵�ͼ��
		Sys_GetCheckedLayers(arrayLayers);

		GetUniqueValues(arrayLayers);

		// ��ȡͼ������
		for (int i=0; i<arrayLayers.GetCount();i++)
		{
//			acutPrintf("\n����ͼ�� %s...\n", arrayLayers.GetAt(i));
			ReadCadLayer(arrayLayers.GetAt(i), Glb_Filter);
		}

		acutPrintf("\nͼ��������\n");
	}

	// Modify
	//// ----- DistCommands._UploadDwgFile command (do not rename)
	//static void DistCommands_UploadDwgFile(void)
	//{
	//	if (!CheckCommandPower("UploadDwgFile"))
	//		return;

	//	LockDocument _Lock;

	//	// ���ļ���������ʱĿ¼�²��ϴ���FTP
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
	//		acutPrintf("\n �洢��ǰͼ����\n");
	//		return;
	//	}

	//	// ���ļ�����һ�ݣ������ϴ�
	//	TCHAR lpLocalFile[MAX_PATH] = {0};
	//	GetTempFileName(lpTempPath, "$SH", 0, lpLocalFile);
	//	CopyFile(lpTempFile, lpLocalFile, FALSE);

	//	// ͨ��cadcontainerʵ��ʣ�๦�ܣ��ϴ�dwg�ļ���update���ݿ��
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
//		//����ѡ��ʵ��
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
//			acutPrintf("\nѡ��ʵ������!");
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
//		//�򿪱�
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
//			if (strSrcLayer.CompareNoCase(strXZZYDDA) == 0) // "ѡַ���õص���"
//				strDestLayer = strYDZYDFA; // "�õ����õط���"
//			else if (strSrcLayer.CompareNoCase(strXZJYDDA) == 0) // "ѡַ���õص���"
//				strDestLayer = strYDJYDFA; // "�õؾ��õط���"
//			else if (strSrcLayer.CompareNoCase(strXZDZYDDA) == 0) // "ѡַ�����õص���"
//				strDestLayer = strYDDZYDFA; // "�õش����õط���"
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
//				acutPrintf("\n ��ģ�Ϳռ�ʧ�ܣ�");
//				pEnt->close();
//				break;
//			}
//
//			AcDbObjectId resultId;
//			//���ʵ��
//			es = pBlkRec->appendAcDbEntity(resultId,pEnt0);
//			if (es != Acad::eOk) 
//			{
//				acutPrintf("\n ���ʵ�嵽ģ�Ϳռ�ʧ�ܣ�");
//				pEnt->close();
//				break;
//			}
//
//			pEnt->close();
//			pEnt0->close();
//
//			// ���ROWID��չ��Ϣ
//			CDistXData tempXdata(resultId, "ROWID_OBJS");
//			if(tempXdata.GetRecordCount() > 0) 
//			{
//				tempXdata.DeleteAllRecord();
//				tempXdata.Close();
//			}
//
//			// ���ѡַ��չ��Ϣ
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
//			// ����õ���չ��Ϣ������ѡַ���Ƶ��õ�
//			sprintf(strXDataName,"DIST_%s",Glb_mapTree[strYDHXFA].strSdeLyName);
//			CDistXData tempXdata_YD(resultId, strXDataName);
//
//			for (int i=0; i<strFldNameArray.GetCount(); i++) 
//			{
//				tempXdata_YD.Add(strFldNameArray.GetAt(i), strFldValueArray.GetAt(i));
//			}
//
//			tempXdata_YD.Modify("OBJECTID", "-1");
//			tempXdata_YD.Modify("XMBH", Glb_BizzInfo.strBizzCode); // ��Ŀ���
//			tempXdata_YD.Modify("XMBH", Glb_BizzInfo.strBizzCode); // ���赥λ
//			tempXdata_YD.Modify("XMBH", Glb_BizzInfo.strBizzCode); // ��Ŀ����
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
//		//ѡ�����
//		ads_name ssName;
//		CString strCadLayerName = strXZJYDFA;//"ѡַ����";
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
//			AfxMessageBox("��ʵ����������Ѿ����ڣ�");
//			return;
//		}
//
//		tempXdata.Add("XMBH", Glb_BizzInfo.strBizzCode); // ��Ŀ���
//		tempXdata.Add("JSDW",Glb_BizzInfo.strBzOwner); // ���赥λ
//		tempXdata.Add("XMMC", Glb_BizzInfo.strBizzName); // ��Ŀ����
//		tempXdata.Add("DZDAH",Glb_BizzInfo.strBizzSame); // ���ӵ�����
//		tempXdata.Add("SPSJ",Glb_BizzInfo.strCurTime); // ����ʱ��
////		tempXdata.Add("ZKZH",strUnit); // ѡַ�������
////		tempXdata.Add("JZMD",strAddr); // �����ܶ�
////		tempXdata.Add("JZXG",strAddr); // �����޸�
//
//		tempXdata.Update();
//		tempXdata.Close();
//
//		AfxMessageBox("������Գɹ�!");
//	}

//	// ----- DistCommands._LoadJpg command (do not rename)
//	static void DistCommands_LoadJpg(void)
//	{
//		if (!CheckCommandPower("LoadJpg"))
//			return;
//
//		LockDocument _Lock;
//
//		ads_point p1, p2;  //�������ε������ȡ
//		if (acedGetPoint(NULL, "��ָ��һ����Χ����Esc��ȡ��\n", p1) == RTNORM)
//		{
//			if (acedGetCorner(p1, "��ָ�����SDE���ݵĶԽǶ���\n", p2) != RTNORM) 
//				return;
//
//			if (p1[0] == p2[0] || p1[1] == p2[1]) //���ο�Ϸ��Լ��
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
//		//����ռ���������
//		DIST_STRUCT_SELECTWAY filter;
//		filter.nEntityType = 4;
//		filter.nSelectType = kSM_AI;
//		filter.ptArray.setLogicalLength(0);
//
//		filter.ptArray.append(ptLB);
//		filter.ptArray.append(ptRT);
//
//		//дSQL��ѯCAD��SDEͼ����ձ�
//		char strSQL[200] = {0};
//		sprintf(strSQL,"F:OBJECTID,ͼ��,T:%s,##",Glb_LayerInfo.strMapLayer);
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
//			CString strRemoteFile = "Ӱ��ͼ\\"+strFile;
//			if (!PathFileExists(lpTempFile))
//			{
//				// �����ļ�
//				if (ftpConn.FtpDownLoad(strRemoteFile, lpTempFile) != S_OK)
//				{
//					acutPrintf("\n����Ӱ��ͼ%sʧ��", strFile);
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
//					acutPrintf("\n����Ӱ��ͼ%sʧ��", strFile);
//					continue;
//				}
//*/			}
//
//			int nStartPos = strFile.Find('-');
//			int nEndPos = strFile.Find('.');
//			pt.y = atof(strFile.Left(nStartPos))*10;
//			pt.x = atof(strFile.Mid(nStartPos+1, nEndPos-nStartPos-1))*10;
//
//			//����ɾ��ͼ�����뵱ǰͼ��
//			AcGePoint2dArray pts;
//			pts.append(AcGePoint2d(pt.x, pt.y));
//			pts.append(AcGePoint2d(pt.x+1000, pt.y+1000));
//
//			AcDbObjectId objectId = FindTifAndInsertToCurrent(lpTempPath, strFile, pt, scale, 0, -1, pts);
//			if(objectId == NULL)
//			{
//				acutPrintf("\n%sͼ���ļ�����ʧ�ܣ�\n",strFile);
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
//		if (MessageBox(acedGetAcadFrame()->m_hWnd, "�Ƿ�Ҫ���з���ת����������", "��ʾ", MB_YESNO) == IDNO)
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
//		// ��ȡͼ������
//		for (int i=0; i<arrayFA.GetCount();i++)
//		{
//			acutPrintf("\n����ͼ�� %s...\n", arrayFA.GetAt(i));
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
//			acutPrintf("\n�鵵ͼ�� %s �� %s...\n", strSrcLayer, strDestLayer);
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
//				MessageBox(acedGetAcadFrame()->m_hWnd, "ͼ��鵵����ʧ��!", "��ʾ", MB_OK);
//				return;
//			}
//		}
//
//		gCommitTrans();
//
//		MessageBox(acedGetAcadFrame()->m_hWnd, "ͼ��鵵�����ɹ�!", "��ʾ", MB_OK);
//
////		DistCommands_ReadSdeLayers();  //���µ�ͼ
//
//		// ��ȡͼ������
//		for (i=0; i<arrayFA.GetCount();i++)
//		{
////			acutPrintf("\n����ͼ�� %s...\n", arrayFA.GetAt(i));
//			ReadCadLayer(arrayFA.GetAt(i));
//		}
//		for (i=0; i<arrayDA.GetCount();i++)
//		{
////			acutPrintf("\n����ͼ�� %s...\n", arrayDA.GetAt(i));
//			ReadCadLayer(arrayDA.GetAt(i));
//		}
//	}

	// ----- DistCommands._DeleteAllEntity command (do not rename)
	static void DistCommands_DeleteAllEntity(void)
	{
		if (!CheckCommandPower("DeleteAllEntity"))
			return;

		LockDocument _Lock;

		// �ر����ݿⷴӦ��
		if (Glb_pExchangeData != NULL)
			Glb_pExchangeData->SetDbReactorStatues(false);

		// ѡ������ʵ��
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

			// ɾ��ʵ��
			AcDbEntity * pEnt = NULL;
			acdbOpenObject(pEnt, objId, AcDb::kForWrite); 
			pEnt->erase();
			pEnt->close();                
		}

		acedSSFree(ents);

		// �ر����ݿⷴӦ��
		if (Glb_pExchangeData != NULL)
			Glb_pExchangeData->SetDbReactorStatues(true);
	}

	// ----- DistCommands._DeleteInsertDwg command (do not rename)
	static void DistCommands_DeleteInsertDwg(void)
	{
		if (!CheckCommandPower("DeleteInsertDwg"))
			return;

		LockDocument _Lock;

		// ɾ������sdeͼ���0��,"DIST_DXT","DIST_TK"֮�������ͼ��
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
					(strcmpi(pStrName, "DIST_DXT") == 0) || // ����ͼ
					(strcmpi(pStrName, "DIST_TK") == 0)) // ͼ��
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

		// ����Ŀ¼��
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

		CString strProjectId = DocVars.docData().GetProjectId(); // ���ߵ���Ϊ0,����չ��Ϊ-1

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
//		// �ر����ݿⷴӦ��
//		if (pExchangeData != NULL)
//			pExchangeData->SetDbReactorStatues(false);
//
////		SendCommandToAutoCAD("DISTDYTK_ ");
//		DeleteExternEntities();
//		Sys_PlotEx();
//
//		// �ر����ݿⷴӦ��
//		if (pExchangeData != NULL)
//			pExchangeData->SetDbReactorStatues(true);
//	}

	//// ----- DistCommands._DistBoundary command (do not rename)
	//static void DistCommands_DistBoundary(void) // ��ȡ�����õ�
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

	//	// ��sde����
	//	/*		���õ����=
	//	���õ����=
	//	�����õ����=
	//	�ݻ���=
	//	�����ܶ�=
	//	�����޸�=
	//	�̵���=
	//	ͣ��λ=
	//	*/
	//	CString strLayer;
	//	if (Glb_BizzInfo.strBzId.Find("ѡַ") >= 0)
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
	//	TCHAR lpSQL[256] = {0}; // ���õ�������ݻ��ʣ������ܶȣ������޸ߣ��̵���, ͣ��λ
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

	//		if (Glb_BizzInfo.strBzId.Find("ѡַ") >= 0)
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
	//	if (Glb_BizzInfo.strBzId.Find("ѡַ") >= 0)
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

	//	CString strZYDMJ; // ���õ����
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

	//		if (Glb_BizzInfo.strBzId.Find("ѡַ") >= 0)
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
	//	if (Glb_BizzInfo.strBzId.Find("ѡַ") >= 0)
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

	//	CString strDZYDMJ; // �����õ����
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

	//		if (Glb_BizzInfo.strBzId.Find("ѡַ") >= 0)
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

	//	///////////////////// дini�ļ�
	//	TCHAR lpFileName[MAX_PATH] = {0};
	//	GetModuleFileName(GetModuleHandle("shDistCommands.arx"), lpFileName, MAX_PATH); 

	//	PathRemoveFileSpec(lpFileName);
	//	PathRemoveFileSpec(lpFileName);
	//	PathAppend(lpFileName, "sys");
	//	if (!PathFileExists(lpFileName))
	//		CreateDirectory(lpFileName, NULL);

	//	PathAppend(lpFileName, "SoftConfig.ini");

	//	WritePrivateProfileString("System", "���õ����", "", lpFileName);
	//	WritePrivateProfileString("System", "���õ����", "", lpFileName);
	//	WritePrivateProfileString("System", "�����õ����", "", lpFileName);
	//	WritePrivateProfileString("System", "�ݻ���", "", lpFileName);
	//	WritePrivateProfileString("System", "�����ܶ�", "", lpFileName);
	//	WritePrivateProfileString("System", "�����޸�", "", lpFileName);
	//	WritePrivateProfileString("System", "�̵���", "", lpFileName);
	//	WritePrivateProfileString("System", "ͣ��λ", "", lpFileName);

	//	WritePrivateProfileString("System", "���õ����", strZYDMJ, lpFileName);
	//	WritePrivateProfileString("System", "���õ����", strJYDMJ, lpFileName);
	//	WritePrivateProfileString("System", "�����õ����", strDZYDMJ, lpFileName);
	//	WritePrivateProfileString("System", "�ݻ���", strRJL, lpFileName);
	//	WritePrivateProfileString("System", "�����ܶ�", strJZMD, lpFileName);
	//	WritePrivateProfileString("System", "�����޸�", strJZXG, lpFileName);
	//	WritePrivateProfileString("System", "�̵���", strLDL, lpFileName);
	//	WritePrivateProfileString("System", "ͣ��λ", strTCW, lpFileName);
	//}

	//// ----- DistCommands._ReadSdeData command (do not rename)
	//static void DistCommands_ReadSdeData(void)
	//{
	//	ReadSdeData();

	//	// ���� ����ָ��� ����
	//	SendCommandToAutoCAD("JJZBB ");
	//}

	//// ----- DistCommands._gongshi command (do not rename)
	//static void DistCommands_gongshi(void) // ��ʾ
	//{
	//	if (!CheckCommandPower("gongshi"))
	//		return;

	//	if (gongshi_array.GetCount() <= 0)
	//		return;

	//	CDlgGongShi2 dlg;

	//	if (dlg.DoModal() != IDOK)
	//		return;

	//	LockDocument _Lock;

	//	// ���ļ���������ʱĿ¼�²��ϴ���FTP
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
	//		acutPrintf("\n �洢��ǰͼ����\n");
	//		return;
	//	}

	//	// ���ļ�����һ�ݣ������ϴ�
	//	TCHAR lpLocalFile[MAX_PATH] = {0};
	//	GetTempFileName(lpTempPath, "$SH", 0, lpLocalFile);
	//	CopyFile(lpTempFile, lpLocalFile, FALSE);

	//	// ͨ��cadcontainerʵ��ʣ�๦�ܣ��ϴ�dwg�ļ���update���ݿ��
	//	CString strCommand = "gongshi";
	//	CStringArray strParamArray;

	//	CString strParam;
	//	strParam.Format("file=%s%s", lpLocalFile, ";");
	//	strParamArray.Add(strParam);
	//	strParam.Format("������=%s", dlg.m_strReceiver);
	//	strParamArray.Add(strParam);
	//	strParam.Format("�ⲿ�ļ���ʾ=%d", 0);
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
//		// ������ʾ���
//		TCHAR lpCadProjectFileName[MAX_PATH] = {0};
//		GetModuleFileName(GetModuleHandle("shDistCommands.arx"), lpCadProjectFileName, MAX_PATH); 
//		PathRemoveFileSpec(lpCadProjectFileName);
//
//		PathAppend(lpCadProjectFileName, "CadProject.ini");
//
//		TCHAR lpRule[100] = {0};
//		GetPrivateProfileString("��ʾ", "��Ź���", "%4y", lpRule, 100, lpCadProjectFileName);
//
//		TCHAR lpPhone[200] = {0};
//		GetPrivateProfileString("��ʾ", "��ϵ�绰", "0576-;0576-;0576-", lpPhone, 200, lpCadProjectFileName);
//
//		TCHAR lpAddress[200] = {0};
//		GetPrivateProfileString("��ʾ", "��ϵ��ַ", "̨���н���滮��;̨���н���滮��;̨���н���滮��", lpAddress, 200, lpCadProjectFileName);
//
//		TCHAR lpWebSite[200] = {0};
//		GetPrivateProfileString("��ʾ", "��վ", "̨���н���滮��;̨���н���滮��;̨���н���滮��", lpWebSite, 200, lpCadProjectFileName);
//
//		CString strCommand = "GeneratePublicizeCode";
//		CStringArray strParamArray;
//
//		CString strParam;
//		strParam.Format("��Ź���=%s", lpRule);
//		strParamArray.Add(strParam);
//		strParam.Format("��ʾ����=%d", dlg3.m_nClass);
//		strParamArray.Add(strParam);
//		strParam.Format("��ʾ��Ŀ=%s", dlg3.m_strGSLM);
//		strParamArray.Add(strParam);
//
//		// ������Ϣ��dap�����ɹ�ʾ���
//		SendCopyDataMessage(Glb_hDapForm, strCommand, strParamArray);
//
//		Sleep(200);
//		
//		CDlgGongShi dlg;
//
//		dlg.m_strBHGZ = lpRule; // ��Ź���
//		dlg.m_strLXDH = lpPhone; // ��ϵ�绰
//		dlg.m_strLXDZ = lpAddress; // ��ϵ��ַ
//		dlg.m_strWebSite = lpWebSite; // ��վ
//		dlg.m_nClass = dlg3.m_nClass;
//		dlg.m_nGSLM = dlg3.m_nGSLM;
//		dlg.m_strBJBH = Glb_BizzInfo.strBizzSame; // �������
//		dlg.m_strXMMC = Glb_BizzInfo.strBizzName; // ��Ŀ����
//		dlg.m_strJSDW = Glb_BizzInfo.strBzOwner; // ���赥λ
//		dlg.m_strYDXZ = ""; // �õ�λ��
//		dlg.m_strGCXZ = ""; // ��������
//
//		// ����ǲ�ͬ����˻ص���Ҫ��ȡ�ϴα����ֵ
//		TCHAR lpTemp[100] = {0};
//		GetPrivateProfileString("��ʾ", "�Ƿ��˻�", "0", lpTemp, 100, "BizzInfo.ini");
//		if (strcmpi(lpTemp, "1") == 0)
//		{
//			lpTemp[0] = '\0';
//			GetPrivateProfileString("��ʾ", "���õ����", "", lpTemp, 100, "BizzInfo.ini");
//			dlg.m_strYDZMJ = lpTemp; dlg.m_strYDZMJ.Trim();
//			lpTemp[0] = '\0';
//			GetPrivateProfileString("��ʾ", "���õ����", "", lpTemp, 100, "BizzInfo.ini");
//			dlg.m_strYDJMJ = lpTemp; dlg.m_strYDJMJ.Trim();
//			lpTemp[0] = '\0';
//			GetPrivateProfileString("��ʾ", "�����õ����", "", lpTemp, 100, "BizzInfo.ini");
//			dlg.m_strYDDZMJ = lpTemp; dlg.m_strYDDZMJ.Trim();
//			lpTemp[0] = '\0';
//			GetPrivateProfileString("��ʾ", "�ݻ���", "", lpTemp, 100, "BizzInfo.ini");
//			dlg.m_strRJL = lpTemp; dlg.m_strRJL.Trim();
//			lpTemp[0] = '\0';
//			GetPrivateProfileString("��ʾ", "�����ܶ�", "", lpTemp, 100, "BizzInfo.ini");
//			dlg.m_strJZMD = lpTemp; dlg.m_strJZMD.Trim();
//			lpTemp[0] = '\0';
//			GetPrivateProfileString("��ʾ", "������ģ", "", lpTemp, 100, "BizzInfo.ini");
//			dlg.m_strJZGM = lpTemp; dlg.m_strJZGM.Trim();
//			lpTemp[0] = '\0';
//			GetPrivateProfileString("��ʾ", "�����ܸ�", "", lpTemp, 100, "BizzInfo.ini");
//			dlg.m_strJZZG = lpTemp; dlg.m_strJZZG.Trim();
//			lpTemp[0] = '\0';
//			GetPrivateProfileString("��ʾ", "�̵���", "", lpTemp, 100, "BizzInfo.ini");
//			dlg.m_strLDL = lpTemp; dlg.m_strLDL.Trim();
//			lpTemp[0] = '\0';
//			GetPrivateProfileString("��ʾ", "�õ�λ��", "", lpTemp, 100, "BizzInfo.ini");
//			dlg.m_strYDXZ = lpTemp; dlg.m_strYDXZ.Trim();
//			lpTemp[0] = '\0';
//			GetPrivateProfileString("��ʾ", "��������", "", lpTemp, 100, "BizzInfo.ini");
//			dlg.m_strGCXZ = lpTemp; dlg.m_strGCXZ.Trim();
//		}
//		else
//		{
//			// ��ȡ�õ�������ݻ��ʵ���Ϣ
//			ReadSdeData();
//
////			Sleep(200);
//
//			///////////////////// ��ini�ļ�
//			TCHAR lpFileName[MAX_PATH] = {0};
//			GetModuleFileName(GetModuleHandle("shDistCommands.arx"), lpFileName, MAX_PATH); 
//
//			PathRemoveFileSpec(lpFileName);
//			PathRemoveFileSpec(lpFileName);
//			PathAppend(lpFileName, "\\sys\\SoftConfig.ini");
//
//			lpTemp[0] = '\0';
//			GetPrivateProfileString("System", "���õ����", "", lpTemp, 100, lpFileName);
//			dlg.m_strYDZMJ = lpTemp; dlg.m_strYDZMJ.Trim();
//			lpTemp[0] = '\0';
//			GetPrivateProfileString("System", "���õ����", "", lpTemp, 100, lpFileName);
//			dlg.m_strYDJMJ = lpTemp; dlg.m_strYDJMJ.Trim();
//			lpTemp[0] = '\0';
//			GetPrivateProfileString("System", "�����õ����", "", lpTemp, 100, lpFileName);
//			dlg.m_strYDDZMJ = lpTemp; dlg.m_strYDDZMJ.Trim();
//			lpTemp[0] = '\0';
//			GetPrivateProfileString("System", "�ݻ���", "", lpTemp, 100, lpFileName);
//			dlg.m_strRJL = lpTemp; dlg.m_strRJL.Trim();
//			lpTemp[0] = '\0';
//			GetPrivateProfileString("System", "�����ܶ�", "", lpTemp, 100, lpFileName);
//			dlg.m_strJZMD = lpTemp; dlg.m_strJZMD.Trim();
//			lpTemp[0] = '\0';
//			GetPrivateProfileString("System", "�̵���", "", lpTemp, 100, lpFileName);
//			dlg.m_strLDL = lpTemp; dlg.m_strLDL.Trim();
//		}
//
//		lpTemp[0] = '\0';
//		GetPrivateProfileString("��ʾ", "��ʾ���", "", lpTemp, 100, "BizzInfo.ini");
//		dlg.m_strGSBH = lpTemp;	dlg.m_strGSBH.Trim();
//
//		// Add your code for command DistCommands._gongshi here
//		if (dlg.DoModal() == IDOK)
//		{
//			TCHAR lpFile[100] = {0};
//			GetPrivateProfileString("��ʾ", dlg.m_strGSLX+dlg.m_strGSLM, "", lpFile, 100, lpCadProjectFileName);
//
//			if (lpFile[0] == '\0')
//			{
//				CString strMessage;
//				strMessage.Format("�޷��ҵ� %s %s ��ģ���ļ�������CadProject.ini�ļ������á�", dlg.m_strGSLX, dlg.m_strGSLM);
//				AfxMessageBox(strMessage);
//				return;
//			}
//			CString strDwgFile;
//			strDwgFile.Format("%s%s%s",DistCommands_GetCurArxAppPath("shDistCommands.arx"), "��ʾģ��\\", lpFile);
//
//			CStringArray strTagArray, strTextArray;
//			strTagArray.Add("��Ŀ����");
//			strTagArray.Add("���赥λ");
//			strTagArray.Add("�õ�λ��");
//			strTagArray.Add("��������");
//			strTagArray.Add("��������");
//			strTagArray.Add("����");
//			strTagArray.Add("�ؿ������");
//			strTagArray.Add("�ؿ龻���");
//			strTagArray.Add("������·�̻����");
//			strTagArray.Add("�ݻ���");
//			strTagArray.Add("�����ܶ�");
//			strTagArray.Add("������ģ");
//			strTagArray.Add("�����ܸ�");
//			strTagArray.Add("�̵���");
//			strTagArray.Add("��ʾ���");
//			strTagArray.Add("��ϵ�绰");
//			strTagArray.Add("��ϵ��ַ");
//			strTagArray.Add("��վ");
//
//			CString strParam;
//
//			strTextArray.Add(dlg.m_strXMMC); // ��Ŀ����
//			strTextArray.Add(dlg.m_strJSDW); // ���赥λ
//			strTextArray.Add(dlg.m_strYDXZ); // �õ�λ��
//			strTextArray.Add(dlg.m_strGCXZ); // ��������
//
//			// ��������
////			strParam.Format("%d��%d��%d��-%d��%d��%d��", dlg.m_tmStart.wYear, dlg.m_tmStart.wMonth, dlg.m_tmStart.wDay, dlg.m_tmEnd.wYear, dlg.m_tmEnd.wMonth, dlg.m_tmEnd.wDay);
////			strParam.Format("%d ��", dlg.m_nLimit);
//			strParam.Format("%s��%s��%s��-%s��%s��%s��", "    ", "  ", "  ", "    ", "  ", "  ");
//			strTextArray.Add(strParam);
//			
//			// ����
//			lpTemp[0] = '\0';
//			GetPrivateProfileString("��ʾ", "��ǰʱ��", "", lpTemp, 100, "BizzInfo.ini");
//			strParam = lpTemp;
//			strTextArray.Add(strParam);
//
//			strTextArray.Add(dlg.m_strYDZMJ); // �ؿ������
//			strTextArray.Add(dlg.m_strYDJMJ); // �ؿ龻���
//			strTextArray.Add(dlg.m_strYDDZMJ); // ������·�̻����
//			strTextArray.Add(dlg.m_strRJL); // �ݻ���
//			strTextArray.Add(dlg.m_strJZMD); // �����ܶ�
//			strTextArray.Add(dlg.m_strJZGM); // ������ģ
//			strTextArray.Add(dlg.m_strJZZG); // �����ܸ�
//			strTextArray.Add(dlg.m_strLDL); // �̵���
//			strTextArray.Add(dlg.m_strGSBH); // ��ʾ���
//			strTextArray.Add(dlg.m_strLXDH); // ��ϵ�绰
//			strTextArray.Add(dlg.m_strLXDZ); // ��ϵ��ַ
//			strTextArray.Add(dlg.m_strWebSite); // ��վ
//
//			// �ر����ݿⷴӦ��
//			if (pExchangeData != NULL)
//				pExchangeData->SetDbReactorStatues(false);
//
//			DeleteExternEntities();
//			BOOL bRet = Sys_PlotEx(strDwgFile, strTagArray, strTextArray);
//
//			// �ر����ݿⷴӦ��
//			if (pExchangeData != NULL)
//				pExchangeData->SetDbReactorStatues(true);
//
//			if (!bRet)
//				return;
//
//			gongshi_array.RemoveAll();
//
//			strParam.Format("��Ź���=%s", dlg.m_strBHGZ);
//			gongshi_array.Add(strParam);
//
//			strParam.Format("��Ŀ����=%s", dlg.m_strXMMC);
//			gongshi_array.Add(strParam);
//			strParam.Format("���赥λ=%s", dlg.m_strJSDW);
//			gongshi_array.Add(strParam);
//			strParam.Format("�õ�λ��=%s", dlg.m_strYDXZ);
//			gongshi_array.Add(strParam);
//			strParam.Format("��������=%s", dlg.m_strGCXZ);
//			gongshi_array.Add(strParam);
//			strParam.Format("��ʾ����=%d", dlg.m_nClass);
//			gongshi_array.Add(strParam);
//			strParam.Format("��ʾ��Ŀ=%s", dlg.m_strGSLM);
//			gongshi_array.Add(strParam);
//			strParam.Format("��ʾ��ʽ=%s", dlg.m_strGSXX);
//			gongshi_array.Add(strParam);
//			strParam.Format("�ؿ������=%s", dlg.m_strYDZMJ);
//			gongshi_array.Add(strParam);
//			strParam.Format("�ؿ龻���=%s", dlg.m_strYDJMJ);
//			gongshi_array.Add(strParam);
//			strParam.Format("������·�̻����=%s", dlg.m_strYDDZMJ);
//			gongshi_array.Add(strParam);
//			strParam.Format("�ݻ���=%s", dlg.m_strRJL);
//			gongshi_array.Add(strParam);
//			strParam.Format("�����ܶ�=%s", dlg.m_strJZMD);
//			gongshi_array.Add(strParam);
//			strParam.Format("������ģ=%s", dlg.m_strJZGM);
//			gongshi_array.Add(strParam);
//			strParam.Format("�����ܸ�=%s", dlg.m_strJZZG);
//			gongshi_array.Add(strParam);
//			strParam.Format("�̵���=%s", dlg.m_strLDL);
//			gongshi_array.Add(strParam);
//			strParam.Format("��ʾ���=%s", dlg.m_strGSBH);
//			gongshi_array.Add(strParam);
//			strParam.Format("��ˮ��=%s", dlg.m_strLSH);
//			gongshi_array.Add(strParam);
//			// ��������
//			strParam.Format("��ʾ��ʼʱ��=%02d-%02d-%04d", dlg.m_tmStart.wDay, dlg.m_tmStart.wMonth, dlg.m_tmStart.wYear);
//			gongshi_array.Add(strParam);
//			strParam.Format("��ʾ����ʱ��=%02d-%02d-%04d", dlg.m_tmStart.wDay, dlg.m_tmStart.wMonth, dlg.m_tmStart.wYear);
//			gongshi_array.Add(strParam);
//
//			strParam.Format("��ʾ����=%d", dlg.m_nLimit);
//			gongshi_array.Add(strParam);
//		}
//	}

//	// ----- DistCommands._GongShiEx command
//	static void DistCommands_GongShiEx(void)
//	{
//		if (!CheckCommandPower("gongshi"))
//			return;
//
//		// ������ʾ���
//		TCHAR lpCadProjectFileName[MAX_PATH] = {0};
//		GetModuleFileName(GetModuleHandle("shDistCommands.arx"), lpCadProjectFileName, MAX_PATH); 
//		PathRemoveFileSpec(lpCadProjectFileName);
//
//		PathAppend(lpCadProjectFileName, "CadProject.ini");
//
//		TCHAR lpRule[100] = {0};
//		GetPrivateProfileString("��ʾ", "��Ź���", "%4y", lpRule, 100, lpCadProjectFileName);
//
//		CString strCommand = "GeneratePublicizeCode";
//		CStringArray strParamArray;
//
//		CString strParam;
//		strParam.Format("��Ź���=%s", lpRule);
//		strParamArray.Add(strParam);
//		strParam.Format("��ʾ����=%d", 0);
//		strParamArray.Add(strParam);
//		strParam.Format("��ʾ��Ŀ=%s", "");
//		strParamArray.Add(strParam);
//
//		// ������Ϣ��dap�����ɹ�ʾ���
//		SendCopyDataMessage(Glb_hDapForm, strCommand, strParamArray);
//
//		CDlgGongShi dlg;
//		dlg.m_bExt = TRUE;
//
//		dlg.m_strBHGZ = lpRule; // ��Ź���
//
//		TCHAR lpTemp[100] = {0};
//		GetPrivateProfileString("��ʾ", "��ʾ���", "", lpTemp, 100, "BizzInfo.ini");
//		dlg.m_strGSBH = lpTemp;	dlg.m_strGSBH.Trim();
//
//		if (dlg.DoModal() != IDOK)
//			return;
//
//		gongshi_array.RemoveAll();
//
//		strParam.Format("��Ź���=%s", dlg.m_strBHGZ);
//		gongshi_array.Add(strParam);
//
//		strParam.Format("��Ŀ����=%s", dlg.m_strXMMC);
//		gongshi_array.Add(strParam);
//		strParam.Format("���赥λ=%s", dlg.m_strJSDW);
//		gongshi_array.Add(strParam);
//		strParam.Format("�õ�λ��=%s", dlg.m_strYDXZ);
//		gongshi_array.Add(strParam);
//		strParam.Format("��������=%s", dlg.m_strGCXZ);
//		gongshi_array.Add(strParam);
//		strParam.Format("��ʾ����=%d", dlg.m_nClass);
//		gongshi_array.Add(strParam);
//		strParam.Format("��ʾ��Ŀ=%s", dlg.m_strGSLM);
//		gongshi_array.Add(strParam);
//		strParam.Format("��ʾ��ʽ=%s", dlg.m_strGSXX);
//		gongshi_array.Add(strParam);
//		strParam.Format("�ؿ������=%s", dlg.m_strYDZMJ);
//		gongshi_array.Add(strParam);
//		strParam.Format("�ؿ龻���=%s", dlg.m_strYDJMJ);
//		gongshi_array.Add(strParam);
//		strParam.Format("������·�̻����=%s", dlg.m_strYDDZMJ);
//		gongshi_array.Add(strParam);
//		strParam.Format("�ݻ���=%s", dlg.m_strRJL);
//		gongshi_array.Add(strParam);
//		strParam.Format("�����ܶ�=%s", dlg.m_strJZMD);
//		gongshi_array.Add(strParam);
//		strParam.Format("������ģ=%s", dlg.m_strJZGM);
//		gongshi_array.Add(strParam);
//		strParam.Format("�����ܸ�=%s", dlg.m_strJZZG);
//		gongshi_array.Add(strParam);
//		strParam.Format("�̵���=%s", dlg.m_strLDL);
//		gongshi_array.Add(strParam);
//		strParam.Format("��ʾ���=%s", dlg.m_strGSBH);
//		gongshi_array.Add(strParam);
//		strParam.Format("��ˮ��=%s", dlg.m_strLSH);
//		gongshi_array.Add(strParam);
//
//		// ��������
//		strParam.Format("��ʾ��ʼʱ��=%02d-%02d-%04d", dlg.m_tmStart.wDay, dlg.m_tmStart.wMonth, dlg.m_tmStart.wYear);
//		gongshi_array.Add(strParam);
//		strParam.Format("��ʾ����ʱ��=%02d-%02d-%04d", dlg.m_tmStart.wDay, dlg.m_tmStart.wMonth, dlg.m_tmStart.wYear);
//		gongshi_array.Add(strParam);
//
//		strParam.Format("��ʾ����=%d", dlg.m_nLimit);
//		gongshi_array.Add(strParam);
//
//		CDlgGongShi2 dlg2;
//
//		if (dlg2.DoModal() != IDOK)
//			return;
//
////		// ���ļ���������ʱĿ¼�²��ϴ���FTP
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
//		// ͨ��cadcontainerʵ��ʣ�๦�ܣ��ϴ�dwg�ļ���update���ݿ��
//		strCommand = "gongshi";
//		strParamArray.RemoveAll();
//
//		strParam.Format("file=%s", strFileList);
//		strParamArray.Add(strParam);
//		strParam.Format("������=%s", dlg2.m_strReceiver);
//		strParamArray.Add(strParam);
//		strParam.Format("�ⲿ�ļ���ʾ=%d", 1);
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
	//	// ȡ�ع�ʾ
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
	//	strDwgFile.Format("%s%s%s%s",DistCommands_GetCurArxAppPath("shDistCommands.arx"), "ͼ��ģ��\\", strTKName, ".dwg");

	//	CStringArray arrayTag, arrayValue;

	//	if (strTKName.CompareNoCase("�õ�") == 0)
	//	{
	//		CTKYongDiDlg dlg;
	//		if (dlg.DoModal() != IDOK)
	//			return;

	//		arrayTag.Append(dlg.arrayTag);
	//		arrayValue.Append(dlg.arrayValue);
	//	}
	//	else if (strTKName.CompareNoCase("����") == 0)
	//	{
	//		CTKGongChengDlg dlg;
	//		if (dlg.DoModal() != IDOK)
	//			return;

	//		arrayTag.Append(dlg.arrayTag);
	//		arrayValue.Append(dlg.arrayValue);
	//	}
	//	else
	//		 return;

	//	// �ر����ݿⷴӦ��
	//	if (pExchangeData != NULL)
	//		pExchangeData->SetDbReactorStatues(false);

	//	Sys_PlotEx_NN(strDwgFile, arrayTag, arrayValue);

	//	// �ر����ݿⷴӦ��
	//	if (pExchangeData != NULL)
	//		pExchangeData->SetDbReactorStatues(true);
	//}

	// ----- DistCommands._DeleteXData command (do not rename)
	static void DistCommands_DeleteXData(void)
	{
		//����ѡ��ʵ��
//		BeginEditorCommand();

		ads_name ssName;
		struct resbuf *filter=NULL;
		if (acedSSGet(":S", NULL, NULL, filter, ssName) != RTNORM)
		{
			acutPrintf("\nѡ��ʵ������!");
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

		// ��ȡ���д򹴵�ͼ��
		Sys_GetCheckedLayers(arrayLayers);

		acutPrintf("\n����ͼ��״̬...\n");

		CString strCommand = "SaveLayerState";
		CStringArray arrayParams;

		// ��ȡͼ������
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

		acutPrintf("\n����ɹ���\n");
	}

	// ----- DistCommands._OnExportSdeData command (do not rename)
	static void DistCommands_OnExportSdeData(void)
	{
		// Add your code for command DistCommands._OnExportSdeData here

		if ((Glb_pConnection == NULL) || (Glb_pExchangeData == NULL))
			return;

		// 1.���ͼ��
//		SendCommandToAutoCAD("DeleteAllEntity ");

		DistCommands_DeleteAllEntity();

		// 2.���µ�ͼ
//		SendCommandToAutoCAD("ReadSdeLayers ");

		DistCommands_ReadSdeLayers();

		// 3.���浱ǰͼ�浽�ļ�
		TCHAR lpTempFile[MAX_PATH] = {0};

		GetTempPath(MAX_PATH, lpTempFile);
		GetTempFileName(lpTempFile, "$SH", 0, lpTempFile);

		AcDbDatabase *pDb;
		pDb = acdbHostApplicationServices()->workingDatabase();
		Acad::ErrorStatus er = pDb->saveAs(lpTempFile);
		if (er != Acad::eOk)
		{
			acutPrintf("\n �洢��ǰͼ����\n");
			return;
		}

		TCHAR lpDwgFile[MAX_PATH] = {0};
		sprintf(lpDwgFile, "%s.dwg", Glb_BizzInfo.strBizzCode);
		CFileDialog fileDlg(false, "*.dwg", lpDwgFile, OFN_LONGNAMES | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST,
			"dwg�ļ� (*.dwg)|*.dwg||", acedGetAcadFrame());
		
		if (IDOK == fileDlg.DoModal())
		{
			CString strFileName = fileDlg.GetPathName();
			CopyFile(lpTempFile, strFileName, FALSE);
		}
	}

	//����Ŀ��Χ��
	// ----- DistCommands._DrawXMFWX command (do not rename)
	static void DistCommands_DrawXMFWX(void)
	{
		//int i,k;                    //ѭ������
		//int Ret;                    //��������ֵ
		//long RowId;                 //����SDE���Id
		BOOL IsEdFwLine=FALSE;      //�Ƿ���·�Χ����
		//AcGePoint3d pt1,pt2,pt3,pt4;//��
		//AcGePoint3dArray FwPts;     //��Χ�㼯
		//AcDbObjectId FwLineId;      //��Χ�ߵ�Id
		CString TemStr;             //��ʱ�ַ���
		CString SqlStrs;             //SQL��ѯ���
		//CString CurLayer;           //��ǰ�����ͼ��
		//CStringArray XDataVals;     //��չ����
		//AcDbEntity* pEnt;           //AutoCADʵ��
		CDistEntityTool tempTool;
		
		long lRt = 0;
		IDistConnection* pConnection = Glb_pConnection;
		IDistExchangeData* pExchangeData = Glb_pExchangeData;

		BOOL bReadOnly = TRUE;

		CString strProjectId = DocVars.docData().GetProjectId();

		if (strProjectId.IsEmpty())
		{
			AfxMessageBox("��ǰ�ĵ���֧�ָò���");
			return;
		}

		STB_CONTENT_TREE xxContentTree = Glb_mapTree[strXMFWX];

		if (xxContentTree.strSdeLyName.IsEmpty())
		{
			AfxMessageBox("δ�ҵ���Ŀ��Χ�߶�Ӧ��SDEͼ�㣡", MB_OK);
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
		strWhere.Format("XMID=%s", strProjectId); // ʹ����ĿID
		ShowLayer(strXMFWX, true, strWhere);

		AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();
		if (tempTool.SetCurLayer(strXMFWX, pDB))
		{
			acutPrintf("\n��ǰͼ���л�Ϊ %s\n", strXMFWX);
		}
		else
		{
			acutPrintf("\nδ���ҵ�ͼ�� %s\n", strXMFWX);
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
			lRt=AfxMessageBox("��ǰ��Ŀ��Χ�����Ѿ�����,���Ƿ�Ҫ�滻��ǰ�ķ�Χ����",MB_YESNO);
			if(lRt!=6) return;
			IsEdFwLine=TRUE;
		}
		
		////�������ε������ȡ
		////k=pGeFun->SetOsMode(0);
		//if(acedGetPoint(NULL,"\n ��ȷ����Χ�ĵĵ�һ�ǵ�",asDblArray(pt1))!=RTNORM)
		//{
		//	//pGeFun->SetOsMode(k);
		//	return;
		//}
		//if(acedGetCorner(asDblArray(pt1),
		//	"\n ����ȷ����Χ�ĵڶ��ǵ�",asDblArray(pt2))!=RTNORM)
		//{
		//	//pGeFun->SetOsMode(k);
		//	return;
		//}
		////pGeFun->SetOsMode(k);
		////���ο�Ϸ��Լ��
		//if(pt1==pt2)
		//{
		//	AfxMessageBox("��ָ����ȷ�ķ�Χ����");
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

		//�����Ŀ��Χ��ͼ��������ʵ��
//		tempTool.DeleteAllEntityInLayer(strXMFWX.GetBuffer(), acdbCurDwg());
//		strXMFWX.ReleaseBuffer();

		AcDbObjectIdArray IdArray;
		if(tempTool.SelectEntityInLayer(IdArray, strXMFWX)>0) //ȡ������ʵ��
		{
			AcDbObjectId eId;
			AcDbEntity *pE;

			for(int i=0;i<IdArray.logicalLength();i++)
			{
				eId=IdArray.at(i);

				char strXDataName[1024]={0};
				sprintf(strXDataName,"DIST_%s", xxContentTree.strSdeLyName);
				CDistXData tempXdata(eId,strXDataName);

				CString strXMID; // ʹ����ĿID
				tempXdata.Select("XMID", strXMID);
				int nCount = tempXdata.GetRecordCount();
				tempXdata.Close();

				strXMID.Trim();

				if ((strXMID.CompareNoCase(DocVars.docData().GetProjectId()) == 0 ) && (nCount > 0) )
				{
					// ɾ��ʵ��
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
		if (!gCmdCreateBound(strXMFWX, "��Ŀ��Χ��", objId))
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
		// �Ե�������д���
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

		// ����ռ��������
		DIST_STRUCT_SELECTWAY filter;
		filter.nEntityType = 4;
		filter.nSelectType = kSM_AI;
		filter.ptArray.setLogicalLength(0);

		filter.ptArray.append(AcGePoint3d(ptLB[0]-100, ptLB[1]-100, ptLB[2]));
		filter.ptArray.append(AcGePoint3d(ptRT[0]+100, ptRT[1]+100, ptRT[2]));

		AddRaletionProjects(xxContentTree.strSdeLyName, "XMID", filter);

		return;

		lRt=AfxMessageBox("���浽���ݿ�",MB_YESNO);
		if(lRt!=6)
		{
			//acdbOpenAcDbEntity(pEnt,FwLineId,AcDb::kForWrite);
			//pEnt->erase();
			//pEnt->close();
			//pEnt=NULL;
			////pGeFun->LockLayer(pGeFun->mFwHxLayerDef.SdeToCad.CadLyName,TRUE);

			//�����Ŀ��Χ��ͼ��������ʵ��
			tempTool.DeleteAllEntityInLayer(strXMFWX.GetBuffer(), acdbCurDwg());
			strXMFWX.ReleaseBuffer();
			return;
		}
		//pGeFun->mFwHxLayerDef.LefDwnPt=pt3;
		//pGeFun->mFwHxLayerDef.RigUpPt=pt4;
		if(IsEdFwLine)
		{
			//ԭ���Ѿ����ڷ�Χ����
			//TemStr.Format("%d",pGeFun->mFwHxLayerDef.RowId);
			//XDataVals.Add("OBJECTID:"+TemStr);//RowId
			//XDataVals.Add("SDELYBJ:ITEMSELF");//��ʵ�����ڵ�SDEͼ��ı��
			//TemStr="PROJECT_ID:"+pGeFun->mProjectId;
			//XDataVals.Add(TemStr);            //��ĿID
			//Ret=pGeFun->SetRedXData(XDataVals,FwLineId,"ENTITY_STYLE");
			//if(Ret!=0)
			//{
			//	//pGeFun->LockLayer(pGeFun->mFwHxLayerDef.SdeToCad.CadLyName,TRUE);
			//	return;//�쳣�˳�
			//}
			//Ret=pGeFun->CalFwRedXData(XDataVals);
			//if(Ret!=0)
			//{
			//	//pGeFun->LockLayer(pGeFun->mFwHxLayerDef.SdeToCad.CadLyName,TRUE);
			//	return;//�쳣�˳�
			//}
			//Ret=pGeFun->SetRedXData(XDataVals,FwLineId);//������ȫ��չ����
			//if(Ret!=0)
			//{
			//	//pGeFun->LockLayer(pGeFun->mFwHxLayerDef.SdeToCad.CadLyName,TRUE);
			//	return;//�쳣�˳�
			//}
   //   //���ع����ߵ���������
   //   for(i=0;i<pGeFun->pLyTrGlInfs[0].SySdeCadCfgs.length();i++)
   //   {
   //     Ret=pGeFun->CalWorkRedXData(XDataVals,i);//��������
   //     if(Ret!=0) continue;
   //     TemStr.Format("%d",i);
   //     TemStr="WORKLINE"+TemStr;
   //     Ret=pGeFun->SetRedXData(XDataVals,FwLineId,TemStr);
   //   }
   //   k=pGeFun->mDimSdeLayers.GetSize()/2;
   //   for(i=0;i<k;i++)
   //   {
   //     Ret=pGeFun->CalDimRedXData(XDataVals,i);//����������[��ע]
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

		//XDataVals.Add("OBJECTID:0");      //��ʱ��RowId[Ŀǰ��û������]
		//XDataVals.Add("SDELYBJ:ITEMSELF");//��ʵ�����ڵ�SDEͼ��ı��
		//TemStr="PROJECT_ID:"+pGeFun->mProjectId;
		//XDataVals.Add(TemStr);            //��ĿID
		//pGeFun->SetRedXData(XDataVals,FwLineId,"ENTITY_STYLE");
		//Ret=pGeFun->CalFwRedXData(XDataVals);//������ȫ��չ����
		//if(Ret!=0)
		//{
		//	//pGeFun->LockLayer(pGeFun->mFwHxLayerDef.SdeToCad.CadLyName,TRUE);
  //  OutputDebugString("����Χ����pGeFun->CalFwRedXDataʧ��");
		//	return;//�쳣�˳�
		//}
		//Ret=pGeFun->SetRedXData(XDataVals,FwLineId);//������ȫ��չ����
		//if(Ret!=0)
		//{
		//	//pGeFun->LockLayer(pGeFun->mFwHxLayerDef.SdeToCad.CadLyName,TRUE);
  //  OutputDebugString("����Χ����pGeFun->SetRedXDataʧ��");
		//	return;//�쳣�˳�
		//}
  //  //���ع����ߵ���������
  //  for(i=0;i<pGeFun->pLyTrGlInfs[0].SySdeCadCfgs.length();i++)
  //  {
  //  Ret=pGeFun->CalWorkRedXData(XDataVals,i);//��������
  //  if(Ret!=0) continue;
  //  TemStr.Format("%d",i);
  //  TemStr="WORKLINE"+TemStr;
  //  Ret=pGeFun->SetRedXData(XDataVals,FwLineId,TemStr);
  //  }
  //  k=pGeFun->mDimSdeLayers.GetSize()/2;
  //  for(i=0;i<k;i++)
  //  {
  //  Ret=pGeFun->CalDimRedXData(XDataVals,i);//����������[��ע]
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
  //  OutputDebugString("����Χ����pGeFun->AddRecordToSDEʧ��");
		//	return;
		//}
		////У����չ����
		//TemStr.Format("%d",RowId);
		//TemStr="OBJECTID:"+TemStr;
		//XDataVals[0]=TemStr;
		//Ret=pGeFun->SetRedXData(XDataVals,FwLineId,"ENTITY_STYLE");
		//if(Ret!=0)
		//{
		//	//pGeFun->LockLayer(pGeFun->mFwHxLayerDef.SdeToCad.CadLyName,TRUE);
  //  OutputDebugString("����Χ����pGeFun->SetRedXDataʧ��");
		//	return;//�쳣�˳�
		//}
		//pGeFun->mFwHxLayerDef.IsHasFanWei=TRUE;
		//pGeFun->mFwHxLayerDef.RowId=RowId;
		//pGeFun->mFwHxLayerDef.CadId=FwLineId;

		////pGeFun->LockLayer(pGeFun->mFwHxLayerDef.SdeToCad.CadLyName,TRUE);
		//Ret=pGeFun->DelAllReadEnts();//�Զ�����ȫ��ɾ������
		//if(Ret!=0) OutputDebugString("������Ĺ����з����˴���");

		IsSaveXMFWX = TRUE;
		SaveCadLayer(strXMFWX.GetBuffer());
		strXMFWX.ReleaseBuffer();
		IsSaveXMFWX = FALSE;

		acutPrintf("\n ����ɹ�");

//		AcGePoint3d ptLB, ptRT;
		// ��ȡ���߻��Ʒ�Χ
		if (!GetRangePoints(ptLB, ptRT))
		{
			Glb_Filter.ptArray.setLogicalLength(0);

			acutPrintf("\nû���ҵ����Ƶ���Ŀ��Χ\n");
//			MessageBox(acedGetAcadFrame()->m_hWnd, "û���ҵ����Ƶ���Ŀ��Χ", "������ʾ", MB_OK);
			return;
		}

		//��ѯ�����Ŀ
		CStringArray arrProjctIDs;
		arrProjctIDs.RemoveAll();
		Glb_Filter.ptArray.setLogicalLength(0);

		Glb_Filter.ptArray.append(AcGePoint3d(ptLB[0]-100, ptLB[1]-100, ptLB[2])); //100�׻�����
		Glb_Filter.ptArray.append(AcGePoint3d(ptRT[0]+100, ptRT[1]+100, ptRT[2])); //100�׻�����
		
		strWhere.Format("XMID!=%s", DocVars.docData().GetProjectId()); // ʹ����ĿID
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
			CString strXMID; // ʹ����ĿID
			tempXdata.Select("XMID", strXMID);
			int nCount = tempXdata.GetRecordCount();
			tempXdata.Close();

			//strXMBH.Trim();
			strXMID.Trim();

			//if ((strXMBH.CompareNoCase(Glb_BizzInfo.strBizzCode) != 0 ) && (nCount > 0) )
			if ((strXMID.CompareNoCase(DocVars.docData().GetProjectId()) != 0 ) && (nCount > 0) )
			{
				// ��¼��ĿID
				arrProjctIDs.Add(strXMID);

				// ɾ��ʵ��
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

		// ��������Ŀ
		// ͨ��cadcontainer���
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
		//дSQL��ѯCAD��SDEͼ����ձ�
		char strSQL[200] = {0};
		sprintf(strSQL,"F:OBJECTID,%s,T:%s,##", lpFieldName, lpSdeLayerName);

		CStringArray arrProjctIDs;
		Glb_pExchangeData->GetSearchFieldArray(Glb_pConnection, strSQL, lpSdeLayerName, "shape", &filter, arrProjctIDs);

		// ��������Ŀ
		// ͨ��cadcontainer���
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
		strWhere.Format("XMID!=%s", DocVars.docData().GetProjectId()); // ʹ����ĿID
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
			CString strXMID; // ʹ����ĿID
			tempXdata.Select("XMID", strXMID);
			int nCount = tempXdata.GetRecordCount();
			tempXdata.Close();

			//strXMBH.Trim();
			strXMID.Trim();

			//if ((strXMBH.CompareNoCase(Glb_BizzInfo.strBizzCode) != 0 ) && (nCount > 0) )
			if ((!strXMID.IsEmpty()) && (strXMID.CompareNoCase(DocVars.docData().GetProjectId()) != 0 ) && (nCount > 0) )
			{
				// ��¼��ĿID
				arrProjctIDs.Add(strXMID);

				// ɾ��ʵ��
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

		// ��������Ŀ
		// ͨ��cadcontainer���
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

	//����Ŀ����
	// ----- DistCommands._DrawXMHX command (do not rename)
	static void DistCommands_DrawXMHX(void)
	{
		//long lRt = 0;
		//IDistConnection* pConnection = Glb_pConnection;
		//IDistExchangeData* pExchangeData = Glb_pExchangeData;

		//STB_CONTENT_TREE xxContentTree = Glb_mapTree[strXMHX];

		//if (xxContentTree.strSdeLyName.IsEmpty())
		//{
		//	AfxMessageBox("δ�ҵ���Ŀ���߶�Ӧ��SDEͼ�㣡", MB_OK);
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
		//	acutPrintf("\n��ȡLayerTableʧ�ܣ����Ժ����ԡ�\n");
		//	return;
		//}

		//if (!pLyTable->has(strXMHX))
		//{
		//	acutPrintf("\nû�ҵ���Ҫ��λ�ĺ���ͼ�� %s ,�������ú����ԡ�\n", strXMHX);
		//	pLyTable->close();
		//	return;
		//}

		//pLyTable->close();


		CString strProjectId = DocVars.docData().GetProjectId();

		if (strProjectId.IsEmpty())
		{
			AfxMessageBox("��ǰ�ĵ���֧�ָò���");
			return;
		}

		CDistEntityTool tempTool;
		if (tempTool.SetCurLayer(strXMHX, pDB))
		{
			//acutPrintf("\n��ǰͼ���л�Ϊ %s\n", strXMHX);
		}
		else
		{
			//acutPrintf("\nδ���ҵ�ͼ�� %s\n", strXMHX);

			CString strWhere;
			strWhere.Format("XMID=%s", strProjectId); // ʹ����ĿID

			ShowLayer(strXMHX, true, strWhere);

			//���ͼ��������ʵ��
//			tempTool.DeleteAllEntityInLayer(strXMHX.GetBuffer(), acdbCurDwg());
//			strXMHX.ReleaseBuffer();
		}

//		gCmdCreateBound(strXMHX, "��Ŀ����");

		AcDbObjectIdArray objIds;

		if ((!gCmdCreatePolyLine(strXMHX, "��Ŀ����", objIds, FALSE, TRUE)) ||
			(objIds.logicalLength() <= 0))
			return;

		STB_CONTENT_TREE xxContentTree = Glb_mapTree[strXMHX];

		if (!SaveXMHX(xxContentTree.strNodeName, xxContentTree.strSdeLyName, objIds))
			return;
	}

	//����·����
	// ----- DistCommands._DrawDLHX command (do not rename)
	static void DistCommands_DrawDLHX(void)
	{
		// ��ʼ�༭
		Sys_StartEdit();

		AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();

		IDistConnection* pConnection = Glb_pConnection;
		IDistExchangeData* pExchangeData = Glb_pExchangeData;
		CString strLayer = strDLHX;
			
		if ((DocVars.docData().GetProjectId().CompareNoCase("0") != 0))
		{
			strLayer += "_����";
		}

		CDistEntityTool tempTool;
		if (tempTool.SetCurLayer(strLayer, pDB))
		{
			//acutPrintf("\n��ǰͼ���л�Ϊ %s\n", strDLHX);
		}
		else
		{
			//acutPrintf("\nδ���ҵ�ͼ�� %s\n", strDLHX);

			int nColorIndex;
			CString strLineType;
			AcDb::LineWeight dLineWeight = AcDb::kLnWtByLwDefault;

			pExchangeData->ReadCadLayerInfo(pConnection, strDLHX, nColorIndex, strLineType, dLineWeight);
			AcDbObjectId tempId;
			tempTool.CreateLayer(tempId, strLayer, nColorIndex, strLineType); 
		}

//		gCmdCreateBound(strDLHX, "��·����");
		AcDbObjectIdArray objIds;
		if (!gCmdCreatePolyLine(strLayer, "��·����", objIds, FALSE) ||
			(objIds.logicalLength() <= 0))
			return;


		STB_CONTENT_TREE xxContentTree = Glb_mapTree[strLayer];

		if (!SaveXMHX(xxContentTree.strNodeName, xxContentTree.strSdeLyName, objIds))
			return;
	}

	//����������
	// ----- DistCommands._DrawCSLX command (do not rename)
	static void DistCommands_DrawCSLX(void)
	{
		// ��ʼ�༭
		Sys_StartEdit();

		AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();

		IDistConnection* pConnection = Glb_pConnection;
		IDistExchangeData* pExchangeData = Glb_pExchangeData;
		CString strLayer = strCSLX;

		if ((DocVars.docData().GetProjectId().CompareNoCase("0") != 0))
		{
			strLayer += "_����";
		}

		CDistEntityTool tempTool;
		if (tempTool.SetCurLayer(strLayer, pDB))
		{
			//acutPrintf("\n��ǰͼ���л�Ϊ %s\n", strCSLX);
		}
		else
		{
			//acutPrintf("\nδ���ҵ�ͼ�� %s\n", strCSLX);

			int nColorIndex;
			CString strLineType;
			AcDb::LineWeight dLineWeight = AcDb::kLnWtByLwDefault;

			pExchangeData->ReadCadLayerInfo(pConnection, strCSLX, nColorIndex, strLineType, dLineWeight);
			AcDbObjectId tempId;
			tempTool.CreateLayer(tempId, strLayer, nColorIndex, strLineType); 
		}

//		gCmdCreateBound(strCSLX, "��������");
		AcDbObjectIdArray objIds;
		if (!gCmdCreatePolyLine(strLayer, "��������", objIds, FALSE) ||
			(objIds.logicalLength() <= 0))
			return;

		STB_CONTENT_TREE xxContentTree = Glb_mapTree[strLayer];

		if (!SaveXMHX(xxContentTree.strNodeName, xxContentTree.strSdeLyName, objIds))
			return;
	}

	//�����л���
	// ----- DistCommands._DrawCSHX command (do not rename)
	static void DistCommands_DrawCSHX(void)
	{
		// ��ʼ�༭
		Sys_StartEdit();

		AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();

		IDistConnection* pConnection = Glb_pConnection;
		IDistExchangeData* pExchangeData = Glb_pExchangeData;
		CString strLayer = strCSHX;

		if ((DocVars.docData().GetProjectId().CompareNoCase("0") != 0))
		{
			strLayer += "_����";
		}

		CDistEntityTool tempTool;
		if (tempTool.SetCurLayer(strLayer, pDB))
		{
			//acutPrintf("\n��ǰͼ���л�Ϊ %s\n", strCSHX);
		}
		else
		{
			//acutPrintf("\nδ���ҵ�ͼ�� %s\n", strCSHX);

			int nColorIndex;
			CString strLineType;
			AcDb::LineWeight dLineWeight = AcDb::kLnWtByLwDefault;

			pExchangeData->ReadCadLayerInfo(pConnection, strCSHX, nColorIndex, strLineType, dLineWeight);
			AcDbObjectId tempId;
			tempTool.CreateLayer(tempId, strLayer, nColorIndex, strLineType); 
		}

//		gCmdCreateBound(strCSHX, "���л���");
		AcDbObjectIdArray objIds;
		if (!gCmdCreatePolyLine(strLayer, "���л���", objIds, FALSE) ||
			(objIds.logicalLength() <= 0))
			return;

		STB_CONTENT_TREE xxContentTree = Glb_mapTree[strLayer];

		if (!SaveXMHX(xxContentTree.strNodeName, xxContentTree.strSdeLyName, objIds))
			return;
	}

	//����������
	// ----- DistCommands._DrawCSLVX command (do not rename)
	static void DistCommands_DrawCSLVX(void)
	{
		// ��ʼ�༭
		Sys_StartEdit();

		AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();

		IDistConnection* pConnection = Glb_pConnection;
		IDistExchangeData* pExchangeData = Glb_pExchangeData;
		CString strLayer = strCSLVX;

		if ((DocVars.docData().GetProjectId().CompareNoCase("0") != 0))
		{
			strLayer += "_����";
		}

		CDistEntityTool tempTool;
		if (tempTool.SetCurLayer(strLayer, pDB))
		{
			//acutPrintf("\n��ǰͼ���л�Ϊ %s\n", strCSLVX);
		}
		else
		{
			//acutPrintf("\nδ���ҵ�ͼ�� %s\n", strCSLVX);

			int nColorIndex;
			CString strLineType;
			AcDb::LineWeight dLineWeight = AcDb::kLnWtByLwDefault;

			pExchangeData->ReadCadLayerInfo(pConnection, strCSLVX, nColorIndex, strLineType, dLineWeight);
			AcDbObjectId tempId;
			tempTool.CreateLayer(tempId, strLayer, nColorIndex, strLineType); 
		}

//		gCmdCreateBound(strCSLVX, "��������");
		AcDbObjectIdArray objIds;
		if (!gCmdCreatePolyLine(strLayer, "��������", objIds, FALSE) ||
			(objIds.logicalLength() <= 0))
			return;

		STB_CONTENT_TREE xxContentTree = Glb_mapTree[strLayer];

		if (!SaveXMHX(xxContentTree.strNodeName, xxContentTree.strSdeLyName, objIds))
			return;
	}

	//����������
	// ----- DistCommands._DrawCSZX command (do not rename)
	static void DistCommands_DrawCSZX(void)
	{
		// ��ʼ�༭
		Sys_StartEdit();

		AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();

		IDistConnection* pConnection = Glb_pConnection;
		IDistExchangeData* pExchangeData = Glb_pExchangeData;
		CString strLayer = strCSZX;

		if ((DocVars.docData().GetProjectId().CompareNoCase("0") != 0))
		{
			strLayer += "_����";
		}

		CDistEntityTool tempTool;
		if (tempTool.SetCurLayer(strLayer, pDB))
		{
			//acutPrintf("\n��ǰͼ���л�Ϊ %s\n", strCSZX);
		}
		else
		{
			//acutPrintf("\nδ���ҵ�ͼ�� %s\n", strCSZX);

			int nColorIndex;
			CString strLineType;
			AcDb::LineWeight dLineWeight = AcDb::kLnWtByLwDefault;

			pExchangeData->ReadCadLayerInfo(pConnection, strCSZX, nColorIndex, strLineType, dLineWeight);
			AcDbObjectId tempId;
			tempTool.CreateLayer(tempId, strLayer, nColorIndex, strLineType); 
		}

//		gCmdCreateBound(strCSZX, "��������");
		AcDbObjectIdArray objIds;
		if (!gCmdCreatePolyLine(strLayer, "��������", objIds, FALSE) ||
			(objIds.logicalLength() <= 0))
			return;

		STB_CONTENT_TREE xxContentTree = Glb_mapTree[strLayer];

		if (!SaveXMHX(xxContentTree.strNodeName, xxContentTree.strSdeLyName, objIds))
			return;
	}

	//�����г���
	// ----- DistCommands._DrawCSCX command (do not rename)
	static void DistCommands_DrawCSCX(void)
	{
		// ��ʼ�༭
		Sys_StartEdit();

		AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();

		IDistConnection* pConnection = Glb_pConnection;
		IDistExchangeData* pExchangeData = Glb_pExchangeData;
		CString strLayer = strCSCX;

		if ((DocVars.docData().GetProjectId().CompareNoCase("0") != 0))
		{
			strLayer += "_����";
		}

		CDistEntityTool tempTool;
		if (tempTool.SetCurLayer(strLayer, pDB))
		{
			//acutPrintf("\n��ǰͼ���л�Ϊ %s\n", strCSCX);
		}
		else
		{
			//acutPrintf("\nδ���ҵ�ͼ�� %s\n", strCSCX);
			int nColorIndex;
			CString strLineType;
			AcDb::LineWeight dLineWeight = AcDb::kLnWtByLwDefault;

			pExchangeData->ReadCadLayerInfo(pConnection, strCSCX, nColorIndex, strLineType, dLineWeight);
			AcDbObjectId tempId;
			tempTool.CreateLayer(tempId, strLayer, nColorIndex, strLineType); 
		}

//		gCmdCreateBound(strCSCX, "���г���");
		AcDbObjectIdArray objIds;
		if (!gCmdCreatePolyLine(strLayer, "���г���", objIds, FALSE) ||
			(objIds.logicalLength() <= 0))
			return;

		STB_CONTENT_TREE xxContentTree = Glb_mapTree[strLayer];

		if (!SaveXMHX(xxContentTree.strNodeName, xxContentTree.strSdeLyName, objIds))
			return;
	}

	// ----- DistCommands._DrawDLZXX command (do not rename)
	static void DistCommands_DrawDLZXX(void)
	{
		// ��ʼ�༭
		Sys_StartEdit();

		AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();

		IDistConnection* pConnection = Glb_pConnection;
		IDistExchangeData* pExchangeData = Glb_pExchangeData;
		CString strLayer = strDLZXX;

		if ((DocVars.docData().GetProjectId().CompareNoCase("0") != 0))
		{
			strLayer += "_����";
		}

		CDistEntityTool tempTool;
		if (tempTool.SetCurLayer(strLayer, pDB))
		{
			//acutPrintf("\n��ǰͼ���л�Ϊ %s\n", strCSCX);
		}
		else
		{
			//acutPrintf("\nδ���ҵ�ͼ�� %s\n", strCSCX);
			int nColorIndex;
			CString strLineType;
			AcDb::LineWeight dLineWeight = AcDb::kLnWtByLwDefault;

			pExchangeData->ReadCadLayerInfo(pConnection, strCSCX, nColorIndex, strLineType, dLineWeight);
			AcDbObjectId tempId;
			tempTool.CreateLayer(tempId, strLayer, nColorIndex, strLineType); 
		}

		//		gCmdCreateBound(strCSCX, "��·������");
		AcDbObjectIdArray objIds;
		if (!gCmdCreatePolyLine(strLayer, "��·������", objIds, FALSE) ||
			(objIds.logicalLength() <= 0))
			return;

		STB_CONTENT_TREE xxContentTree = Glb_mapTree[strLayer];

		if (!SaveXMHX(xxContentTree.strNodeName, xxContentTree.strSdeLyName, objIds))
			return;
	}

	// ----- DistCommands._DrawHDZXX command (do not rename)
	static void DistCommands_DrawHDZXX(void)
	{
		// ��ʼ�༭
		Sys_StartEdit();

		AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();

		IDistConnection* pConnection = Glb_pConnection;
		IDistExchangeData* pExchangeData = Glb_pExchangeData;
		CString strLayer = strHDZXX;

		if ((DocVars.docData().GetProjectId().CompareNoCase("0") != 0))
		{
			strLayer += "_����";
		}

		CDistEntityTool tempTool;
		if (tempTool.SetCurLayer(strLayer, pDB))
		{
			//acutPrintf("\n��ǰͼ���л�Ϊ %s\n", strCSCX);
		}
		else
		{
			//acutPrintf("\nδ���ҵ�ͼ�� %s\n", strCSCX);
			int nColorIndex;
			CString strLineType;
			AcDb::LineWeight dLineWeight = AcDb::kLnWtByLwDefault;

			pExchangeData->ReadCadLayerInfo(pConnection, strCSCX, nColorIndex, strLineType, dLineWeight);
			AcDbObjectId tempId;
			tempTool.CreateLayer(tempId, strLayer, nColorIndex, strLineType); 
		}

		//		gCmdCreateBound(strCSCX, "�ӵ�������");
		AcDbObjectIdArray objIds;
		if (!gCmdCreatePolyLine(strLayer, "�ӵ�������", objIds, FALSE) ||
			(objIds.logicalLength() <= 0))
			return;

		STB_CONTENT_TREE xxContentTree = Glb_mapTree[strLayer];

		if (!SaveXMHX(xxContentTree.strNodeName, xxContentTree.strSdeLyName, objIds))
			return;
	}

	//��ѯ�����Ŀ
	// ----- DistCommands._DistSelProject command (do not rename)
	static void DistCommands_DistSelProject(void)
	{

	}

	//��ѯ�����Ŀ
	// ----- DistCommands._LoadMenu command (do not rename)
	static void DistCommands_LoadMenu(void)
	{
		Sys_LoadDistMenu("�滮��ͼ");
	}

	//��ѯ�����Ŀ
	// ----- DistCommands._LoadMenu2 command (do not rename)
	static void DistCommands_LoadMenu2(void)
	{
		Sys_LoadDistMenu("���ߵ���");
	}

	//��ѯ�����Ŀ
	// ----- DistCommands._UnLoadMenu command (do not rename)
	static void DistCommands_UnLoadMenu(void)
	{
		Sys_UnLoadDistMenu("�滮��ͼ");
	}
	// ----- DistCommands._UnLoadMenu2 command (do not rename)
	static void DistCommands_UnLoadMenu2(void)
	{
		Sys_UnLoadDistMenu("���ߵ���");
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

		CString strPrompt = "\nѡ������ (1)�޸����� (2)ɾ������ ";
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
				"��·����","��·������","��·��ʩ��","��·ע��","��·������","��·������","��·��ʩ��","��·ע��","���߸�����",

				"�ӵ�����","�ӵ�������","�ӵ�������","���߸�����","�ӵ�ע��",

				"���л���","���߸�����","����ע��",

				"��������","���߸�����","����ע��",

				"��������","���߸�����","����ע��",

				"���г���","���߸�����","����ע��",
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
							if(tempXData.GetRecordCount()>0)  //��չ���Դ��ڣ����޸�
							{
								tempXData.Select("OBJECTID", strObjectId);
								if (ObjectIdMaps[strObjectId] == "1") // ����Ѿ����ڣ������߶δ�ϻ�������������
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
	static void DistCommandsStartEdit(void) // ��ʼ�༭
	{
		// Add your code for command DistCommands.StartEdit here
		dataContainer.m_mapNewData.RemoveAll();
		dataContainer.m_mapAttrEditData.RemoveAll();
		dataContainer.m_mapShapeEditData.RemoveAll();
		dataContainer.m_mapDelData.RemoveAll();

		AcApDocument *pDoc = acDocManager->curDocument();
		acDocManager->lockDocument(pDoc);

		//������Ӧ�� 
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
	static void DistCommandsEndEdit(void) // �����༭
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

		// ����������Ҫ�����չ���ԣ��߶δ�ϻ��߸��Ƶ��������չ�������صģ�
		POSITION pos = dataContainer.m_mapNewData.GetStartPosition();

		while(pos)
		{
			dataContainer.m_mapNewData.GetNextAssoc(pos,rCADId,rAttrData);

			AcDbObjectId tmpObjId;

			tmpObjId.setFromOldId(rCADId);

			CDistXData tempXData(tmpObjId, "ROWID_OBJS");
			CString strObjectId;
			if(tempXData.GetRecordCount()>0)  // ɾ����չ����
			{
				tempXData.DeleteAllRecord();
				tempXData.Close();
			}
			tempXData.Close();
		}

		CString layers[] = 
		{
			"��·����","��·������","��·��ʩ��","��·ע��","��·������","��·������","��·��ʩ��","��·ע��","���߸�����",

			"�ӵ�����","�ӵ�������","�ӵ�������","���߸�����","�ӵ�ע��",

			"���л���","���߸�����","����ע��",

			"��������","���߸�����","����ע��",

			"��������","���߸�����","����ע��",

			"���г���","���߸�����","����ע��",
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

			// ����
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

			// �޸�
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

			// ɾ��
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
				// �����޸���ʷ
				CString strHistoryLayer = strSdeLayer + "_LS";
				CopyRecordsToHistory(pConnection, historyRowIds, strSdeLayer, strHistoryLayer, Glb_BizzInfo.strLoginUserName, nEditRowId);
			}

			if (newObjIds.logicalLength() > 0) // ����
			{
				SaveSdeLayerInsert(pConnection, pExchangeData, strSdeLayer, newObjIds, false);
			}
			if (modifyObjIds.logicalLength() > 0) // �޸�
			{
				SaveSdeLayerModify(pConnection, pExchangeData, strSdeLayer, modifyObjIds, false);
			}
			if (deleteRowIds.GetCount() > 0) // ɾ��
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
	static void DistCommandsCancelEdit(void) // ȡ���༭
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

		//�����ĵ�
		LockDocument lock; 

		AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();

		if (NULL == pDB)
			return;

		// ��ʱ��֧�ֶ�ѡ
		AcDbObjectIdArray EntIds;

		ads_name ssName;
		if(acedSSGet(NULL,NULL,NULL,NULL,ssName)!=RTNORM)
			return;

		SSToIds(ssName,EntIds);
		acedSSFree(ssName);

		// ����ѡ��ʵ���ȡͼ�����б�
		if (EntIds.logicalLength() > 1)
		{
			AfxMessageBox("��֧�ֶ�ѡ��");
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

		Glb_BizzInfo.strLoginUserName = "��������Ա";
		Glb_BizzInfo.strUserId = "621";
		Glb_BizzInfo.strBizzCode = "��Ŀ���";
		Glb_BizzInfo.strBizzName = "��Ŀ����";
		Glb_BizzInfo.strBizzName = "���赥λ";

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
		WritePrivateProfileString("BizzInfo","��Ŀ���",Glb_BizzInfo.strBizzCode,strIniFile);
		WritePrivateProfileString("BizzInfo","��Ŀ����",Glb_BizzInfo.strBizzName,strIniFile);
		WritePrivateProfileString("BizzInfo","���赥λ",Glb_BizzInfo.strBzOwner,strIniFile);
		WritePrivateProfileString("BizzInfo","ҵ������",Glb_BizzInfo.strBzId,strIniFile);
		WritePrivateProfileString("BizzInfo","��������",Glb_BizzInfo.strBztmOver,strIniFile);
*/

		// �о�SDE���Ӳ���
		Glb_SdeInfo.strServer = "192.168.1.23"; //parameters["server"];
		Glb_SdeInfo.strService = "5151"; //parameters["service"];
		Glb_SdeInfo.strDatabase = ""; //parameters["database"];
		Glb_SdeInfo.strUserName = "csdata"; //parameters["username"];
		Glb_SdeInfo.strPassword = "csdata"; //parameters["password"];

		// ����SDE���ݿ�
		CreateSdeConnection(Glb_SdeInfo.strServer.GetBuffer(), Glb_SdeInfo.strService.GetBuffer(), Glb_SdeInfo.strDatabase.GetBuffer(), Glb_SdeInfo.strUserName.GetBuffer(), Glb_SdeInfo.strPassword.GetBuffer());

		// SDEͼ������
	/*	Glb_LayerInfo.strAnnotionLine = parameters["��ע�߲�"]; // ��ע�߲�
		Glb_LayerInfo.strAnnotionText = parameters["��ע���ֲ�"]; // ��ע���ֲ�
		Glb_LayerInfo.strLayer = parameters["��Χͼ��"];
		Glb_LayerInfo.strField = parameters["��Χͼ���ֶ�"];
		Glb_LayerInfo.strMapLayer = parameters["����ͼ"];
		Glb_LayerInfo.strIndexField = parameters["����ͼ�����ֶ�"];

		if (Glb_LayerInfo.strIndexField.IsEmpty())
			Glb_LayerInfo.strIndexField = "ͼ��";
*/
		// ���ͼ����Ϣ
		Glb_layerArray.RemoveAll();

		CString strProjectId = "0";
		DocVars.docData().SetProjectId(strProjectId);

		if (strProjectId.CompareNoCase("0") == 0) // ���ߵ���ʱProjectIdΪ0
		{
			Sys_UnLoadDistMenu("�滮��ͼ");
			Sys_LoadDistMenu("���ߵ���");
		}
		else if (strProjectId.CompareNoCase("-1") == 0) // ����չ��
		{
			Sys_UnLoadDistMenu("���ߵ���");
			Sys_UnLoadDistMenu("�滮��ͼ");
		}
		else
		{
			g_bAutoReadSdeData = TRUE;
			Sys_UnLoadDistMenu("���ߵ���");
			Sys_LoadDistMenu("�滮��ͼ");
		}

		Glb_mapCheck.clear();

		if (strProjectId.CompareNoCase("-1") == 0) // ����չ��
		{
			return;
		}

		// ����ͼ����
		LoadLayerTree();

		if (strProjectId.CompareNoCase("0") == 0) // ���ߵ���
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

// �Զ���Ĵ��ں���
LRESULT CALLBACK DistMainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch( message )
	{
	case WM_COPYDATA:
		{
			COPYDATASTRUCT * cds = (COPYDATASTRUCT*)lParam;

			if (cds->dwData == 1) // ϵͳ����
			{
				break;
			}
			else if (cds->dwData == 2) // �Զ�������
			{
				CString strCommand = (LPCTSTR)cds->lpData;
				if (strCommand.CompareNoCase("��ʼ�༭") == 0)
				{
					acutPrintf("\n��ʼ�༭��\n");
					CDistCommandsApp::DistCommandsStartEdit();
				}
				else if (strCommand.CompareNoCase("�����༭") == 0)
				{
					acutPrintf("\n�����༭��\n");
					CDistCommandsApp::DistCommandsEndEdit();
				}
				else if (strCommand.CompareNoCase("ȡ���༭") == 0)
				{
					acutPrintf("\nȡ���༭��\n");
					CDistCommandsApp::DistCommandsCancelEdit();
				}
				break;
			}
			else // �Զ�����Ϣ
			{
			}

			CString strXML = (LPCTSTR)cds->lpData;

			OutputDebugString("WM_COPYDATA");

			CString strCommand;
			MapString2String parameters;

			// �ֽ����
			if (!ParseParameters(strXML, strCommand, parameters))
				return 0;

			// ��������
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
//ACED_ARXCOMMAND_ENTRY_AUTO(CDistCommandsApp, DistCommands, _TuKuang, TuKuang, ACRX_CMD_TRANSPARENT, NULL) // ֻ���������Ŀ
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
