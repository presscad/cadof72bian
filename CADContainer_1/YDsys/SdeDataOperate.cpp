#include "StdAfx.h"
#include "SdeDataOperate.h"
#include "..\SdeData\DistBaseInterface.h"
#include "..\SdeData\DistExchangeInterface.h"

#include "DistEntityTool.h"
#include "DistPrjCommand.h"
#include "DistXData.h"


extern IDistConnection*    g_pConnection;         //SDE数据库连接对象指针
extern IDistExchangeData*  g_pExchangeData;       //SDE<->CAD数据交换对象指针

CSdeDataOperate::CSdeDataOperate(void)
{
	//m_pConnection = NULL;
	//m_pExchangeData = NULL;
	memset(m_strGdbOwner,0,sizeof(char)*255);
}

CSdeDataOperate::~CSdeDataOperate(void)
{

}


long CSdeDataOperate::ReadAllSdeLayer(AcDbObjectIdArray& arrId,
									  char* strSdeLyName,
								      DIST_STRUCT_SELECTWAY* pFilter,
									  char* strFieldSql,
									  int nPower)
{
	if(NULL == g_pConnection)
	{
		acutPrintf("\n空间数据库没有连接！"); return 0;
	}

	if(IsDKLayer(strSdeLyName))
	{
		OutputDebugString("IsDKLayer OK");
		return ReadDKLayer(arrId,strSdeLyName,pFilter,strFieldSql);
	}

	if(IsXMLayer(strSdeLyName))
	{
		OutputDebugString("IsXMLayer OK");
		return ReadXMLayer(arrId,strSdeLyName,pFilter,strFieldSql);
	}

	if(IsDKLayerOld(strSdeLyName))
	{
		OutputDebugString("IsDKLayerOld OK");
		return ReadDKLayerOld(arrId,strSdeLyName,pFilter,strFieldSql);
	}

	int i;
	long nResult = 0;
	CString sSdeLN = strSdeLyName;


	// 处理通配符
	if (sSdeLN.Find('*') >= 0)
	{
		IDistCommand* pCmd = CreateSDECommandObjcet();
		pCmd->SetConnectObj(g_pConnection);

		long nNameCount = 0;
		char ** strAllName = NULL;
		nResult = pCmd->GetCurUserAllLyName(&strAllName, &nNameCount);
		if (nResult != 1 || nNameCount == 0)
		{
			pCmd->Release(); return nResult;
		}

		CStringArray strSdeLyNameArray;
		for (i = 0; i< nNameCount; i++)
		{
			CString sLNtemp = strAllName[i];
			if (CmpTwoString(sSdeLN, sLNtemp))
			{
				strSdeLyNameArray.Add(sLNtemp);
			}
		}
		pCmd->Release();

		
		for (i= 0; i< strSdeLyNameArray.GetSize(); i++)
		{
			AcDbObjectIdArray arrTemp;	
			arrTemp.setLogicalLength(0);
			if(i>0)
				gChangeEmpty(FALSE);

			nResult = ReadSdeLayer(arrTemp, strSdeLyNameArray.GetAt(i), pFilter, strFieldSql,NULL,nPower); //插入字段

			if (nResult != 1)
			{
				gChangeEmpty(TRUE);
				return nResult;
			}
			arrId.append(arrTemp);
		}
		gChangeEmpty(TRUE);

		return 1;
	}
	else
	{
		
		return ReadSdeLayer(arrId, strSdeLyName, pFilter, strFieldSql,NULL,nPower);
	}
}

long CSdeDataOperate::SaveAllSdeLayer(const char* strSdeLyName,DIST_STRUCT_SELECTWAY* pFilter /* = NULL */,
								      bool bAfterSaveToDelete /* = true */)
{
	long lRt = 0;

	CString sSdeLN = strSdeLyName;
	// 处理通配符
	if (sSdeLN.Find('*') >= 0)
	{
		IDistCommand* pCmd = CreateSDECommandObjcet();
		pCmd->SetConnectObj(g_pConnection);

		char ** strAllName = NULL;
		long lCount = 0;
		lRt = pCmd->GetCurUserAllLyName(&strAllName, &lCount);
		if (lRt != 1 || lCount == 0)
		{
			pCmd->Release();
			return lRt;
		}

		CStringArray arraySdeLN;
		for (long l = 0; l < lCount; l++)
		{
			CString sLNtemp = strAllName[l];
			if (CmpTwoString(sSdeLN, sLNtemp))
			{
				arraySdeLN.Add(sLNtemp);
			}
		}
		pCmd->Release();

		for (long l = 0; l < arraySdeLN.GetSize(); l++)
		{
			lRt = SaveSdeLayer(arraySdeLN.GetAt(l), pFilter, bAfterSaveToDelete);
			if (lRt != 1)
			{
				return lRt;
			}
		}

		return 1;
	}
	else
	{
		return SaveSdeLayer(strSdeLyName, pFilter, bAfterSaveToDelete);
	}
}

long CSdeDataOperate::ReadSdeLayer(AcDbObjectIdArray& arrId,        //返回创建CAD实体ID
								   const char* strSdeLyName,        //SDE图层名称
								   DIST_STRUCT_SELECTWAY* pFilter,  //空间过滤条件
								   const char* strFieldSql,         //Where条件
								   CStringArray* pFldArray,         //查询字段
								   int nPower)                      //读写权限
{
	if(NULL == g_pConnection)
	{
		acutPrintf("\n空间数据库没有连接！"); return 0;
	}

	OutputDebugString("ReadSdeLayer start");

	long nResult = 0;
	CStringArray strFNameArray;
	strFNameArray.RemoveAll();

	if(NULL != pFldArray)
	{
		if(pFldArray->GetCount()>0)
		{
			CString strTemp = pFldArray->GetAt(0);
			if(strTemp.Find('*')>=0) //选择全部字段
			{
				IDistCommand* pCmd = CreateSDECommandObjcet();
				pCmd->SetConnectObj(g_pConnection);

				IDistParameter* pParam = NULL;
				int nParamNum = 0;
				nResult = pCmd->GetTableInfo(&pParam, &nParamNum, strSdeLyName, m_strGdbOwner);
				if (nResult != 1)
				{
					pCmd->Release(); acutPrintf("\n%s",ReturnErrorCodeInfo(nResult));
					return nResult;
				}

				for (int i = 0; i < nParamNum; i++)
				{
					if(pParam[i].fld_nType == kShape) continue;
					if(pParam[i].fld_nRowIdType == kRowIdColumnSde) continue;
					strFNameArray.Add(pParam[i].fld_strName);
				}
				pCmd->Release();
			}
			else
				strFNameArray.Append(*pFldArray);
		}
	}
	
	//合成注册扩展属性名称
	char strRegName[255]={0};
	sprintf(strRegName,"DIST_%s",strSdeLyName);

	
	//读配置信息
	CStringArray* pFNameArray = NULL;
	if(strFNameArray.GetCount() > 0)
		pFNameArray = &strFNameArray;
	nResult = g_pExchangeData->ReadSymbolInfo(g_pConnection, NULL, NULL, strSdeLyName, strRegName, pFNameArray);
	if (nResult != 1)
	{
		acutPrintf("\n%s",ReturnErrorCodeInfo(nResult)); return nResult;
	}

	//读SDE空间数据
	nResult= g_pExchangeData->ReadSdeDBToCAD(&arrId, pFilter, strFieldSql, TRUE, (1 == nPower));
	if (nResult != 1)
	{
		acutPrintf("\n%s",ReturnErrorCodeInfo(nResult)); return nResult;
	}
	return 1;
}



long CSdeDataOperate::SaveSdeLayer(const char* strSdeLyName,
								DIST_STRUCT_SELECTWAY* pFilter /* = NULL */,
								bool bAfterSaveToDelete /* = true */)
{
	if(NULL == g_pConnection)
	{
		acutPrintf("\n空间数据库没有连接！"); return 0;
	}

	long nResult = 0;

	CStringArray saFieldName;

	IDistCommand* pCmd = CreateSDECommandObjcet();
	pCmd->SetConnectObj(g_pConnection);

	IDistParameter* pParam = NULL;
	int nParamNum = 0;
	nResult = pCmd->GetTableInfo(&pParam, &nParamNum, strSdeLyName, m_strGdbOwner);
	if (nResult != 1)
	{
		pCmd->Release();
		return nResult;
	}

	for (int i = 0; i < nParamNum; i++)
	{
		CString sFieldName = pParam[i].fld_strName;

		if (sFieldName.CollateNoCase("SHAPE") == 0)
		{
			continue;
		}

		saFieldName.Add(sFieldName);
	}
	pCmd->Release();

	CString sAppName = strSdeLyName;
	sAppName = "DIST_" + sAppName;
	//sAppName = "DIST_XDATA";

	nResult = g_pExchangeData->ReadSymbolInfo(g_pConnection, NULL, NULL, strSdeLyName, sAppName, &saFieldName);
	if (nResult != 1)
	{
		return nResult;
	}
	return g_pExchangeData->SaveEntityToSdeDB(pFilter, bAfterSaveToDelete);
}

// 比较包含通配符*的字符串，匹配返回TRUE，否则返回FALSE
BOOL CSdeDataOperate::CmpTwoString(CString str1, CString str2)
{
	int pos1 = str1.Find('*');
	int pos2 = str2.Find('*');
	if (pos1 == -1 && pos2 == -1)
	{
		if (str1.Collate(str2) == 0)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else if (pos1 != -1 && pos2 == -1)
	{
		CString sLeft = str1.Left(pos1);
		CString sRight = str1.Right(str1.GetLength() - pos1 - 1);
		if (str2.Left(sLeft.GetLength()).Collate(sLeft) != 0)
		{
			return FALSE;
		}
		else if (str2.Right(sRight.GetLength()).Collate(sRight) != 0)
		{
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
	else if (pos1 == -1 && pos2 != -1)
	{
		CString sLeft = str2.Left(pos2);
		CString sRight = str2.Right(str2.GetLength() - pos2 - 1);
		if (str1.Left(sLeft.GetLength()).Collate(sLeft) != 0)
		{
			return FALSE;
		}
		else if (str1.Right(sRight.GetLength()).Collate(sRight) != 0)
		{
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
	else
	{
		CString sLeft1 = str1.Left(pos1);
		CString sRight1 = str1.Right(str1.GetLength() - pos1 - 1);
		CString sLeft2 = str2.Left(pos2);
		CString sRight2 = str2.Right(str2.GetLength() - pos2 - 1);
		if ((sLeft1.Find(sLeft2) != -1 || sLeft2.Find(sLeft1) != -1) &&
			(sRight1.Find(sRight2) != -1 || sRight2.Find(sRight1) != -1))
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
}


void CSdeDataOperate::GetCadLayerBySdeLayer( CStringArray& strCadLyNameArray,const CStringArray& strSdeLyNameArray)
{
	if(NULL == g_pConnection)
	{
		acutPrintf("\n空间数据库没有连接！"); return;
	}

	strCadLyNameArray.RemoveAll();
	IDistCommand* pCmd = CreateSDECommandObjcet();
	pCmd->SetConnectObj(g_pConnection);

	int nCount = strSdeLyNameArray.GetSize();

	for (int i = 0; i < nCount; i++)
	{
		CString sLikeSql;
		sLikeSql.Format("F:CADLYNAME,T:CADMAPTOSDE,W:SDELYNAME = \'%s\',##",strSdeLyNameArray.GetAt(i));
		IDistRecordSet* pSdeRst = NULL;
		long lRt = pCmd->SelectData(sLikeSql.GetBuffer(), &pSdeRst);
		if (lRt != 1)
		{
			//AfxMessageBox("查找CAD图层名失败.SelectData");
			continue;
		}

		if (pSdeRst == NULL)
		{
			//AfxMessageBox("查找CAD图层名失败.IDistRecordSet");
			continue;
		}

		int RecordCount = pSdeRst->GetRecordCount();
		if (RecordCount != 1)
		{
			//AfxMessageBox("查找CAD图层名失败.RecordCount");
			pSdeRst->Release();
			continue;
		}

		if (pSdeRst->BatchRead()==1)
		{
			CString strTemp = (char*)((*pSdeRst)["CADLYNAME"]);
			strCadLyNameArray.Add(strTemp);
		}
		pSdeRst->Release();
	}
	pCmd->Release();
}

void CSdeDataOperate::GetSdeLayerByCadLayer(CStringArray& strSdeLyNameArray,const CStringArray& strCadLyNameArray)
{
	if(NULL == g_pConnection)
	{
		acutPrintf("\n空间数据库没有连接！"); return;
	}

	strSdeLyNameArray.RemoveAll();
	IDistCommand* pCmd = CreateSDECommandObjcet();
	pCmd->SetConnectObj(g_pConnection);

	int nCount = strCadLyNameArray.GetSize();
	for (int i = 0; i < nCount; i++)
	{
		CString sLikeSql;
		sLikeSql.Format("F:SDELYNAME,T:CADMAPTOSDE,W:CADLYNAME = \'%s\',##",strCadLyNameArray.GetAt(i));
		IDistRecordSet* pSdeRst = NULL;
		long lRt = pCmd->SelectData(sLikeSql.GetBuffer(), &pSdeRst);
		if (lRt != 1)
		{
			//AfxMessageBox("读取SDE图层名失败.SelectData");
			continue;
		}

		if (pSdeRst == NULL)
		{
			//AfxMessageBox("读取SDE图层名失败.IDistRecordSet");
			continue;
		}

		int RecordCount = pSdeRst->GetRecordCount();
		if (RecordCount != 1)
		{
			//AfxMessageBox("读取SDE图层名失败.RecordCount");
			pSdeRst->Release();
			continue;
		}

		if (pSdeRst->BatchRead()==1)
		{
			CString sSdeLayer = (char*)((*pSdeRst)["SDELYNAME"]);
			strSdeLyNameArray.Add(sSdeLayer);
		}
		pSdeRst->Release();
	}
	pCmd->Release();
}

//获取指定SDE层中指定条件下SHAPE的点数组
BOOL CSdeDataOperate::GetSdeOneShapePoints(AcGePoint3dArray& ptArray,const char* strSdeLyName,
									const char* strFName,const char* strFValue)
{
	char strSQL[255]={0};
	sprintf(strSQL,"F:OBJECTID,SHAPE,T:%s,W:%s=%s,##", strSdeLyName,strFName,strFValue);

	/*
	CStringArray strArray;
	strArray.Add("OBJECTID");
	CString strRegName = "DIST_" + strSdeLyNameXMBH;
	m_pExchangeData->ReadSymbolInfo(m_pConnection, NULL,NULL,strSdeLyNameXMBH,strRegName,&strArray);*/

	g_pExchangeData->GetSearchShapePointArray(strSQL, &ptArray);
	return (!ptArray.isEmpty());
}



//将属性信息保存到块引用
BOOL CSdeDataOperate::SaveAttrToRef(AcDbObjectId ObjIdRef,CStringArray& strArrayTag,CStringArray& strArrayValue)
{
	int nCount = strArrayTag.GetCount();
	if(nCount==0 || nCount != strArrayValue.GetCount() || ObjIdRef.isNull())
	{
		acutPrintf("\n保存块引用信息,参数有误!"); return FALSE;
	}

	AcDbBlockReference * pBckRef = NULL;
	if (Acad::eOk != acdbOpenObject(pBckRef, ObjIdRef, AcDb::kForRead)) return FALSE;
	pBckRef->close();

	AcDbObjectIterator * pObjIter = pBckRef->attributeIterator();
	if (pObjIter == NULL) return FALSE;

	for ( pObjIter->start(); !pObjIter->done(); pObjIter->step() )
	{
		AcDbObject * pEnt = NULL;
		AcDbObjectId IdTmp = pObjIter->objectId();
		if (Acad::eOk != acdbOpenObject(pEnt, IdTmp, AcDb::kForWrite)) continue;

		AcDbAttributeDefinition * pAttr = NULL;
		pAttr = (AcDbAttributeDefinition*)pEnt;
		if (pAttr == NULL)
		{
			pEnt->close();continue;
		}

		//ARX 2005
		//CString strTemp = pAttr->tagConst();
		//ARX 2004
		CString strTemp = pAttr->tag();
		for (int i=0; i<nCount; i++)
		{
			if (strTemp.CompareNoCase(strArrayTag.GetAt(i)) == 0)
			{
				strTemp = strArrayValue.GetAt(i);
				pAttr->setTextString(strTemp.GetBuffer());
				break;
			}
		}

		pEnt->close();
	}


	return TRUE;
}



BOOL CSdeDataOperate::IsDKLayer(CString strSdeLyName)
{

	// 临时
	//if(strSdeLyName.CompareNoCase("XMGHSDE.SP_XZLX_ZB_POLYGON")==0) return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_YDLX_ZB_POLYGON")==0) return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_HXLX_ZB_POLYGON")==0) return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_YXLX_ZB_POLYGON")==0) return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_CBLX_ZB_POLYGON")==0) return TRUE;

	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_XZLX_DA_POLYGON")==0) return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_YDLX_DA_POLYGON")==0) return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_HXLX_DA_POLYGON")==0) return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_YXLX_DA_POLYGON")==0) return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_CBLX_DA_POLYGON")==0) return TRUE;

	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_XZLX_LS_POLYGON")==0) return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_YDLX_LS_POLYGON")==0) return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_HXLX_LS_POLYGON")==0) return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_YXLX_LS_POLYGON")==0) return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_CBLX_LS_POLYGON")==0) return TRUE;
	if(strSdeLyName.CompareNoCase("SDE.SP_XZLX_ZB_POLYGON")==0) return TRUE;
	else if(strSdeLyName.CompareNoCase("SDE.SP_YDLX_ZB_POLYGON")==0) return TRUE;

	else if(strSdeLyName.CompareNoCase("SDE.SP_XZLX_DA_POLYGON")==0) return TRUE;
	else if(strSdeLyName.CompareNoCase("SDE.SP_YDLX_DA_POLYGON")==0) return TRUE;

	else if(strSdeLyName.CompareNoCase("SDE.SP_XZLX_LS_POLYGON")==0) return TRUE;
	else if(strSdeLyName.CompareNoCase("SDE.SP_YDLX_LS_POLYGON")==0) return TRUE;
	return FALSE;
}

BOOL CSdeDataOperate::IsDKLayerOld(CString strSdeLyName)
{
	// 临时
	//if(strSdeLyName.CompareNoCase("XMGHSDE.XMGH_LX")==0) return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.XMGH_HX")==0)return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.XMGH_LX_XZ")==0)return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.XMGH_LX_YD")==0)return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.XMGH_LX_YD_DL")==0)return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.XMGH_LX_YD_ZFCB")==0)return TRUE;
	//else return FALSE;
	return FALSE;
}

BOOL CSdeDataOperate::IsXMLayer(CString strSdeLyName)
{
	// 临时
	//if(strSdeLyName.CompareNoCase("XMGHSDE.SP_XZLX_ZB_PT")==0) return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_YDLX_ZB_PT")==0) return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_HXLX_ZB_PT")==0) return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_YXLX_ZB_PT")==0) return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_CBLX_ZB_PT")==0) return TRUE;

	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_XZLX_DA_PT")==0) return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_YDLX_DA_PT")==0) return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_HXLX_DA_PT")==0) return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_YXLX_DA_PT")==0) return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_CBLX_DA_PT")==0) return TRUE;

	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_XZLX_LS_PT")==0) return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_YDLX_LS_PT")==0) return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_HXLX_LS_PT")==0) return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_YXLX_LS_PT")==0) return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_CBLX_LS_PT")==0) return TRUE;
	if(strSdeLyName.CompareNoCase("SDE.SP_XZLX_ZB_PT")==0) return TRUE;
	else if(strSdeLyName.CompareNoCase("SDE.SP_YDLX_ZB_PT")==0) return TRUE;

	else if(strSdeLyName.CompareNoCase("SDE.SP_XZLX_DA_PT")==0) return TRUE;
	else if(strSdeLyName.CompareNoCase("SDE.SP_YDLX_DA_PT")==0) return TRUE;

	else if(strSdeLyName.CompareNoCase("SDE.SP_XZLX_LS_PT")==0) return TRUE;
	else if(strSdeLyName.CompareNoCase("SDE.SP_YDLX_LS_PT")==0) return TRUE;
	return FALSE;
}

long CSdeDataOperate::ReadXMLayer(AcDbObjectIdArray& arrId,
								  char* strSdeLyName,
								  DIST_STRUCT_SELECTWAY* pFilter,
								  const char* strFieldSql)
{
	//读图层
	CStringArray strFldNameArray;
	strFldNameArray.Add("PROJECT_CODE");strFldNameArray.Add("LICENCE_CODE");strFldNameArray.Add("REG_TIME");
	strFldNameArray.Add("PROJECT_NAME");strFldNameArray.Add("BUILD_UNIT");strFldNameArray.Add("USELAND_TYPEID");
	strFldNameArray.Add("JBRY");strFldNameArray.Add("JSYDMJ");strFldNameArray.Add("DLYDMJ");
	strFldNameArray.Add("LHYDMJ");strFldNameArray.Add("QTYDMJ");strFldNameArray.Add("ZYDMJ");      	
	CStringArray* pArray = &strFldNameArray; 
	if(1 != ReadSdeLayer(arrId, strSdeLyName, pFilter, strFieldSql,pArray,1)) return 0;

	CString strLXLX;
	CString strTempSdeLyName = strSdeLyName;
	if(strTempSdeLyName.Find("_XZLX_")>0)
		strLXLX = "选址";
	else if (strTempSdeLyName.Find("_YDLX_")>0)
		strLXLX = "用地";
	else if (strTempSdeLyName.Find("_HXLX_")>0)
		strLXLX = "红线";
	else if (strTempSdeLyName.Find("_YXLX_")>0)
		strLXLX = "预选";
	else if (strTempSdeLyName.Find("_CBLX_")>0)
		strLXLX = "储备";

	CStringArray strANameArray;
	strANameArray.Add("项目显示编号"); strANameArray.Add("发证编号"); strANameArray.Add("登记日期");strANameArray.Add("项目名称");
	strANameArray.Add("建设单位"); strANameArray.Add("用地性质编号"); strANameArray.Add("蓝线类型");
	strANameArray.Add("经办人员"); strANameArray.Add("建设用地面积"); strANameArray.Add("道路用地面积");
	strANameArray.Add("绿化用地面积"); strANameArray.Add("其他用地面积"); strANameArray.Add("总用地面积");

	char strRegName[260]={0};
	sprintf(strRegName,"DIST_%s",strSdeLyName);

	int nCount = arrId.length();
	AcDbObjectId tempId;
	for(int i=0; i<nCount; i++)
	{
		CStringArray strValueArray;
		tempId = arrId.at(i);
		CDistXData tempXData(tempId,strRegName);
		for(int j=0; j<12; j++)  //读扩展属性
		{
			CString strValue;
			CString strTemp = strFldNameArray.GetAt(j);
			tempXData.Select(strTemp.GetBuffer(0),strValue);
			strValueArray.Add(strValue);
		}
		strValueArray.InsertAt(6,strLXLX);
		tempXData.DeleteAllRecord();
		tempXData.Update(); tempXData.Close();

		//修改属性内容
		SaveAttrToRef(tempId,strANameArray,strValueArray);
	}

	return 1;
}

long CSdeDataOperate::ReadDKLayer(AcDbObjectIdArray& arrId,
								  char* strSdeLyName,
								  DIST_STRUCT_SELECTWAY* pFilter,
								  const char* strFieldSql)
{
	//读SDE层对应的CAD层名称
	char strSQL[255]={0};
	sprintf(strSQL,"F:CADLYNAME,T:CADMAPTOSDE,W:SDELYNAME='%s',##",strSdeLyName);
	IDistCommand *pCommand = CreateSDECommandObjcet();
	if(NULL == pCommand) return 0;
	pCommand->SetConnectObj(g_pConnection);
	IDistRecordSet* pRcdSet = NULL;
	if(1 != pCommand->SelectData(strSQL,&pRcdSet))
	{
		pCommand->Release(); 
		if(NULL != pRcdSet) pRcdSet->Release(); return 0;
	}

	CString strCadLyName;
	if(pRcdSet->BatchRead()==1)
	{
		strCadLyName = (char*)(*pRcdSet)[0];
	}
	pRcdSet->Release(); pCommand->Release();
	if(strCadLyName.IsEmpty()) strCadLyName = "SDE_地块信息";


	//读图层
	CStringArray strFldNameArray;
	strFldNameArray.Add("PROJECT_CODE");strFldNameArray.Add("LICENCE_CODE");
	strFldNameArray.Add("REG_TIME");strFldNameArray.Add("PROJECT_NAME");
	strFldNameArray.Add("BUILD_UNIT");strFldNameArray.Add("USELAND_TYPEID"); //strFldNameArray.Add("LXLX");
	strFldNameArray.Add("JBRY");strFldNameArray.Add("DKBH");
	strFldNameArray.Add("YDLX");strFldNameArray.Add("YDMJ");strFldNameArray.Add("ZG");      
	strFldNameArray.Add("XK");strFldNameArray.Add("X");strFldNameArray.Add("Y");strFldNameArray.Add("BZ"); 
	CStringArray* pArray = &strFldNameArray; 
	if(1 != ReadSdeLayer(arrId, strSdeLyName, pFilter, strFieldSql,pArray,1)) return 0;




	//删除地块标注层所有实体
	char strRegName[260]={0};
	sprintf(strRegName,"DIST_%s",strSdeLyName);
	CString strTempCadLyName;
	strTempCadLyName.Format("%s地块标注",strCadLyName);
	CDistEntityTool tempTool;
	tempTool.DeleteAllEntityInLayer(strTempCadLyName.GetBuffer());



	CString strLXLX;
	CString strTempSdeLyName = strSdeLyName;
	if(strTempSdeLyName.Find("_XZLX_")>0)
		strLXLX = "选址";
	else if (strTempSdeLyName.Find("_YDLX_")>0)
		strLXLX = "用地";
	else if (strTempSdeLyName.Find("_HXLX_")>0)
		strLXLX = "红线";
	else if (strTempSdeLyName.Find("_YXLX_")>0)
		strLXLX = "预选";
	else if (strTempSdeLyName.Find("_CBLX_")>0)
		strLXLX = "储备";

	CStringArray strANameArray;
	strANameArray.Add("项目显示编号");strANameArray.Add("发证编号");strANameArray.Add("登记日期");		
	strANameArray.Add("项目名称");strANameArray.Add("建设单位");strANameArray.Add("用地性质编号");
	strANameArray.Add("蓝线类型");strANameArray.Add("经办人员");strANameArray.Add("地块编号");		
	strANameArray.Add("用地类型");strANameArray.Add("用地面积");strANameArray.Add("字高");			
	strANameArray.Add("线宽");strANameArray.Add("X值");strANameArray.Add("Y值");strANameArray.Add("备注");
	int nCount = arrId.length();
	AcDbObjectId tempId;
	for(int i=0; i<nCount; i++)
	{
		tempId = arrId.at(i);
		CDistXData tempXData(tempId,strRegName);
		CStringArray strNameArray,strValueArray;

		for(int jj=0; jj<15; jj++)
		{
			CString strValue;
			CString strTemp = strFldNameArray.GetAt(jj);
			tempXData.Select(strTemp.GetBuffer(0),strValue);
			strValueArray.Add(strValue);
		}
		strValueArray.InsertAt(6,strLXLX);
		tempXData.Close();

		double dx  = atof(strValueArray.GetAt(13));
		double dy  = atof(strValueArray.GetAt(14));
		double dZG = atof(strValueArray.GetAt(11));


		AcDbObjectId resultId;
		tempTool.InsertBlkAttrRef(resultId,"_地块属性",strTempCadLyName.GetBuffer(),AcGePoint3d(dx,dy,0),dZG,strANameArray,strValueArray);
		CDistXData tXData(resultId,"ROWID_OBJS");
		tXData.Add("SDELYNAME",strSdeLyName);
		tXData.Update(); tXData.Close();

		AcDbObjectId OutId;
		gTurnArc(OutId,tempId,strRegName,4,2.0);
	}

	return 1;
}


long CSdeDataOperate::ReadDKLayerOld(AcDbObjectIdArray& arrId,
								  char* strSdeLyName,
								  DIST_STRUCT_SELECTWAY* pFilter,
								  const char* strFieldSql)
{
	//读SDE层对应的CAD层名称
	char strSQL[255]={0};
	sprintf(strSQL,"F:CADLYNAME,T:CADMAPTOSDE,W:SDELYNAME='%s',##",strSdeLyName);
	IDistCommand *pCommand = CreateSDECommandObjcet();
	if(NULL == pCommand) return 0;
	pCommand->SetConnectObj(g_pConnection);
	IDistRecordSet* pRcdSet = NULL;
	if(1 != pCommand->SelectData(strSQL,&pRcdSet))
	{
		pCommand->Release(); 
		if(NULL != pRcdSet) pRcdSet->Release(); return 0;
	}

	CString strCadLyName;
	if(pRcdSet->BatchRead()==1)
	{
		strCadLyName = (char*)(*pRcdSet)[0];
	}
	pRcdSet->Release(); pCommand->Release();
	if(strCadLyName.IsEmpty()) strCadLyName = "SDE_原项目地块";


	//读图层
	CStringArray strFldNameArray;
	strFldNameArray.Add("项目名称");
	CStringArray* pArray = &strFldNameArray; 
	if(1 != ReadSdeLayer(arrId, strSdeLyName, pFilter, strFieldSql,pArray,1)) return 0;

	//删除地块标注层所有实体
	char strRegName[260]={0};
	sprintf(strRegName,"DIST_%s",strSdeLyName);
	CString strTempCadLyName;
	strTempCadLyName.Format("%s项目标注",strCadLyName);
	AcDbObjectId xxxId;
	CDistEntityTool tempTool;
	tempTool.CreateLayer(xxxId,strTempCadLyName.GetBuffer(0),4);
	tempTool.DeleteAllEntityInLayer(strTempCadLyName.GetBuffer());

	int nObjLen = arrId.length();
	CString strXMMC;
	for(int j=0; j<nObjLen; j++)
	{
		AcDbObjectId tempId = arrId.at(j);
		CDistXData tempXData(tempId,strRegName);
		tempXData.Select("项目名称",strXMMC);
		tempXData.Close();
		AcDbPolyline* pline = NULL;
		if(Acad::eOk == acdbOpenObject(pline,tempId,AcDb::kForRead))
		{
			AcDbExtents extents;
			AcGePoint3d ptPos;
			pline->getGeomExtents(extents);
			ptPos.x = (extents.maxPoint().x+extents.minPoint().x)/2;
			ptPos.y = (extents.maxPoint().y+extents.minPoint().y)/2; ptPos.z = 0;
			pline->close();
			AcDbObjectId resultId;
			tempTool.CreateText(resultId,ptPos,strXMMC.GetBuffer(0),strTempCadLyName.GetBuffer(0),4,"宋体",1,1,5,0.8);

			CDistXData tXData(resultId,"ROWID_OBJS");
			tXData.Add("SDELYNAME",strSdeLyName);
			tXData.Update(); tXData.Close();
		}
	}
	return 1;
}



