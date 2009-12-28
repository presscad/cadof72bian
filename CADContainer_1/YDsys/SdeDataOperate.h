#ifndef  __SDEDATA__OPERATE__H__
#define  __SDEDATA__OPERATE__H__


#include "..\SdeData\DistExchangeInterface.h"

class CSdeDataOperate
{
public:
		     CSdeDataOperate(void);
	virtual ~CSdeDataOperate(void);

public:
		
	//功能：读SDE数据到CAD中
	long ReadAllSdeLayer(AcDbObjectIdArray& arrId,              //返回创建 CAD 实体的 ID
		                 char* strSdeLyName,                    //指定 SDE 图层名称
						 DIST_STRUCT_SELECTWAY* pFilter = NULL, //指定空间过滤条件
						 char* strFieldSql = NULL,		        //指定SQL查询条件
						 int nPower=1);                         //图层读写权限（1只读 2只写 3读写 ）



	//功能：保存CAD数据到SDE中
	long SaveAllSdeLayer(const char* strSdeLyName,              //SDE图层名称(允许带通配符)
		                 DIST_STRUCT_SELECTWAY* pFilter = NULL, //空间过滤条件
						 bool bAfterSaveToDelete = true);       //保存后是否从CAD图层中删除对应的实体

	// 获取项目编号的点数组，存储在CDefineFile::g_XMBHpts
	BOOL GetSdeOneShapePoints(AcGePoint3dArray& ptArray,const char* strSdeLyName,const char* strFName,const char* strFValue);

	void GetCadLayerBySdeLayer(CStringArray& strCadLyNameArray,const CStringArray& strSdeLyNameArray);
	void GetSdeLayerByCadLayer(CStringArray& strSdeLyNameArray,const CStringArray& strCadLyNameArray);


private:
	//功能：读SDE数据到CAD中
	//参数：
	//      strSdeLyName		SDE图层名称(不允许带通配符)
	//		pFilter				空间查询方式
	//返回：成功返回 1，失败返回 0，其他参考定义
	long ReadSdeLayer(AcDbObjectIdArray& arrId, const char* strSdeLyName, DIST_STRUCT_SELECTWAY* pFilter = NULL, 
		              const char* strFieldSql = NULL,CStringArray* pFldArray = NULL,int nPower=1);

	//功能：保存CAD数据到SDE中
	//参数：
	//      strSdeLyName		SDE图层名称(不允许带通配符)
	//		pFilter				空间查询方式
	//      bAfterSaveToDelete  保存后是否从CAD图层中删除对应的实体
	//返回：成功返回 1，失败返回 0，其他参考定义
	long SaveSdeLayer(const char* strSdeLyName, DIST_STRUCT_SELECTWAY* pFilter = NULL, bool bAfterSaveToDelete = true);

	BOOL CmpTwoString(CString str1, CString str2);

	BOOL SaveAttrToRef(AcDbObjectId ObjIdRef,CStringArray& strArrayTag,CStringArray& strArrayValue);

	BOOL IsDKLayer(CString strSdeLyName);
	BOOL IsDKLayerOld(CString strSdeLyName);
	BOOL IsXMLayer(CString strSdeLyName);
	long ReadDKLayer(AcDbObjectIdArray& arrId,char* strSdeLyName,DIST_STRUCT_SELECTWAY* pFilter = NULL,const char* strFieldSql = NULL);
	long ReadXMLayer(AcDbObjectIdArray& arrId,char* strSdeLyName,DIST_STRUCT_SELECTWAY* pFilter = NULL,const char* strFieldSql = NULL);
	long ReadDKLayerOld(AcDbObjectIdArray& arrId,char* strSdeLyName,DIST_STRUCT_SELECTWAY* pFilter = NULL,const char* strFieldSql = NULL);
private:
	//IDistConnection*   m_pConnection;
	//IDistExchangeData* m_pExchangeData;
	char               m_strGdbOwner[255];
};

#endif// __SDEDATA__OPERATE__H__