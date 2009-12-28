// BaseSdeEntity.cpp: implementation of the CBaseSdeEntity class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <rxmfcapi.h>
#include "DistSelSet.h"
#include "DistBaseInterface.h"
#include "DistBaseExchange.h"
#include "DistDatabaseReactor.h"
#include "DistXData.h"

#ifdef _DEBUG
#include "time.h"
#endif

#define  ROW_PART_XDATA  "ROWID_OBJS"

#define MAX_SYMBOL_COUNT 256

//全局常量
static CDistDatabaseReactor* m_pDbReactor = CDistDatabaseReactor::GetInstance();  //数据库反应器对象

void OutputDebugStringX(LPCTSTR fmt, ...)
{
	char lpBuffer[1024] = {0};

	va_list marker;
	va_start(marker, fmt);
	vsprintf(lpBuffer, fmt, marker);
	va_end(marker);

	OutputDebugString(lpBuffer);
}


//================================================ export function ==============================================//														===========================//

//创建SDE，CAD数据互换类对象
IDistExchangeData* WINAPI CreateExchangeObjcet(void)
{
	return new CExchangeData;
}

//功能： 检测指定坐标范围是否与某个图层上的实体交叉或重叠
//参数： pConnect
//       strSdeLyName
//       strSQL
//       ptArray[注意闭合]
//       nPtCount  
//返回：0 失败，1 没有交叉重叠，2有重叠交叉
//"F:OBJECTID,T:%s,W:PROJECT_ID<>'%s',##"
long WINAPI gCheckCrossOrContain(IDistConnection* pConnect,const char* strSdeLyName,const char*strSQL,
								 IDistPoint* ptArray,int nPtCount)
{
	int nResult = 0;
	if(NULL == pConnect || NULL == strSdeLyName || NULL == ptArray) return nResult;
	if(nPtCount == 0) return nResult;

	//创建命令对象
	IDistCommand* pCommand = CreateSDECommandObjcet();
	pCommand->SetConnectObj(pConnect);

	//创建坐标系
	SE_COORDREF pCoordref=NULL;
	SE_coordref_create(&pCoordref);
	pCommand->GetLayerCoordref(strSdeLyName,"SHAPE",(void**)&pCoordref);

	//创建SHAPE对象
	SE_SHAPE shape=NULL;
	long rc = SE_shape_create(pCoordref,&shape);
	SE_POINT* pts=NULL;
	pts = (SE_POINT*) malloc (nPtCount * sizeof(SE_POINT));
	for(int i=0; i<nPtCount; i++){
		pts[i].x = ptArray[i].x;  pts[i].y = ptArray[i].y;
	}
	rc = SE_shape_generate_polygon(nPtCount,1,NULL,pts,NULL,NULL,shape);
	free(pts); SE_coordref_free(pCoordref);


	//设置Filter
	SE_FILTER Filter;
	Filter.filter_type = SE_SHAPE_FILTER; 
	Filter.filter.shape = shape; 
	Filter.method = kSM_AI;//kSM_AI_OR_ET; 
	strcpy(Filter.table,strSdeLyName);
	strcpy(Filter.column,"SHAPE");
	Filter.truth = TRUE; 

	//根据条件查询数据库
	IDistRecordSet* pRcdSet = NULL;
	if(1 != pCommand->SelectData(strSQL,&pRcdSet,(void*)&Filter))
	{
		pCommand->Release(); 
		if(pRcdSet != NULL)
			pRcdSet->Release();
		return nResult;	
	}

	if(pRcdSet->GetRecordCount()==0)
		nResult = 1;
	else
		nResult = 2; //有重叠交叉

	//释放
	SE_shape_free(shape);
	pCommand->Release(); pRcdSet->Release();

	return nResult;
}


//================================================ globle function ==============================================//	
// 功能：根据错误代码显示信息提示
// 参数：rc         错误代码
//       szComment  在代码中错误所在函数位置
// 返回：无
void check_error (long rc, char *szComment)
{
#ifdef _DEBUG
	if(rc == 0 || rc == 1) return;
	char* strInfo = ReturnErrorCodeInfo(rc);
	tm systime;
	_getsystime(&systime);
	acutPrintf("\n\n time:%d,%d,function:%s, Error code:%d Message:%s",systime.tm_min,systime.tm_sec,szComment,rc,strInfo);
#endif
}


// 功能：比较两个字符串
// 参数：strOne   字符串A
//       strTwo   字符串B
//返回：  0表示A=B，正值表示A>B，负值表示A<B
static int CompareString(const char* strOne,const char* strTwo)
{
	char strA[1024];
	char strB[1024];
	memset(strA,0,sizeof(char)*1024);
	memset(strB,0,sizeof(char)*1024);
	strcpy(strA,strOne);
	strcpy(strB,strTwo);

	strupr(strA);
	strupr(strB);

	return strcmp(strA,strB);
}


static CString GetCurArxAppPath(CString strArxName)
{
	TCHAR exeFullPath[MAX_PATH]; 
	CString strPath; 
	GetModuleFileName(GetModuleHandle(strArxName),exeFullPath,MAX_PATH); 
	strPath=(CString)exeFullPath; 
	int position=strPath.ReverseFind('\\'); 
	strPath=strPath.Left(position+1); 
	return strPath;
}

//===============================================  class CExchangeInfo  =========================================//

CExchangeInfo::CExchangeInfo()
{
	m_pConnection = NULL;
	m_pSymbolInfo = NULL;
	m_nSymbolCount   = 0;
	m_nPos = 0;
	m_nShapeType  = 0;
	m_Coordref = NULL;

	m_pParam = NULL;
	m_nParamCount = 0;

	memset(m_strCadLyName,0,sizeof(char)*160);
	memset(m_strSdeLyName,0,sizeof(char)*160);
	memset(m_strSdeLyKind,0,sizeof(char)*60);
	memset(m_strRowIdFldName,0,sizeof(char)*60);
	memset(m_strShapeFldName,0,sizeof(char)*60);
	memset(m_strXDataName,0,sizeof(char)*255);

	m_strFldNameArray.RemoveAll();
	m_strXDataArrayOut.RemoveAll();

}

CExchangeInfo::~CExchangeInfo()
{
	if(NULL != m_pSymbolInfo)
		delete[] m_pSymbolInfo;

	if(NULL != m_pParam)
		delete[] m_pParam;

	m_strFldNameArray.RemoveAll();
	m_strXDataArrayOut.RemoveAll();

	if(NULL != m_Coordref)
		SE_coordref_free(m_Coordref);
}





//===============================================  class CExchangeData  =========================================//
CExchangeData::CExchangeData()
{
}

CExchangeData::~CExchangeData()
{
/*	if(gPtArray!=NULL)
	{
		delete[] gPtArray;
		gPtArray = NULL;
	}
*/
}


//功能：释放对象
//参数：无
//返回：无
void CExchangeData::Release(void)
{
	delete this;
}


//功能：设置数据库反应器开关
//参数：bOpen  是否打开数据库反应器
//返回：无
void CExchangeData::SetDbReactorStatues(bool bOpen)
{
	AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();
	if(bOpen == true)
		pDB->addReactor(m_pDbReactor);
	else
		pDB->removeReactor(m_pDbReactor); 
}


//功能：获取当前SDE层对应的CAD层名称
//参数：返回CAD图层名称
//返回：无
void CExchangeData::GetCurCadLyName(char* strCadLyName)
{
	strcpy(strCadLyName,m_info.m_strCadLyName);
}


int CExchangeData::CopyRecordFromOneToAnother(IDistConnection* pConnection, LPCTSTR lpSrcTable, LPCTSTR lpDestTable, LPCTSTR lpWhere)
{
	if ((NULL == pConnection) || (NULL==lpSrcTable) || (NULL==lpDestTable) || (NULL==lpWhere)) 
		return 0;

	//创建命令对象
	IDistCommand *pCommandSrc = CreateSDECommandObjcet(); 
	if(NULL == pCommandSrc) 
	{
		check_error(-1,"\n GetSearchShapePointArray->The Command Object Created error!");
		return 0;
	}
	pCommandSrc->SetConnectObj(pConnection);//m_info.m_pConnection);

	int nParamCount = 0;
	IDistParameter* pParam = NULL;
	if(1 != pCommandSrc->GetTableInfo(&pParam,&nParamCount,lpSrcTable))
		return 0;

	char strFldNames[2048]={0};
	int nPos =0;
	int nRowPos = 0;
	for(int i=0; i<nParamCount;i++)
	{
		if(1 == pParam[i].fld_nRowIdType)
		{
			nRowPos = i; continue;
		}
		int nLen = strlen(pParam[i].fld_strName);
		memcpy(strFldNames+nPos,pParam[i].fld_strName,nLen);
		nPos = nPos+nLen;
		strFldNames[nPos]=',';
		nPos++;
	}

	nParamCount = nParamCount-1;
	char strSQL[1024]={0};
	sprintf(strSQL,"F:%sT:%s,W:%s,##",strFldNames,lpSrcTable,lpWhere);
	IDistRecordSet* pRcdSet = NULL;
	IDistCommand* pCommand = CreateSDECommandObjcet();

	if(NULL == pCommand) return 0;

	pCommand->SetConnectObj(pConnection);
	if(pCommand->SelectData(strSQL,&pRcdSet)!=1)
	{
		pCommand->Release();
		if(pRcdSet != NULL) pRcdSet->Release(); return 0;
	}

	int nRcdCount = pRcdSet->GetRecordCount();
	if(nRcdCount == 0)
	{
		if(pRcdSet != NULL) pRcdSet->Release(); return 2;
	}

	char strOtherSQL[1024]={0};
	sprintf(strOtherSQL,"F:%sT:%s,##",strFldNames,lpDestTable);
	char** strValues = new char* [nParamCount];
	for(int j=0; j<nParamCount; j++)
	{
		strValues[j] = new char[500];
		memset(strValues[j],0,sizeof(char)*500);
	}
	while(pRcdSet->BatchRead()==1)
	{
		for(int k=0; k<nParamCount; k++)
			pRcdSet->GetValueAsString(strValues[k],k);
		int nRowId =0;
		if(1 != pCommandSrc->InsertDataAsString(&nRowId,strOtherSQL,strValues))
		{
			pCommand->Release(); pRcdSet->Release();
			for(j=0; j<nParamCount; j++)
			{
				delete[] strValues[j];
			}
			delete[] strValues;
			return 0;
		}
	}

	pCommandSrc->Release();
	pCommand->Release(); pRcdSet->Release();
	for(j=0; j<nParamCount; j++)
	{
		delete[] strValues[j];
	}
	delete[] strValues;

	return 1;
}

//功能：获取指定搜索条件下SHAPE的点数组
//参数：strSQL  搜索条件
//      ptArray 返回Shape结构的点数组
//返回：无
void CExchangeData::GetSearchShapePointArray(IDistConnection* pConnection, const char* strSQL,AcGePoint3dArray* ptArray)
{
	//参数合法性检测
	if(NULL == strSQL || NULL == ptArray)
	{
		check_error(-1,"\n GetSearchShapePointArray->Input Parameter is error!");
		return ;
	}

	//清空返回数组
	ptArray->setLogicalLength(0);


	//判断SDE库是否已经连接
	if(NULL == pConnection)//m_info.m_pConnection) 
	{
		check_error(-1,"\n GetSearchShapePointArray->The Database have not connected!");
		return;
	}

	//创建命令对象
	IDistCommand *pCommand = CreateSDECommandObjcet(); 
	if(NULL == pCommand) 
	{
		check_error(-1,"\n GetSearchShapePointArray->The Command Object Created error!");
		return;
	}
	pCommand->SetConnectObj(pConnection);//m_info.m_pConnection);

	//char strSQL[255]={0};
	//sprintf(strSQL,"F:OBJECTID,SHAPE,T:XMGHSDE.GHLXDK_Polygon,W:Project_ID=%s,##",);
	//根据SQL条件，查询记录
	long rc=-1;
	IDistRecordSet* pRcd = NULL;
	if((rc=pCommand->SelectData(strSQL,&pRcd,NULL)) != 1)
	{
		check_error(rc,"GetSearchShapePointArray->SelectData");
		pCommand->Release(); if(pRcd != NULL) pRcd->Release(); //预防性释放
		return;
	}
	if(pRcd->GetRecordCount() == 0)//没有记录
	{
		check_error(-1,"GetSearchShapePointArray->No Result!");
		pCommand->Release();pRcd->Release();return;
	}

	//从记录集中读SHAPE数据
	if(pRcd->BatchRead())
	{
		SE_SHAPE *pShape = (SE_SHAPE*)(*pRcd)["SHAPE"];//注：pShape空间由pRcd分配并释放，所以pShape不需要释放
		if(NULL == pShape)
		{
			check_error(-1,"GetSearchShapePointArray->Get Shape Data error Or Data is invalid!");
			pCommand->Release();pRcd->Release();return;
		}

		//从Shape结构中获取坐标点数量
		long nAllPtNum=0;
		rc = SE_shape_get_num_points(*pShape,0,0,&nAllPtNum);  //获取Shape中所有点数量
		check_error(rc,"GetSearchShapePointArray->SE_shape_get_num_points");

		//获取点数组
		SE_POINT *pts = (SE_POINT*) malloc (nAllPtNum*sizeof(SE_POINT));
		if(NULL == pts)
		{
			check_error(-1,"GetSearchShapePointArray->Malloc memory error!");
			pCommand->Release();pRcd->Release();return;
		}
		rc = SE_shape_get_all_points(*pShape,SE_DEFAULT_ROTATION,NULL,NULL,pts,NULL,NULL); //Get point array
		check_error(rc,"GetSearchShapePointArray->SE_shape_get_all_points");
		if(rc == 0)
		{
			for(int i=0;i<nAllPtNum; i++)// 结果返回
				ptArray->append(AcGePoint3d(pts[i].x,pts[i].y,0));
		}

		free(pts);  pts=NULL;
	}

	//.... ....

	//对点数组进一步处理....

	//.... ....

	pRcd->Release();
	pCommand->Release();

}

void CExchangeData::GetSearchFieldArray(IDistConnection* pConnection, LPCTSTR strSQL, LPCTSTR lpTable, LPCTSTR lpField, DIST_STRUCT_SELECTWAY* pFilter, CStringArray& array)
{
	//获取图层坐标系信息
	if(NULL != m_info.m_Coordref)
	{
		SE_coordref_free(m_info.m_Coordref); //预防性释放
		m_info.m_Coordref =  NULL;
	}
	long rc = SE_coordref_create(&m_info.m_Coordref);
	check_error (rc, "ReadSymbolInfo->SE_coordref_create");

	SE_LAYERINFO pLyInfo = NULL;
	rc = SE_layerinfo_create(NULL,&pLyInfo);
	check_error (rc, "ReadSymbolInfo->SE_layerinfo_create");

	SE_CONNECTION pConnectObj = (SE_CONNECTION) pConnection->GetConnectObjPointer();
	rc = SE_layer_get_info(pConnectObj, lpTable, lpField, pLyInfo);
	check_error (rc, "ReadSymbolInfo->SE_layer_get_info");

	rc = SE_layerinfo_get_coordref (pLyInfo,m_info.m_Coordref);
	check_error(rc,"ReadSymbolInfo->SE_layerinfo_get_coordref");

	SE_layerinfo_free(pLyInfo);

	//创建空间搜索过滤器
	SE_FILTER filter;
	CreateFilter(pFilter,&filter);
	strcpy(filter.table, lpTable);
	strcpy(filter.column, lpField);
	filter.truth = TRUE; 

	//创建命令对象
	IDistCommand *pCommand = CreateSDECommandObjcet();
	if(NULL == pCommand) 
	{
		return; //系统错误
	}

	pCommand->SetConnectObj(pConnection);
	IDistRecordSet* pRcd = NULL;

	if((rc=pCommand->SelectData(strSQL, &pRcd, &filter)) !=1 )
	{
		check_error(rc,"ReadInfoToCreateCadLayer->SelectData");
		if(pRcd != NULL)
			pRcd->Release();
		pCommand->Release(); 
		return;
	}

	while(pRcd->BatchRead() == 1)
	{
//		CString strValue; = (char*)(*pRcd)[1]; //"图号"
		TCHAR strValue[256] = {0};
		pRcd->GetValueAsString(strValue, 1); //获取到RowID的值
		array.Add(strValue);
	}

	pRcd->Release(); 
	pCommand->Release();
}

//功能：根据提供的位置结构信息，生成空间过滤条件
//参数：pFilterInfo  位置结构信息
//      pFilter      空间过滤条件
//返回：成功返回 1，失败返回 0，其他参考定义
long CExchangeData::CreateFilter(DIST_STRUCT_SELECTWAY* pFilterInfo,SE_FILTER *pFilter)
{
	long rc;
	if(pFilterInfo == NULL) return 0;

	// 1 点  2 线  3圆  4 矩形  5多边形 6选择所有
	SE_SHAPE shape;
	rc = SE_shape_create(m_info.m_Coordref,&shape);
	check_error(rc,"CreateFilter->SE_shape_create");

	SE_POINT* pts = NULL;

	int i,nCount;
	int nType = pFilterInfo->nEntityType;
	if(nType == 1)
	{
		nCount = 1;
		pts = (SE_POINT*) malloc (nCount * sizeof(SE_POINT));
		pts[0].x = pFilterInfo->ptArray[0].x; 
		pts[0].y = pFilterInfo->ptArray[0].y;
		rc = SE_shape_generate_point(nCount,pts,NULL,NULL,shape);
		check_error(rc,"CreateFilter->SE_shape_generate_point");
	}
	else if(nType == 2)
	{
		nCount = pFilterInfo->ptArray.length();
		pts = (SE_POINT*) malloc (nCount * sizeof(SE_POINT));
		for(i=0; i<nCount; i++)
		{
			pts[i].x = pFilterInfo->ptArray[i].x; 
			pts[i].y = pFilterInfo->ptArray[i].y;
		}
		rc = SE_shape_generate_line(nCount,1,NULL,pts,NULL,NULL,shape);
		check_error(rc,"CreateFilter->SE_shape_generate_line");
	}
	else if(nType == 3)
	{
		
		nCount = 1;
		pts = (SE_POINT*) malloc (nCount * sizeof(SE_POINT));
		pts[0].x = pFilterInfo->ptArray[0].x; 
		pts[0].y = pFilterInfo->ptArray[0].y;
		
		double dR =  pts[1].x;  //注意半径的存放位置

		rc = SE_shape_generate_circle(pts[0],dR,20,shape);
		check_error(rc,"CreateFilter->SE_shape_generate_circle");
	}
	else if(nType == 4)
	{
		SE_ENVELOPE rect;
		rect.minx = pFilterInfo->ptArray[0].x; rect.miny = pFilterInfo->ptArray[0].y; 
		rect.maxx = pFilterInfo->ptArray[1].x; rect.maxy = pFilterInfo->ptArray[1].y;
		rc = SE_shape_generate_rectangle(&rect,shape);
		check_error(rc,"CreateFilter->SE_shape_generate_rectangle");
	}
	else if(nType == 5)
	{
		nCount = pFilterInfo->ptArray.length();
		pts = (SE_POINT*) malloc (nCount * sizeof(SE_POINT));
		for(i=0; i<nCount; i++)
		{
			pts[i].x = pFilterInfo->ptArray[i].x; 
			pts[i].y = pFilterInfo->ptArray[i].y;
		}
		rc = SE_shape_generate_polygon(nCount,1,NULL,pts,NULL,NULL,shape);
		check_error(rc,"CreateFilter->SE_shape_generate_polygon");
	}
	else
	{
		return 0;
	}

	if(NULL != pts)
		free(pts);

	pFilter->filter_type = SE_SHAPE_FILTER; 
	pFilter->filter.shape = shape; 
	pFilter->method = pFilterInfo->nSelectType; 
	strcpy(pFilter->table,m_info.m_strSdeLyName);
	strcpy(pFilter->column,m_info.m_strShapeFldName);
	pFilter->truth = TRUE; 

	return 1;

}


//功能：选择当前查询范围内中的所有实体(避免重复提交)
//参数：strCadName   CAD图层名
//      strSdeName   SDE图层名(如果没有SDE图层条件，说明CAD层与SDE层是多对一或一对一)
//      pFilter      空间过滤条件
//返回：成功返回 1，失败返回 0，其他参考定义
long   CExchangeData::SelectEntityInCAD(AcDbObjectIdArray& ObjIdArray,const char* strCadName,
										const char* strSdeName,DIST_STRUCT_SELECTWAY* pFilter)
{
	//设置图层过滤器
	struct resbuf *filter=NULL;
	filter=acutNewRb(AcDb::kDxfLayerName);
	filter->restype=AcDb::kDxfLayerName;
	filter->resval.rstring = (char*) malloc(strlen(strCadName)*sizeof(char)+1);
	strcpy(filter->resval.rstring,strCadName);
	filter->rbnext=NULL;

	//设置空间过滤(目前只考虑矩形和多边形)
	CDistSelSet tempSelect;
	ObjIdArray.setLogicalLength(0);
	if(NULL == pFilter)
		tempSelect.AllSelect(filter);
	else
	{
		int nSelectType = pFilter->nSelectType;
		if(pFilter->nEntityType == 4)
		{
			if(pFilter->nSelectType == kSM_SC)
				tempSelect.WindowSelect(pFilter->ptArray[0],pFilter->ptArray[1],filter);
			else if(pFilter->nSelectType == kSM_SC_NO_ET)
				tempSelect.BoxSelect(pFilter->ptArray[0],pFilter->ptArray[1],filter);
			else
				tempSelect.AllSelect(filter);
		}
		else if(pFilter->nEntityType == 5)
		{
			if(pFilter->nSelectType == kSM_SC)
				tempSelect.WindowPolygonSelect(pFilter->ptArray,filter);
			else if(pFilter->nSelectType == kSM_SC_NO_ET)
				tempSelect.FenceSelect(pFilter->ptArray,filter);
			else
				tempSelect.AllSelect(filter);
		}
		else
			tempSelect.AllSelect(filter);
	}
	

	tempSelect.AsArray(ObjIdArray);
	int nCount = ObjIdArray.length();
	if(nCount == 0 || NULL == strSdeName) return 1;

	AcDbObjectId tempId;
	CString strValue;
	for(int i=nCount-1; i>=0; i--)
	{
		tempId = ObjIdArray.at(i);
		CDistXData tempXData(tempId,ROW_PART_XDATA);
		tempXData.Select("SDELYNAME",strValue);
		if(strValue.CompareNoCase(strSdeName)!=0)
			ObjIdArray.removeAt(i);
	}

	return 1;
}

//功能：删除当前查询范围内中的所有实体(避免重复绘制)
//参数：strCadName   CAD图层名
//      strSdeName   SDE图层名(如果没有SDE图层条件，说明CAD层与SDE层是多对一或一对一)
//      pFilter      空间过滤条件
//返回：成功返回 1，失败返回 0，其他参考定义
long CExchangeData::DeleteEntityInCAD(const char* strCadName,const char* strSdeName,
		                              DIST_STRUCT_SELECTWAY* pFilter)
{
	AcDbObjectIdArray ObjIdArray;
	SelectEntityInCAD(ObjIdArray,strCadName,strSdeName,pFilter);
	int nCount = ObjIdArray.length();
	AcDbObjectId tempId;
	AcDbEntity *pEnt = NULL;
	for(int i=0;i<nCount;i++)
	{
		tempId = ObjIdArray.at(i);
		if(Acad::eOk == acdbOpenAcDbEntity(pEnt,tempId,AcDb::kForWrite))
		{
			pEnt->erase();
			pEnt->close();
		}
	}
	
	return 1;
}


//功能：判断两个字符串是否通配(例如：abc_*_cd  == abc_sadfsadfsafsdf_cd)
//参数：strSub  子字符串
//      strTP   通配字符串
//返回：如果直接相等返回2，通配返回 1，不通配返回0，其他参考定义
//备注：需特别注意返回值的定义，否则会引起使用错误
long   CExchangeData::StringLikeString(CString strSub,CString strTP)
{
	if(CompareString(strSub,strTP) == 0) return 2;
	int nPos = strTP.Find('*',0);
	if(nPos < 0) return 0;

	CString strLeft,strRight,strTempLeft,strTempRight;
	if(nPos == 0)
	{
		strTempRight = strTP.Right(strTP.GetLength()-1);
		strRight= strSub.Right(strTP.GetLength()-1);
		if(strRight.CompareNoCase(strTempRight)==0)
			return 1;
		else
			return 0;
	}
	else if(nPos == strTP.GetLength()-1)
	{
		strTempLeft = strTP.Left(strTP.GetLength()-1);
		strLeft = strSub.Left(strTP.GetLength()-1);
		if(strLeft.CompareNoCase(strTempLeft)==0)
			return 1;
		else
			return 0;
	}
	else
	{
		strTempLeft = strTP.Left(nPos);
		strTempRight = strTP.Right(strTP.GetLength()-nPos-1);

		strLeft = strSub.Left(nPos);
		strRight = strSub.Right(strTP.GetLength()-nPos-1);

		if(strLeft.CompareNoCase(strTempLeft)==0 && strRight.CompareNoCase(strTempRight)==0)
			return 1;
		else
			return 0;
	}

	return 1;
}


//功能：根据图数据库信息创建CAD图层
//参数：strSymbolTableName    符号画表名称
//      strCadMapSdeTableName SDE与CAD图层对照表
//      strSdeLyName          SDE图层名称
//返回：成功返回 1，失败返回 0，其他参考定义
long   CExchangeData::ReadInfoToCreateCadLayer(const char *strCadMapSdeTableName,const char* strSdeName)
{
	OutputDebugString("CExchangeData::ReadInfoToCreateCadLayer::  strSdeName");
	OutputDebugString(strSdeName);

	//创建命令对象
	IDistCommand *pCommand = CreateSDECommandObjcet();
	if(NULL == pCommand) return -1; //系统错误
	pCommand->SetConnectObj(m_info.m_pConnection);
	IDistRecordSet* pRcd = NULL;

	//写SQL查询CAD和SDE图层对照表
	char strSQL[200] = {0};
	sprintf(strSQL,"F:SDELYNAME,CADLYNAME,CADCLRINDEX,CADLINETYPE,CADLINESCALE,"
		           "CADLINEWIDTH,T:%s,##",strCadMapSdeTableName);
	long rc =-1;
	if((rc=pCommand->SelectData(strSQL,&pRcd)) !=1 )
	{
		check_error(rc,"ReadInfoToCreateCadLayer->SelectData");
		if(pRcd != NULL)
			pRcd->Release();
		pCommand->Release(); 
		return rc;
	}

	char   strLnType[60]={0};
	int    nClrIndex = 256;
	double dLnScal = 1.0;
	double dLnWidth = 0.00;

	char* strValue= NULL;
	char strTempSdeName[160]={0};
	int nResult = 0;
	int nFound = 0;
	while(pRcd->BatchRead() == 1)
	{
		memset(strTempSdeName,0,sizeof(char)*160); 
		strValue = (char*)(*pRcd)[0];
		if(strValue == NULL) continue;
		strcpy(strTempSdeName,strValue);  //SdeLyName

		if((nResult =StringLikeString(strSdeName,strTempSdeName)) > 0) //通配符号匹配
		{
			strValue = (char*)(*pRcd)[1];//"CadLyName"
			if(NULL != strValue)
				strcpy(m_info.m_strCadLyName,strValue);

			strValue = (char*)(*pRcd)[2];//"CadClrIndex"
			if(NULL != strValue)
				CBaseExchange::ParseColorInfo(nClrIndex,strValue);

			strValue = (char*)(*pRcd)[3];//"CadLineType"
			if(NULL != strValue)
				strcpy(strLnType,strValue);

			strValue = (char*)(*pRcd)[4];//"CadLineScale"
			if(NULL != strValue)
				dLnScal = atof(strValue);

			strValue = (char*)(*pRcd)[5];//"CadLineWidth"
			if(NULL != strValue)
				CBaseExchange::ParseLnWidth(dLnWidth,strValue);
				//dLnWidth = atoi(strValue);

			nFound = nResult;
		}
		if(nResult == 2) break;
	}
	pRcd->Release(); 
	pCommand->Release();

	if(nFound > 0) //创建图层
	{
		AcDbObjectId tempId;
		m_info.m_EntTool.CreateLayer(tempId,m_info.m_strCadLyName,nClrIndex,strLnType); 
	}

	return (nFound > 0 ? 1:0);
}


//功能：读符号化配置信息到列表结构中保存
//参数：strSymbolTableName    符号画表名称
//      strSdeLyName          SDE图层名称
//返回：成功返回 1，失败返回 0，其他参考定义
long  CExchangeData::ReadSymbolListToSave(const char *strSymbolTableName,const char* strSdeName)
{
	//创建命令对象
	IDistCommand *pCommand = CreateSDECommandObjcet();
	if(NULL == pCommand) return -1; //系统错误
	pCommand->SetConnectObj(m_info.m_pConnection);
	IDistRecordSet* pRcd = NULL;

	//写SQL查询语句
	char strSQL[200]={0};
	memset(strSQL,0,sizeof(char)*200);
	sprintf(strSQL,"F:SDELYNAME,SDELYKIND,SQLCONDITION,FLD1,FLD2,FLD3,FLD4,"
		           "FLD5,FLD6,FLD7,FLD8,FLD9,FLD10,T:%s,##",strSymbolTableName);

	//查询记录
	long rc = pCommand->SelectData(strSQL,&pRcd);
	if(rc != 1) 
	{
		if(pRcd != NULL) pRcd->Release();
		pCommand->Release();  return 0;
	}
	if(pRcd->GetRecordCount()<=0)
	{
		pRcd->Release();
		pCommand->Release();  return 0;
	}

	//预防性删除,配置信息存储结构   
	if(NULL != m_info.m_pSymbolInfo)  
	{  
		delete[] m_info.m_pSymbolInfo;
		m_info.m_pSymbolInfo = NULL;
	}

	
	DIST_SYMBOLFLD* pTemSymbol = new DIST_SYMBOLFLD[MAX_SYMBOL_COUNT];  //由于考虑到相同和匹配问题，只能遍历所有记录（开辟临时空间）
	if(NULL == pTemSymbol)
	{
		pRcd->Release();
		pCommand->Release();  return -1;
	}

	//遍历记录数据
    

	int nPos = 0;
	CString strTemp;
	char tempFldName[10]={0};
	char tempSdeName[160]={0};
	int nEquat = 0;
	while(pRcd->BatchRead()==1) //遍历
	{
		memset(tempSdeName,0,sizeof(char)*160);
		strcpy(tempSdeName,(char*)(*pRcd)["SDELYNAME"]);
		int nResult = StringLikeString(strSdeName,tempSdeName);

		if(nResult >0 && nResult>=nEquat)
		{
			if(nResult > nEquat)//有相等的存在，不需要通配
			{
				nEquat = nResult;
				nPos = 0;
			}
			
			strcpy(m_info.m_strSdeLyKind,(char*)(*pRcd)["SDELYKIND"]);  //-> 读Sde图层类型
			strcpy(pTemSymbol[nPos].m_strSqlCondition,(char*)(*pRcd)["SQLCONDITION"]); //-> 读查询条件SQL

			pTemSymbol[nPos].m_nNum = 0;//计数清零
			for(int k=0; k<10; k++)  //读FLDx字段信息
			{
				memset(tempFldName,0,sizeof(char)*10);
				sprintf(tempFldName,"FLD%d",k+1);//合成字段名称
				strcpy(pTemSymbol[nPos].m_strFld[k],(char*)(*pRcd)[tempFldName]); //-> 读字段信息（值或指定字段名称）

				int nFind =-1;
				strTemp = pTemSymbol[nPos].m_strFld[k]; 
				if((nFind = strTemp.Find('=',0)) >= 0)//当包含‘=’时表示直接取值
				{
					strTemp = strTemp.Right(strTemp.GetLength()-nFind-1);//去除字符‘=’
					strTemp.TrimLeft();strTemp.TrimRight();
					memset(pTemSymbol[nPos].m_strFld[k],0,sizeof(char)*60);
					strcpy(pTemSymbol[nPos].m_strFld[k],strTemp.GetBuffer(0));
				}
				else 
				{
					pTemSymbol[nPos].m_nPosArray[pTemSymbol[nPos].m_nNum] =k;  //记录从指定字段名中读取在数组中的位置
					pTemSymbol[nPos].m_nNum++;//计数增加
				}
			}
			if(nPos < MAX_SYMBOL_COUNT-1)  nPos++;
		}	
	}

	//释放空间
	pRcd->Release();
	pCommand->Release();

	if(nPos < 1) return 0;

	m_info.m_nSymbolCount = nPos;
	m_info.m_pSymbolInfo = new DIST_SYMBOLFLD[m_info.m_nSymbolCount]; //开辟符号画信息存储空间
	memcpy(m_info.m_pSymbolInfo,pTemSymbol,sizeof(DIST_SYMBOLFLD)*m_info.m_nSymbolCount);
	delete[] pTemSymbol; pTemSymbol = NULL;

	if(m_info.m_nSymbolCount == 1)  //只有DEFAULT
	{
		m_info.m_pSymbolInfo[0].m_strSqlCondition[0] = 0;
//		memset(m_info.m_pSymbolInfo[0].m_strSqlCondition,0,sizeof(char)*2048); //将条件清空，表示全部记录
		m_info.m_pSymbolInfo[0].m_bIsDefault = true;
		return 1;
	}

	//整理SQL查询条件，主要针对DEFAULT条件
	CString strDefaultSql;
	nPos = -1;
	for (int i=0;i<m_info.m_nSymbolCount;i++)
	{
		strTemp = m_info.m_pSymbolInfo[i].m_strSqlCondition;
		strTemp.MakeUpper();//大写
		if(strTemp.Find("DEFAULT")>=0){
			nPos = i;  //记录DEFAULT在数组中的位置
			m_info.m_pSymbolInfo[i].m_bIsDefault = true;
		}
		else
		{
			m_info.m_pSymbolInfo[i].m_bIsDefault = false;
			strDefaultSql += "("+strTemp+")OR";
		}	
	}

	if(nPos >=0) //为DEFAULT生成反查询条件
	{  
		strDefaultSql.Delete(strDefaultSql.GetLength()-2,2);  //删除最后一个OR
		strDefaultSql = "NOT("+strDefaultSql+")";
		m_info.m_pSymbolInfo[nPos].m_strSqlCondition[0] = 0;
//		memset(m_info.m_pSymbolInfo[nPos].m_strSqlCondition,0,sizeof(char)*2048);
		strcpy(m_info.m_pSymbolInfo[nPos].m_strSqlCondition,strDefaultSql.GetBuffer(0));
	}
	

	return 1;
}



//功能：读符号画配置表信息
//参数：pConnect              数据库连接对象指针
//      strSymbolTableName    符号画表名称
//      strCadMapSdeTableName SDE与CAD图层对照表
//      strSdeLyName          SDE图层名称
//      pXdataArray           需要写入实体扩展属性的字段数组
//返回：成功返回 1，失败返回 0，其他参考定义
//
//表格定义如下：
// 编号  Sde图层名    类型       查询条件      字段1   字段2   字段3   字段4   字段5   字段6  字段7  字段8 字段9 字段10 
// Id    SdeLyName    SdeLyKind  SqlCondition  Fld1    Fld2    Fld3    Fld4    Fld5    Fld6    Fld7  Fld8  Fld9   Fld10

long CExchangeData::ReadSymbolInfo(IDistConnection *pConnect,
							       const char* strSymbolTableName,
							       const char *strCadMapSdeTableName,
							       const char *strSdeLyName,
							       const char* strXDataName,
							       CStringArray* pXdataArray)

{
	long rc =-1;
	//合法性检查
	if(NULL == pConnect) return 0;

	char strTempSymbolTableName[160] = {0};
	char strTempCadMapSdeTableName[160]={0};
	//设置必要信息
	m_info.m_pConnection = pConnect;
	strcpy(m_info.m_strXDataName,strXDataName);
    
	memset(strTempSymbolTableName,0,sizeof(char)*160);//符号画表名称
	if(NULL == strSymbolTableName)
		strcpy(strTempSymbolTableName,"SYMBOLTABLE");
	else
		strcpy(strTempSymbolTableName,strSymbolTableName); 

	memset(strTempCadMapSdeTableName,0,sizeof(char)*160);//CAD和SDE图层对照表名称
	if(NULL == strCadMapSdeTableName)
		strcpy(strTempCadMapSdeTableName,"CADMAPTOSDE"); 
	else
		strcpy(strTempCadMapSdeTableName,strCadMapSdeTableName);  

	memset(m_info.m_strSdeLyName,0,sizeof(char)*160);
	strcpy(m_info.m_strSdeLyName,strSdeLyName);

	m_info.m_strXDataArrayOut.RemoveAll();
	if(NULL != pXdataArray)
	{
		m_info.m_strXDataArrayOut.Append(*pXdataArray);
	}

	if(ReadInfoToCreateCadLayer(strTempCadMapSdeTableName,strSdeLyName) != 1)
	{
		check_error(-1,"\nCadMapToSde table not exists  or Record is empty!");
		return 0;
	}

	if(ReadSymbolListToSave(strTempSymbolTableName,strSdeLyName) != 1)
	{
		OutputDebugString("!ReadSymbolListToSave");
		check_error(-1,"\nSymbol table not exists  or Record is empty!");
		return 0;
	}


	//读取SDE层字段信息（OBJECTID和SHAPE字段名称）
	m_info.m_strFldNameArray.RemoveAll();

	if(m_info.m_pParam != NULL)  //预防性删除，否则会内存泄漏
	{
		delete [] m_info.m_pParam;
		m_info.m_pParam = NULL;
		m_info.m_nParamCount = 0;
	}

	//创建命令对象
	IDistCommand *pCommand = CreateSDECommandObjcet();
	if(NULL == pCommand) return -1; //系统错误
	pCommand->SetConnectObj(m_info.m_pConnection);

	IDistParameter* pTempParam = NULL;
	if(pCommand->GetTableInfo(&pTempParam ,&m_info.m_nParamCount,m_info.m_strSdeLyName,NULL)!=1) return 0;
	m_info.m_pParam = new IDistParameter[m_info.m_nParamCount];
	for(int k=0; k<m_info.m_nParamCount;k++)
		m_info.m_pParam[k] = pTempParam[k];


	for(k = 0; k<m_info.m_nParamCount ; k++) 
	{
		if(m_info.m_pParam[k].fld_nType == kShape)
		{
			memset(m_info.m_strShapeFldName,0,sizeof(char)*60);
			strcpy(m_info.m_strShapeFldName,m_info.m_pParam[k].fld_strName);  //获取SHAPE字段名称
		}
		else if(m_info.m_pParam[k].fld_nRowIdType == SE_REGISTRATION_ROW_ID_COLUMN_TYPE_SDE)
		{
			memset(m_info.m_strRowIdFldName,0,sizeof(char)*60);
			strcpy(m_info.m_strRowIdFldName,m_info.m_pParam[k].fld_strName);//获取RowID字段名称
		}
	}

	m_pDbReactor->SetParam(m_info.m_strRowIdFldName,ROW_PART_XDATA);

    //取当前图层SHAPE类型
	if((rc=pCommand->GetLayerType(m_info.m_strSdeLyName,
		                          m_info.m_strShapeFldName,&m_info.m_nShapeType))!=1)
	{
		//CString strError;
		//strError.Format("取当前图层Shape类型失败，原因：%s",ReturnErrorCodeInfo(rc));
		//MessageBox(strError,"错误提示");
		pCommand->Release();
		return 0;
	}
	

	//获取图层坐标系信息
	if(NULL != m_info.m_Coordref)
	{
		SE_coordref_free(m_info.m_Coordref); //预防性释放
		m_info.m_Coordref =  NULL;
	}
	rc = SE_coordref_create(&m_info.m_Coordref);
	check_error (rc, "ReadSymbolInfo->SE_coordref_create");

	SE_LAYERINFO pLyInfo = NULL;
	rc = SE_layerinfo_create(NULL,&pLyInfo);
	check_error (rc, "ReadSymbolInfo->SE_layerinfo_create");

	SE_CONNECTION pConnectObj = (SE_CONNECTION) m_info.m_pConnection->GetConnectObjPointer();
	rc = SE_layer_get_info(pConnectObj,m_info.m_strSdeLyName,m_info.m_strShapeFldName,pLyInfo);
	check_error (rc, "ReadSymbolInfo->SE_layer_get_info");

	rc = SE_layerinfo_get_coordref (pLyInfo,m_info.m_Coordref);
	check_error(rc,"ReadSymbolInfo->SE_layerinfo_get_coordref");

	SE_layerinfo_free(pLyInfo);
	
	return 1;
}


// 通过CADMAPTOSDE表读取CAD图层的样式（颜色，线型，线宽）
long CExchangeData::ReadCadLayerInfo(IDistConnection * pConnection, LPCTSTR lpCadLayer, int& nColorIndex, CString& lpLineType, AcDb::LineWeight& dLineWeigth)
{
//	OutputDebugStringX("CExchangeData::ReadCadLayerInfo::%s", lpCadLayer);

	//创建命令对象
	IDistCommand *pCommand = CreateSDECommandObjcet();
	if(NULL == pCommand) return -1; //系统错误
	pCommand->SetConnectObj(pConnection);
	IDistRecordSet* pRcd = NULL;

	//写SQL查询CAD和SDE图层对照表
	char strSQL[200] = {0};
	sprintf(strSQL,"F:SDELYNAME,CADLYNAME,CADCLRINDEX,CADLINETYPE,CADLINESCALE,"
		"CADLINEWIDTH,T:%s,W:CADLYNAME='%s',##","CADMAPTOSDE", lpCadLayer);
	long rc =-1;
	if((rc=pCommand->SelectData(strSQL,&pRcd)) !=1 )
	{
		check_error(rc,"ReadInfoToCreateCadLayer->SelectData");
		if(pRcd != NULL)
			pRcd->Release();
		pCommand->Release(); 
		return rc;
	}

	char   strLnType[60]={0};
	double dLnScal = 1.0;
	double dLnWidth = 0.00;

	char* strValue= NULL;
	char strTempSdeName[160]={0};
	int nResult = 0;
	int nFound = 0;
	if (pRcd->BatchRead() == 1)
	{
		strValue = (char*)(*pRcd)[1];//"CadLyName"
		if(NULL != strValue)
			strcpy(m_info.m_strCadLyName,strValue);

		strValue = (char*)(*pRcd)[2];//"CadClrIndex"
		if(NULL != strValue)
			CBaseExchange::ParseColorInfo(nColorIndex,strValue);

		strValue = (char*)(*pRcd)[3];//"CadLineType"
		if(NULL != strValue)
			lpLineType = strValue;

		strValue = (char*)(*pRcd)[4];//"CadLineScale"
		if(NULL != strValue)
			dLnScal = atof(strValue);

		strValue = (char*)(*pRcd)[5];//"CadLineWidth"
		if(NULL != strValue)
			CBaseExchange::ParseLnWidth(dLnWidth,strValue);
		//dLnWidth = atoi(strValue);

		nFound = 1;
	}
	pRcd->Release(); 
	pCommand->Release();

	if(nFound <= 0)
		return 0;

	return 1;
}

//功能：读SDE数据到CAD中显示
//参数：pObjIdArray  返回绘制CAD实体的ID数组(类型为AcDbObjectIdArray)
//      pFilter      空间查询方式
//      strUserWhere 用户自定义的SQL查询条件
//返回：成功返回 1，失败返回 0，其他参考定义
long CExchangeData::ReadSdeDBToCAD(void* pObjIdArray,DIST_STRUCT_SELECTWAY* pFilter,
								   const char* strUserWhere,BOOL bSingleToSingle,BOOL bIsReadOnly, BOOL bDeleteExistEntity)
{
	//1.合法性检查
	if(NULL == m_info.m_pConnection){                 //a.判断数据库是否连接
		//acutPrintf("\n Database not connected , read error!");
		return 0;
	}
	if( 0 == m_info.m_nSymbolCount || NULL == m_info.m_pSymbolInfo){  //b.是否有符号化信息
		//acutPrintf("\nSymbol table is not exists or Symbol record info is empty!");
		return 0;
	}

	AcDbObjectIdArray * pArray = (AcDbObjectIdArray*) pObjIdArray;
	pArray->setLogicalLength(0);

	//关闭数据库反应器
	AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();
	pDB->removeReactor(m_pDbReactor);    

	//在数据库反应器中设置只读图层
	if(bIsReadOnly==TRUE)
		m_pDbReactor->InsertReadOnlyLayer(m_info.m_strSdeLyName);
	else
		m_pDbReactor->DeleteReadOnlyLayer(m_info.m_strSdeLyName);

	//删除指定图层中的所有实体
	if (bDeleteExistEntity)
	{
		if(bSingleToSingle == TRUE)
		{
			DeleteEntityInCAD(m_info.m_strCadLyName,NULL,NULL);
		}
		else
			DeleteEntityInCAD(m_info.m_strCadLyName,m_info.m_strSdeLyName,pFilter);

		//删除图层相对的扩展记录集
		m_pDbReactor->DeleteLayerRowIdArray(m_info.m_strCadLyName,m_info.m_strSdeLyName);
	}

	//创建命令对象
	IDistCommand* pCommand = CreateSDECommandObjcet();    
	pCommand->SetConnectObj(m_info.m_pConnection);

	//创建空间搜索过滤器
	SE_FILTER filter;
	SE_FILTER *pTempFilter = NULL;
	if(CreateFilter(pFilter,&filter)==1)
		pTempFilter = &filter;

	int i,j,k;
	char strSQL[2048] ={0};
	CString strTemp = "";
	CStringArray strTempArray;

	//根据SQL条件，进行一次或多次查询
	for(i=0; i<m_info.m_nSymbolCount; i++)
	{
		m_info.m_nPos = i;

		//读取符号画字段
		strTempArray.RemoveAll();
		m_info.m_strFldNameArray.RemoveAll();
		for(j=0; j<m_info.m_pSymbolInfo[i].m_nNum;j++)
		{
			int ntempIndex = m_info.m_pSymbolInfo[i].m_nPosArray[j];
			m_info.m_strFldNameArray.Add(m_info.m_pSymbolInfo[i].m_strFld[ntempIndex]);
		}
		strTempArray.InsertAt(0,m_info.m_strShapeFldName); //Shape字段名称
		strTempArray.InsertAt(1,m_info.m_strRowIdFldName); //SDE记录唯一字段名称（XDataIn）
		if(m_info.m_strXDataArrayOut.GetCount()>0)
			strTempArray.Append(m_info.m_strXDataArrayOut);    //用户给定需要获取的信息名称（XDataOut）
		strTempArray.Append(m_info.m_strFldNameArray);     //Shape + RowId + XdataOut字段 + 符号画字段 
		                                            

		//生成查询条件
		strTemp.Empty();
		int nCount = strTempArray.GetSize();
		for(j=0; j<nCount; j++)
			strTemp = strTemp + strTempArray.GetAt(j)+",";

//		memset(strSQL,0,sizeof(char)*2048);
		strSQL[0] = '\0';
		if( (NULL == strUserWhere)&&(strcmp(m_info.m_pSymbolInfo[i].m_strSqlCondition, "") == 0))
			sprintf(strSQL,"F:%sT:%s,##",strTemp.GetBuffer(),m_info.m_strSdeLyName);
		else if((NULL == strUserWhere)&&(strcmp(m_info.m_pSymbolInfo[i].m_strSqlCondition, "") != 0))
			sprintf(strSQL,"F:%sT:%s,W:%s,##",strTemp.GetBuffer(),m_info.m_strSdeLyName,m_info.m_pSymbolInfo[i].m_strSqlCondition);
		else if((NULL != strUserWhere)&&(strcmp(m_info.m_pSymbolInfo[i].m_strSqlCondition, "") == 0))
			sprintf(strSQL,"F:%sT:%s,W:%s,##",strTemp.GetBuffer(), m_info.m_strSdeLyName,strUserWhere);
		else
			sprintf(strSQL,"F:%sT:%s,W:%s AND %s,##",strTemp.GetBuffer(),m_info.m_strSdeLyName,m_info.m_pSymbolInfo[i].m_strSqlCondition,strUserWhere);

		strTemp.ReleaseBuffer();

		acedSetStatusBarProgressMeter("正在加载数据，请稍后．．．",0,100);  //初始化进度条

		//根据条件查询记录
		IDistRecordSet* pRcdSet = NULL;
		long rc = pCommand->SelectData(strSQL,&pRcdSet,pTempFilter); //读取字段

		if(rc != 1) 
		{
			OutputDebugString("CExchangeData::ReadSdeDBToCAD ->Access database table Error!");
			if(NULL != pRcdSet)
			{
				pRcdSet->Release(); 
				pRcdSet = NULL;
			}
			continue;
		}

		//根据不同的实体类型，创建相应的实体转换类
		CBaseExchange* pObj = NULL;		
		if(0 == CompareString(m_info.m_strSdeLyKind,"POINT"))
			pObj = new CPointExchange(&m_info);
		else if(0 == CompareString(m_info.m_strSdeLyKind,"TEXT"))
			pObj = new CTextExchange(&m_info);
		else if(0 == CompareString(m_info.m_strSdeLyKind,"POLYLINE"))
			pObj = new CPolylineExchange(&m_info);
		else if(0== CompareString(m_info.m_strSdeLyKind,"POLYGON"))
			pObj = new CPolygonExchange(&m_info);
		else if(0== CompareString(m_info.m_strSdeLyKind,"MULTIPATCH"))
			pObj = new CMultpatchExchange(&m_info);
		else
			pObj = NULL;

		int nRecordCount = pRcdSet->GetRecordCount();
		acedSetStatusBarProgressMeter("正在加载数据，请稍后．．．",0,nRecordCount/500);  //初始化进度条

		int nTimes = 0;
		k = 0;
		//遍历读取记录数据，并绘制CAD实体
		while(pRcdSet->BatchRead()==1)
		{
			//绘制CAD实体
			AcDbObjectId tempId;
			if(NULL != pObj)
			{
				pObj->DrawEntity(tempId,pRcdSet);  // 绘制实体
				pArray->append(tempId);
			}

			nTimes++;
			if(nTimes >=500)
			{
				//acedCommand(RTSTR, "_QSave",0);  //当数据量很大时，CAD出错，提示内存不足；使用该语句可以避免
				acedUpdateDisplay();

				if(acedUsrBrk())
				{
					pRcdSet->Release(); pRcdSet=NULL;
					pCommand->Release();
					acutPrintf("\n操作被中断!");
					acedRestoreStatusBar();
					return 1;
				}
				
				nTimes = 0; k++;
				acedSetStatusBarProgressMeterPos(k); //进度条更新
			}
		}

		struct resbuf * rb_cmdecho_On = acutBuildList(RTSHORT,1,0);
		acedSetVar("cmdecho", rb_cmdecho_On);
		acutRelRb(rb_cmdecho_On);

		//释放
		if(NULL != pObj) delete pObj;
		if(pRcdSet != NULL)
		{
			pRcdSet->Release(); 
			pRcdSet=NULL;
		}
	}//end for i
	
	//释放
	pCommand->Release();
	
	//5.打开数据库反应器（捕捉指定实体编辑和删除消息）
	pDB->addReactor(m_pDbReactor);

	acedRestoreStatusBar();

	CString strTempA;
	strTempA.Format("\nRead Sde Layer[%s] Success!",m_info.m_strSdeLyName);
	OutputDebugString(strTempA);

	return 1;
}

long CExchangeData::SaveEntityToSdeDB(AcDbObjectIdArray& objIds, CString strXMID, BOOL bAfterSaveToDelete, BOOL bSingleToSingle)
{
	//判断数据库是否连接
	if(NULL == m_info.m_pConnection){
		check_error(-1,"\n Database not connected , Read Error!");

		OutputDebugString("Error::CExchangeData::SaveEntityToSdeDB  ::  NULL == m_info.m_pConnection");
		return 0;
	}

	//创建命令对象
	IDistCommand* pCommand = CreateSDECommandObjcet(); 
	pCommand->SetConnectObj(m_info.m_pConnection);

	//关闭数据库反应器
	AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();
	pDB->removeReactor(m_pDbReactor);

	//合法性判断
	int nNum = objIds.length(); 
	//	if( 0 == nNum) return -1;


	//根据不同的实体类型，创建不同的交换类pObj
	CBaseExchange* pObj = NULL;		
	if(0 == CompareString(m_info.m_strSdeLyKind,"POINT"))
		pObj = new CPointExchange(&m_info);
	else if(0 == CompareString(m_info.m_strSdeLyKind,"TEXT"))
		pObj = new CTextExchange(&m_info);
	else if(0 == CompareString(m_info.m_strSdeLyKind,"POLYLINE"))
		pObj = new CPolylineExchange(&m_info);
	else if(0== CompareString(m_info.m_strSdeLyKind,"POLYGON"))
		pObj = new CPolygonExchange(&m_info);
	else if(0== CompareString(m_info.m_strSdeLyKind,"MULTIPATCH"))
		pObj = new CMultpatchExchange(&m_info);
	else
		pObj = NULL;

	acedSetStatusBarProgressMeter("正在保存数据，请稍后．．．",0,nNum/100);  //初始化进度条
	int nTimes = 0,k=0;

	int nResult = -1;
	for(int i=0; i<nNum; i++)
	{
		AcDbObjectId tempObjId = objIds.at(i);
		if(NULL != pObj)
		{
			int nLastRowId = -1;
			//			if(pObj->IsObjectHaveUsed(tempObjId)==1) continue; //考虑到多部分实体情况，避免重复添加或修改
			int nResult = pObj->WriteToSDE(&nLastRowId,pCommand,tempObjId);

			if(nResult == 0)
			{
				delete pObj;pObj = NULL;
				pCommand->Release();
				pDB->addReactor(m_pDbReactor);
				acedRestoreStatusBar();
				return 0;
			}
			if(nLastRowId > 0 && bAfterSaveToDelete ==FALSE)//向图形实体加入扩展属性（返回的RowId）
			{
				//将记录的RowId等信息写到实体的扩展属性中去
				CString strRowId;
				strRowId.Format("%d",nLastRowId);
				CStringArray FldNameArray,FldValueArray;
				FldNameArray.Add("SDELYNAME");
				FldNameArray.Add("SYMBOLPOS");
				FldNameArray.Add("OBJECTID");
				FldNameArray.Add("PARTNUM");
				FldNameArray.Add("ENTITYID1");

				FldValueArray.Add(m_info.m_strSdeLyName);
				FldValueArray.Add("0");
				FldValueArray.Add(strRowId);
				FldValueArray.Add("1");
				CString strTempId;
				strTempId.Format("%d",tempObjId.asOldId());
				FldValueArray.Add(strTempId);

				CDistXData tempXData(tempObjId,ROW_PART_XDATA);
				tempXData.Add(FldNameArray,FldValueArray);
				tempXData.Update();
				tempXData.Close();
			}
		}

		nTimes++;
		if(nTimes >=100)
		{
			nTimes = 0; k++;
			acedSetStatusBarProgressMeterPos(k); //进度条更新
		}
	}

	acedRestoreStatusBar();

	//关闭释放
	if(NULL != pObj){
		delete pObj; pObj = NULL;
	}


	//删除现有记录集合中不存在的RowID对应的SDE记录
	nNum = m_pDbReactor->GetRowIdCount(m_info.m_strCadLyName,m_info.m_strSdeLyName);
	char strSQL[200] = {0};
	for(i=0;i<nNum;i++)
	{

		memset(strSQL,0,sizeof(char)*200);
		int nRowId ;
		if(m_pDbReactor->SelectRowId(nRowId,m_info.m_strCadLyName,m_info.m_strSdeLyName,i)==1)
		{
			sprintf(strSQL,"T:%s,W:%s=%d and XMID='%s',##",m_info.m_strSdeLyName,m_info.m_strRowIdFldName,nRowId, strXMID);
			pCommand->DeleteData(strSQL);
		}

	}

	//清空扩展记录（记录修改，删除）
	m_pDbReactor->DeleteLayerRowIdArray(m_info.m_strCadLyName,m_info.m_strSdeLyName);

	//删除图层中指定范围下扩展属性SDELYNAME值为SDE层名称的所有实体
	if(bAfterSaveToDelete==TRUE) 
	{
		if(bSingleToSingle == TRUE)
			DeleteEntityInCAD(m_info.m_strCadLyName,NULL,NULL);
		else
			DeleteEntityInCAD(m_info.m_strCadLyName,m_info.m_strSdeLyName,NULL);
	}

	//释放 
	pCommand->Release();

	//打开数据库反应器
	pDB->addReactor(m_pDbReactor);

#ifdef _DEBUG
	acutPrintf("\n Save CAD Entity to SDE OK!");
#endif

	return 1;
}

// 保存新增对象
long CExchangeData::SaveInsertEntityToSdeDB(AcDbObjectIdArray& objIds, BOOL bAfterSaveToDelete, BOOL bSingleToSingle)
{
	//判断数据库是否连接
	if(NULL == m_info.m_pConnection){
		check_error(-1,"\n Database not connected , Read Error!");
		return 0;
	}

	//创建命令对象
	IDistCommand* pCommand = CreateSDECommandObjcet(); 
	pCommand->SetConnectObj(m_info.m_pConnection);

	//关闭数据库反应器
	AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();
	pDB->removeReactor(m_pDbReactor);

	//合法性判断
	int nNum = objIds.length(); 
	if( 0 == nNum) return -1;


	//根据不同的实体类型，创建不同的交换类pObj
	CBaseExchange* pObj = NULL;		
	if(0 == CompareString(m_info.m_strSdeLyKind,"POINT"))
		pObj = new CPointExchange(&m_info);
	else if(0 == CompareString(m_info.m_strSdeLyKind,"TEXT"))
		pObj = new CTextExchange(&m_info);
	else if(0 == CompareString(m_info.m_strSdeLyKind,"POLYLINE"))
		pObj = new CPolylineExchange(&m_info);
	else if(0== CompareString(m_info.m_strSdeLyKind,"POLYGON"))
		pObj = new CPolygonExchange(&m_info);
	else if(0== CompareString(m_info.m_strSdeLyKind,"MULTIPATCH"))
		pObj = new CMultpatchExchange(&m_info);
	else
		pObj = NULL;

	acedSetStatusBarProgressMeter("正在保存数据，请稍后．．．",0,nNum/100);  //初始化进度条
	int nTimes = 0,k=0;

	int nResult = -1;
	for(int i=0; i<nNum; i++)
	{
		AcDbObjectId tempObjId = objIds.at(i);
		if(NULL != pObj)
		{
			int nLastRowId = -1;

			int nResult = pObj->WriteToSDE(&nLastRowId,pCommand,tempObjId, 1); // 保存新增对象

			if(nResult == 0)
			{
				delete pObj;pObj = NULL;
				pCommand->Release();
				pDB->addReactor(m_pDbReactor);
				acedRestoreStatusBar();
				return 0;
			}
			if(nLastRowId > 0 && bAfterSaveToDelete ==FALSE)//向图形实体加入扩展属性（返回的RowId）
			{
				//将记录的RowId等信息写到实体的扩展属性中去
				CString strRowId;
				strRowId.Format("%d",nLastRowId);
				CStringArray FldNameArray,FldValueArray;
				FldNameArray.Add("SDELYNAME");
				FldNameArray.Add("SYMBOLPOS");
				FldNameArray.Add("OBJECTID");
				FldNameArray.Add("PARTNUM");
				FldNameArray.Add("ENTITYID1");

				FldValueArray.Add(m_info.m_strSdeLyName);
				FldValueArray.Add("0");
				FldValueArray.Add(strRowId);
				FldValueArray.Add("1");
				CString strTempId;
				strTempId.Format("%d",tempObjId.asOldId());
				FldValueArray.Add(strTempId);

				CDistXData tempXData(tempObjId,ROW_PART_XDATA);
				tempXData.Add(FldNameArray,FldValueArray);
				tempXData.Update();
				tempXData.Close();
			}
		}

		nTimes++;
		if(nTimes >=100)
		{
			nTimes = 0; k++;
			acedSetStatusBarProgressMeterPos(k); //进度条更新
		}
	}

	acedRestoreStatusBar();

	//关闭释放
	if(NULL != pObj){
		delete pObj; pObj = NULL;
	}

	//清空扩展记录（记录修改，删除）
	m_pDbReactor->DeleteLayerRowIdArray(m_info.m_strCadLyName,m_info.m_strSdeLyName);

	//删除图层中指定范围下扩展属性SDELYNAME值为SDE层名称的所有实体
	if(bAfterSaveToDelete==TRUE) 
	{
		if(bSingleToSingle == TRUE)
			DeleteEntityInCAD(m_info.m_strCadLyName,NULL,NULL);
		else
			DeleteEntityInCAD(m_info.m_strCadLyName,m_info.m_strSdeLyName,NULL);
	}

	//释放 
	pCommand->Release();

	//打开数据库反应器
	pDB->addReactor(m_pDbReactor);

#ifdef _DEBUG
	acutPrintf("\n Save CAD Entity to SDE OK!");
#endif

	return 1;
}

// 保存修改对象
long CExchangeData::SaveModifyEntityToSdeDB(AcDbObjectIdArray& objIds, BOOL bAfterSaveToDelete, BOOL bSingleToSingle)
{
	//判断数据库是否连接
	if(NULL == m_info.m_pConnection){
		check_error(-1,"\n Database not connected , Read Error!");
		return 0;
	}

	//创建命令对象
	IDistCommand* pCommand = CreateSDECommandObjcet(); 
	pCommand->SetConnectObj(m_info.m_pConnection);

	//关闭数据库反应器
	AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();
	pDB->removeReactor(m_pDbReactor);

	//合法性判断
	int nNum = objIds.length(); 
	if( 0 == nNum) return -1;


	//根据不同的实体类型，创建不同的交换类pObj
	CBaseExchange* pObj = NULL;		
	if(0 == CompareString(m_info.m_strSdeLyKind,"POINT"))
		pObj = new CPointExchange(&m_info);
	else if(0 == CompareString(m_info.m_strSdeLyKind,"TEXT"))
		pObj = new CTextExchange(&m_info);
	else if(0 == CompareString(m_info.m_strSdeLyKind,"POLYLINE"))
		pObj = new CPolylineExchange(&m_info);
	else if(0== CompareString(m_info.m_strSdeLyKind,"POLYGON"))
		pObj = new CPolygonExchange(&m_info);
	else if(0== CompareString(m_info.m_strSdeLyKind,"MULTIPATCH"))
		pObj = new CMultpatchExchange(&m_info);
	else
		pObj = NULL;

	acedSetStatusBarProgressMeter("正在保存数据，请稍后．．．",0,nNum/100);  //初始化进度条
	int nTimes = 0,k=0;

	int nResult = -1;
	for(int i=0; i<nNum; i++)
	{
		AcDbObjectId tempObjId = objIds.at(i);
		if(NULL != pObj)
		{
			int nLastRowId = -1;

			int nResult = pObj->WriteToSDE(&nLastRowId,pCommand,tempObjId, 2); // 保存修改对象

			if(nResult == 0)
			{
				delete pObj;pObj = NULL;
				pCommand->Release();
				pDB->addReactor(m_pDbReactor);
				acedRestoreStatusBar();
				return 0;
			}
			if(nLastRowId > 0 && bAfterSaveToDelete ==FALSE)//向图形实体加入扩展属性（返回的RowId）
			{
				//将记录的RowId等信息写到实体的扩展属性中去
				CString strRowId;
				strRowId.Format("%d",nLastRowId);
				CStringArray FldNameArray,FldValueArray;
				FldNameArray.Add("SDELYNAME");
				FldNameArray.Add("SYMBOLPOS");
				FldNameArray.Add("OBJECTID");
				FldNameArray.Add("PARTNUM");
				FldNameArray.Add("ENTITYID1");

				FldValueArray.Add(m_info.m_strSdeLyName);
				FldValueArray.Add("0");
				FldValueArray.Add(strRowId);
				FldValueArray.Add("1");
				CString strTempId;
				strTempId.Format("%d",tempObjId.asOldId());
				FldValueArray.Add(strTempId);

				CDistXData tempXData(tempObjId,ROW_PART_XDATA);
				tempXData.Add(FldNameArray,FldValueArray);
				tempXData.Update();
				tempXData.Close();
			}
		}

		nTimes++;
		if(nTimes >=100)
		{
			nTimes = 0; k++;
			acedSetStatusBarProgressMeterPos(k); //进度条更新
		}
	}

	acedRestoreStatusBar();

	//关闭释放
	if(NULL != pObj){
		delete pObj; pObj = NULL;
	}

	//清空扩展记录（记录修改，删除）
	m_pDbReactor->DeleteLayerRowIdArray(m_info.m_strCadLyName,m_info.m_strSdeLyName);

	//删除图层中指定范围下扩展属性SDELYNAME值为SDE层名称的所有实体
	if(bAfterSaveToDelete==TRUE) 
	{
		if(bSingleToSingle == TRUE)
			DeleteEntityInCAD(m_info.m_strCadLyName,NULL,NULL);
		else
			DeleteEntityInCAD(m_info.m_strCadLyName,m_info.m_strSdeLyName,NULL);
	}

	//释放 
	pCommand->Release();

	//打开数据库反应器
	pDB->addReactor(m_pDbReactor);

#ifdef _DEBUG
	acutPrintf("\n Save CAD Entity to SDE OK!");
#endif

	return 1;
}

long CExchangeData::SaveEntityToSdeDB(AcDbObjectIdArray& objIds, BOOL bAfterSaveToDelete, BOOL bSingleToSingle)
{
	//判断数据库是否连接
	if(NULL == m_info.m_pConnection){
		check_error(-1,"\n Database not connected , Read Error!");
		return 0;
	}

	//创建命令对象
	IDistCommand* pCommand = CreateSDECommandObjcet(); 
	pCommand->SetConnectObj(m_info.m_pConnection);

	//关闭数据库反应器
	AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();
	pDB->removeReactor(m_pDbReactor);

	//合法性判断
	int nNum = objIds.length(); 
	//	if( 0 == nNum) return -1;


	//根据不同的实体类型，创建不同的交换类pObj
	CBaseExchange* pObj = NULL;		
	if(0 == CompareString(m_info.m_strSdeLyKind,"POINT"))
		pObj = new CPointExchange(&m_info);
	else if(0 == CompareString(m_info.m_strSdeLyKind,"TEXT"))
		pObj = new CTextExchange(&m_info);
	else if(0 == CompareString(m_info.m_strSdeLyKind,"POLYLINE"))
		pObj = new CPolylineExchange(&m_info);
	else if(0== CompareString(m_info.m_strSdeLyKind,"POLYGON"))
		pObj = new CPolygonExchange(&m_info);
	else if(0== CompareString(m_info.m_strSdeLyKind,"MULTIPATCH"))
		pObj = new CMultpatchExchange(&m_info);
	else
		pObj = NULL;

	acedSetStatusBarProgressMeter("正在保存数据，请稍后．．．",0,nNum/100);  //初始化进度条
	int nTimes = 0,k=0;

	int nResult = -1;
	for(int i=0; i<nNum; i++)
	{
		AcDbObjectId tempObjId = objIds.at(i);
		if(NULL != pObj)
		{
			int nLastRowId = -1;
//			if(pObj->IsObjectHaveUsed(tempObjId)==1) continue; //考虑到多部分实体情况，避免重复添加或修改
			int nResult = pObj->WriteToSDE(&nLastRowId,pCommand,tempObjId);

			if(nResult == 0)
			{
				delete pObj;pObj = NULL;
				pCommand->Release();
				pDB->addReactor(m_pDbReactor);
				acedRestoreStatusBar();
				return 0;
			}
			if(nLastRowId > 0 && bAfterSaveToDelete ==FALSE)//向图形实体加入扩展属性（返回的RowId）
			{
				//将记录的RowId等信息写到实体的扩展属性中去
				CString strRowId;
				strRowId.Format("%d",nLastRowId);
				CStringArray FldNameArray,FldValueArray;
				FldNameArray.Add("SDELYNAME");
				FldNameArray.Add("SYMBOLPOS");
				FldNameArray.Add("OBJECTID");
				FldNameArray.Add("PARTNUM");
				FldNameArray.Add("ENTITYID1");

				FldValueArray.Add(m_info.m_strSdeLyName);
				FldValueArray.Add("0");
				FldValueArray.Add(strRowId);
				FldValueArray.Add("1");
				CString strTempId;
				strTempId.Format("%d",tempObjId.asOldId());
				FldValueArray.Add(strTempId);

				CDistXData tempXData(tempObjId,ROW_PART_XDATA);
				tempXData.Add(FldNameArray,FldValueArray);
				tempXData.Update();
				tempXData.Close();
			}
		}

		nTimes++;
		if(nTimes >=100)
		{
			nTimes = 0; k++;
			acedSetStatusBarProgressMeterPos(k); //进度条更新
		}
	}

	acedRestoreStatusBar();

	//关闭释放
	if(NULL != pObj){
		delete pObj; pObj = NULL;
	}
/*
	//删除现有记录集合中不存在的RowID对应的SDE记录
	nNum = m_pDbReactor->GetRowIdCount(m_info.m_strCadLyName,m_info.m_strSdeLyName);
	char strSQL[200] = {0};
	for(i=0;i<nNum;i++)
	{

		memset(strSQL,0,sizeof(char)*200);
		int nRowId ;
		if(m_pDbReactor->SelectRowId(nRowId,m_info.m_strCadLyName,m_info.m_strSdeLyName,i)==1)
		{
			sprintf(strSQL,"T:%s,W:%s=%d,##",m_info.m_strSdeLyName,m_info.m_strRowIdFldName,nRowId);
			pCommand->DeleteData(strSQL);
		}

	}
*/
	//清空扩展记录（记录修改，删除）
	m_pDbReactor->DeleteLayerRowIdArray(m_info.m_strCadLyName,m_info.m_strSdeLyName);

	//删除图层中指定范围下扩展属性SDELYNAME值为SDE层名称的所有实体
	if(bAfterSaveToDelete==TRUE) 
	{
		if(bSingleToSingle == TRUE)
			DeleteEntityInCAD(m_info.m_strCadLyName,NULL,NULL);
		else
			DeleteEntityInCAD(m_info.m_strCadLyName,m_info.m_strSdeLyName,NULL);
	}

	//释放 
	pCommand->Release();

	//打开数据库反应器
	pDB->addReactor(m_pDbReactor);

#ifdef _DEBUG
	acutPrintf("\n Save CAD Entity to SDE OK!");
#endif

	return 1;
}

//功能：保存CAD实体信息到SDE中
//参数：SelectWay           空间查询方式
//      bAfterSaveToDelete  保存后是否从CAD图层中删除对应的实体
//返回：成功返回 1，失败返回 0，-1表示某些条件不满足，其他参考定义
long CExchangeData::SaveEntityToSdeDB(DIST_STRUCT_SELECTWAY* pFilter,BOOL bAfterSaveToDelete,BOOL bSingleToSingle)
{
	//判断数据库是否连接
	if(NULL == m_info.m_pConnection){
		check_error(-1,"\n Database not connected , Read Error!");
		return 0;
	}

	//创建命令对象
	IDistCommand* pCommand = CreateSDECommandObjcet(); 
	pCommand->SetConnectObj(m_info.m_pConnection);

	//关闭数据库反应器
	AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();
	pDB->removeReactor(m_pDbReactor);


	//选择指定图层中指定范围下扩展属性SDELYNAME值为SDE层名称的所有实体(注意有三个条件)
	AcDbObjectIdArray ObjIds;
	if(bSingleToSingle == TRUE)
		SelectEntityInCAD(ObjIds,m_info.m_strCadLyName,NULL,NULL);
	else
		SelectEntityInCAD(ObjIds,m_info.m_strCadLyName,m_info.m_strSdeLyName,pFilter);


	//合法性判断
	int nNum = ObjIds.length(); 
//	if( 0 == nNum) return -1;


	//根据不同的实体类型，创建不同的交换类pObj
	CBaseExchange* pObj = NULL;		
	if(0 == CompareString(m_info.m_strSdeLyKind,"POINT"))
		pObj = new CPointExchange(&m_info);
	else if(0 == CompareString(m_info.m_strSdeLyKind,"TEXT"))
		pObj = new CTextExchange(&m_info);
	else if(0 == CompareString(m_info.m_strSdeLyKind,"POLYLINE"))
		pObj = new CPolylineExchange(&m_info);
	else if(0== CompareString(m_info.m_strSdeLyKind,"POLYGON"))
		pObj = new CPolygonExchange(&m_info);
	else if(0== CompareString(m_info.m_strSdeLyKind,"MULTIPATCH"))
		pObj = new CMultpatchExchange(&m_info);
	else
		pObj = NULL;

	acedSetStatusBarProgressMeter("正在保存数据，请稍后．．．",0,nNum/100);  //初始化进度条
	int nTimes = 0,k=0;
	
	int nResult = -1;
	for(int i=0; i<nNum; i++)
	{
		AcDbObjectId tempObjId = ObjIds.at(i);

		CDistXData tempXData(tempObjId,ROW_PART_XDATA);

		if(NULL != pObj)
		{
			int nLastRowId = -1;
			if(pObj->IsObjectHaveUsed(tempObjId)==1) continue; //考虑到多部分实体情况，避免重复添加或修改
			int nResult = pObj->WriteToSDE(&nLastRowId,pCommand,tempObjId);
			
			if(nResult == 0)
			{
				delete pObj;pObj = NULL;
				pCommand->Release();
				pDB->addReactor(m_pDbReactor);
				acedRestoreStatusBar();
				return 0;
			}
			if(nLastRowId > 0 && bAfterSaveToDelete ==FALSE)//向图形实体加入扩展属性（返回的RowId）
			{
				//将记录的RowId等信息写到实体的扩展属性中去
				CString strRowId;
				strRowId.Format("%d",nLastRowId);
				CStringArray FldNameArray,FldValueArray;
				FldNameArray.Add("SDELYNAME");
				FldNameArray.Add("SYMBOLPOS");
				FldNameArray.Add("OBJECTID");
				FldNameArray.Add("PARTNUM");
				FldNameArray.Add("ENTITYID1");

				FldValueArray.Add(m_info.m_strSdeLyName);
				FldValueArray.Add("0");
				FldValueArray.Add(strRowId);
				FldValueArray.Add("1");
				CString strTempId;
				strTempId.Format("%d",tempObjId.asOldId());
				FldValueArray.Add(strTempId);


				CDistXData tempXData(tempObjId,ROW_PART_XDATA);
				tempXData.Add(FldNameArray,FldValueArray);
				tempXData.Update();
				tempXData.Close();
			}
		}

		nTimes++;
		if(nTimes >=100)
		{
			nTimes = 0; k++;
			acedSetStatusBarProgressMeterPos(k); //进度条更新
		}
	}

	acedRestoreStatusBar();

	//关闭释放
	if(NULL != pObj){
		delete pObj; pObj = NULL;
	}


	//删除现有记录集合中不存在的RowID对应的SDE记录
	nNum = m_pDbReactor->GetRowIdCount(m_info.m_strCadLyName,m_info.m_strSdeLyName);
	char strSQL[200] = {0};
	for(i=0;i<nNum;i++)
	{
		 
		memset(strSQL,0,sizeof(char)*200);
		int nRowId ;
		if(m_pDbReactor->SelectRowId(nRowId,m_info.m_strCadLyName,m_info.m_strSdeLyName,i)==1)
		{
			sprintf(strSQL,"T:%s,W:%s=%d,##",m_info.m_strSdeLyName,m_info.m_strRowIdFldName,nRowId);
			pCommand->DeleteData(strSQL);
		}
		
	}

	//清空扩展记录（记录修改，删除）
	m_pDbReactor->DeleteLayerRowIdArray(m_info.m_strCadLyName,m_info.m_strSdeLyName);

	//删除图层中指定范围下扩展属性SDELYNAME值为SDE层名称的所有实体
	if(bAfterSaveToDelete==TRUE) 
	{
		if(bSingleToSingle == TRUE)
			DeleteEntityInCAD(m_info.m_strCadLyName,NULL,pFilter);
		else
			DeleteEntityInCAD(m_info.m_strCadLyName,m_info.m_strSdeLyName,pFilter);
	}

	//释放 
	pCommand->Release();

	//打开数据库反应器
	pDB->addReactor(m_pDbReactor);

#ifdef _DEBUG
	acutPrintf("\n Save CAD Entity to SDE OK!");
#endif

	return 1;
}





//===============================  class CBaseExchange  ===============================//

CBaseExchange::CBaseExchange()
{
	m_pInfo = NULL;
	m_ObjIdArrayHaveUsed.setLogicalLength(0);
}

CBaseExchange::CBaseExchange(CExchangeInfo* pInfo)
{
	m_pInfo = pInfo;
	m_ObjIdArrayHaveUsed.setLogicalLength(0);
}

CBaseExchange::~CBaseExchange()
{
	m_ObjIdArrayHaveUsed.setLogicalLength(0);
}


//功能：比较两个点是否相等
//参数：ptA，ptB    待比较的两个二维点
//      dPrecision  两点比较的精度
//返回：相等返回 1，不相等返回 0，其他参考定义
long CBaseExchange::IsPointEqual(AcGePoint3d ptA,AcGePoint3d ptB,double dPrecision)
{
	
	if ( (fabs(ptA.x - ptB.x) < dPrecision)  &&  (fabs(ptA.y-ptB.y)<dPrecision) )
		return 1;
	else
		return 0;
}


//功能：判断当前操作实体是否在已经操作实体数组中存在
//参数：tempObjId   判断实体ID
//返回： 存在返回 1，不存在返回 0 ，其他参考定义
//备注：该函数主要针对多部分SHAPE，就是一个SHAPE对应多个关联 CAD 实体；为提高效率，提交时，遍历到其中一个
//      子实体，就会将其他所有成员一并提交，然后在数组中记录他们已经操作过，避免重复操作
long CBaseExchange::IsObjectHaveUsed(AcDbObjectId& tempObjId)
{
	int nResult = 0;
	int nCount = m_ObjIdArrayHaveUsed.length();
	for(int i=0; i<nCount; i++)
	{
		if(tempObjId == m_ObjIdArrayHaveUsed.at(i))
		{
			nResult = 1;
			break;
		}
	}
	return nResult;
}


///功能：根据实体ID，读取扩展属性中的RowID等信息
//参数：nSymbolPos  符号化信息在其数组中的位置
//      nRowId      返回SDE记录编号
//      nPartNum    返回部分数量
//      ObjIdArray  返回所以部分对应Entity的ObjectId
//      ObjId       CAD实体ID
//返回：成功返回 1，失败返回 0，其他参考定义
long CBaseExchange::CheckObjAndReadXDataInInfo(int& nIsExists,int& nSymbolPos,int& nRowId,int& nPartNum,AcDbObjectIdArray& ObjIdArray,AcDbObjectId& ObjId)
{
	//初始化
	nIsExists = 0; nSymbolPos = 0; nRowId = -1;  nPartNum = 0; ObjIdArray.setLogicalLength(0); 

	//取实体内部扩展属性信息
	CStringArray strFldNameArray,strFldValueArray;
	CDistXData tXData(ObjId,ROW_PART_XDATA);
	tXData.Select(strFldNameArray,strFldValueArray);
	tXData.Close();//尽快关闭实体

	//提取扩展属性信息中的RowId
	CString strName,strValue;
	if(strFldValueArray.GetSize()>1)
	{
		GetValueFromArray(strValue,m_pInfo->m_strRowIdFldName,strFldNameArray,strFldValueArray);
		strValue.TrimLeft(); strValue.TrimRight();
		if(!strValue.IsEmpty())
			nRowId = atoi(strValue.GetBuffer(0));
	}


	//判断是否在对应的扩展字典中有相应得记录
	nIsExists =m_pDbReactor->IsRowIdExists(m_pInfo->m_strCadLyName,m_pInfo->m_strSdeLyName,nRowId);
	if(nRowId > 0 && (nIsExists != 1)) return 0;  //原来记录，且没有改动，不需要重新写它的XData；直接返回
	

	//分解扩展属性中的其他信息（SYMBOLPOS，OBJECTID[ROWID]，PARTNUM,ENTITYID1，...，ENTITYIDn）
	GetValueFromArray(strValue,"SYMBOLPOS",strFldNameArray,strFldValueArray);
	strValue.TrimLeft();
	strValue.TrimRight();
	if(!strValue.IsEmpty())
		nSymbolPos = atoi(strValue.GetBuffer(0));

	GetValueFromArray(strValue,"PARTNUM",strFldNameArray,strFldValueArray);
	strValue.TrimLeft();
	strValue.TrimRight();
	if(!strValue.IsEmpty())
		nPartNum = atoi(strValue.GetBuffer(0));

	AcDbObjectId tempObjId;
	for(int k=0; k<nPartNum; k++)
	{
		strName.Format("ENTITYID%d",k+1);
		GetValueFromArray(strValue,strName,strFldNameArray,strFldValueArray);
		strValue.TrimLeft();
		strValue.TrimRight();
		if(!strValue.IsEmpty())
		{
			tempObjId.setFromOldId(atoi(strValue.GetBuffer(0)));
			ObjIdArray.append(tempObjId);
		}
	}
	if(ObjIdArray.length() == 0) //考虑到新增实体，扩展属性为空
		ObjIdArray.append(ObjId);

	//if(nPartNum>0)
	m_ObjIdArrayHaveUsed.append(ObjIdArray); //将已经读取过的ObjectID保存

	return 1;
}




//功能：将内部和外部使用的信息写入实体的扩展属性
//参数：strNameArray      内部字段名称
//      nPos              在符号化信息结构中的次序
//      strRowId          实体在SDE库中对应记录的唯一编号
//      strObjArray       内部字段对应值
//      strOutXDataArray  外部字段对应值
//返回：成功返回 1，失败返回 0，其他参考定义
long CBaseExchange::WriteXDataToObjects(CStringArray& strNameArray,int nPos,const char* strRowId,CStringArray& strObjArray,CStringArray& strOutXDataArray)
{
	//将相关信息写入XData扩展属性（外部使用和内部使用）
	//内部：SYMBOLPOS->ROWID(OBJECTID)->PARTNUM->ENTITYID1->...->EntityIdn
	int nCount = strObjArray.GetSize();
	if(nCount < 1) return 0;

	strNameArray.InsertAt(0,"SDELYNAME");
	strNameArray.InsertAt(1,"SYMBOLPOS");
	strNameArray.InsertAt(2,m_pInfo->m_strRowIdFldName);
	strNameArray.InsertAt(3,"PARTNUM");

	CString strTemp;
	strObjArray.InsertAt(0,m_pInfo->m_strSdeLyName);

	strTemp.Format("%d",nPos);
	strObjArray.InsertAt(1,strTemp);
	strTemp = strRowId;
	strObjArray.InsertAt(2,strTemp);
	strTemp.Format("%d",nCount);
	strObjArray.InsertAt(3,strTemp);

	for(int i=0; i<nCount; i++)
	{
		CString strTempRowId = strObjArray.GetAt(4+i);
		int nTempRowId = atoi(strTempRowId.GetBuffer(0));
		AcDbObjectId tempID;
		tempID.setFromOldId(nTempRowId);

		CDistXData tempXData(tempID,ROW_PART_XDATA);    //写内部扩展属性
		tempXData.Add(strNameArray,strObjArray);
		tempXData.Update();

		tempXData.SetEntity(tempID,m_pInfo->m_strXDataName);//写外部扩展属性
		tempXData.Add(m_pInfo->m_strXDataArrayOut,strOutXDataArray);
		tempXData.Update();

		tempXData.Close();
	}

	return 1;
}



//功能：更新数据记录（以字符串形式提供修改值）
//参数：pCommand              执行命令对象指针
//      strFldNameArray       修改记录字段名
//      strFldValueArray      修改记录值
//      nRowId                记录的编号
//      nInsertOrUpdate       功能标记（1 表示新增插入， 2 表示修改更新）
//返回：成功返回 1，失败返回 0，其他参考定义
long CBaseExchange::SaveValuesToSDE(IDistCommand* pCommand,CStringArray& strFldNameArray,CStringArray& strFldValueArray,int& nRowId,int nInsertOrUpdate)
{
	//根据符号画表中的字段信息，依次更新需要更新的字段内容 
	int k;
	CString strFlds,strTempFldName,strTempFldValue;

	int nCount = strFldNameArray.GetSize();
	if(nCount == 0) return 1;

	char** pValues = new char*[nCount];
	int nFound = 0;
	BOOL bHasXMBH = FALSE;
	for(k=0;k<nCount;k++)
	{
		strTempFldName = strFldNameArray.GetAt(k);
		strTempFldValue = strFldValueArray.GetAt(k);

		if(CompareString(strTempFldName.GetBuffer(0),m_pInfo->m_strRowIdFldName) != 0)
		{
			for (int i=0; i<m_pInfo->m_nParamCount; i++) 
			{
				if (strcmpi(m_pInfo->m_pParam[i].fld_strName, strTempFldName) == 0)
					break;
			}

			if (i == m_pInfo->m_nParamCount)
				continue;

			strFlds = strFlds + strTempFldName +",";
			pValues[nFound] = new char[strTempFldValue.GetLength()+1];
			memset(pValues[nFound],0,sizeof(strTempFldValue.GetLength()+1));
			strcpy(pValues[nFound],strTempFldValue.GetBuffer(0));
			nFound++;
		}
	}

	long rc = -1;
	char strSQL[2048]={0};
	memset(strSQL,0,sizeof(char)*2048);
	if(nInsertOrUpdate == 2)
	{
		sprintf(strSQL,"F:%sT:%s,W:%s=%d,##",strFlds.GetBuffer(0),m_pInfo->m_strSdeLyName,m_pInfo->m_strRowIdFldName,nRowId);
		rc = pCommand->UpdateDataAsString(strSQL,pValues);

		if (rc != 1)
		{
			OutputDebugStringX("Error::CBaseExchange::SaveValuesToSDE :: pCommand->UpdateDataAsString :: %s", strSQL);
		}
	}
	else
	{
		sprintf(strSQL,"F:%sT:%s,##",strFlds.GetBuffer(0),m_pInfo->m_strSdeLyName);
		rc = pCommand->InsertDataAsString(&nRowId,strSQL,pValues);

		if (rc != 1)
		{
			OutputDebugStringX("Error::CBaseExchange::SaveValuesToSDE :: pCommand->InsertDataAsString :: %s", strSQL);
		}
	}
	

	for(k=0;k<nFound;k++)
		delete[] pValues[k];
	delete[] pValues;

	return rc;

}



//功能：从数组中获取指定名称字段的值
//参数：strValue          返回值
//      strName           指定名称
//      strFldNameArray   指定名称数组
//      strFldValueArray  指定值数组
//返回：无
void CBaseExchange::GetValueFromArray(CString& strValue,CString strName,CStringArray&strFldNameArray,CStringArray& strFldValueArray)
{
	strValue.Empty();
	int nCount = strFldNameArray.GetSize();
	for(int i=0; i<nCount; i++)
	{
		if(strName.CompareNoCase(strFldNameArray.GetAt(i))==0)
		{
			strValue = strFldValueArray.GetAt(i);
			break;
		}
	}
}


//功能：保存指定名称的值到数组中
//参数：strValue          指定值
//      strName           指定名称
//      strFldNameArray   指定名称数组
//      strFldValueArray  指定值数组
//返回：无
void CBaseExchange::SetValueToArray(CString strValue,CString strName,CStringArray&strFldNameArray,CStringArray& strFldValueArray)
{
	int nCount = strFldNameArray.GetSize();
	for(int i=0; i<nCount; i++)
	{
		if(strName.CompareNoCase(strFldNameArray.GetAt(i))==0)
		{
			strFldValueArray.SetAt(i,strValue);
			return;
		}
	}
	strFldNameArray.Add(strName);
	strFldValueArray.Add(strValue);
}

//功能：从数组中删除指定名称和值
//参数：strName           指定名称
//      strFldNameArray   指定名称数组
//      strFldValueArray  指定值数组
//返回：无
void CBaseExchange::DeleteValueFromArray(CString strName,CStringArray&strFldNameArray,CStringArray& strFldValueArray)
{
	int nCount = strFldValueArray.GetSize();
	for(int i=0; i<nCount; i++)
	{
		if(strName.CompareNoCase(strFldNameArray.GetAt(i))==0)
		{
			strFldNameArray.RemoveAt(i);
			strFldValueArray.RemoveAt(i);
			break;
		}
	}

}

//根据字符串信息，获取角度信息
//参数： dAngle   角度返回值（弧度）
//       strAngle 字符信息
//返回：无
//数据形式：角度  或  弧度#( 配置工具没有区分角度弧度,需要改进)
void CBaseExchange::ParseAngleInfo(double& dAngle,const char* strAngle)
{
	dAngle = 0.0;
	CString strTempAngle = strAngle;
	strTempAngle.TrimLeft();strTempAngle.TrimRight();
	
	if(strTempAngle.IsEmpty()) return;

	int nPos = strTempAngle.Find('#');
	if(nPos < 0)
	{
		dAngle = atof(strTempAngle.GetBuffer(0));
		dAngle = 3.14159* dAngle /180;
	}
	else
	{
		strTempAngle.Delete(nPos,1);//删除 # 号
		dAngle = atof(strTempAngle.GetBuffer(0));
	}
}

//根据字符串信息，获取颜色序号
//参数： nColorIndex   返回颜色序号
//       strColor      字符信息
//       bIsFillColor  是否为填充颜色号(需额外处理)
//返回：无
//数据形式：序号  或  R,G,B
void CBaseExchange::ParseColorInfo(int& nColorIndex,const char* strColor,DIST_COLORMAPARRAY* pData)
{
	CString strTempColor = strColor;
	strTempColor.TrimLeft(); strTempColor.TrimRight();
	nColorIndex = 256;
	if(strTempColor.IsEmpty()) return;

	int nPos = strTempColor.Find(',',0);//直接为ColorIndex
	if(nPos < 0)
	{
		if(pData!=NULL)
		{
			int nCount = pData->GetCount();
			for(int ii=0; ii<nCount; ii++)
			{
				DIST_FILLCOLORMAP tempData = pData->GetAt(ii);
				if(strTempColor.CompareNoCase(tempData.m_strDKYDXZ)==0)
				{
					nColorIndex = tempData.m_nClrIndex;
					return;
				}
			}
		}

		nColorIndex = atoi(strTempColor.GetBuffer(0));
	}
	else //分解 R,G,B#
	{
		CString strTemp;
		int nR=0,nG=0,nB=0;
		nPos = strTempColor.Find(',',0);
		if(nPos < 0) return;
		strTemp = strTempColor.Left(nPos);
		nR = atoi(strTemp.GetBuffer(0));
		strTempColor = strTempColor.Right(strTempColor.GetLength()-nPos-1);

		nPos = strTempColor.Find(',',0);
		if(nPos < 0) return;
		strTemp = strTempColor.Left(nPos);
		nG = atoi(strTemp.GetBuffer(0));
		strTempColor = strTempColor.Right(strTempColor.GetLength()-nPos-1);

		nB = atoi(strTempColor.GetBuffer(0));


		AcCmColor tempCmColor;
		tempCmColor.setRGB(nR,nG,nB);
		nColorIndex = tempCmColor.colorIndex();
	}

}


//根据字符串信息，获取块比例
//参数： dSx,dSy,dSz   块X，Y，Z方向的比例系数
//       strScale      字符信息
//返回：无
//数据形式：X比例，Y比例，Z比例 （可以是X，或X，Y 或 X，Y，Z）
void CBaseExchange::ParseScaleInfo(double& dSx,double& dSy,double& dSz,const char* strScale)
{
	dSx = 1.0; dSy = 1.0; dSz = 1.0;
	CString strTempScale = strScale;
	strTempScale.TrimLeft(); strTempScale.TrimRight();
	if(strTempScale.IsEmpty()) return;

	CString strTemp;
	int nPos = strTempScale.Find(',',0);
	if(nPos >= 0)
	{
		nPos = strTempScale.Find(',',0);
		if(nPos < 0) return;
		strTemp = strTempScale.Left(nPos);
		dSx = atof(strTemp.GetBuffer(0));
		strTempScale = strTempScale.Right(strTempScale.GetLength()-nPos-1);

		nPos = strTempScale.Find(',',0);
		if(nPos < 0) return;
		strTemp = strTempScale.Left(nPos);
		dSy = atof(strTemp.GetBuffer(0));
		strTempScale = strTempScale.Right(strTempScale.GetLength()-nPos-1);

		dSz = atoi(strTempScale.GetBuffer(0));
	}
	else
	{
		dSx = atof(strTempScale.GetBuffer(0));
	}
}


//根据字符串信息，获取文字高度
//参数： dHeight       返回文字高度
//       strTextHeight 字符信息
//返回：无
//数据形式：文字高度 或 文字高度，比例系数#
void CBaseExchange::ParseTextHeight(double& dHeight,const char* strTextHeight)
{
	dHeight = 8.0;
	CString strTempTextHeight = strTextHeight;
	strTempTextHeight.TrimLeft(); strTempTextHeight.TrimRight();
	if(strTempTextHeight.IsEmpty()) return;

	CString strTemp;
	int nPos = strTempTextHeight.Find('#',0);

	if(nPos < 0)
		dHeight = atof(strTempTextHeight.GetBuffer(0));
	else
	{
		strTempTextHeight.Delete(nPos,1); //删除 # 号
		nPos = strTempTextHeight.Find(',',0);
		if(nPos < 0) return;
		strTemp = strTempTextHeight.Left(nPos);
		dHeight = atof(strTemp.GetBuffer(0));
		strTempTextHeight = strTempTextHeight.Right(strTempTextHeight.GetLength()-nPos-1);

		if(!strTempTextHeight.IsEmpty())
		{
			dHeight = dHeight/atof(strTempTextHeight.GetBuffer(0));
		}
	}
}


//根据字符串信息，获取CAD字体样式
//参数： strStyle       返回字体样式
//       strTextStyle   字符信息
//返回：无
//数据形式：CAD字体样式  或  字体#
void CBaseExchange::ParseTextStyle(char *strStyle,const char* strTextStyle)
{
	AcDbObjectId tempId;
	AcDbTextStyleTable textTable;

	CDistEntityTool tempTool;
	if(tempTool.CreateTextStyle(tempId,strTextStyle,strTextStyle,strTextStyle)==1)
		strcpy(strStyle,strTextStyle);
	else
		strcpy(strStyle,"Standard");
}

//根据字符串信息，获取横向对齐方式
//参数： nAlign       返回对齐方式（0左对齐  1中心对齐  2右对齐）
//       strAlign     字符信息
//返回：无
//数据形式：左，中，右  或  Left，Mid，Right 或 0，1，2
void CBaseExchange::ParseTextAlign(int& nAlign,const char* strAlign)
{
	nAlign = 1;
	CString strTempAlign = strAlign;
	strTempAlign.MakeUpper();
	if(strTempAlign.Find("左",0) >= 0)
		nAlign = 0;
	else if(strTempAlign.Find("中",0) >=0)
		nAlign = 1;
	else if(strTempAlign.Find("右",0) >=0)
		nAlign = 2;
	else if(strTempAlign.Find("LEFT",0) >= 0)
		nAlign = 0;
	else if(strTempAlign.Find("CEN",0) >=0)
		nAlign = 1;
	else if(strTempAlign.Find("RIGHT",0) >=0)
		nAlign = 2;
	else if(strTempAlign.Find("0",0) >= 0)
		nAlign = 0;
	else if(strTempAlign.Find("1",0) >=0)
		nAlign = 1;
	else if(strTempAlign.Find("2",0) >=0)
		nAlign = 2;
}

void CBaseExchange::ParseLnWidth(double& dLnWidth,const char* strLnWidth)
{
	CString strTemp = strLnWidth;
	char strArray[27][8]={"Default","ByBlock","ByLayer","0.00","0.05","0.09","0.13","0.15",
                          "0.18","0.20","0.25","0.30","0.35","0.40","0.50","0.53","0.60",
                          "0.70","0.80","0.90","1.00","1.06","1.20","1.40","1.58","2.00","2.11"};

	double dArray[27]={-3,-2,-1,0.00,0.05,0.09,0.13,0.15,0.18,0.20,0.25,0.30,0.35,0.40,
		              0.50,0.53,0.60,0.70,0.80,0.90,1.00,1.06,1.20,1.40,1.58,2.00,2.11};

	dLnWidth = 0.00;
	for(int i=0;i<27;i++)
	{
		if(strTemp.Find(strArray[i],0)>=0)
		{
			dLnWidth = dArray[i]; return;
		}
	}

}
//绘制不同实体的接口函数，由不同子类自己实现
long CBaseExchange::DrawEntity(AcDbObjectId& resultId,IDistRecordSet* pRcdSet)
{
	return 0;
}


//保存不同实体数据的接口函数，由不同子类自己实现
long CBaseExchange::WriteToSDE(int* pRowId,IDistCommand* pCommand,AcDbObjectId& ObjId)
{
	return 0;	
}

long CBaseExchange::WriteToSDE(int* pRowId,IDistCommand* pCommand,AcDbObjectId& ObjId, long nType) // nType: insert=1; modify=2
{
	return 0;
}

//=============================  class CPointExchange  ==============================//

CPointExchange::CPointExchange()
{
}

CPointExchange::CPointExchange(CExchangeInfo* pInfo):CBaseExchange(pInfo)
{
	
}

CPointExchange::~CPointExchange()
{
}


long CPointExchange::DrawEntity(AcDbObjectId& resultId,IDistRecordSet* pRcdSet)
{
	long rc ;
	int i,k,nCount;
	SE_SHAPE *pShape = NULL;

	//取记录数据（数据）
	char strRowID[255]={0};
	CStringArray strXDataVArray;
	char strValue[1024] ={0};
	pShape = (SE_SHAPE*)(*pRcdSet)[m_pInfo->m_strShapeFldName];     //SHAPE值
	pRcdSet->GetValueAsString(strRowID,m_pInfo->m_strRowIdFldName); //获取到RowID的值
	

	strXDataVArray.RemoveAll();  //获取其他需要存放到外部XData的属性值
	nCount = m_pInfo->m_strXDataArrayOut.GetSize();  //XData值
	for(i=0; i<nCount; i++)
	{
		memset(strValue,0,sizeof(char)*1024);
		pRcdSet->GetValueAsString(strValue,m_pInfo->m_strXDataArrayOut.GetAt(i));
		strXDataVArray.Add(strValue);
	}

	
	//根据指定字段，读取值(符号画表信息)  【 颜色号  块名称  缩放比例  旋转角度 块属性 块路径 】
	int  nClrIndex=0;
	char strBlockName[255]={0};
	char strScale[255]={0};
	double dAngle = 0.0;
	double dSx,dSy,dSz;
	CString strAttValue;
	CString strPath;  //块路径

	int nPos = m_pInfo->m_nPos;
	int nIndex = 0;
	CString strTempValue;
	for(i=0; i<6; i++)
	{
		if(i == m_pInfo->m_pSymbolInfo[nPos].m_nPosArray[nIndex])
		{
			memset(strValue,0,sizeof(char)*1024);
			pRcdSet->GetValueAsString(strValue,m_pInfo->m_strFldNameArray.GetAt(nIndex));
			strTempValue = strValue;
			nIndex++;
		}
		else
		{
			strTempValue = m_pInfo->m_pSymbolInfo[nPos].m_strFld[i];
		}
		switch(i)
		{
		case 0: ParseColorInfo(nClrIndex,strTempValue); 
			break;
		case 1:memset(strBlockName,0,sizeof(char)*255);
			strcpy(strBlockName,strTempValue.GetBuffer(0));
			break;
		case 2:ParseScaleInfo(dSx,dSy,dSz,strTempValue);
			sprintf(strScale,"%0.2f#%0.2f#%0.2f#",dSx,dSy,dSz);
			break;
		case 3:ParseAngleInfo(dAngle,strTempValue);
			break;
		case 4:strAttValue = strTempValue;  //块属性显示字段
			   strAttValue.TrimLeft(); strAttValue.TrimRight();
		    break;
		case 5:strPath = strTempValue;  // 块文件存放子路径
		default:
			break;
		}
	}


	//从Shape结构中获取点坐标
	long nPartNum,nSubPartNum,nAllPtNum;
	rc = SE_shape_get_num_parts(*pShape,&nPartNum,&nSubPartNum); //获取Shape的部分和子部分数
	rc = SE_shape_get_num_points(*pShape,0,0,&nAllPtNum);  //获取Shape中所有点数量
	
	//获取点数组，offsetPart和offsetSubPart数组
	SE_POINT *ptArray = (SE_POINT*) malloc (nAllPtNum*sizeof(SE_POINT));
	long *pOffsetPart = (long*) malloc(nPartNum*sizeof(long));
	long *pOffsetSubPart = (long*) malloc(nSubPartNum*sizeof(long));

	rc = SE_shape_get_all_points(*pShape,SE_DEFAULT_ROTATION,pOffsetPart,pOffsetSubPart,ptArray,NULL,NULL);

	CStringArray strTempObjArray,strTempNameArray;
	strTempObjArray.RemoveAll();
	strTempNameArray.RemoveAll();
	CString strTemp;
	
	char strBuf[300]={0};
	memset(strBuf,0,sizeof(char)*300);
	strcpy(strBuf,strPath.GetBuffer(0));
	int nPosBuf =0;
	while(strBuf[nPosBuf] != 0)
	{
		if(strBuf[nPosBuf] == '\\')
			strBuf[nPosBuf]='_';
		nPosBuf++;
	}
	
	//strTempPathBlk.Replace()
	//while()


	CString tempBlockName =strBlockName;
	tempBlockName.TrimLeft();
	tempBlockName.TrimRight();
	AcGePoint3d pt;
	if(nAllPtNum == 1)
	{
		pt = AcGePoint3d(ptArray[0].x,ptArray[0].y,0);

		// 颜色号  块名称  缩放比例  旋转角度
		if(!tempBlockName.IsEmpty())
		{
			CString strDwgFName;
			strPath.TrimLeft(); strPath.TrimRight();
			if(strPath.IsEmpty())
				strDwgFName.Format("%sBlocks\\%s.dwg",GetCurArxAppPath("shDistSdeData.arx"),tempBlockName);
			else
				strDwgFName.Format("%sBlocks\\%s\\%s.dwg",GetCurArxAppPath("shDistSdeData.arx"),strPath,tempBlockName);

			tempBlockName.Format("%s%s",strBuf,tempBlockName);
			m_pInfo->m_EntTool.CreateBlockByDwgFile(tempBlockName,strDwgFName);
			m_pInfo->m_EntTool.CreateBlkRefAttr(resultId,pt,tempBlockName.GetBuffer(0),m_pInfo->m_strCadLyName,nClrIndex,strScale,dAngle,strAttValue);
		}
		else//如果块名称为空就创建点
			m_pInfo->m_EntTool.CreatePoint(resultId,pt,m_pInfo->m_strCadLyName,nClrIndex);

		strTemp.Format("%d",resultId.asOldId());
		strTempObjArray.Add(strTemp);
		strTempNameArray.Add("ENTITYID1");

	}
	else
	{
		int nTotalCount = 0;
		for(int iPart=0;iPart<nPartNum; iPart++)//遍历每部分
		{
			long tempSubPartNum = 0;
			rc = SE_shape_get_num_subparts(*pShape, iPart + 1, &tempSubPartNum); //获取Shape指定Part的子Part的数量

			for(int iSubPart = pOffsetPart[iPart]; iSubPart<pOffsetPart[iPart]+tempSubPartNum; iSubPart++)
			{
				long nSubPartPtsCount = 0;
				rc = SE_shape_get_num_points(*pShape,iPart+1,iSubPart+1,&nSubPartPtsCount);
				if(rc != SE_SUCCESS) continue;

				for(k=0; k<nSubPartPtsCount; k++)
				{
					pt = AcGePoint3d(ptArray[pOffsetSubPart[iSubPart]+k].x,ptArray[pOffsetSubPart[iSubPart]+k].y,0);

					// 颜色号  块名称  缩放比例  旋转角度
					if(!tempBlockName.IsEmpty())
					{
						CString strDwgFName;
						strPath.TrimLeft(); strPath.TrimRight();
						if(strPath.IsEmpty())
							strDwgFName.Format("%sBlocks\\%s.dwg",GetCurArxAppPath("shDistSdeData.arx"),tempBlockName);
						else
							strDwgFName.Format("%sBlocks\\%s\\%s.dwg",GetCurArxAppPath("shDistSdeData.arx"),strPath,tempBlockName);

						m_pInfo->m_EntTool.CreateBlockByDwgFile(tempBlockName,strDwgFName);
						m_pInfo->m_EntTool.CreateBlkRefAttr(resultId,pt,tempBlockName.GetBuffer(0),m_pInfo->m_strCadLyName,nClrIndex,strScale,dAngle,strAttValue);
					}
					else//如果块名称为空就创建点
						m_pInfo->m_EntTool.CreatePoint(resultId,pt,m_pInfo->m_strCadLyName,nClrIndex);
					
					strTemp.Format("%d",resultId.asOldId());
					strTempObjArray.Add(strTemp);
					nTotalCount++;
					strTemp.Format("ENTITYID%d",nTotalCount);
					strTempNameArray.Add(strTemp);
				}
			}
		}
	}
	free(pOffsetPart); free(pOffsetSubPart); free(ptArray);


	//将相关信息写入XData扩展属性（外部使用和内部使用）
	//内部：SYMBOLPOS->ROWID(OBJECTID)->PARTNUM->ENTITYID1->...->EntityIdn
	return WriteXDataToObjects(strTempNameArray,nPos,strRowID,strTempObjArray,strXDataVArray);
}

long CPointExchange::WriteToSDE(int* pRowId,IDistCommand* pCommand,AcDbObjectId& ObjId)
{
	//只处理AcDbPoint,AcDbBlockReference两种实体
	long rc = 0;
	int nIsExists,nSymbolPos,nRowId,nPartNum;
	AcDbObjectIdArray ObjIdArray;
	if(CheckObjAndReadXDataInInfo(nIsExists,nSymbolPos,nRowId,
		                          nPartNum,ObjIdArray,ObjId)==0) return -1; //(返回意思是让外部不在做后续处理)
	
	
	//读取实体相关属性（颜色号  块名称  缩放比例  旋转角度） 
	int nClrIndex=0;
	char strScal[50]={0};
	double dAngle=0.0;
	char *strBlockName = NULL;

	//以读的方式打开图形实体
	AcGePoint3d tempPt;
	AcDbEntity* pEnt = NULL;
	AcDbPoint *pPoint = NULL;
	AcDbBlockReference *pBlkRef = NULL;

	
	SE_SHAPE shape;
	SE_shape_create(m_pInfo->m_Coordref,&shape);
	check_error(rc,"WriteToSDE->SE_shape_create");

	//生成单点SHAPE或多点SHAPE
	int nPartPos = 1;
	AcDbObjectId tempObjId;
	int nObjCount = ObjIdArray.length();
	for(int i=0; i<nObjCount; i++)
	{
		tempObjId = ObjIdArray.at(i);
		if(Acad::eOk !=acdbOpenObject(pEnt,tempObjId,AcDb::kForRead)) continue;

		if(pEnt->isKindOf(AcDbBlockReference::desc()))
		{
			pBlkRef = AcDbBlockReference::cast(pEnt);

			if(i==0)
			{
				nClrIndex = pBlkRef->colorIndex(); 
				dAngle = pBlkRef->rotation();
				sprintf(strScal,"%0.2f#0.2f%0.2f#",pBlkRef->scaleFactors().sx,pBlkRef->scaleFactors().sy,pBlkRef->scaleFactors().sz);
				AcDbSymbolTableRecord *pRcd = NULL;
				AcDbObjectId ObjIdRcd = pBlkRef->blockTableRecord();
				if(Acad::eOk == acdbOpenObject(pRcd,ObjIdRcd,AcDb::kForRead))
				{
					pRcd->getName(strBlockName);//块名称
					pRcd->close();
				}
			}
			tempPt = pBlkRef->position();
		}
		else if(pEnt->isKindOf(AcDbPoint::desc()))
		{
			pPoint = AcDbPoint::cast(pEnt);
			if(i==0)
			{
				nClrIndex = pPoint->colorIndex();
				strcpy(strScal,"1.00#1.00#1.00#");
			}
			tempPt = pPoint->position();
		}
		else
		{
			pEnt->close(); continue;
		}
		pEnt->close(); pEnt = NULL;

		SE_POINT sePt ;
		sePt.x = tempPt.x;sePt.y = tempPt.y;
		if(nObjCount >1)
		{
			SE_SHAPE shape_part;
			rc = SE_shape_create(m_pInfo->m_Coordref,&shape_part);
			check_error(rc,"WriteToSDE->SE_shape_create");

			rc = SE_shape_generate_point(1,&sePt,NULL,NULL,shape_part);
			check_error(rc,"WriteToSDE->SE_shape_generate_point");

			rc = SE_shape_insert_part(shape_part,nPartPos,shape);
			check_error(rc,"WriteToSDE->SE_shape_insert_part");
			nPartPos++;
			SE_shape_free(shape_part);
		}
		else
		{
			rc = SE_shape_generate_point(1,&sePt,NULL,NULL,shape);
			check_error(rc,"WriteToSDE->SE_shape_generate_point");
		}
	}




	/*//生成SHAPE插入或修改SQL语句
	char  strSQL[2048];
	memset(strSQL,0,sizeof(char)*2048);

	if(nRowId < 1) //插入(扩展属性中没有RowId或RowId值小于1，即为新增加记录)
	{
		sprintf(strSQL,"F:%s,T:%s,##",m_pInfo->m_strShapeFldName,m_pInfo->m_strSdeLyName);
		rc = pCommand->InsertData(&nRowId,strSQL,shape); //直接插入SHAPE
	}
	else if(nIsExists == 1) //在改动数组中存在
	{
		//更新操作
		sprintf(strSQL,"F:%s,T:%s,W:%s=%d,##",m_pInfo->m_strShapeFldName,m_pInfo->m_strSdeLyName,m_pInfo->m_strRowIdFldName,nRowId);
		rc = pCommand->UpdateData(strSQL,NULL,shape);
		m_pDbReactor->DeleteRowId(m_pInfo->m_strCadLyName,m_pInfo->m_strSdeLyName,nRowId);//删除修改的记录
	}

    SE_shape_free(shape);*/


	//根据符号画表中的字段信息，依次更新需要更新的字段内容
	CDistXData tempXData(ObjId,m_pInfo->m_strXDataName);
	CStringArray strFldNameArray,strFldValueArray;
	
	tempXData.Select(strFldNameArray,strFldValueArray);  //读扩展属性字段
	
	CString strTempFldName,strTempFldValue;
	BOOL bHasXMBH = FALSE;
	for(int k=0; k<m_pInfo->m_pSymbolInfo[nSymbolPos].m_nNum; k++)
	{
		int nIndex =m_pInfo->m_pSymbolInfo[nSymbolPos].m_nPosArray[k];
		strTempFldName = m_pInfo->m_pSymbolInfo[nSymbolPos].m_strFld[nIndex];
		strTempFldValue.Empty();
		switch(nIndex)
		{
		case 0: strTempFldValue.Format("%d",nClrIndex);break;
		case 1: strTempFldValue = strBlockName;break;
		case 2: strTempFldValue = strScal;break;
		case 3: strTempFldValue.Format("%0.2f",dAngle);break;
		default:break;
		}

		SetValueToArray(strTempFldValue,strTempFldName,strFldNameArray,strFldValueArray); 
	}
	DeleteValueFromArray(m_pInfo->m_strRowIdFldName,strFldNameArray,strFldValueArray);//删除RowId字段

	CString strShapeValue;
	strShapeValue.Format("%d",(int)(shape));
	strFldValueArray.Add(strShapeValue);
	strFldNameArray.Add(m_pInfo->m_strShapeFldName);

	int nResult = 0;
	if(nRowId < 1)
		nResult = SaveValuesToSDE(pCommand,strFldNameArray,strFldValueArray,*pRowId,1);
	else if(nIsExists == 1)
	{
		*pRowId = nRowId;
		nResult = SaveValuesToSDE(pCommand,strFldNameArray,strFldValueArray,*pRowId,2);
		if(nResult == 1)
			m_pDbReactor->DeleteRowId(m_pInfo->m_strCadLyName,m_pInfo->m_strSdeLyName,*pRowId);//删除修改的记录
	}

	SE_shape_free(shape);

	if(nResult != 1)
		nResult = 0;
	return nResult;
}

void arx_getXdata( AcDbObject* pObj , const char *cAppName , int iDxf , CStringArray& aValue )
{

	struct resbuf *pRb , *pTemp ;
	pRb = pObj->xData( cAppName );

	if ( pRb != NULL )
	{
		for ( pTemp = pRb ; pTemp != NULL ; pTemp = pTemp->rbnext )
		{
			if ( pTemp->restype == iDxf ) aValue.Add( pTemp->resval.rstring );
		}
	}

	acutRelRb( pRb ) ;
}

long CPointExchange::WriteToSDE(int* pRowId,IDistCommand* pCommand,AcDbObjectId& ObjId, long nType) // nType: insert=1; modify=2
{
	//只处理AcDbPoint,AcDbBlockReference两种实体
	long rc = 0;
	int nSymbolPos = 0;
	int nRowId = -1;
	int nPartNum = 0;

	if (nType == 2)
	{
		AcDbObject * pEnt  = NULL;
		ErrorStatus es = acdbOpenObject( pEnt , ObjId , AcDb::kForRead );
		if (es == ErrorStatus::eOk)
		{
			CStringArray aValue;
			arx_getXdata(pEnt,"ROWID_OBJS",AcDb::kDxfXdAsciiString,aValue);

			if (aValue.GetCount() > 5)
				nRowId = atoi(aValue[5]);

			pEnt->close();
		}

/*
		//取实体内部扩展属性信息
		CStringArray strFldNameArray,strFldValueArray;
		CDistXData tXData(ObjId,ROW_PART_XDATA);
		tXData.Select(strFldNameArray,strFldValueArray);
		tXData.Close();//尽快关闭实体

		//提取扩展属性信息中的RowId
		CString strName,strValue;
		if(strFldValueArray.GetSize()>1)
		{
			GetValueFromArray(strValue,m_pInfo->m_strRowIdFldName,strFldNameArray,strFldValueArray);
			strValue.TrimLeft(); strValue.TrimRight();
			if(!strValue.IsEmpty())
				nRowId = atoi(strValue.GetBuffer(0));
		}
*/	}

	//读取实体相关属性（颜色号  块名称  缩放比例  旋转角度） 
	int nClrIndex=0;
	char strScal[50]={0};
	double dAngle=0.0;
	char *strBlockName = NULL;

	//以读的方式打开图形实体
	AcGePoint3d tempPt;
	AcDbEntity* pEnt = NULL;
	AcDbPoint *pPoint = NULL;
	AcDbBlockReference *pBlkRef = NULL;


	SE_SHAPE shape;
	SE_shape_create(m_pInfo->m_Coordref,&shape);
	check_error(rc,"WriteToSDE->SE_shape_create");

	//生成单点SHAPE或多点SHAPE
	int nPartPos = 1;

	if(Acad::eOk !=acdbOpenObject(pEnt,ObjId,AcDb::kForRead)) return 0;

	if(pEnt->isKindOf(AcDbBlockReference::desc()))
	{
		pBlkRef = AcDbBlockReference::cast(pEnt);

		nClrIndex = pBlkRef->colorIndex(); 
		dAngle = pBlkRef->rotation();
		sprintf(strScal,"%0.2f#0.2f%0.2f#",pBlkRef->scaleFactors().sx,pBlkRef->scaleFactors().sy,pBlkRef->scaleFactors().sz);
		AcDbSymbolTableRecord *pRcd = NULL;
		AcDbObjectId ObjIdRcd = pBlkRef->blockTableRecord();
		if(Acad::eOk == acdbOpenObject(pRcd,ObjIdRcd,AcDb::kForRead))
		{
			pRcd->getName(strBlockName);//块名称
			pRcd->close();
		}
		tempPt = pBlkRef->position();
	}
	else if(pEnt->isKindOf(AcDbPoint::desc()))
	{
		pPoint = AcDbPoint::cast(pEnt);
		nClrIndex = pPoint->colorIndex();
		strcpy(strScal,"1.00#1.00#1.00#");

		tempPt = pPoint->position();
	}
	else
	{
		pEnt->close(); return 0;
	}
	pEnt->close(); pEnt = NULL;

	SE_POINT sePt ;
	sePt.x = tempPt.x;sePt.y = tempPt.y;

	rc = SE_shape_generate_point(1,&sePt,NULL,NULL,shape);
	check_error(rc,"WriteToSDE->SE_shape_generate_point");

	//根据符号画表中的字段信息，依次更新需要更新的字段内容
	CDistXData tempXData(ObjId,m_pInfo->m_strXDataName);
	CStringArray strFldNameArray,strFldValueArray;

	tempXData.Select(strFldNameArray,strFldValueArray);  //读扩展属性字段

	CString strTempFldName,strTempFldValue;
	BOOL bHasXMBH = FALSE;
	for(int k=0; k<m_pInfo->m_pSymbolInfo[nSymbolPos].m_nNum; k++)
	{
		int nIndex =m_pInfo->m_pSymbolInfo[nSymbolPos].m_nPosArray[k];
		strTempFldName = m_pInfo->m_pSymbolInfo[nSymbolPos].m_strFld[nIndex];
		strTempFldValue.Empty();
		switch(nIndex)
		{
		case 0: strTempFldValue.Format("%d",nClrIndex);break;
		case 1: strTempFldValue = strBlockName;break;
		case 2: strTempFldValue = strScal;break;
		case 3: strTempFldValue.Format("%0.2f",dAngle);break;
		default:break;
		}

		SetValueToArray(strTempFldValue,strTempFldName,strFldNameArray,strFldValueArray); 
	}
	DeleteValueFromArray(m_pInfo->m_strRowIdFldName,strFldNameArray,strFldValueArray);//删除RowId字段

	CString strShapeValue;
	strShapeValue.Format("%d",(int)(shape));
	strFldValueArray.Add(strShapeValue);
	strFldNameArray.Add(m_pInfo->m_strShapeFldName);

	int nResult = 0;
	if(nType == 1)
		nResult = SaveValuesToSDE(pCommand,strFldNameArray,strFldValueArray,*pRowId,1);
	else
	{
		*pRowId = nRowId;
		nResult = SaveValuesToSDE(pCommand,strFldNameArray,strFldValueArray,*pRowId,2);
		if(nResult == 1)
			m_pDbReactor->DeleteRowId(m_pInfo->m_strCadLyName,m_pInfo->m_strSdeLyName,*pRowId);//删除修改的记录
	}

	SE_shape_free(shape);

	if(nResult != 1)
		nResult = 0;
	return nResult;
}



//======================================  class CTextExchange  ==================================================//

CTextExchange::CTextExchange()
{
}

CTextExchange::CTextExchange(CExchangeInfo* pInfo):CBaseExchange(pInfo)
{
}

CTextExchange::~CTextExchange()
{
}


long CTextExchange::DrawEntity(AcDbObjectId& resultId,IDistRecordSet* pRcdSet)
{
	int i,nCount;
	SE_SHAPE *pShape = NULL;

	//取记录数据（所有数据）
	
	char strValue[1024] ={0};
	pShape = (SE_SHAPE*)(*pRcdSet)[m_pInfo->m_strShapeFldName]; //SHAPE值
	int nRowId = *(int*)(*pRcdSet)["OBJECTID"];

	CStringArray strXDataVArray;
	strXDataVArray.RemoveAll();
	nCount = m_pInfo->m_strXDataArrayOut.GetSize();  //XData值
	for(i=0; i<nCount; i++)
	{
		memset(strValue,0,sizeof(char)*1024);
		pRcdSet->GetValueAsString(strValue,m_pInfo->m_strXDataArrayOut.GetAt(i));
		strXDataVArray.Add(strValue);
	}


	//根据指定字段，读取值(符号画表信息)  颜色号  内容  字体  文字高度  旋转角度  对齐方式
	int  nClrIndex=0;
	char strText[255]={0};
	char strFont[255]={0};
	double dHeight = 0.0;
	double dAngle = 0.0;
    int nAlign;

	int nPos = m_pInfo->m_nPos;
	int nIndex = 0;
	CString strTempValue;
	for(i=0; i<6; i++)
	{
		if(i == m_pInfo->m_pSymbolInfo[m_pInfo->m_nPos].m_nPosArray[nIndex])
		{
			memset(strValue,0,sizeof(char)*1024);
			pRcdSet->GetValueAsString(strValue,m_pInfo->m_strFldNameArray.GetAt(nIndex));
			strTempValue = strValue;
			nIndex++;
		}
		else
		{
			strTempValue = m_pInfo->m_pSymbolInfo[nPos].m_strFld[i];
		}
		switch(i)
		{
		case 0:ParseColorInfo(nClrIndex,strTempValue);
			break;
		case 1:memset(strText,0,sizeof(char)*255);
			strcpy(strText,strTempValue.GetBuffer(0));
			break;
		case 2:ParseTextStyle(strFont,strTempValue);
			break;
		case 3:ParseTextHeight(dHeight,strTempValue);
			break;
		case 4:ParseAngleInfo(dAngle,strTempValue);
			break;
		case 5:ParseTextAlign(nAlign,strTempValue);
			break;
		default:
			break;
		}
	}

	

	long lPointsNum = 0,rc =0;
	long lPartsNum = 0, lSubpartsNum = 0;
	SE_shape_get_num_parts(*pShape, &lPartsNum, &lSubpartsNum);
	SE_shape_get_num_points(*pShape, 0, 0, &lPointsNum);
	SE_POINT* pSdePoints = (SE_POINT *)malloc(lPointsNum * sizeof(SE_POINT));
	rc = SE_shape_get_all_points(*pShape, SE_DEFAULT_ROTATION, NULL, NULL, pSdePoints, NULL, NULL);

	 

	//3.创建图形实体，并添加到模型空间
	if (rc == SE_SUCCESS)
	{
		// 颜色号  内容  字体  文字高度  旋转角度  对齐方式
		AcGePoint3d pt = AcGePoint3d(pSdePoints[0].x,pSdePoints[0].y,0);
		m_pInfo->m_EntTool.CreateText(resultId,pt,strText,m_pInfo->m_strCadLyName,nClrIndex,strFont,nAlign,1,dHeight,0.8,dAngle);
	}
	free(pSdePoints);

	//将记录的RowId等信息写到实体的扩展属性中去
	CDistXData tempXData(resultId,m_pInfo->m_strXDataName);
	tempXData.Add(m_pInfo->m_strXDataArrayOut,strXDataVArray);
	tempXData.Update();
	tempXData.Close();


	CString strRowId;
	strRowId.Format("%d",nRowId);
	CStringArray FldNameArray,FldValueArray;
	FldNameArray.Add("SDELYNAME");
	FldNameArray.Add("SYMBOLPOS");
	FldNameArray.Add("OBJECTID");
	FldNameArray.Add("PARTNUM");
	FldNameArray.Add("ENTITYID1");

	FldValueArray.Add(m_pInfo->m_strSdeLyName);
	FldValueArray.Add("0");
	FldValueArray.Add(strRowId);
	FldValueArray.Add("1");
	CString strTempId;
	strTempId.Format("%d",resultId.asOldId());
	FldValueArray.Add(strTempId);


	CDistXData tempXDataA(resultId,ROW_PART_XDATA);
	tempXDataA.Add(FldNameArray,FldValueArray);
	tempXDataA.Update();
	tempXDataA.Close();	

	return 1;
}



long CTextExchange::WriteToSDE(int* pRowId,IDistCommand* pCommand,AcDbObjectId& ObjId)
{
	//只处理AcDbText,AcDbMText两种实体
	long rc = 0;

	int nIsExists,nSymbolPos,nRowId,nPartNum;
	AcDbObjectIdArray ObjIdArray;
	if(CheckObjAndReadXDataInInfo(nIsExists,nSymbolPos,nRowId,
		                          nPartNum,ObjIdArray,ObjId)==0) return -1; //(返回－1意思是让外部不在做后续处理)

	AcGePoint3d tempPt;
	int nClrIndex;
	char strText[255]={0};
	char strFont[60]={0};
	double dAngle;
	double dHeight;
	int nAlign;

	
	//以读的方式打开图形实体
	AcDbEntity* pEnt = NULL;
	if(Acad::eOk !=acdbOpenObject(pEnt,ObjId,AcDb::kForRead)) return -1;
	// 颜色号  内容  字体  文字高度  旋转角度  对齐方式
	if(pEnt->isKindOf(AcDbText::desc()))
	{
		AcDbText *pText = AcDbText::cast(pEnt);

		tempPt = pText->position();
		nClrIndex = pText->colorIndex();
		dHeight = pText->height();
		strcpy(strText,pText->textString());
		m_pInfo->m_EntTool.GetTextStyleName(strFont,pText->textStyle());
		dAngle = pText->rotation();
		
		nAlign = pText->horizontalMode();
	}
	else if(pEnt->isKindOf(AcDbMText::desc()))
	{
		AcDbMText *pMText = AcDbMText::cast(pEnt);

		tempPt = pMText->location();
		nClrIndex = pMText->colorIndex();
		dHeight = pMText->textHeight();
		dAngle = pMText->rotation();
		strcpy(strText,pMText->contents());
		m_pInfo->m_EntTool.GetTextStyleName(strFont,pMText->textStyle());
		nAlign = 1;
	}
	else
		return 0;
	pEnt->close();

	//取位置点
	SE_SHAPE shape;
	SE_shape_create(m_pInfo->m_Coordref,&shape);

	if(m_pInfo->m_nShapeType & kShapePoint == kShapePoint)
	{
		//注记取中心点入库
		AcDbExtents extents;
		pEnt->getGeomExtents(extents);
		AcGePoint3d maxPt = extents.maxPoint();
		AcGePoint3d minPt = extents.minPoint();
		AcGePoint3d midPt = AcGePoint3d( (maxPt.x + minPt.x)/2.0, (maxPt.y + minPt.y)/2.0,(maxPt.z + minPt.z)/2.0 );
		tempPt = minPt;
		
		SE_POINT* pts = (SE_POINT*) malloc (1 * sizeof(SE_POINT));
		pts[0].x = tempPt.x; pts[0].y = tempPt.y;
		SE_shape_generate_point(1,pts,NULL,NULL,shape);
		free(pts);
		
	}
	else if(m_pInfo->m_nShapeType & kShapeArea == kShapeArea)
	{
		//计算文字内容宽度
		AcDbExtents extents;
		pEnt->getGeomExtents(extents);
		AcGePoint3d minPt=extents.minPoint(),maxPt=extents.maxPoint();
		double dTextWidth = acutDistance(asDblArray(minPt),asDblArray(maxPt));

		SE_POINT* pts = (SE_POINT*) malloc (5 * sizeof(SE_POINT));
		pts[0].x = tempPt.x; pts[0].y = tempPt.y;
		pts[1].x = tempPt.x+dTextWidth; pts[1].y = tempPt.y;
		pts[2].x = tempPt.x+dTextWidth; pts[2].y = tempPt.y+dHeight;
		pts[3].x = tempPt.x; pts[3].y = tempPt.y+dHeight;
		pts[4].x = tempPt.x; pts[4].y = tempPt.y;

		SE_shape_generate_polygon(5,1,NULL,pts,NULL,NULL,shape);
		free(pts);
	}

	/*
	char  strSQL[500];
	memset(strSQL,0,sizeof(char)*500);

	if(nRowId < 0) //插入(扩展属性中没有RowId，即为新增加记录)
	{
		sprintf(strSQL,"F:%s,T:%s,##",m_pInfo->m_strShapeFldName,m_pInfo->m_strSdeLyName);
		pCommand->InsertData(&nRowId,strSQL,shape); //直接插入SHAPE
	}
	else if(nIsExists==1) //在改动数组中存在
	{
		//更新操作
		sprintf(strSQL,"F:%s,T:%s,W:%s=%d,##",m_pInfo->m_strShapeFldName,m_pInfo->m_strSdeLyName,m_pInfo->m_strRowIdFldName,nRowId);
		pCommand->UpdateData(strSQL,NULL,shape);
		m_pDbReactor->DeleteRowId(m_pInfo->m_strCadLyName,m_pInfo->m_strSdeLyName,nRowId);//删除修改的记录
	}
	SE_shape_free(shape);*/

	//根据符号画表中的字段信息，依次更新需要更新的字段内容 ( 颜色号  内容  字体  文字高度  旋转角度  对齐方式 )
	CString strTempFldName,strTempFldValue;
	CStringArray strFldNameArray,strFldValueArray;
	CDistXData tempXData(ObjId,m_pInfo->m_strXDataName);
	tempXData.Select(strFldNameArray,strFldValueArray);

	BOOL bHasXMBH = FALSE;
	for(int k=0; k<m_pInfo->m_pSymbolInfo[nSymbolPos].m_nNum; k++)
	{
		int nIndex =m_pInfo->m_pSymbolInfo[nSymbolPos].m_nPosArray[k];
		strTempFldName = m_pInfo->m_pSymbolInfo[nSymbolPos].m_strFld[nIndex];
		strTempFldValue.Empty();
		switch(nIndex)
		{
		case 0: strTempFldValue.Format("%d",nClrIndex);break;  //颜色号
		case 1: strTempFldValue = strText;break;               //内容
		case 2: strTempFldValue = strFont;break;               //字体
		case 3: strTempFldValue.Format("%0.2f",dHeight);break; //文字高度
		case 4: strTempFldValue.Format("%0.2f",dAngle);break;  //旋转角度
		case 5: strTempFldValue.Format("%d",nAlign);break;     //对齐方式
		default:break;
		}

		SetValueToArray(strTempFldValue,strTempFldName,strFldNameArray,strFldValueArray); 
	}
	DeleteValueFromArray(m_pInfo->m_strRowIdFldName,strFldNameArray,strFldValueArray);//删除RowId字段



	CString strShapeValue;
	strShapeValue.Format("%d",(int)(shape));
	strFldValueArray.Add(strShapeValue);
	strFldNameArray.Add(m_pInfo->m_strShapeFldName);

	int nResult = -1;
	if(nRowId < 1)
		nResult = SaveValuesToSDE(pCommand,strFldNameArray,strFldValueArray,*pRowId,1);
	else if(nIsExists == 1)
	{
		*pRowId = nRowId;
		nResult = SaveValuesToSDE(pCommand,strFldNameArray,strFldValueArray,*pRowId,2);
		if(nResult == 1)
			m_pDbReactor->DeleteRowId(m_pInfo->m_strCadLyName,m_pInfo->m_strSdeLyName,*pRowId);//删除修改的记录
	}

	SE_shape_free(shape);

	if(nResult != 1)
		nResult = 0;
	return nResult;
}

long CTextExchange::WriteToSDE(int* pRowId,IDistCommand* pCommand,AcDbObjectId& ObjId, long nType) // nType: insert=1; modify=2
{
	//只处理AcDbText,AcDbMText两种实体
	long rc = 0;

	int nSymbolPos = 0;
	int nRowId = -1;
	int nPartNum = 0;

	if (nType == 2)
	{
		//取实体内部扩展属性信息
		CStringArray strFldNameArray,strFldValueArray;
		CDistXData tXData(ObjId,ROW_PART_XDATA);
		tXData.Select(strFldNameArray,strFldValueArray);
		tXData.Close();//尽快关闭实体

		//提取扩展属性信息中的RowId
		CString strName,strValue;
		if(strFldValueArray.GetSize()>1)
		{
			GetValueFromArray(strValue,m_pInfo->m_strRowIdFldName,strFldNameArray,strFldValueArray);
			strValue.TrimLeft(); strValue.TrimRight();
			if(!strValue.IsEmpty())
				nRowId = atoi(strValue.GetBuffer(0));
		}
	}

	AcGePoint3d tempPt;
	int nClrIndex;
	char strText[255]={0};
	char strFont[60]={0};
	double dAngle;
	double dHeight;
	int nAlign;


	//以读的方式打开图形实体
	AcDbEntity* pEnt = NULL;
	if(Acad::eOk !=acdbOpenObject(pEnt,ObjId,AcDb::kForRead)) return -1;
	// 颜色号  内容  字体  文字高度  旋转角度  对齐方式
	if(pEnt->isKindOf(AcDbText::desc()))
	{
		AcDbText *pText = AcDbText::cast(pEnt);

		tempPt = pText->position();
		nClrIndex = pText->colorIndex();
		dHeight = pText->height();
		strcpy(strText,pText->textString());
		m_pInfo->m_EntTool.GetTextStyleName(strFont,pText->textStyle());
		dAngle = pText->rotation();

		nAlign = pText->horizontalMode();
	}
	else if(pEnt->isKindOf(AcDbMText::desc()))
	{
		AcDbMText *pMText = AcDbMText::cast(pEnt);

		tempPt = pMText->location();
		nClrIndex = pMText->colorIndex();
		dHeight = pMText->textHeight();
		dAngle = pMText->rotation();
		strcpy(strText,pMText->contents());
		m_pInfo->m_EntTool.GetTextStyleName(strFont,pMText->textStyle());
		nAlign = 1;
	}
	else
		return 0;
	pEnt->close();

	//取位置点
	SE_SHAPE shape;
	SE_shape_create(m_pInfo->m_Coordref,&shape);

	if(m_pInfo->m_nShapeType & kShapePoint == kShapePoint)
	{
		//注记取中心点入库
		AcDbExtents extents;
		pEnt->getGeomExtents(extents);
		AcGePoint3d maxPt = extents.maxPoint();
		AcGePoint3d minPt = extents.minPoint();
		AcGePoint3d midPt = AcGePoint3d( (maxPt.x + minPt.x)/2.0, (maxPt.y + minPt.y)/2.0,(maxPt.z + minPt.z)/2.0 );
		tempPt = minPt;

		SE_POINT* pts = (SE_POINT*) malloc (1 * sizeof(SE_POINT));
		pts[0].x = tempPt.x; pts[0].y = tempPt.y;
		SE_shape_generate_point(1,pts,NULL,NULL,shape);
		free(pts);

	}
	else if(m_pInfo->m_nShapeType & kShapeArea == kShapeArea)
	{
		//计算文字内容宽度
		AcDbExtents extents;
		pEnt->getGeomExtents(extents);
		AcGePoint3d minPt=extents.minPoint(),maxPt=extents.maxPoint();
		double dTextWidth = acutDistance(asDblArray(minPt),asDblArray(maxPt));

		SE_POINT* pts = (SE_POINT*) malloc (5 * sizeof(SE_POINT));
		pts[0].x = tempPt.x; pts[0].y = tempPt.y;
		pts[1].x = tempPt.x+dTextWidth; pts[1].y = tempPt.y;
		pts[2].x = tempPt.x+dTextWidth; pts[2].y = tempPt.y+dHeight;
		pts[3].x = tempPt.x; pts[3].y = tempPt.y+dHeight;
		pts[4].x = tempPt.x; pts[4].y = tempPt.y;

		SE_shape_generate_polygon(5,1,NULL,pts,NULL,NULL,shape);
		free(pts);
	}

	//根据符号画表中的字段信息，依次更新需要更新的字段内容 ( 颜色号  内容  字体  文字高度  旋转角度  对齐方式 )
	CString strTempFldName,strTempFldValue;
	CStringArray strFldNameArray,strFldValueArray;
	CDistXData tempXData(ObjId,m_pInfo->m_strXDataName);
	tempXData.Select(strFldNameArray,strFldValueArray);

	BOOL bHasXMBH = FALSE;
	for(int k=0; k<m_pInfo->m_pSymbolInfo[nSymbolPos].m_nNum; k++)
	{
		int nIndex =m_pInfo->m_pSymbolInfo[nSymbolPos].m_nPosArray[k];
		strTempFldName = m_pInfo->m_pSymbolInfo[nSymbolPos].m_strFld[nIndex];
		strTempFldValue.Empty();
		switch(nIndex)
		{
		case 0: strTempFldValue.Format("%d",nClrIndex);break;  //颜色号
		case 1: strTempFldValue = strText;break;               //内容
		case 2: strTempFldValue = strFont;break;               //字体
		case 3: strTempFldValue.Format("%0.2f",dHeight);break; //文字高度
		case 4: strTempFldValue.Format("%0.2f",dAngle);break;  //旋转角度
		case 5: strTempFldValue.Format("%d",nAlign);break;     //对齐方式
		default:break;
		}

		SetValueToArray(strTempFldValue,strTempFldName,strFldNameArray,strFldValueArray); 
	}
	DeleteValueFromArray(m_pInfo->m_strRowIdFldName,strFldNameArray,strFldValueArray);//删除RowId字段



	CString strShapeValue;
	strShapeValue.Format("%d",(int)(shape));
	strFldValueArray.Add(strShapeValue);
	strFldNameArray.Add(m_pInfo->m_strShapeFldName);

	int nResult = -1;
	if(nType == 1)
		nResult = SaveValuesToSDE(pCommand,strFldNameArray,strFldValueArray,*pRowId,1);
	else if(nType == 2)
	{
		*pRowId = nRowId;
		nResult = SaveValuesToSDE(pCommand,strFldNameArray,strFldValueArray,*pRowId,2);
		if(nResult == 1)
			m_pDbReactor->DeleteRowId(m_pInfo->m_strCadLyName,m_pInfo->m_strSdeLyName,*pRowId);//删除修改的记录
	}

	SE_shape_free(shape);

	if(nResult != 1)
		nResult = 0;
	return nResult;
}



//=============================  class CPolylineExchange  ==============================//

CPolylineExchange::CPolylineExchange()
{
}

CPolylineExchange::CPolylineExchange(CExchangeInfo* pInfo):CBaseExchange(pInfo)
{
}

CPolylineExchange::~CPolylineExchange()
{
}

long CPolylineExchange::DrawEntity(AcDbObjectId& resultId,IDistRecordSet* pRcdSet)
{
	long rc;
	int i,nCount;
	SE_SHAPE *pShape = NULL;

	//取记录数据（数据）
	char strRowID[255]={0};
	CStringArray strXDataVArray;
	char strValue[1024] ={0};
	pShape = (SE_SHAPE*)(*pRcdSet)[m_pInfo->m_strShapeFldName];     //SHAPE值
	pRcdSet->GetValueAsString(strRowID,m_pInfo->m_strRowIdFldName); //获取到RowID的值

	strXDataVArray.RemoveAll();
	nCount = m_pInfo->m_strXDataArrayOut.GetSize();  //XData值
	for(i=0; i<nCount; i++)
	{
		memset(strValue,0,sizeof(char)*1024);
		pRcdSet->GetValueAsString(strValue,m_pInfo->m_strXDataArrayOut.GetAt(i));
		strXDataVArray.Add(strValue);
	}


	//根据指定字段，读取值(符号画表信息) // 颜色号  线型    线宽      全局线宽   是否闭合
	int  nClrIndex=0;
	char strLnType[255]={0};
	double dWidth =0.00;
	double dGWidth = 0.00;
	bool   bIsClosed = false;

	int nPos = m_pInfo->m_nPos;
	int nIndex = 0;
	CString strTempValue;
	for(i=0; i<5; i++)
	{
		if(i == m_pInfo->m_pSymbolInfo[nPos].m_nPosArray[nIndex])
		{
			memset(strValue,0,sizeof(char)*1024);
			pRcdSet->GetValueAsString(strValue,m_pInfo->m_strFldNameArray.GetAt(nIndex));
			strTempValue = strValue;
			nIndex++;
		}
		else
		{
			strTempValue = m_pInfo->m_pSymbolInfo[nPos].m_strFld[i];
		}
		switch(i)
		{
		case 0:ParseColorInfo(nClrIndex,strTempValue);
			break;
		case 1:memset(strLnType,0,sizeof(char)*255);
			strcpy(strLnType,strTempValue.GetBuffer(0));
			break;
		case 2:
			ParseLnWidth(dWidth,strTempValue);
			break;
		case 3:
			if(strTempValue.IsEmpty())
				dGWidth = 0.0;
			else
				dGWidth = atof(strTempValue.GetBuffer(0));
			break;
		case 4:
			if(strTempValue.GetAt(0) == 't' || strTempValue.GetAt(0) == 'T')
				bIsClosed = true;
			else
				bIsClosed = false;
			break;
		default:
			break;
		}
	}



	//从Shape结构中获取点坐标
	long nPartNum,nSubPartNum,nAllPtNum;
	rc = SE_shape_get_num_parts(*pShape,&nPartNum,&nSubPartNum); //获取Shape的部分和子部分数
	rc = SE_shape_get_num_points(*pShape,0,0,&nAllPtNum);  //获取Shape中所有点数量

	//获取点数组，offsetPart和offsetSubPart数组
	SE_POINT *ptArray = (SE_POINT*) malloc (nAllPtNum*sizeof(SE_POINT));
	long *pOffsetPart = (long*) malloc(nPartNum*sizeof(long));
	long *pOffsetSubPart = (long*) malloc(nSubPartNum*sizeof(long));

	rc = SE_shape_get_all_points(*pShape,SE_DEFAULT_ROTATION,pOffsetPart,pOffsetSubPart,ptArray,NULL,NULL);

	CStringArray strTempObjArray,strTempNameArray;
	strTempObjArray.RemoveAll();
	strTempNameArray.RemoveAll();
	CString strTemp;

	int nTotalCount = 0;
	for(int iPart=0;iPart<nPartNum; iPart++)//遍历每部分
	{
		long tempSubPartNum = 0;
		rc = SE_shape_get_num_subparts(*pShape, iPart + 1, &tempSubPartNum); //获取Shape指定Part的子Part的数量

		for(int iSubPart = pOffsetPart[iPart]; iSubPart<pOffsetPart[iPart]+tempSubPartNum; iSubPart++)
		{
			long nSubPartPtsCount = 0;
			rc = SE_shape_get_num_points(*pShape,iPart+1,iSubPart+1,&nSubPartPtsCount);
			if(rc != SE_SUCCESS) continue;
			AcGePoint3dArray pt3dArray;
			for(int k=0; k<nSubPartPtsCount; k++)
			{
				AcGePoint3d pt3d = AcGePoint3d(ptArray[pOffsetSubPart[iSubPart]+k].x,ptArray[pOffsetSubPart[iSubPart]+k].y,0);
				pt3dArray.append(pt3d);

			}

			
			if(IsPointEqual(pt3dArray[0],pt3dArray[pt3dArray.length()-1],0.01) == 1)
			{
				bIsClosed = true;
				pt3dArray.removeAt(pt3dArray.length()-1);
			}
			else
				bIsClosed = false;

			if(pt3dArray.length()<2)  continue;

			m_pInfo->m_EntTool.CreatePolylineX(resultId,pt3dArray,m_pInfo->m_strCadLyName,nClrIndex,strLnType,m_pInfo->m_EntTool.DoubleToLnWeight(dWidth),dGWidth,bIsClosed);

			strTemp.Format("%d",resultId.asOldId());
			strTempObjArray.Add(strTemp);
			nTotalCount++;
			strTemp.Format("ENTITYID%d",nTotalCount);
			strTempNameArray.Add(strTemp);
			
		}
	}
	free(pOffsetPart); free(pOffsetSubPart); free(ptArray);

	//将相关信息写入XData扩展属性（外部使用和内部使用）
	//内部：SYMBOLPOS->ROWID(OBJECTID)->PARTNUM->ENTITYID1->...->EntityIdn
	return WriteXDataToObjects(strTempNameArray,nPos,strRowID,strTempObjArray,strXDataVArray);
}

long CPolylineExchange::WriteToSDE(int* pRowId,IDistCommand* pCommand,AcDbObjectId& ObjId)
{
	//只处理AcDbLine,AcDb2dPolyline,AcDbPolyline,AcDb3dPolyline,AcDbCircle,AcDbEplise六种实体
	long rc = 0;

	int nIsExists,nSymbolPos,nRowId,nPartNum;
	AcDbObjectIdArray ObjIdArray;
	if(CheckObjAndReadXDataInInfo(nIsExists,nSymbolPos,nRowId,
		                          nPartNum,ObjIdArray,ObjId)==0) return -1; //(返回－1意思是让外部不在做后续处理)

	///////////////////////////////////////WYL 20081013
	ObjIdArray.setLogicalLength(0);
	ObjIdArray.append(ObjId);
	///////////////////////////////////////WYL 20081013

	AcGePoint3d tempPt;
	AcGePoint3dArray tempPtArray;
	int nClrIndex;
	char strLnType[60]={0};
	double dWidth = 0.00;
	double dWidthG=0.0;
	bool   bClosed = false;

	int nPtNum =0;

	
	SE_SHAPE shape;
	SE_shape_create(m_pInfo->m_Coordref,&shape);
	check_error(rc,"WriteToSDE->SE_shape_create");

	//生成单点SHAPE或多点SHAPE
	int nPartPos = 1;
	AcDbObjectId tempObjId;
	int nObjCount = ObjIdArray.length();
	for(int i=0; i<nObjCount; i++)
	{
		tempObjId = ObjIdArray.at(i);
		AcDbEntity* pEnt = NULL;
		if(Acad::eOk !=acdbOpenObject(pEnt,tempObjId,AcDb::kForRead)) continue;

		// 颜色号  线型    线宽      全局线宽   是否闭合
		nClrIndex = pEnt->colorIndex();
		strcpy(strLnType, pEnt->linetype());
		dWidth = m_pInfo->m_EntTool.LnWeightToDouble(pEnt->lineWeight());

		tempPtArray.setLogicalLength(0);
		if(pEnt->isKindOf(AcDbLine::desc()))
		{
			//一般属性
			AcDbLine *pline = AcDbLine::cast(pEnt);
			//SHAPE属性
			tempPt = pline->startPoint(); tempPtArray.append(tempPt);
			tempPt = pline->endPoint(); tempPtArray.append(tempPt);
			bClosed = false;
			nPtNum = 2;
		}
		else if(pEnt->isKindOf(AcDb2dPolyline::desc()))
		{
			AcDb2dPolyline *pline = AcDb2dPolyline::cast(pEnt);
			dWidthG = pline->defaultStartWidth();
			bClosed = (Adesk::kTrue ==pline->isClosed());

			//SHAPE属性
			m_pInfo->m_EntTool.GetPoly2dlinePts(pline,tempPtArray);
			nPtNum = tempPtArray.length();	
		}
		else if(pEnt->isKindOf(AcDb3dPolyline::desc()))
		{
			AcDb3dPolyline *pline = AcDb3dPolyline::cast(pEnt);
			bClosed = (Adesk::kTrue ==pline->isClosed());

			m_pInfo->m_EntTool.GetPoly3dlinePts(pline,tempPtArray);
			nPtNum = tempPtArray.length();
		}
		else if(pEnt->isKindOf(AcDbPolyline::desc()))
		{
			AcDbPolyline *pline = AcDbPolyline::cast(pEnt);
			bClosed = (Adesk::kTrue ==pline->isClosed());

			//SHAPE属性
			m_pInfo->m_EntTool.GetPolylinePts(pline,tempPtArray);
			nPtNum = tempPtArray.length();
		}
		
		else if(pEnt->isKindOf(AcDbSpline::desc()))
		{
			AcDbSpline* pSpline = AcDbSpline::cast(pEnt);
			bClosed = (Adesk::kTrue ==pSpline->isClosed());

			m_pInfo->m_EntTool.GetSplinePts(tempPtArray,pSpline);
			nPtNum = tempPtArray.length();
		}

		else if(pEnt->isKindOf(AcDbEllipse::desc()))
		{
			AcDbEllipse* pE = AcDbEllipse::cast(pEnt);
			bClosed = (Adesk::kTrue ==pE->isClosed());

			m_pInfo->m_EntTool.GetEllipsPts(tempPtArray,pE,20);
			nPtNum = tempPtArray.length();
		}
		else if(pEnt->isKindOf(AcDbArc::desc()))
		{
			AcDbArc* pArc = AcDbArc::cast(pEnt);
			bClosed = (fabs(pArc->endAngle() - pArc->startAngle())>3.14159);

			m_pInfo->m_EntTool.GetArcPts(tempPtArray,pArc,20);
			nPtNum = tempPtArray.length();
		}
		else if(pEnt->isKindOf(AcDbCircle::desc()))
		{
			AcDbCircle* pCircle = AcDbCircle::cast(pEnt);
			bClosed = true;

			m_pInfo->m_EntTool.GetCirclePts(tempPtArray,pCircle,20);
			nPtNum = tempPtArray.length();
		}
		else
		{
			pEnt->close(); continue;
		}
		pEnt->close();

		if(nPtNum <2) continue;

		if(bClosed == true)
			tempPtArray.append(tempPtArray[0]); //如果是闭合，需要多加一个点


		nPtNum = tempPtArray.length();

		SE_POINT* ptArray = (SE_POINT*) malloc(nPtNum*sizeof(SE_POINT));  //复制点数组
		memset(ptArray,0,sizeof(SE_POINT)*nPtNum);
		for(int k=0; k<nPtNum; k++)
		{
			ptArray[k].x = tempPtArray[k].x;
			ptArray[k].y = tempPtArray[k].y;
		}

		if(nObjCount >1)
		{
			SE_SHAPE shape_part;
			rc = SE_shape_create(m_pInfo->m_Coordref,&shape_part);
			check_error(rc,"WriteToSDE->SE_shape_create");

			rc = SE_shape_generate_simple_line(nPtNum,1,NULL,ptArray,NULL,NULL,shape_part);
			check_error(rc,"WriteToSDE->SE_shape_generate_simple_line");

			rc = SE_shape_insert_part(shape_part,nPartPos,shape);
			check_error(rc,"WriteToSDE->SE_shape_insert_part");
			nPartPos++;
			SE_shape_free(shape_part);
		}
		else
		{
			rc = SE_shape_generate_line(nPtNum,1,NULL,ptArray,NULL,NULL,shape);
			check_error(rc,"WriteToSDE->SE_shape_generate_point");
		}
		free(ptArray);
	}


	/*char  strSQL[500];
	memset(strSQL,0,sizeof(char)*500);

	if(nRowId < 0) //插入(扩展属性中没有RowId，即为新增加记录)
	{
		sprintf(strSQL,"F:%s,T:%s,##",m_pInfo->m_strShapeFldName,m_pInfo->m_strSdeLyName);
		pCommand->InsertData(&nRowId,strSQL,shape); //直接插入SHAPE
	}
	else if(nIsExists==1) //在改动数组中存在
	{
		//更新操作
		sprintf(strSQL,"F:%s,T:%s,W:%s=%d,##",m_pInfo->m_strShapeFldName,m_pInfo->m_strSdeLyName,m_pInfo->m_strRowIdFldName,nRowId);
		pCommand->UpdateData(strSQL,NULL,shape);
		m_pDbReactor->DeleteRowId(m_pInfo->m_strCadLyName,m_pInfo->m_strSdeLyName,nRowId);//删除修改的记录
	}
	SE_shape_free(shape);*/

	//根据符号画表中的字段信息，依次更新需要更新的字段内容 (  颜色号  线型    线宽      全局线宽   是否闭合 )
	CString strTempFldName,strTempFldValue;
	CStringArray strFldNameArray,strFldValueArray;
	CDistXData tempXData(ObjId,m_pInfo->m_strXDataName);
	tempXData.Select(strFldNameArray,strFldValueArray);

	BOOL bHasXMBH = FALSE;
	for(int k=0; k<m_pInfo->m_pSymbolInfo[nSymbolPos].m_nNum; k++)
	{
		int nIndex =m_pInfo->m_pSymbolInfo[nSymbolPos].m_nPosArray[k];
		strTempFldName = m_pInfo->m_pSymbolInfo[nSymbolPos].m_strFld[nIndex];
		strTempFldValue.Empty();
		switch(nIndex)
		{
		case 0: strTempFldValue.Format("%d",nClrIndex);break;    //颜色号
		case 1: strTempFldValue = strLnType;break;               //线型
		case 2: strTempFldValue.Format("%0.2f",dWidth);break;    //线宽
		case 3: strTempFldValue.Format("%0.2f",dWidthG);break;   //全局线宽
		case 4: if(bClosed)                                      //是否闭合
					strTempFldValue = "TRUE";
				else
					strTempFldValue = "FALSE";
			    break;  
		default:break;
		}

		SetValueToArray(strTempFldValue,strTempFldName,strFldNameArray,strFldValueArray); 
	}
	DeleteValueFromArray(m_pInfo->m_strRowIdFldName,strFldNameArray,strFldValueArray);//删除RowId字段



	CString strShapeValue;
	strShapeValue.Format("%d",(int)(shape));
	strFldValueArray.Add(strShapeValue);
	strFldNameArray.Add(m_pInfo->m_strShapeFldName);

	int nResult = -1;
	if(nRowId < 1)
		nResult = SaveValuesToSDE(pCommand,strFldNameArray,strFldValueArray,*pRowId,1);
	else if(nIsExists == 1)
	{
		*pRowId = nRowId;
		nResult = SaveValuesToSDE(pCommand,strFldNameArray,strFldValueArray,*pRowId,2);
		if(nResult == 1)
			m_pDbReactor->DeleteRowId(m_pInfo->m_strCadLyName,m_pInfo->m_strSdeLyName,*pRowId);//删除修改的记录
	}

	SE_shape_free(shape);
	

	if(nResult != 1)
		nResult = 0;
	return nResult;
}

long CPolylineExchange::WriteToSDE(int* pRowId,IDistCommand* pCommand,AcDbObjectId& ObjId, long nType) // nType: insert=1; modify=2
{
	//只处理AcDbLine,AcDb2dPolyline,AcDbPolyline,AcDb3dPolyline,AcDbCircle,AcDbEplise六种实体
	long rc = 0;

	int nSymbolPos = 0;
	int nRowId = -1;
	int nPartNum = 0;

	if (nType == 2)
	{
		AcDbObject * pEnt  = NULL;
		ErrorStatus es = acdbOpenObject( pEnt , ObjId , AcDb::kForRead );
		if (es == ErrorStatus::eOk)
		{
			CStringArray aValue;
			arx_getXdata(pEnt,"ROWID_OBJS",AcDb::kDxfXdAsciiString,aValue);

			if (aValue.GetCount() > 5)
				nRowId = atoi(aValue[5]);

			pEnt->close();
		}

/*		//取实体内部扩展属性信息
		CStringArray strFldNameArray,strFldValueArray;
		CDistXData tXData(ObjId,ROW_PART_XDATA);
		tXData.Select(strFldNameArray,strFldValueArray);
		tXData.Close();//尽快关闭实体

		//提取扩展属性信息中的RowId
		CString strName,strValue;
		if(strFldValueArray.GetSize()>1)
		{
			GetValueFromArray(strValue,m_pInfo->m_strRowIdFldName,strFldNameArray,strFldValueArray);
			strValue.TrimLeft(); strValue.TrimRight();
			if(!strValue.IsEmpty())
				nRowId = atoi(strValue.GetBuffer(0));
		}
*/	}

	AcGePoint3d tempPt;
	AcGePoint3dArray tempPtArray;
	int nClrIndex;
	char strLnType[60]={0};
	double dWidth = 0.00;
	double dWidthG=0.0;
	bool   bClosed = false;

	int nPtNum =0;

	SE_SHAPE shape;
	SE_shape_create(m_pInfo->m_Coordref,&shape);
	check_error(rc,"WriteToSDE->SE_shape_create");

	//生成单点SHAPE或多点SHAPE
	int nPartPos = 1;

	AcDbEntity* pEnt = NULL;
	if(Acad::eOk !=acdbOpenObject(pEnt,ObjId,AcDb::kForRead)) return -1;

	// 颜色号  线型    线宽      全局线宽   是否闭合
	nClrIndex = pEnt->colorIndex();
	strcpy(strLnType, pEnt->linetype());
	dWidth = m_pInfo->m_EntTool.LnWeightToDouble(pEnt->lineWeight());

	tempPtArray.setLogicalLength(0);
	if(pEnt->isKindOf(AcDbLine::desc()))
	{
		//一般属性
		AcDbLine *pline = AcDbLine::cast(pEnt);
		//SHAPE属性
		tempPt = pline->startPoint(); tempPtArray.append(tempPt);
		tempPt = pline->endPoint(); tempPtArray.append(tempPt);
		bClosed = false;
		nPtNum = 2;
	}
	else if(pEnt->isKindOf(AcDb2dPolyline::desc()))
	{
		AcDb2dPolyline *pline = AcDb2dPolyline::cast(pEnt);
		dWidthG = pline->defaultStartWidth();
		bClosed = (Adesk::kTrue ==pline->isClosed());

		//SHAPE属性
		m_pInfo->m_EntTool.GetPoly2dlinePts(pline,tempPtArray);
		nPtNum = tempPtArray.length();	
	}
	else if(pEnt->isKindOf(AcDb3dPolyline::desc()))
	{
		AcDb3dPolyline *pline = AcDb3dPolyline::cast(pEnt);
		bClosed = (Adesk::kTrue ==pline->isClosed());

		m_pInfo->m_EntTool.GetPoly3dlinePts(pline,tempPtArray);
		nPtNum = tempPtArray.length();
	}
	else if(pEnt->isKindOf(AcDbPolyline::desc()))
	{
		AcDbPolyline *pline = AcDbPolyline::cast(pEnt);
		bClosed = (Adesk::kTrue ==pline->isClosed());

		//SHAPE属性
		m_pInfo->m_EntTool.GetPolylinePts(pline,tempPtArray);
		nPtNum = tempPtArray.length();
	}

	else if(pEnt->isKindOf(AcDbSpline::desc()))
	{
		AcDbSpline* pSpline = AcDbSpline::cast(pEnt);
		bClosed = (Adesk::kTrue ==pSpline->isClosed());

		m_pInfo->m_EntTool.GetSplinePts(tempPtArray,pSpline);
		nPtNum = tempPtArray.length();
	}

	else if(pEnt->isKindOf(AcDbEllipse::desc()))
	{
		AcDbEllipse* pE = AcDbEllipse::cast(pEnt);
		bClosed = (Adesk::kTrue ==pE->isClosed());

		m_pInfo->m_EntTool.GetEllipsPts(tempPtArray,pE,20);
		nPtNum = tempPtArray.length();
	}
	else if(pEnt->isKindOf(AcDbArc::desc()))
	{
		AcDbArc* pArc = AcDbArc::cast(pEnt);
		double end = pArc->endAngle();
		double start = pArc->startAngle();
		if ((end < start) && (start > 3.14159)) start -= 3.14159;
		bClosed = (fabs(end - start)>3.14159);

		m_pInfo->m_EntTool.GetArcPts(tempPtArray,pArc,20);
		nPtNum = tempPtArray.length();
	}
	else if(pEnt->isKindOf(AcDbCircle::desc()))
	{
		AcDbCircle* pCircle = AcDbCircle::cast(pEnt);
		bClosed = true;

		m_pInfo->m_EntTool.GetCirclePts(tempPtArray,pCircle,20);
		nPtNum = tempPtArray.length();
	}
	else
	{
		pEnt->close(); return -1;
	}
	pEnt->close();

	if(nPtNum <2) return -1;

	if(bClosed == true)
		tempPtArray.append(tempPtArray[0]); //如果是闭合，需要多加一个点

	nPtNum = tempPtArray.length();

	SE_POINT* ptArray = (SE_POINT*) malloc(nPtNum*sizeof(SE_POINT));  //复制点数组
	memset(ptArray,0,sizeof(SE_POINT)*nPtNum);
	for(int k=0; k<nPtNum; k++)
	{
		ptArray[k].x = tempPtArray[k].x;
		ptArray[k].y = tempPtArray[k].y;
	}

	rc = SE_shape_generate_line(nPtNum,1,NULL,ptArray,NULL,NULL,shape);
	check_error(rc,"WriteToSDE->SE_shape_generate_point");

	free(ptArray);

	//根据符号画表中的字段信息，依次更新需要更新的字段内容 (  颜色号  线型    线宽      全局线宽   是否闭合 )
	CString strTempFldName,strTempFldValue;
	CStringArray strFldNameArray,strFldValueArray;
	CDistXData tempXData(ObjId,m_pInfo->m_strXDataName);
	tempXData.Select(strFldNameArray,strFldValueArray);

	BOOL bHasXMBH = FALSE;
	for(int k=0; k<m_pInfo->m_pSymbolInfo[nSymbolPos].m_nNum; k++)
	{
		int nIndex =m_pInfo->m_pSymbolInfo[nSymbolPos].m_nPosArray[k];
		strTempFldName = m_pInfo->m_pSymbolInfo[nSymbolPos].m_strFld[nIndex];
		strTempFldValue.Empty();
		switch(nIndex)
		{
		case 0: strTempFldValue.Format("%d",nClrIndex);break;    //颜色号
		case 1: strTempFldValue = strLnType;break;               //线型
		case 2: strTempFldValue.Format("%0.2f",dWidth);break;    //线宽
		case 3: strTempFldValue.Format("%0.2f",dWidthG);break;   //全局线宽
		case 4: if(bClosed)                                      //是否闭合
					strTempFldValue = "TRUE";
				else
					strTempFldValue = "FALSE";
			break;  
		default:break;
		}

		SetValueToArray(strTempFldValue,strTempFldName,strFldNameArray,strFldValueArray); 
	}
	DeleteValueFromArray(m_pInfo->m_strRowIdFldName,strFldNameArray,strFldValueArray);//删除RowId字段


	CString strShapeValue;
	strShapeValue.Format("%d",(int)(shape));
	strFldValueArray.Add(strShapeValue);
	strFldNameArray.Add(m_pInfo->m_strShapeFldName);

	int nResult = -1;
	if(nType == 1)
		nResult = SaveValuesToSDE(pCommand,strFldNameArray,strFldValueArray,*pRowId,1);
	else if(nType == 2)
	{
		*pRowId = nRowId;
		nResult = SaveValuesToSDE(pCommand,strFldNameArray,strFldValueArray,*pRowId,2);
		if(nResult == 1)
			m_pDbReactor->DeleteRowId(m_pInfo->m_strCadLyName,m_pInfo->m_strSdeLyName,*pRowId);//删除修改的记录
	}

	SE_shape_free(shape);

	if(nResult != 1)
		nResult = 0;
	return nResult;
}


//=============================  class CPolygonExchange  ==============================//

CPolygonExchange::CPolygonExchange()
{
}

CPolygonExchange::CPolygonExchange(CExchangeInfo* pInfo):CBaseExchange(pInfo)
{
}

CPolygonExchange::~CPolygonExchange()
{
}


long CPolygonExchange::DrawEntity(AcDbObjectId& resultId,IDistRecordSet* pRcdSet)
{
	long rc;
	int i,nCount;
	SE_SHAPE *pShape = NULL;

	//取记录数据（数据）
	char strRowID[255]={0};
	CStringArray strXDataVArray;
	char strValue[1024] ={0};
	pShape = (SE_SHAPE*)(*pRcdSet)[m_pInfo->m_strShapeFldName];     //SHAPE值
	pRcdSet->GetValueAsString(strRowID,m_pInfo->m_strRowIdFldName); //获取到RowID的值

	strXDataVArray.RemoveAll();
	nCount = m_pInfo->m_strXDataArrayOut.GetSize();  //XData值
	for(i=0; i<nCount; i++)
	{
		memset(strValue,0,sizeof(char)*1024);
		pRcdSet->GetValueAsString(strValue,m_pInfo->m_strXDataArrayOut.GetAt(i));
		strXDataVArray.Add(strValue);
	}



	//根据指定字段，读取值(符号画表信息) // 颜色号  线型 线宽  全局线宽 填充色号 图案名称 图案比例 图案角度
	int  nClrIndex=0;
	char strLnType[255]={0};
	double dWidth =0.00;
	double dGWidth = 0.00;
	int nClrIndexF = 0;
	char strFillName[255]={0};
	double dScale = 1.0;
	double dAngle = 0.0;

	BOOL bNotFill = FALSE;
	int nPos = m_pInfo->m_nPos;
	int nIndex = 0;
	CString strTempValue;
	for(i=0; i<8; i++)
	{
		if(i == m_pInfo->m_pSymbolInfo[nPos].m_nPosArray[nIndex])
		{
			memset(strValue,0,sizeof(char)*1024);
			if(nIndex < m_pInfo->m_strFldNameArray.GetCount())
			{
				pRcdSet->GetValueAsString(strValue,m_pInfo->m_strFldNameArray.GetAt(nIndex));
				strTempValue = strValue;
			}
			else
				strTempValue.Empty();
			
			nIndex++;
		}
		else
		{
			strTempValue = m_pInfo->m_pSymbolInfo[nPos].m_strFld[i];
		}
		switch(i)
		{
		case 0:
			if(strTempValue.IsEmpty())
				nClrIndex = 0;
			else
				ParseColorInfo(nClrIndex,strTempValue.GetBuffer(0));
			break;
		case 1:memset(strLnType,0,sizeof(char)*255);
			strcpy(strLnType,strTempValue.GetBuffer(0));
			break;
		case 2:
			ParseLnWidth(dWidth,strTempValue);
		case 3:
			if(strTempValue.IsEmpty())
				dGWidth = 0.0;
			else
				dGWidth = atof(strTempValue.GetBuffer(0));
			break;
		case 4:
			if(strTempValue.IsEmpty())
			{
				nClrIndexF = 256; bNotFill = TRUE;
			}
			else
				ParseColorInfo(nClrIndexF,strTempValue.GetBuffer(0),&m_pInfo->m_MapColorArray);
			break;
		case 5:
			if(strTempValue.IsEmpty())
			{
				strcpy(strFillName," "); bNotFill = TRUE;
			}
			else
				strcpy(strFillName,strTempValue.GetBuffer(0));
			break;
		case 6:
			if(strTempValue.IsEmpty())
			{
				dScale = 1.0; bNotFill = TRUE;
			}
			else
				dScale = atof(strTempValue.GetBuffer(0));
			break;
		case 7:
			if(strTempValue.IsEmpty())
			{
				dAngle = 1.0; bNotFill = TRUE;
			}
			else
				dAngle = atof(strTempValue.GetBuffer(0));
			break;

		default:
			break;
		}
	}


	//////////////////////////////////////////////////////////////////////////
	//从Shape结构中获取点坐标
	long nPartNum,nSubPartNum,nAllPtNum;
	rc = SE_shape_get_num_parts(*pShape,&nPartNum,&nSubPartNum); //获取Shape的部分和子部分数
	rc = SE_shape_get_num_points(*pShape,0,0,&nAllPtNum);  //获取Shape中所有点数量

	//获取点数组，offsetPart和offsetSubPart数组
	SE_POINT *ptArray = (SE_POINT*) malloc (nAllPtNum*sizeof(SE_POINT));
	long *pOffsetPart = (long*) malloc(nPartNum*sizeof(long));
	long *pOffsetSubPart = (long*) malloc(nSubPartNum*sizeof(long));

	rc = SE_shape_get_all_points(*pShape,SE_DEFAULT_ROTATION,pOffsetPart,pOffsetSubPart,ptArray,NULL,NULL);

	CStringArray strTempObjArray,strTempNameArray;
	strTempObjArray.RemoveAll();
	strTempNameArray.RemoveAll();
	CString strTemp;

	int nTotalCount = 0;
	AcDbObjectIdArray FillObjArray;
	for(int iPart=0;iPart<nPartNum; iPart++)//遍历每部分
	{
		long tempSubPartNum = 0;
		rc = SE_shape_get_num_subparts(*pShape, iPart + 1, &tempSubPartNum); //获取Shape指定Part的子Part的数量

		for(int iSubPart = pOffsetPart[iPart]; iSubPart<pOffsetPart[iPart]+tempSubPartNum; iSubPart++)
		{
			long nSubPartPtsCount = 0;
			rc = SE_shape_get_num_points(*pShape,iPart+1,iSubPart+1,&nSubPartPtsCount);
			if(rc != SE_SUCCESS) continue;
			AcGePoint3dArray pt3dArray;
			pt3dArray.setLogicalLength(0);
			for(int k=0; k<nSubPartPtsCount; k++)
			{
				AcGePoint3d pt3d = AcGePoint3d(ptArray[pOffsetSubPart[iSubPart]+k].x,ptArray[pOffsetSubPart[iSubPart]+k].y,0);
				pt3dArray.append(pt3d);
			}

			//if(pt3dArray[0] == pt3dArray[pt3dArray.length()-1])
				pt3dArray.removeAt(pt3dArray.length()-1);

			if(pt3dArray.length()<3)  continue;

			m_pInfo->m_EntTool.CreatePolylineX(resultId,pt3dArray,m_pInfo->m_strCadLyName,nClrIndex,
				                               strLnType,m_pInfo->m_EntTool.DoubleToLnWeight(dWidth),dGWidth,true);
			FillObjArray.append(resultId);

			strTemp.Format("%d",resultId.asOldId());
			strTempObjArray.Add(strTemp);
			nTotalCount++;
			strTemp.Format("ENTITYID%d",nTotalCount);
			strTempNameArray.Add(strTemp);
		}
	}
	free(pOffsetPart); free(pOffsetSubPart); free(ptArray);

	AcDbObjectId tempId;
	if(bNotFill == FALSE)
	{
		CString strHatchLyName;
		strHatchLyName.Format("%s_填充",m_pInfo->m_strCadLyName);
		m_pInfo->m_EntTool.CreateLayer(tempId,strHatchLyName,253);
		m_pInfo->m_EntTool.CreatePolyHatch(tempId,FillObjArray,strFillName,dScale,dAngle,strHatchLyName,nClrIndexF);
		CDistXData tempXData(tempId,"ROWID_OBJS");
		tempXData.Add("dist","dist");
		tempXData.Update(); tempXData.Close();
	}

	//将相关信息写入XData扩展属性（外部使用和内部使用）
	//内部：SYMBOLPOS->ROWID(OBJECTID)->PARTNUM->ENTITYID1->...->EntityIdn
	return WriteXDataToObjects(strTempNameArray,nPos,strRowID,strTempObjArray,strXDataVArray);
}

long CPolygonExchange::WriteToSDE(int* pRowId,IDistCommand* pCommand,AcDbObjectId& ObjId)
{
	//只处理AcDb2dPolyline,AcDbPolyline,AcDb3dPolyline,AcDbCircle,AcDbEplise六种实体
	long rc = 0;

	int nIsExists,nSymbolPos,nRowId,nPartNum;
	AcDbObjectIdArray ObjIdArray;
	//判断实体是否被修改，并返回实体的一些扩展属性值
	if(CheckObjAndReadXDataInInfo(nIsExists,nSymbolPos,nRowId,
		                          nPartNum,ObjIdArray,ObjId)==0) 
	{
		OutputDebugStringX("Error::CPolygonExchange::WriteToSDE :: CheckObjAndReadXDataInInfo");
		return -1; //(返回－1意思是让外部不在做后续处理)
	}


	// 颜色号  线型 线宽  全局线宽 填充色号 图案名称 图案比例 图案角度
	AcGePoint3d tempPt;
	AcGePoint3dArray tempPtArray;
	int nClrIndex;
	char strLnType[60]={0};
	double dWidth = 0.00;
	double dWidthG=0.0;
	int nClrIndexFill=0;
	char strFillName[60] = {0};
	double dScale=1.0;
	double dAngle = 0.0;

	int nPtNum =0;

	SE_SHAPE shape;
	rc = SE_shape_create(m_pInfo->m_Coordref,&shape);
	check_error(rc,"WriteToSDE->SE_shape_create");

	//生成单点SHAPE或多点SHAPE
	int nPartPos = 1;
	AcDbObjectId tempObjId;
	int nObjCount = ObjIdArray.length();
	AcDbEntity* pEnt = NULL;
	for(int i=0; i<nObjCount; i++)
	{
		tempObjId = ObjIdArray.at(i);
		tempPtArray.setLogicalLength(0);
		if(Acad::eOk !=acdbOpenObject(pEnt,tempObjId,AcDb::kForRead))
		{
			continue;
		}

		if(pEnt->isKindOf(AcDb2dPolyline::desc()))
		{
			AcDb2dPolyline *pline = AcDb2dPolyline::cast(pEnt);
			
			if(i==0)
			{
				nClrIndex = pline->colorIndex();
				strcpy(strLnType, pline->linetype());
				dWidth = m_pInfo->m_EntTool.LnWeightToDouble(pline->lineWeight());
				dWidthG = pline->defaultStartWidth();
			}
		
			//SHAPE属性
			m_pInfo->m_EntTool.GetPoly2dlinePts(pline,tempPtArray);
			nPtNum = tempPtArray.length();
		}
		else if(pEnt->isKindOf(AcDbPolyline::desc()))
		{
			AcDbPolyline *pline = AcDbPolyline::cast(pEnt);

			if(i==0)
			{
				nClrIndex = pline->colorIndex();
				strcpy(strLnType, pline->linetype());
				dWidth = m_pInfo->m_EntTool.LnWeightToDouble(pline->lineWeight());
				pline->getWidthsAt(0,dWidthG,dWidthG);
			}
			
			//SHAPE属性
			m_pInfo->m_EntTool.GetPolylinePts(pline,tempPtArray);
			nPtNum = tempPtArray.length();
		}
		else if(pEnt->isKindOf(AcDbCircle::desc()))
		{
			AcDbCircle *pline = AcDbCircle::cast(pEnt);

			if(i==0)
			{
				nClrIndex = pline->colorIndex();
				strcpy(strLnType,pline->linetype());
				dWidth = m_pInfo->m_EntTool.LnWeightToDouble(pline->lineWeight());
				dWidthG = 0;
			}
			
			double dR = pline->radius();

			if(dR <=10)
				nPtNum = 9;
			else
				nPtNum = (dR/10+1)*9;
			m_pInfo->m_EntTool.GetCirclePts(tempPtArray,pline,nPtNum);

		}
		else if(pEnt->isKindOf(AcDbEllipse::desc()))
		{
			AcDbEllipse *pline = AcDbEllipse::cast(pEnt);
			if(i==0)
			{
				nClrIndex = pline->colorIndex();
				strcpy(strLnType,pline->linetype());
				dWidth = m_pInfo->m_EntTool.LnWeightToDouble(pline->lineWeight());
				dWidthG = 0;
			}
			nPtNum = 20;
			m_pInfo->m_EntTool.GetEllipsPts(tempPtArray,pline,nPtNum);
		}
		else if(pEnt->isKindOf(AcDbSpline::desc()))
		{
			AcDbSpline *pline = AcDbSpline::cast(pEnt);
			if(i==0)
			{
				nClrIndex = pline->colorIndex();
				strcpy(strLnType,pline->linetype());
				dWidth = m_pInfo->m_EntTool.LnWeightToDouble(pline->lineWeight());
				dWidthG = 0;
			}

			nPtNum = 50;
			m_pInfo->m_EntTool.GetSplinePts(tempPtArray,pline,nPtNum);
		}
		else
		{
			pEnt->close();continue;
		}
		pEnt->close();


		nPtNum = tempPtArray.length();
		
		SE_POINT* ptArray = (SE_POINT*) malloc (nPtNum * sizeof(SE_POINT));

		for(int k=0; k<nPtNum; k++)
		{
			tempPt = tempPtArray.at(k);
			ptArray[k].x = tempPt.x; ptArray[k].y = tempPt.y;
		}

		if(fabs(ptArray[0].x - ptArray[nPtNum-1].x) > 0.01 || fabs(ptArray[0].y - ptArray[nPtNum-1].y) > 0.01)
		{
			ptArray[nPtNum]=ptArray[0]; nPtNum++;
		}
		if(nObjCount >1)
		{
			SE_SHAPE shape_part;
			rc = SE_shape_create(m_pInfo->m_Coordref,&shape_part);
			check_error(rc,"WriteToSDE->SE_shape_create");

			rc = SE_shape_generate_polygon(nPtNum,1,NULL,ptArray,NULL,NULL,shape_part);
			check_error(rc,"WriteToSDE->SE_shape_generate_polygon");

			rc = SE_shape_add_part(shape_part,shape);
			check_error(rc,"WriteToSDE->SE_shape_add_part");

			nPartPos++;
			SE_shape_free(shape_part);
		}
		else
		{
			rc = SE_shape_generate_polygon(nPtNum,1,NULL,ptArray,NULL,NULL,shape);
			check_error(rc,"WriteToSDE->SE_shape_generate_polygon");
		}
		free(ptArray);
	}

	/*char  strSQL[500];
	memset(strSQL,0,sizeof(char)*500);

	if(nRowId < 0) //插入(扩展属性中没有RowId，即为新增加记录)
	{
		sprintf(strSQL,"F:%s,T:%s,##",m_pInfo->m_strShapeFldName,m_pInfo->m_strSdeLyName);
		pCommand->InsertData(&nRowId,strSQL,shape); //直接插入SHAPE
	}
	else if(nIsExists==1) //在改动数组中存在
	{
		//更新操作
		sprintf(strSQL,"F:%s,T:%s,W:%s=%d,##",m_pInfo->m_strShapeFldName,m_pInfo->m_strSdeLyName,m_pInfo->m_strRowIdFldName,nRowId);
		rc = pCommand->UpdateData(strSQL,NULL,shape);
		//acutPrintf(strSQL);
		if(rc != 1)
			MessageBox(0,ReturnErrorCodeInfo(rc),"",MB_OK);
		m_pDbReactor->DeleteRowId(m_pInfo->m_strCadLyName,m_pInfo->m_strSdeLyName,nRowId);//删除修改的记录
	}

	SE_shape_free(shape);*/
	
	//获取实体填充对象Hatch，并读取相关属性
	AcDbObjectId HatchId;
	if(Acad::eOk ==acdbOpenObject(pEnt,tempObjId,AcDb::kForRead))
	{
		nClrIndexFill = 0;
		dScale = 1.0;
		dAngle = 0.0;
		strcpy(strFillName," ");
		if(1 == GetEntityHatch(HatchId,pEnt))//nClrIndexFill,strFillName[60],dScale,dAngle;
		{
			AcDbEntity *pTemp = NULL;
			AcDbHatch  *pHatch = NULL;
			if(Acad::eOk == acdbOpenAcDbEntity(pTemp,HatchId,AcDb::kForRead))
			{
				pHatch = (AcDbHatch*) pTemp;
				nClrIndexFill = pHatch->colorIndex();
				strcpy(strFillName,pHatch->patternName());
				dScale = pHatch->patternScale();
				dAngle = pHatch->patternAngle();
				pTemp->close();
			}
		}

		pEnt->close();
	}
	

	//根据符号画表中的字段信息，依次更新需要更新的字段内容 ( 颜色号  线型 线宽  全局线宽 填充色号 图案名称 图案比例 图案角度 )
	CString strTempFldName,strTempFldValue;
	CStringArray strFldNameArray,strFldValueArray;
	CDistXData tempXData(ObjId,m_pInfo->m_strXDataName);
	tempXData.Select(strFldNameArray,strFldValueArray);

	BOOL bHasXMBH = FALSE;
	for(int k=0; k<m_pInfo->m_pSymbolInfo[nSymbolPos].m_nNum; k++)
	{
		int nIndex =m_pInfo->m_pSymbolInfo[nSymbolPos].m_nPosArray[k];
		strTempFldName = m_pInfo->m_pSymbolInfo[nSymbolPos].m_strFld[nIndex];
		strTempFldValue.Empty();
		switch(nIndex)
		{
		case 0: strTempFldValue.Format("%d",nClrIndex);break;       //颜色号
		case 1: strTempFldValue = strLnType;break;                  //线型
		case 2: strTempFldValue.Format("%0.2f",dWidth);break;       //线宽
		case 3: strTempFldValue.Format("%0.2f",dWidthG);break;      //全局线宽
		case 4: strTempFldValue.Format("%d",nClrIndexFill);break;   //填充色号
		case 5: strTempFldValue = strFillName; break;               //图案名称
		case 6: strTempFldValue.Format("%0.2f",dScale);break;       //图案比例
		case 7: strTempFldValue.Format("%0.2f",dAngle);break;       //图案角度
		default:break;
		}

		SetValueToArray(strTempFldValue,strTempFldName,strFldNameArray,strFldValueArray); 
	}
	DeleteValueFromArray(m_pInfo->m_strRowIdFldName,strFldNameArray,strFldValueArray);//删除RowId字段



	CString strShapeValue;
	strShapeValue.Format("%d",(int)(shape));
	strFldValueArray.Add(strShapeValue);
	strFldNameArray.Add(m_pInfo->m_strShapeFldName);

	int nResult = -1;
	if(nRowId < 1)
		nResult = SaveValuesToSDE(pCommand,strFldNameArray,strFldValueArray,*pRowId,1);
	else if(nIsExists == 1)
	{
		*pRowId = nRowId;
		nResult = SaveValuesToSDE(pCommand,strFldNameArray,strFldValueArray,*pRowId,2);
		if(nResult == 1)
			m_pDbReactor->DeleteRowId(m_pInfo->m_strCadLyName,m_pInfo->m_strSdeLyName,*pRowId);//删除修改的记录
	}

	SE_shape_free(shape);

	if(nResult != 1)
		nResult = 0;
	return nResult;
}

long CPolygonExchange::WriteToSDE(int* pRowId,IDistCommand* pCommand,AcDbObjectId& ObjId, long nType) // nType: insert=1; modify=2
{
	//只处理AcDb2dPolyline,AcDbPolyline,AcDb3dPolyline,AcDbCircle,AcDbEplise六种实体
	long rc = 0;

	int nSymbolPos = 0;
	int nRowId = -1;
	int nPartNum = 0;

	if (nType == 2)
	{
		//取实体内部扩展属性信息
		CStringArray strFldNameArray,strFldValueArray;
		CDistXData tXData(ObjId,ROW_PART_XDATA);
		tXData.Select(strFldNameArray,strFldValueArray);
		tXData.Close();//尽快关闭实体

		//提取扩展属性信息中的RowId
		CString strName,strValue;
		if(strFldValueArray.GetSize()>1)
		{
			GetValueFromArray(strValue,m_pInfo->m_strRowIdFldName,strFldNameArray,strFldValueArray);
			strValue.TrimLeft(); strValue.TrimRight();
			if(!strValue.IsEmpty())
				nRowId = atoi(strValue.GetBuffer(0));
		}
	}

	// 颜色号  线型 线宽  全局线宽 填充色号 图案名称 图案比例 图案角度
	AcGePoint3d tempPt;
	AcGePoint3dArray tempPtArray;
	int nClrIndex;
	char strLnType[60]={0};
	double dWidth = 0.00;
	double dWidthG=0.0;
	int nClrIndexFill=0;
	char strFillName[60] = {0};
	double dScale=1.0;
	double dAngle = 0.0;

	int nPtNum =0;

	SE_SHAPE shape;
	rc = SE_shape_create(m_pInfo->m_Coordref,&shape);
	check_error(rc,"WriteToSDE->SE_shape_create");

	//生成单点SHAPE或多点SHAPE
	int nPartPos = 1;
	AcDbEntity* pEnt = NULL;

	tempPtArray.setLogicalLength(0);
	if(Acad::eOk !=acdbOpenObject(pEnt,ObjId,AcDb::kForRead))
	{
		return -1;
	}

	if(pEnt->isKindOf(AcDb2dPolyline::desc()))
	{
		AcDb2dPolyline *pline = AcDb2dPolyline::cast(pEnt);

		nClrIndex = pline->colorIndex();
		strcpy(strLnType, pline->linetype());
		dWidth = m_pInfo->m_EntTool.LnWeightToDouble(pline->lineWeight());
		dWidthG = pline->defaultStartWidth();

		//SHAPE属性
		m_pInfo->m_EntTool.GetPoly2dlinePts(pline,tempPtArray);
		nPtNum = tempPtArray.length();
	}
	else if(pEnt->isKindOf(AcDbPolyline::desc()))
	{
		AcDbPolyline *pline = AcDbPolyline::cast(pEnt);

		nClrIndex = pline->colorIndex();
		strcpy(strLnType, pline->linetype());
		dWidth = m_pInfo->m_EntTool.LnWeightToDouble(pline->lineWeight());
		pline->getWidthsAt(0,dWidthG,dWidthG);

		//SHAPE属性
		m_pInfo->m_EntTool.GetPolylinePts(pline,tempPtArray);
		nPtNum = tempPtArray.length();
	}
	else if(pEnt->isKindOf(AcDbCircle::desc()))
	{
		AcDbCircle *pline = AcDbCircle::cast(pEnt);

		nClrIndex = pline->colorIndex();
		strcpy(strLnType,pline->linetype());
		dWidth = m_pInfo->m_EntTool.LnWeightToDouble(pline->lineWeight());
		dWidthG = 0;

		double dR = pline->radius();

		if(dR <=10)
			nPtNum = 9;
		else
			nPtNum = (dR/10+1)*9;
		m_pInfo->m_EntTool.GetCirclePts(tempPtArray,pline,nPtNum);

	}
	else if(pEnt->isKindOf(AcDbEllipse::desc()))
	{
		AcDbEllipse *pline = AcDbEllipse::cast(pEnt);

		nClrIndex = pline->colorIndex();
		strcpy(strLnType,pline->linetype());
		dWidth = m_pInfo->m_EntTool.LnWeightToDouble(pline->lineWeight());
		dWidthG = 0;

		nPtNum = 20;
		m_pInfo->m_EntTool.GetEllipsPts(tempPtArray,pline,nPtNum);
	}
	else if(pEnt->isKindOf(AcDbSpline::desc()))
	{
		AcDbSpline *pline = AcDbSpline::cast(pEnt);

		nClrIndex = pline->colorIndex();
		strcpy(strLnType,pline->linetype());
		dWidth = m_pInfo->m_EntTool.LnWeightToDouble(pline->lineWeight());
		dWidthG = 0;

		nPtNum = 50;
		m_pInfo->m_EntTool.GetSplinePts(tempPtArray,pline,nPtNum);
	}
	else
	{
		pEnt->close();return -1;
	}
	pEnt->close();


	nPtNum = tempPtArray.length();

	SE_POINT* ptArray = (SE_POINT*) malloc (nPtNum * sizeof(SE_POINT));

	for(int k=0; k<nPtNum; k++)
	{
		tempPt = tempPtArray.at(k);
		ptArray[k].x = tempPt.x; ptArray[k].y = tempPt.y;
	}

	if(fabs(ptArray[0].x - ptArray[nPtNum-1].x) > 0.01 || fabs(ptArray[0].y - ptArray[nPtNum-1].y) > 0.01)
	{
		ptArray[nPtNum]=ptArray[0]; nPtNum++;
	}

	rc = SE_shape_generate_polygon(nPtNum,1,NULL,ptArray,NULL,NULL,shape);
	check_error(rc,"WriteToSDE->SE_shape_generate_polygon");

	free(ptArray);

	//获取实体填充对象Hatch，并读取相关属性
	AcDbObjectId HatchId;
	if(Acad::eOk ==acdbOpenObject(pEnt,ObjId,AcDb::kForRead))
	{
		nClrIndexFill = 0;
		dScale = 1.0;
		dAngle = 0.0;
		strcpy(strFillName," ");
		if(1 == GetEntityHatch(HatchId,pEnt))//nClrIndexFill,strFillName[60],dScale,dAngle;
		{
			AcDbEntity *pTemp = NULL;
			AcDbHatch  *pHatch = NULL;
			if(Acad::eOk == acdbOpenAcDbEntity(pTemp,HatchId,AcDb::kForRead))
			{
				pHatch = (AcDbHatch*) pTemp;
				nClrIndexFill = pHatch->colorIndex();
				strcpy(strFillName,pHatch->patternName());
				dScale = pHatch->patternScale();
				dAngle = pHatch->patternAngle();
				pTemp->close();
			}
		}

		pEnt->close();
	}


	//根据符号画表中的字段信息，依次更新需要更新的字段内容 ( 颜色号  线型 线宽  全局线宽 填充色号 图案名称 图案比例 图案角度 )
	CString strTempFldName,strTempFldValue;
	CStringArray strFldNameArray,strFldValueArray;
	CDistXData tempXData(ObjId,m_pInfo->m_strXDataName);
	tempXData.Select(strFldNameArray,strFldValueArray);

	BOOL bHasXMBH = FALSE;
	for(int k=0; k<m_pInfo->m_pSymbolInfo[nSymbolPos].m_nNum; k++)
	{
		int nIndex =m_pInfo->m_pSymbolInfo[nSymbolPos].m_nPosArray[k];
		strTempFldName = m_pInfo->m_pSymbolInfo[nSymbolPos].m_strFld[nIndex];
		strTempFldValue.Empty();
		switch(nIndex)
		{
		case 0: strTempFldValue.Format("%d",nClrIndex);break;       //颜色号
		case 1: strTempFldValue = strLnType;break;                  //线型
		case 2: strTempFldValue.Format("%0.2f",dWidth);break;       //线宽
		case 3: strTempFldValue.Format("%0.2f",dWidthG);break;      //全局线宽
		case 4: strTempFldValue.Format("%d",nClrIndexFill);break;   //填充色号
		case 5: strTempFldValue = strFillName; break;               //图案名称
		case 6: strTempFldValue.Format("%0.2f",dScale);break;       //图案比例
		case 7: strTempFldValue.Format("%0.2f",dAngle);break;       //图案角度
		default:break;
		}

		SetValueToArray(strTempFldValue,strTempFldName,strFldNameArray,strFldValueArray); 
	}
	DeleteValueFromArray(m_pInfo->m_strRowIdFldName,strFldNameArray,strFldValueArray);//删除RowId字段



	CString strShapeValue;
	strShapeValue.Format("%d",(int)(shape));
	strFldValueArray.Add(strShapeValue);
	strFldNameArray.Add(m_pInfo->m_strShapeFldName);

	int nResult = -1;
	if(nType == 1)
		nResult = SaveValuesToSDE(pCommand,strFldNameArray,strFldValueArray,*pRowId,1);
	else if(nType == 2)
	{
		*pRowId = nRowId;
		nResult = SaveValuesToSDE(pCommand,strFldNameArray,strFldValueArray,*pRowId,2);
		if(nResult == 1)
			m_pDbReactor->DeleteRowId(m_pInfo->m_strCadLyName,m_pInfo->m_strSdeLyName,*pRowId);//删除修改的记录
	}

	SE_shape_free(shape);

	if(nResult != 1)
		nResult = 0;
	return nResult;
}

long CPolygonExchange::GetEntityHatch(AcDbObjectId& resultId,AcDbEntity* pEnt)
{
	void* pSomething=NULL;
	AcDbObject *pObj =NULL;
	resultId.setNull();
	AcDbVoidPtrArray* pReactors = pEnt->reactors();
	if (pReactors != NULL) 
	{
		for (int i=0; i<pReactors->length(); i++) 
		{
			pSomething = pReactors->at(i);
			resultId = acdbPersistentReactorObjectId(pSomething);
			acdbOpenObject(pObj,resultId,AcDb::kForRead);
			if(pObj->isKindOf(AcDbHatch::desc()))
			{
				pObj->close(); return 1;
			}
			resultId.setNull();
			pObj->close();
		}
	}
	return 0;
}




//=============================  class CMultpatchExchange  ==============================//

CMultpatchExchange::CMultpatchExchange()
{
}

CMultpatchExchange::CMultpatchExchange(CExchangeInfo* pInfo):CBaseExchange(pInfo)
{
}

CMultpatchExchange::~CMultpatchExchange()
{
}


long CMultpatchExchange::DrawEntity(AcDbObjectId& resultId,IDistRecordSet* pRcdSet)
{
	return 0;
}

long CMultpatchExchange::WriteToSDE(int* pRowId,IDistCommand* pCommand,AcDbObjectId& ObjId)
{
	return 0;	
}

long CMultpatchExchange::WriteToSDE(int* pRowId,IDistCommand* pCommand,AcDbObjectId& ObjId, long nType) // nType: insert=1; modify=2
{
	return 0;
}
