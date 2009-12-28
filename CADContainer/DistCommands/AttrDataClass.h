#pragma once

#include "StdAfx.h"
#include "AfxTempl.h"

class CAttrDataClass
{
public:
	CAttrDataClass(void);
	~CAttrDataClass(void);

	long m_fid;					//对象在SDE数据库中的Id
								//不是SDE数据库中的对象，此值为-1
	long m_lCadId;
	CString m_strCADLayer;		//对象当前所在CAD图层名称
	CString m_strOldCADLayer;	//对象原本所在CAD图层名称
	CString m_strSDELayer;		//对象当前所在SDE图层名称
	CString m_strOldSDELayer;	//对象原本所在SDE图层名称
	long m_lShapeEditNum;		//图形修改计数器
	long m_lLayerMoveNum;		//图层更换计数器
	CStringArray aAttr;			//对象当前SDE属性数组
								//每个属性以 属性名称:属性值 的格式存储
	CStringArray aOldAttr;		//对象原始SDE属性数组

	CAttrDataClass &operator = (const CAttrDataClass &AttrDataClass);

};

typedef CMap<long,long,CAttrDataClass,CAttrDataClass&> CMapCADIdToAttrData;