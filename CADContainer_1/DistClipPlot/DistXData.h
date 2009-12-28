///////////////////////////////////////////////////////////////////////////////
//上海数慧系统技术有限公司拥有本代码之完全版权，未经上海数慧系统技术有限公司 //
//的明确书面许可，不得复制、取用、分发此源代码。                             //
//任何单位和个人在未获得许可情况下，不得以任何形式复制、修改和发布本软件的任 //
//何部分，否则将视为非法侵害，我公司将保留依法追究其责任的权利。在未获得许可 //
//情况下，任何组织或个人发布的产品都不能使用Dist的标志和Logo。               //
//                                                                           //
//support@dist.com.cn                                                        //
//www.dist.com.cn                                                            //
//                                                                           //
//作者 : 王晖                                                                //
//                                                                           //
// 版权所有 (C) 2007－2010 Dist Inc. 保留所有权利。                          //
///////////////////////////////////////////////////////////////////////////////

#ifndef  __DISTXDATA__H__
#define  __DISTXDATA__H__
//类说明  ：扩展属性操作类
//数据形式：XDATA名称 = 字段名称1->字段值1->字段名称2->字段值2->...->字段名称n->字段值n->NULL

#include "StdAfx.h"

class CDistXData
{
private:
	         CDistXData(void);
public:
			 CDistXData(AcDbEntity* pEnt,const char *strRegName="DIST_XDATA");
			 CDistXData(AcDbObjectId& ObjId,const char *strRegName="DIST_XDATA");
	virtual ~CDistXData(void);
	
public:
	void SetEntity(AcDbEntity* pEnt,const char *strRegName="DIST_XDATA");
	void SetEntity(AcDbObjectId& ObjId,const char *strRegName="DIST_XDATA");
    void Close();
	int  GetRecordCount();

public:
	BOOL Add(const char *strFldName,const char *strFldValue,BOOL bIfExistsToUpdate=TRUE);
	BOOL Add(CStringArray& strFldNameArray,CStringArray& strFldValueArray,BOOL bRemoveOld=TRUE);
    BOOL Modify(const char *strFldName,const char *strFldValue);
	BOOL Delete(const char *strFldName);
	BOOL Select(const char *strFldName,CString& strValue);
	BOOL Select(int nIndex,CString& strValue);
	BOOL Select(CStringArray& strFldNameArray,CStringArray& strFldValueArray);
	BOOL DeleteAllRecord();

	void Update();

	void DeleteEntitySelf();

private:
	void ReadXData();
private:
	AcDbEntity*   m_pEnt;
	char          m_strRegName[1024];
	CStringArray  m_ArrayFldName;
	CStringArray  m_ArrayFldValue;
	BOOL          m_bOpenAndClose;
};



#endif  //__DISTXDATA__H__