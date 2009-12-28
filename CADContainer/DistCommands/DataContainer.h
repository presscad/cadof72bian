#pragma once

#include "StdAfx.h"
#include "AttrDataClass.h"

class CDataContainer
{
public:
	CDataContainer(void);
	~CDataContainer(void);


	/*AcDbObjectIdArray aAppendedIdArray;
	AcDbObjectIdArray aErasedIdArray;*/

	CMapCADIdToAttrData m_mapNewData;
	CMapCADIdToAttrData m_mapDelData;
	CMapCADIdToAttrData m_mapShapeEditData;
	CMapCADIdToAttrData m_mapAttrEditData;
};
