#include "stdafx.h"
#include "DistDatabaseReactor.h"
#include "DistEntityTool.h"
#include "DistXData.h"


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


ACRX_NO_CONS_DEFINE_MEMBERS(CDistDatabaseReactor, AcDbDatabaseReactor);

CDistDatabaseReactor* CDistDatabaseReactor::m_pInstance = NULL;  //唯一实例对象指针

CDistDatabaseReactor::CDistDatabaseReactor()
{
	m_pRowIdData = NULL;
	m_pReadOnlyLayer = NULL;
	memset(m_strRowIdName,0,sizeof(char)*60);
	memset(m_strXDataName,0,sizeof(char)*160);

	m_XDataFilterArray.RemoveAll();
}


CDistDatabaseReactor::~CDistDatabaseReactor()
{
	//释放链表
	DIST_STRUCT_CHANGEROWID* pTemp = m_pRowIdData;
	while(pTemp != NULL)
	{
		m_pRowIdData = m_pRowIdData->pNext;
		pTemp->RowIdArray.RemoveAll();
		delete pTemp;
		pTemp = m_pRowIdData;
	}

	DIST_STRUCT_READONLYLAYER* pTempLy = m_pReadOnlyLayer;
	while(pTempLy != NULL)
	{
		m_pReadOnlyLayer = m_pReadOnlyLayer->pNext;
		delete pTempLy;
		pTempLy = m_pReadOnlyLayer;
	}
}

void CDistDatabaseReactor::DeleteAllLayer()
{
	//释放链表
	DIST_STRUCT_CHANGEROWID* pTemp = m_pRowIdData;
	while(pTemp != NULL)
	{
		m_pRowIdData = m_pRowIdData->pNext;
		pTemp->RowIdArray.RemoveAll();
		delete pTemp;
		pTemp = m_pRowIdData;
	}

	DIST_STRUCT_READONLYLAYER* pTempLy = m_pReadOnlyLayer;
	while(pTempLy != NULL)
	{
		m_pReadOnlyLayer = m_pReadOnlyLayer->pNext;
		delete pTempLy;
		pTempLy = m_pReadOnlyLayer;
	}
}

// 功能：获取类唯一实例对象指针
// 参数：无
// 返回：实例对象指针
CDistDatabaseReactor* CDistDatabaseReactor::GetInstance()
{
	if(m_pInstance == NULL)
		m_pInstance = new CDistDatabaseReactor;
	
	return m_pInstance;
}


// 功能：释放对象
// 参数：无
// 返回：无
void CDistDatabaseReactor::Release()
{
	if(m_pInstance != NULL)
		delete m_pInstance;
}


// 功能：设置参数(SDE图层中RowId名称，扩展属性名称，默认为RowId_Objs)
// 参数：strRowId      SDE图层中RowId名称
//       strXDataName  扩展属性名称
// 返回：无
void CDistDatabaseReactor::SetParam(const char* strRowIdName,const char* strXDataName)
{
	strcpy(m_strRowIdName,strRowIdName);
	strcpy(m_strXDataName,strXDataName);
}


// 功能：判断指定属性实体是否存在
// 参数：strCadLyName     CAD图层名称
//       strSdeLyName     SDE图层名称
//       nRowId           实体RowId
//       pDB              数据库指针(主要区分多文档)
// 返回：存在返回 1 ；不存在返回 0 ；其他参考定义
long  CDistDatabaseReactor::IsRowIdExists(const char* strCadLyName,const char *strSdeLyName,
										  int nRowId,AcDbDatabase* pDB)
{
	if(nRowId < 0) return 0;  //RowId值小于0,认为是新增实体，链表不会记录它们的改动

	int nResult = 0;
	DIST_STRUCT_CHANGEROWID* pTemp = m_pRowIdData;

	int nID = int(pDB);


	while(pTemp != NULL)
	{
		if((nID == pTemp->nID) && (CompareString(strCadLyName,pTemp->strCadLyName)==0)
			                 && (CompareString(strSdeLyName,pTemp->strSdeLyName)==0))
		{
			int nCount = pTemp->RowIdArray.GetSize();
			for(int i=0; i<nCount; i++)
			{
				if(nRowId == pTemp->RowIdArray.GetAt(i))
				{
					nResult = 1;
					break;
				}
			}
		}
		pTemp = pTemp->pNext;
	}

	return nResult;
}

long CDistDatabaseReactor::GetRowIdCount(const char* strCadLyName,const char *strSdeLyName,AcDbDatabase* pDB)
{
	int nResult = 0;
	DIST_STRUCT_CHANGEROWID* pTemp = m_pRowIdData;

	int nID = int(pDB);
	while(pTemp != NULL)
	{
		if(nID == pTemp->nID && CompareString(strCadLyName,pTemp->strCadLyName)==0
			                 && CompareString(strSdeLyName,pTemp->strSdeLyName)==0)
		{
			nResult= pTemp->RowIdArray.GetSize();
			break;
		}
		pTemp = pTemp->pNext;
	}
	return nResult;
}

long CDistDatabaseReactor::SelectRowId(int& nRowId,const char* strCadLyName,const char *strSdeLyName,int nIndex,AcDbDatabase* pDB)
{
	int nResult = 0;
	DIST_STRUCT_CHANGEROWID* pTemp = m_pRowIdData;

	int nID = int(pDB);
	nRowId = -1;

	while(pTemp != NULL)
	{
		if(nID == pTemp->nID && CompareString(strCadLyName,pTemp->strCadLyName)==0
			                 && CompareString(strSdeLyName,pTemp->strSdeLyName)==0)
		{
			int nCount = pTemp->RowIdArray.GetSize();
			if(nIndex>=0 && nIndex < nCount)
			{
				nRowId = pTemp->RowIdArray.GetAt(nIndex);
				nResult = 1;
			}
			break;
		}
		pTemp = pTemp->pNext;
	}

	return nResult;
}

void CDistDatabaseReactor::InsertRowId(const char* strCadLyName,const char *strSdeLyName,int nRowId,AcDbDatabase* pDB)
{
	if(nRowId < 0) return;//RowId值小于0,认为是新增实体，链表不会记录它们的改动

	DIST_STRUCT_CHANGEROWID* pTemp = m_pRowIdData;
	DIST_STRUCT_CHANGEROWID* pTempPre = pTemp;

	int nID = int(pDB);

	while(pTemp != NULL)
	{
		if(nID == pTemp->nID && CompareString(strCadLyName,pTemp->strCadLyName)==0
			                 && CompareString(strSdeLyName,pTemp->strSdeLyName)==0)
		{
			int nCount = pTemp->RowIdArray.GetSize();
			bool bFound = false;
			for(int i=0; i<nCount; i++)
			{
				if(nRowId == pTemp->RowIdArray.GetAt(i))
				{
					bFound = true;
					break;
				}
			}
			if(!bFound)
				pTemp->RowIdArray.Add(nRowId);

			return;
		}

		pTempPre = pTemp;
		pTemp = pTemp->pNext;
	}

	pTemp = new DIST_STRUCT_CHANGEROWID;
	strcpy(pTemp->strCadLyName,strCadLyName);
	strcpy(pTemp->strSdeLyName,strSdeLyName);
	pTemp->nID = nID;
	pTemp->pNext = NULL;
	pTemp->RowIdArray.Add(nRowId);

	if(m_pRowIdData == NULL)
		m_pRowIdData = pTemp;
	else
		pTempPre->pNext = pTemp;
}


void CDistDatabaseReactor::DeleteRowId(const char* strCadLyName,const char *strSdeLyName,int nRowId,AcDbDatabase* pDB)
{
	DIST_STRUCT_CHANGEROWID* pTemp = m_pRowIdData;

	int nID = int(pDB);

	while(pTemp != NULL)
	{
		if(nID == pTemp->nID && CompareString(strCadLyName,pTemp->strCadLyName)==0
			                 && CompareString(strSdeLyName,pTemp->strSdeLyName)==0)
		{
			int nCount = pTemp->RowIdArray.GetSize();
			for(int i=0; i<nCount; i++)
			{
				if(nRowId == pTemp->RowIdArray.GetAt(i))
				{
					pTemp->RowIdArray.RemoveAt(i);  return;
				}
			}
		}
		pTemp = pTemp->pNext;
	}
}


void CDistDatabaseReactor::DeleteLayerRowIdArray(const char* strCadLyName,const char *strSdeLyName,AcDbDatabase* pDB)
{
	DIST_STRUCT_CHANGEROWID* pTemp = m_pRowIdData;

	int nID = int(pDB);

	while(pTemp != NULL)
	{
		if(nID == pTemp->nID && CompareString(strCadLyName,pTemp->strCadLyName)==0
			                 && CompareString(strSdeLyName,pTemp->strSdeLyName)==0)
		{
			pTemp->RowIdArray.RemoveAll();
			return;
		}
		pTemp = pTemp->pNext;
	}
}

void CDistDatabaseReactor::SetXDataFilter(CStringArray& XDataFilterArray)
{
	m_XDataFilterArray.RemoveAll();
	m_XDataFilterArray.Append(XDataFilterArray);
}

void CDistDatabaseReactor::WriteInfoToXRecord(const AcDbObject* pObj,const AcDbDatabase* pDB)
{
	if(!pObj->isKindOf(AcDbEntity::desc())) return;

	AcDbEntity *pEnt= AcDbEntity::cast(pObj);
	char strLyName[255]={0};
	strcpy(strLyName,pEnt->layer());//图层名称
	
	CString strRowId,strSdeLyName;              //取RowId
	CDistXData tempXData(pEnt,m_strXDataName);
	tempXData.Select(m_strRowIdName,strRowId);
	tempXData.Select("SDELYNAME",strSdeLyName);
	tempXData.Close();

	if(IsReadOnlyLayer(strSdeLyName,(AcDbDatabase*)pDB)==1) return;  //如果是只读图层，直接返回

	strRowId.TrimLeft();
	strRowId.TrimRight();

	int nRowId = -1;
	try
	{
		if(!strRowId.IsEmpty())
			nRowId = atoi(strRowId);
	}
	catch (...)
	{
		nRowId = -1;
	}
	if(nRowId < 0) return;

	pEnt= AcDbEntity::cast(pObj);
	CString strAppName = strSdeLyName;
	strAppName = "DIST_" + strAppName;
	CDistXData distXData(pEnt, strAppName);

	for (int j=0; j<m_XDataFilterArray.GetCount(); j++)
	{
		CString strFilter = m_XDataFilterArray.GetAt(j);
		CString strTempAttr = strFilter.Left(strFilter.Find('='));
		CString strTempValue = strFilter.Right(strFilter.GetLength()-strFilter.Find('=')-1);

		CString strValue;
		distXData.Select(strTempAttr, strValue);

		if (strValue.CompareNoCase(strTempValue) != 0)
		{
			distXData.Close();
			return;
		}
	}
	distXData.Close();

	InsertRowId(strLyName,strSdeLyName,nRowId,(AcDbDatabase*)pDB);

}


long CDistDatabaseReactor::IsReadOnlyLayer(const char *strSdeLyName,AcDbDatabase* pDB)
{
	int nResult = 0;
	DIST_STRUCT_READONLYLAYER* pTemp = m_pReadOnlyLayer;
	int nID = int(pDB);

	while(pTemp != NULL)
	{
		if((nID == pTemp->nID) && (CompareString(strSdeLyName,pTemp->strSdeLyName)==0))
		{
			nResult = 1;
			break;
		}
		pTemp = pTemp->pNext;
	}

	return nResult;
}


void CDistDatabaseReactor::InsertReadOnlyLayer(const char *strSdeLyName,AcDbDatabase* pDB)
{
	DIST_STRUCT_READONLYLAYER* pTemp = m_pReadOnlyLayer;
	DIST_STRUCT_READONLYLAYER* pTempPre = pTemp;

	int nID = int(pDB);

	while(pTemp != NULL)
	{
		if((nID == pTemp->nID) && (CompareString(strSdeLyName,pTemp->strSdeLyName)==0))
			return; //已经存在，直接退出

		pTempPre = pTemp;
		pTemp = pTemp->pNext;
	}

	pTemp = new DIST_STRUCT_READONLYLAYER;
	strcpy(pTemp->strSdeLyName,strSdeLyName);
	pTemp->nID = nID;
	pTemp->pNext = NULL;

	if(m_pReadOnlyLayer == NULL)
		m_pReadOnlyLayer = pTemp;
	else
		pTempPre->pNext = pTemp;
	
}


void CDistDatabaseReactor::DeleteReadOnlyLayer(const char *strSdeLyName,AcDbDatabase* pDB)
{
	DIST_STRUCT_READONLYLAYER* pTemp = m_pReadOnlyLayer;
	DIST_STRUCT_READONLYLAYER* pTempPre = pTemp;
	int nID = int(pDB);

	while(pTemp != NULL)
	{
		if((nID == pTemp->nID) && (CompareString(strSdeLyName,pTemp->strSdeLyName)==0))
		{
			pTempPre->pNext = pTemp->pNext;
			delete pTemp; pTemp = NULL;  return;
		}
		pTempPre = pTemp;
		pTemp = pTemp->pNext;
	}
}


void CDistDatabaseReactor::objectAppended(const AcDbDatabase* db, const AcDbObject* obj)
{
	//
}


void CDistDatabaseReactor::objectUnAppended(const AcDbDatabase* db, const AcDbObject* obj)
{
    //
}


void CDistDatabaseReactor::objectReAppended(const AcDbDatabase* db, const AcDbObject* obj)
{
    //
}


void CDistDatabaseReactor::objectOpenedForModify(const AcDbDatabase* db, const AcDbObject* obj)
{
    //
}

void CDistDatabaseReactor::objectModified(const AcDbDatabase* db, const AcDbObject* obj)
{
	WriteInfoToXRecord(obj,db);
}


void CDistDatabaseReactor::objectErased(const AcDbDatabase* db, const AcDbObject* obj,Adesk::Boolean erased)
{
	WriteInfoToXRecord(obj,db);
}



void CDistDatabaseReactor::headerSysVarWillChange(const AcDbDatabase* db, const char* name)
{
    //
}


void CDistDatabaseReactor::headerSysVarChanged(const AcDbDatabase* db, const char* name,
                                        Adesk::Boolean bSuccess)
{
    //
}



void CDistDatabaseReactor::proxyResurrectionCompleted(const AcDbDatabase* db,
										const char* appname, AcDbObjectIdArray& objects)
{
	//
}



void CDistDatabaseReactor::goodbye(const AcDbDatabase* db)
{
	//
}
